#include <cmath>
#include <iomanip>

#include "GaussianBlurFilter.h"
//-----------------moidify hw2-------------------------
// GaussianBlurFilter::GaussianBlurFilter(sc_module_name n) : sc_module(n) {
//   SC_THREAD(do_filter);
//   sensitive << i_clk.pos();
//   dont_initialize();
//   reset_signal_is(i_rst, false);
// }
GaussianBlurFilter::GaussianBlurFilter(sc_module_name n)
    : sc_module(n), t_skt("t_skt"), base_offset(0) {
  SC_THREAD(do_filter);

  t_skt.register_b_transport(this, &GaussianBlurFilter::blocking_transport);
}
//----------------------------------------------------------
// gaussian blur mask
const double mask[MASK_N][MASK_X][MASK_Y] = //{{{0,0,0,0,0}, {0,0,0,0,0}, {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0}}};
                                            /*{{{ 0.00390625, 0.015625, 0.0234375, 0.015625, 0.00390625},
                                              { 0.015625  , 0.0625  , 0.09375,   0.0625,    0.015625}, 
                                              { 0.0234375,  0.09375, 0.140625, 0.09375, 0.0234375}, 
                                              { 0.015625  , 0.0625  , 0.09375,   0.0625,    0.015625}, 
                                              { 0.00390625, 0.015625, 0.0234375, 0.015625, 0.00390625}}};*/
{{{0.077847, 0.123317, 0.077847}, {0.123317, 0.195346, 0.123317}, {0.077847, 0.123317, 0.077847}}};      //,
                                          //{{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}}};

void GaussianBlurFilter::do_filter() {
  while (true) {
    for (unsigned int i = 0; i < MASK_N; ++i) {
      //------------modify hw1--------------
      //val[i] = 0;
      val_red[i] = 0;
      val_green[i] = 0;
      val_blue[i] = 0;
    }
    for (unsigned int v = 0; v < MASK_Y; ++v) {
      for (unsigned int u = 0; u < MASK_X; ++u) {
        //unsigned char grey = (i_r.read() + i_g.read() + i_b.read()) / 3;
        unsigned char red = (i_r.read());
        unsigned char green = (i_g.read());
        unsigned char blue = (i_b.read());
        for (unsigned int i = 0; i != MASK_N; ++i) {
          //val[i] += grey * mask[i][u][v];
          val_red[i] += red * mask[i][u][v];
          val_green[i] += green * mask[i][u][v];
          val_blue[i] += blue * mask[i][u][v];


        //--------------------------------do 25 times
        }
      }
    }
    //------------modify hw1--------------
                              /*double total = 0;
                              for (unsigned int i = 0; i != MASK_N; ++i) {
                                total += val[i] * val[i];
                              }
                              int result = (int)(std::sqrt(total));
                              o_result.write(result);*/

    double total_red = 0;
    for (unsigned int i = 0; i != MASK_N; ++i) {
      total_red += val_red[i] * val_red[i];
    }
    int result_red = (int)(std::sqrt(total_red));
    o_result.write(result_red);
    wait(10 * CLOCK_PERIOD, SC_NS);
    //wait(10);

    double total_green = 0;
    for (unsigned int i = 0; i != MASK_N; ++i) {
      total_green += val_green[i] * val_green[i];
    }
    int result_green = (int)(std::sqrt(total_green));
    o_result.write(result_green);
    wait(10 * CLOCK_PERIOD, SC_NS);
    //wait(10);

    double total_blue = 0;
    for (unsigned int i = 0; i != MASK_N; ++i) {
      total_blue += val_blue[i] * val_blue[i];
    }
    int result_blue = (int)(std::sqrt(total_blue));
    o_result.write(result_blue);
    wait(10 * CLOCK_PERIOD, SC_NS);
    
    //----------------------------------//do 1 times
    //wait(10); //emulate module delay*/
  }
}

void GaussianBlurFilter::blocking_transport(tlm::tlm_generic_payload &payload,
                                     sc_core::sc_time &delay) {
  sc_dt::uint64 addr = payload.get_address();
  addr = addr - base_offset;
  unsigned char *mask_ptr = payload.get_byte_enable_ptr();
  unsigned char *data_ptr = payload.get_data_ptr();
  word buffer;
  switch (payload.get_command()) {
  case tlm::TLM_READ_COMMAND:
    switch (addr) {
    case GAUSSIANBLUR_FILTER_RESULT_ADDR:
      buffer.uint = o_result.read();
      break;
    case GAUSSIANBLUR_FILTER_CHECK_ADDR:
      buffer.uint = o_result.num_available();
      break;
    default:
      std::cerr << "Error! GaussianBlurFilter::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
      break;
    }
    data_ptr[0] = buffer.uc[0];
    data_ptr[1] = buffer.uc[1];
    data_ptr[2] = buffer.uc[2];
    data_ptr[3] = buffer.uc[3];
    break;

  case tlm::TLM_WRITE_COMMAND:
    switch (addr) {
    case GAUSSIANBLUR_FILTER_R_ADDR:
      if (mask_ptr[0] == 0xff) {
        i_r.write(data_ptr[0]);
      }
      if (mask_ptr[1] == 0xff) {
        i_g.write(data_ptr[1]);
      }
      if (mask_ptr[2] == 0xff) {
        i_b.write(data_ptr[2]);
      }
      break;
    default:
      std::cerr << "Error! GaussianBlurFilter::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
      break;
    }
    break;

  case tlm::TLM_IGNORE_COMMAND:
    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    return;
  default:
    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    return;
  }
  payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
}
