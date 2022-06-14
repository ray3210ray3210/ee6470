#ifndef MERGE_LV2_P0_H
#define MERGE_LV2_p0_H
#include <systemc>
#include <cmath>
#include <iomanip>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "defines.h"

struct MergeLV2_p0 : public sc_module {
  tlm_utils::simple_target_socket<MergeLV2_p0> tsock;

    //---FIFO IN---
  sc_fifo<unsigned int> i_data_lv0_p0;
  sc_fifo<unsigned int> i_data_lv0_p1;
//   sc_fifo<unsigned char> i_data_lv0_p0_size0;//fifo channel 4x8 bits = 32bits
//   sc_fifo<unsigned char> i_data_lv0_p0_size1;
//   sc_fifo<unsigned char> i_data_lv0_p0_size2;
//   sc_fifo<unsigned char> i_data_lv0_p0_size3;

//     //---FIFO OUT---
  sc_fifo<unsigned int> o_data_lv2_p0;
//   sc_fifo<unsigned char> o_data_lv2_p0_size0;
//   sc_fifo<unsigned char> o_data_lv2_p0_size1;
//   sc_fifo<unsigned char> o_data_lv2_p0_size2;
//   sc_fifo<unsigned char> o_data_lv2_p0_size3;

  SC_HAS_PROCESS(MergeLV2_p0);

  MergeLV2_p0(sc_module_name n): 
    sc_module(n), 
    tsock("t_skt"), 
    base_offset(0) 
  {
    tsock.register_b_transport(this, &MergeLV2_p0::blocking_transport);
    SC_THREAD(merge_lv2_p0);
  }

  ~MergeLV2_p0() {
	}

	unsigned int sort_array[DATA_NUM];		//SIZE x32
	unsigned int tmp_array[DATA_NUM];
    unsigned int base_offset;
////===============================================================
// The thread function for the design
    void merge_lv2_p0()
    {
        { wait(CLOCK_PERIOD, SC_NS); }
        while( true )
        {
            // get the sorted value in array
            for(int n=0 ; n<DATA_NUM ; n++)
            {
                unsigned int in_val = i_data_lv0_p0.read();
                sort_array[n] = in_val;
                printf("input data =%u\n", in_val);
                printf("==============\n");

                wait(CLOCK_PERIOD, SC_NS);

            }
            //sort the array
            //******************************************
            int low = SORT_START_POSITION;
            int rght, wid, rend;
            int i,j,m,t;
            for (int k=DATA_NUM_L1; k < DATA_NUM_L2; k *= 2 ) {       
                for (int left=low; left+k < DATA_NUM_L2; left += k*2 ) {
                    rght = left + k;        
                    rend = rght + k;
                    if (rend > DATA_NUM_L2) rend = DATA_NUM_L2; 
                    m = left; i = left; j = rght; 
                    wait(CLOCK_PERIOD, SC_NS);
                    while (i < rght && j < rend) { 
                        if (sort_array[i] <= sort_array[j]) {         
                            tmp_array[m] = sort_array[i]; i++;
                             wait(CLOCK_PERIOD, SC_NS);
                        } else {
                            tmp_array[m] = sort_array[j]; j++;
                            wait(CLOCK_PERIOD, SC_NS);
                        }
                        m++;
                    }
                    while (i < rght) { 
                        tmp_array[m]=sort_array[i]; 
                        i++; m++;
                        wait(CLOCK_PERIOD, SC_NS);

                    }
                    while (j < rend) { 
                        tmp_array[m]=sort_array[j]; 
                        j++; m++;
                        wait(CLOCK_PERIOD, SC_NS);
                    }
                    for (m=left; m < rend; m++) {
                        sort_array[m] = tmp_array[m]; 
                        wait(CLOCK_PERIOD, SC_NS);
                    }
                }
            }
            //******************************************

            // output the result
            for(int n=0 ; n<DATA_NUM_L2 ; n++)
            {
                unsigned int out_val = sort_array[n];
                o_data_lv2_p0.write( out_val );
                printf("out data =%u\n", out_val);
                printf("==============\n******************************************\n");
                
                wait(CLOCK_PERIOD, SC_NS);
            }
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
    unsigned int data_b;
    switch (cmd) {
      case tlm::TLM_READ_COMMAND:
        // cout << "READ" << endl;
        switch (addr) {
          case MERGE_SORT_RESULT_ADDR:
            buffer.uint = o_data_lv2_p0.read();

            break;
          default:
            std::cerr << "READ Error! MergeSort::blocking_transport: address 0x"
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
          case MERGE_SORT_RS_R_ADDR:
            data_b = fourCharToInt(data_ptr[3], data_ptr[2], data_ptr[1],data_ptr[0] );
            i_data_lv0_p0.write(data_b);

            printf("TLM 0  =%u\n", data_ptr[0]);
            printf("TLM 1 =%u\n", data_ptr[1]);
            printf("TLM 2 =%u\n", data_ptr[2]);
            printf("TLM 3 =%u\n", data_ptr[3]);


            break;
          default:
            std::cerr << "WRITE Error! MergeSort::blocking_transport: address 0x"
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