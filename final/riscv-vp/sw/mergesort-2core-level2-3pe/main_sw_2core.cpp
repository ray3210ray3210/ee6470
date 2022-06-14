#include <string>
#include "string.h"
#include "cassert"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "defines.h"	// The type definitions for the input and output
using namespace std;

#define PROCESSORS 2
//the barrier synchronization objects
uint32_t barrier0_counter=0, barrier0_lock, barrier0_sem; 
uint32_t barrier1_counter=0, barrier1_lock, barrier1_sem; 
uint32_t barrier2_counter=0, barrier2_lock, barrier2_sem; 

//the mutex object to control global summation
uint32_t lock_lv1_to_lv2;  
uint32_t lock_open_file;
uint32_t lock_print;
uint32_t lock_dma;
//print synchronication semaphore (print in core order)
uint32_t print_sem[PROCESSORS]; 
//global summation variable
unsigned int data_read[DATA_NUM];

int sem_init (uint32_t *__sem, uint32_t count) __THROW
{
  *__sem=count;
  return 0;
}

int sem_wait (uint32_t *__sem) __THROW
{
  uint32_t value, success; //RV32A
  __asm__ __volatile__("\
L%=:\n\t\
     lr.w %[value],(%[__sem])            # load reserved\n\t\
     beqz %[value],L%=                   # if zero, try again\n\t\
     addi %[value],%[value],-1           # value --\n\t\
     sc.w %[success],%[value],(%[__sem]) # store conditionally\n\t\
     bnez %[success], L%=                # if the store failed, try again\n\t\
"
    : [value] "=r"(value), [success]"=r"(success)
    : [__sem] "r"(__sem)
    : "memory");
  return 0;
}

int sem_post (uint32_t *__sem) __THROW
{
  uint32_t value, success; //RV32A
  __asm__ __volatile__("\
L%=:\n\t\
     lr.w %[value],(%[__sem])            # load reserved\n\t\
     addi %[value],%[value], 1           # value ++\n\t\
     sc.w %[success],%[value],(%[__sem]) # store conditionally\n\t\
     bnez %[success], L%=                # if the store failed, try again\n\t\
"
    : [value] "=r"(value), [success]"=r"(success)
    : [__sem] "r"(__sem)
    : "memory");
  return 0;
}

int barrier(uint32_t *__sem, uint32_t *__lock, uint32_t *counter, uint32_t thread_count) {
	sem_wait(__lock);
	if (*counter == thread_count - 1) { //all finished
		*counter = 0;
		sem_post(__lock);
		for (int j = 0; j < thread_count - 1; ++j) sem_post(__sem);
	} else {
		(*counter)++;
		sem_post(__lock);
		sem_wait(__sem);
	}
	return 0;
}
union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

// MergeSort ACC
static char* const MERGELV1_p0_START_ADDR = reinterpret_cast<char* const>(0x73000000);
static char* const MERGELV1_p0_READ_ADDR  = reinterpret_cast<char* const>(0x73000004);
static char* const MERGELV1_p1_START_ADDR = reinterpret_cast<char* const>(0x74000000);
static char* const MERGELV1_p1_READ_ADDR  = reinterpret_cast<char* const>(0x74000004);
static char* const MERGELV2_p0_START_ADDR = reinterpret_cast<char* const>(0x75000000);
static char* const MERGELV2_p0_READ_ADDR  = reinterpret_cast<char* const>(0x75000004);
// DMA 
static volatile uint32_t * const DMA_SRC_ADDR  = (uint32_t * const)0x70000000;
static volatile uint32_t * const DMA_DST_ADDR  = (uint32_t * const)0x70000004;
static volatile uint32_t * const DMA_LEN_ADDR  = (uint32_t * const)0x70000008;
static volatile uint32_t * const DMA_OP_ADDR   = (uint32_t * const)0x7000000C;
static volatile uint32_t * const DMA_STAT_ADDR = (uint32_t * const)0x70000010;
static const uint32_t DMA_OP_MEMCPY = 1;

bool _is_using_dma = true;


