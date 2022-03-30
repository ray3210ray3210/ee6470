#ifndef GAUSSIAN_BLUR_FILTER_H_
#define GAUSSIAN_BLUR_FILTER_H_
#include <systemc>
using namespace sc_core;

//--------modify hw2----------

#include "tlm"
#include "tlm_utils/simple_target_socket.h"
//-----------------------


#include "filter_def.h"

class GaussianBlurFilter : public sc_module {
public:
  //--------modify hw2----------
  tlm_utils::simple_target_socket<GaussianBlurFilter> t_skt;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<int> o_result;
  // sc_in_clk i_clk;
  // sc_in<bool> i_rst;
  // sc_fifo_in<unsigned char> i_r;
  // sc_fifo_in<unsigned char> i_g;
  // sc_fifo_in<unsigned char> i_b;
  // sc_fifo_out<int> o_result;

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
  //--------modify hw2----------
  unsigned int base_offset;
  void blocking_transport(tlm::tlm_generic_payload &payload,
                          sc_core::sc_time &delay);
  //-----------------------
};
#endif
