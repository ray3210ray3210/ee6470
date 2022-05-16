#ifndef GAUSSIANBLUR_FILTER_H_
#define GAUSSIANBLUR_FILTER_H_
#include <systemc>
#include <cmath>
#include <iomanip>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

struct GaussianBlurFilter : public sc_module {
  tlm_utils::simple_target_socket<GaussianBlurFilter> tsock;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  // sc_fifo<int> o_result;
  sc_fifo<unsigned char> o_r;
  sc_fifo<unsigned char> o_g;
  sc_fifo<unsigned char> o_b;

  SC_HAS_PROCESS(GaussianBlurFilter);

  GaussianBlurFilter(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &GaussianBlurFilter::blocking_transport);
    SC_THREAD(do_filter);
  }

  ~GaussianBlurFilter() {
	}

  int result_r, result_g, result_b;
  int result;
  unsigned int base_offset;

  void do_filter(){
    { wait(CLOCK_PERIOD, SC_NS); }
    while (true) {
      result_r = 0;
      wait(CLOCK_PERIOD, SC_NS);
      result_g = 0;
      wait(CLOCK_PERIOD, SC_NS);
      result_b = 0;
      wait(CLOCK_PERIOD, SC_NS);
      // result = 0;
      // wait(CLOCK_PERIOD, SC_NS);

      for (unsigned int v = 0; v < MASK_Y; ++v) {
        for (unsigned int u = 0; u < MASK_X; ++u) {
          // unsigned char grey = (i_r.read() + i_g.read() + i_b.read()) / 3;
          unsigned char red = i_r.read();
          unsigned char green = i_g.read();
          unsigned char blue = i_b.read();
          wait(CLOCK_PERIOD, SC_NS);

          //val[i] += grey * mask[i][u][v];
          // result += grey * mask[0][u][v];
          // wait(CLOCK_PERIOD, SC_NS);
          result_r += red * mask[0][u][v];
          wait(CLOCK_PERIOD, SC_NS);
          result_g += green * mask[0][u][v];
          wait(CLOCK_PERIOD, SC_NS);
          result_b += blue * mask[0][u][v];
          wait(CLOCK_PERIOD, SC_NS);
        }
      }


      // double total = 0;
      // for (unsigned int i = 0; i != MASK_N; ++i) {
      // total += result * result;
      // wait(CLOCK_PERIOD, SC_NS);
      // }
      // int result_grey = static_cast<int>(std::sqrt(total));

      // cout << (int)result << endl;

      // o_result.write(result);
      o_r.write(result_r);
      o_g.write(result_g);
      o_b.write(result_b);
    }
  }

  void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay){
    wait(delay);
    // unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    // auto len = payload.get_data_length();
    tlm::tlm_command cmd = payload.get_command();
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *data_ptr = payload.get_data_ptr();

    addr -= base_offset;


    // cout << (int)data_ptr[0] << endl;
    // cout << (int)data_ptr[1] << endl;
    // cout << (int)data_ptr[2] << endl;
    word buffer;

    switch (cmd) {
      case tlm::TLM_READ_COMMAND:
        // cout << "READ" << endl;
        switch (addr) {
          case GAUSSIANBLUR_FILTER_RESULT_ADDR:
            // buffer.uint = o_result.read();
            buffer.uc[0] = o_r.read();
            buffer.uc[1] = o_g.read();
            buffer.uc[2] = o_b.read();
            buffer.uc[3] = 0;

            break;
          default:
            std::cerr << "READ Error! GaussianBlurFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
          }
        data_ptr[0] = buffer.uc[0];
        data_ptr[1] = buffer.uc[1];
        data_ptr[2] = buffer.uc[2];
        data_ptr[3] = buffer.uc[3];
        break;
      case tlm::TLM_WRITE_COMMAND:
        // cout << "WRITE" << endl;
        switch (addr) {
          case GAUSSIANBLUR_FILTER_R_ADDR:
            i_r.write(data_ptr[0]);
            i_g.write(data_ptr[1]);
            i_b.write(data_ptr[2]);
            break;
          default:
            std::cerr << "WRITE Error! GaussianBlurFilter::blocking_transport: address 0x"
                      << std::setfill('0') << std::setw(8) << std::hex << addr
                      << std::dec << " is not valid" << std::endl;
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
};
#endif