void tran_data_from_ACC_to_ACC(char* ADDR_src, char* ADDR_dst, int len){
  if(_is_using_dma){  
    // Using DMA 
    *DMA_SRC_ADDR = (uint32_t)(ADDR_src);
    *DMA_DST_ADDR = (uint32_t)(ADDR_dst);
    *DMA_LEN_ADDR = len;
    *DMA_OP_ADDR  = DMA_OP_MEMCPY;
  }else{
    // Directly Send
    memcpy(ADDR_dst, ADDR_src, sizeof(unsigned char)*len);
  }
}
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
int main(unsigned hart_id) {

	/////////////////////////////
	// thread and barrier init //
	/////////////////////////////
	
	if (hart_id == 0) {
		// create a barrier object with a count of PROCESSORS
		sem_init(&barrier0_lock, 1);
		sem_init(&barrier1_lock, 1);
		sem_init(&barrier2_lock, 1);

		sem_init(&barrier0_sem, 0); //lock all cores initially
		sem_init(&barrier1_sem, 0); //lock all cores initially
		sem_init(&barrier2_sem, 0); //lock all cores initially
		for(int i=0; i< PROCESSORS; ++i){
			sem_init(&print_sem[i], 0); //lock printing initially
		}
		// Create mutex lock
		sem_init(&lock_lv1_to_lv2, 1);
		sem_init(&lock_open_file, 1);
		sem_init(&lock_print, 1);
		sem_init(&lock_dma, 1);

	}

    // printf("======================================\n");
    // printf("\t  merge sort start\n");
    // printf("======================================\n");
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

		if(hart_id==0){
			sem_init(&barrier0_lock, 1);
			sem_init(&barrier1_lock, 1);
			sem_init(&barrier2_lock, 1);

			sem_init(&barrier0_sem, 0); //lock all cores initially
			sem_init(&barrier1_sem, 0); //lock all cores initially
			sem_init(&barrier2_sem, 0); //lock all cores initially
		}
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
				sem_wait(&lock_open_file);
                printf("Couldn't open stimulus.dat for reading.5206\n");
                printf("number of file\n", count_f);
				sem_post(&lock_open_file);
				exit(0);
			}
		}
		else{
			fail = 0;
		}
		// Read stimulus values from file and send to MergeSort
		// sem_wait(&lock_print);
        // printf("start reading file\n");
		// sem_post(&lock_print);
        
		for( int i = 0; i < DATA_NUM; i++ )
		{
			unsigned int value;
			fscanf(infp, "%u\n", &value);
			if( hart_id==0 ){
				if( i<DATA_NUM_L1 ){
					buffer[0] = value & (0b11111111);
					buffer[1] = (value >> 8) & (0b11111111);
					buffer[2] = (value >> 16) & (0b11111111);
					buffer[3] = (value >> 24) & (0b11111111);
					// printf("sw write data : %u, %d, %d, %d, %d\n", value, buffer[0], buffer[1], buffer[2], buffer[3]);
					// printf("i=%d\n", i);
					sem_wait(&lock_dma);
					write_data_to_ACC(MERGELV1_p0_START_ADDR, buffer, 4);			// send the stimulus value
					sem_post(&lock_dma);
				}
			} else if ( hart_id==1 ){
				if( i>=DATA_NUM_L1 ){
					buffer[0] = value & (0b11111111);
					buffer[1] = (value >> 8) & (0b11111111);
					buffer[2] = (value >> 16) & (0b11111111);
					buffer[3] = (value >> 24) & (0b11111111);
					//printf("sw write data : %u, %d, %d, %d, %d\n", value, buffer[0], buffer[1], buffer[2], buffer[3]);
					// printf("i=%d\n", i);
					sem_wait(&lock_dma);
					write_data_to_ACC(MERGELV1_p1_START_ADDR, buffer, 4);			// send the stimulus value
					sem_post(&lock_dma);
				}
			}
		}
		fclose( infp );

		
		//======barrier======
		barrier(&barrier0_sem, &barrier0_lock, &barrier0_counter, PROCESSORS);

		// sem_wait(&lock_print);
		// printf("123456789\n");
		// sem_post(&lock_print);

		if(hart_id==0){
			sem_wait(&lock_lv1_to_lv2);
			for( int i = 0; i < DATA_NUM_L1; i++ ){
				tran_data_from_ACC_to_ACC(MERGELV1_p0_READ_ADDR, MERGELV2_p0_START_ADDR, 4);
				// printf("core0: i=%d\n", i);
			}
			sem_post(&lock_lv1_to_lv2);
		} else if( hart_id==1 ){
			sem_wait(&lock_lv1_to_lv2);
			for( int i = 0; i < DATA_NUM_L1; i++ ){
				tran_data_from_ACC_to_ACC(MERGELV1_p1_READ_ADDR, MERGELV2_p0_START_ADDR, 4);
				// printf("core1: i=%d\n", i);
			}
			sem_post(&lock_lv1_to_lv2);
		}


		barrier(&barrier1_sem, &barrier1_lock, &barrier1_counter, PROCESSORS);
		sem_wait(&lock_print);
		// printf("core%d: barrier1\n", hart_id);
		sem_post(&lock_print);
		if(hart_id==0){
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
			// printf("start writing file\n");
			for( int i = 0; i < DATA_NUM; i++ )
			{
				read_data_from_ACC(MERGELV2_p0_READ_ADDR, buffer, 4);
				unsigned int outVal = fourCharToInt(buffer[3], buffer[2], buffer[1], buffer[0]) ;

				// fprintf( outfp, "%u\n", (int)inVal );	// write value to response file
				fprintf( outfp, "%u\n", outVal );	// write value to response file
			}
			fclose( outfp );
		}
		barrier(&barrier2_sem, &barrier2_lock, &barrier2_counter, PROCESSORS);
		sem_wait(&lock_print);
		// printf("file:%d ,core%d finish.\n", count_f, hart_id);
		sem_post(&lock_print);

	}
}