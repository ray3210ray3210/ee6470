#ifndef FILTER_DEF_H_
#define FILTER_DEF_H_

const int MASK_N = 1;
const int MASK_X = 3;
const int MASK_Y = 3;

//-------------modify hw2---------
const int GAUSSIANBLUR_FILTER_R_ADDR = 0x00000000;
const int GAUSSIANBLUR_FILTER_RESULT_ADDR = 0x00000004;
const int GAUSSIANBLUR_FILTER_CHECK_ADDR = 0x00000008;

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};
//--------------------------------
#endif
