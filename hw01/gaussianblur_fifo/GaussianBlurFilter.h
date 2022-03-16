#ifndef GAUSSIAN_BLUR_FILTER_H_
#define GAUSSIAN_BLUR_FILTER_H_
#include <systemc>
using namespace sc_core;

#include "filter_def.h"

class GaussianBlurFilter : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  sc_fifo_in<unsigned char> i_r;
  sc_fifo_in<unsigned char> i_g;
  sc_fifo_in<unsigned char> i_b;
  //--------modify
  sc_fifo_out<int> o_result;
  //sc_fifo_out<int> o_result_r;
  //sc_fifo_out<int> o_result_g;
  //sc_fifo_out<int> o_result_b;
  //-----------------------

  SC_HAS_PROCESS(GaussianBlurFilter);
  GaussianBlurFilter(sc_module_name n);
  ~GaussianBlurFilter() = default;

private:
  void do_filter();
  int val[MASK_N];
  int val_red[MASK_N];
  int val_green[MASK_N];
  int val_blue[MASK_N];
};
#endif
