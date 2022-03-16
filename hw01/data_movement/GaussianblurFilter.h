#ifndef GAUSSIANBLUR_FILTER_H_
#define GAUSSIANBLUR_FILTER_H_
#include <systemc>
using namespace sc_core;

#include "filter_def.h"

class GaussianblurFilter : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  sc_fifo_in<unsigned char> i_r;
  sc_fifo_in<unsigned char> i_g;
  sc_fifo_in<unsigned char> i_b;
  sc_fifo_out<int> o_result;

  SC_HAS_PROCESS(GaussianblurFilter);
  GaussianblurFilter(sc_module_name n);
  ~GaussianblurFilter() = default;

private:
  void do_filter();
  int val_red[256];
  int val_green[256];
  int val_blue[256];

  int width;
  int rowbuffer_red[256][MASK_Y];
  int rowbuffer_green[256][MASK_Y];
  int rowbuffer_blue[256][MASK_Y];

};
#endif
