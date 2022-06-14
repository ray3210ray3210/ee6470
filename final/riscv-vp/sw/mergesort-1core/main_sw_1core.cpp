#include <string>
#include "string.h"
#include "cassert"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "defines.h"	// The type definitions for the input and output
using namespace std;

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

// MergeSort ACC
static char* const MERGESORT_START_ADDR = reinterpret_cast<char* const>(0x73000000);
static char* const MERGESORT_READ_ADDR  = reinterpret_cast<char* const>(0x73000004);

// DMA 
static volatile uint32_t * const DMA_SRC_ADDR  = (uint32_t * const)0x70000000;
static volatile uint32_t * const DMA_DST_ADDR  = (uint32_t * const)0x70000004;
static volatile uint32_t * const DMA_LEN_ADDR  = (uint32_t * const)0x70000008;
static volatile uint32_t * const DMA_OP_ADDR   = (uint32_t * const)0x7000000C;
static volatile uint32_t * const DMA_STAT_ADDR = (uint32_t * const)0x70000010;
static const uint32_t DMA_OP_MEMCPY = 1;

bool _is_using_dma = false;

void write_data_to_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){  
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(buffer);
    *DMA_DST_ADDR = (uint32_t)(ADDR);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Send
    memcpy(ADDR, buffer, sizeof(unsigned char)*len);
  }
}
void read_data_from_ACC(char* ADDR, unsigned char* buffer, int len){
  if(_is_using_dma){
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(ADDR);
    *DMA_DST_ADDR = (uint32_t)(buffer);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Read
    memcpy(buffer, ADDR, sizeof(unsigned char)*len);
  }
}

int main() {
    printf("======================================\n");
    printf("\t  merge sort start\n");
    printf("======================================\n");
    unsigned char  buffer[4] = {0};
    word data;

    FILE *infp;			// File pointer for stimulus file
    FILE *outfp;		// File pointer for results file
	char count_f_char[32];
//--- input declare---
    string stim_file = "./stimulus_dir/stimulus0.dat";
//--------------------
//---output declare---
	string output_file = "./response_dir/response0.dat";
    char output_file_char[256];

	sprintf( output_file_char, "./response_dir/response0.dat");
	outfp = fopen(output_file_char, "wb");

	int file_name_offset = strlen(output_file_char)-5;
//--------------------
	for( int count_f = 0 ,fail = 1 ;count_f<FILE_NUM ; count_f++ ){

			//update stimulus file name
		sprintf(count_f_char, "%d", count_f);
		stim_file.erase(file_name_offset, 1);							//offset 23 to modify filename
		stim_file.insert(file_name_offset,count_f_char);
			// Open the stimulus file
	    char const *stim_file_pointer = stim_file.c_str();
		infp = fopen( stim_file_pointer, "r" );
		if( infp == NULL )
		{
			if(fail == 1){
                printf("Couldn't open stimulus.dat for reading.5206\n");
                printf("number of file\n", count_f);

				exit(0);
			}
		}
		else{
			fail = 0;
		}
		// Read stimulus values from file and send to MergeSort
        printf("start reading file\n");
        
		for( int i = 0; i < DATA_NUM; i++ )
		{
			unsigned int value;
			fscanf(infp, "%u\n", &value);
            buffer[0] = value & (0b11111111);
            buffer[1] = (value >> 8) & (0b11111111);
            buffer[2] = (value >> 16) & (0b11111111);
            buffer[3] = (value >> 24) & (0b11111111);
            printf("sw write data : %u, %d, %d, %d, %d\n", value, buffer[0], buffer[1], buffer[2], buffer[3]);

            write_data_to_ACC(MERGESORT_START_ADDR, buffer, 4);			// send the stimulus value
		}
		fclose( infp );


		unsigned long sample_latency = 0;								//latency for sorting this file

		sprintf(count_f_char, "%d", count_f);

		output_file.erase(file_name_offset, 1);								// offset 23 to modify filename
		output_file.insert(file_name_offset,count_f_char);
			// Open the stimulus file
	    char const *output_file_pointer = output_file.c_str();
		//cout <<  "******file name is " <<output_file_char << "******" << endl;
		outfp = fopen(output_file_pointer, "wb");
		//cout << "------------------open the file ------------------." << output_file_char << endl;

		if (outfp == NULL)
		{
			printf("Couldn't open output.dat for writing.\n");
			exit(0);
		}
	// Read outputs from MergeSort
        printf("start writing file\n");
		for( int i = 0; i < DATA_NUM; i++ )
		{
            read_data_from_ACC(MERGESORT_READ_ADDR, buffer, 4);
			unsigned int outVal = fourCharToInt(buffer[3], buffer[2], buffer[1], buffer[0]) ;

			// fprintf( outfp, "%u\n", (int)inVal );	// write value to response file
			fprintf( outfp, "%u\n", outVal );	// write value to response file
		}
		fclose( outfp );
	}
}