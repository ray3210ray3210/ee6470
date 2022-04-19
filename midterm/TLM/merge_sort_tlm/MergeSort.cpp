#include <cmath>
#include <iomanip>

#include "MergeSort.h"

MergeSort::MergeSort(sc_module_name n)
    : sc_module(n), t_skt("t_skt"), base_offset(0) {
  SC_THREAD(do_merge);

  t_skt.register_b_transport(this, &MergeSort::blocking_transport);
}

MergeSort::~MergeSort() = default;

void MergeSort::do_merge() {
  while (true) {
  //----------merge sort-------------------------------------------
    int rght, wid, rend;
    int i,j,m,t;
    for(int i_count =0; i_count!= SIZE; i_count++){      //i_count = input counter
      sort_array[i_count] = i_r.read() ;
    }
    for (int k=1; k < SIZE; k *= 2 ) {       
        for (int left=0; left+k < SIZE; left += k*2 ) {
            rght = left + k;
            rend = rght + k;
            if (rend > SIZE) rend = SIZE; 
            m = left; i = left; j = rght;
            while (i < rght && j < rend) { 
                if (sort_array[i] <= sort_array[j]) {         
                    tmp_array[m] = sort_array[i]; i++;
                } else {
                    tmp_array[m] = sort_array[j]; j++;
                }
                m++;
            }
            while (i < rght) { 
                tmp_array[m]=sort_array[i];
                i++; m++;
            }
            while (j < rend) { 
                tmp_array[m]=sort_array[j];
                j++; m++;
            }
            for (m=left; m < rend; m++) { 
                sort_array[m] = tmp_array[m]; 
            }
        }
    }

    for(int o_count =0; o_count!= SIZE; o_count++){     //o_count = output counter
        o_result.write(sort_array[o_count]);
    }
  //----------0419-------------------------------------------------

  }
}

void MergeSort::blocking_transport(tlm::tlm_generic_payload &payload,
                                     sc_core::sc_time &delay) {
  sc_dt::uint64 addr = payload.get_address();
  addr -= base_offset;
  unsigned char *mask_ptr = payload.get_byte_enable_ptr();
  unsigned char *data_ptr = payload.get_data_ptr();
  word buffer;
  switch (payload.get_command()) {
  case tlm::TLM_READ_COMMAND:
    switch (addr) {
    case MERGE_SORT_RESULT_ADDR:
      buffer.uint = o_result.read();
      break;
    case MERGE_SORT_CHECK_ADDR:
      buffer.uint = o_result.num_available();
    break;
    default:
      std::cerr << "Error! MergeSort::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
    }
    data_ptr[0] = buffer.uc[0];
    data_ptr[1] = buffer.uc[1];
    data_ptr[2] = buffer.uc[2];
    data_ptr[3] = buffer.uc[3];
    break;
  case tlm::TLM_WRITE_COMMAND:
    switch (addr) {
    case MERGE_SORT_R_ADDR:
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
      std::cerr << "Error! MergeSort::blocking_transport: address 0x"
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

  // Timing Annotation
  sc_time latency(0, SC_NS);
  delay = delay + latency;
}