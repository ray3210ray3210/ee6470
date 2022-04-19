#ifndef FILTER_DEF_H_
#define FILTER_DEF_H_

#define SIZE 16
#define CHAR_SIZE 4
// Sobel Filter inner transport addresses
// Used between blocking_transport() & do_filter()
const int MERGE_SORT_R_ADDR = 0x00000000;
const int MERGE_SORT_RESULT_ADDR = 0x00000004;
const int MERGE_SORT_CHECK_ADDR = 0x00000008;

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

#endif
