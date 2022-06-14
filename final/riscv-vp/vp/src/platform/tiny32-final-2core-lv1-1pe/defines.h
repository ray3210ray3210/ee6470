#ifndef _DEFINES_PLATFORM_H_
#define _DEFINES_PLATFORM_H_

#define CLOCK_PERIOD 10
#define DATA_NUM 64     //Number of stimulus values per file
                        // must be even 
#define DATA_NUM_L1 32
#define DATA_NUM_L2 64
#define FILE_NUM 8
#define SORT_START_POSITION 0

typedef uint32_t input_t;
typedef uint32_t output_t;
typedef uint32_t data_t;



// MergeSort inner transport addresses
// Used between blocking_transport() & merge_lv1_p0()
const int MERGE_SORT_R_ADDR = 0x00000000;
const int MERGE_SORT_RESULT_ADDR = 0x00000004;

const int MERGE_SORT_RS_R_ADDR   = 0x00000000;
const int MERGE_SORT_RS_W_WIDTH  = 0x00000004;
const int MERGE_SORT_RS_W_HEIGHT = 0x00000008;
const int MERGE_SORT_RS_W_DATA   = 0x0000000C;
const int MERGE_SORT_RS_RESULT_ADDR = 0x00800000;


unsigned int fourCharToInt(unsigned char a, unsigned char b, unsigned char c, unsigned char d){
	unsigned int ret_val = 0;
	ret_val  =  a;
	ret_val <<= 8;
	ret_val |=  b;
	ret_val <<= 8;
	ret_val |=  c;
	ret_val <<= 8;
	ret_val |=  d;

	return ret_val;
}
union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};
#endif // _DEFINES_PLATFORM_H_
