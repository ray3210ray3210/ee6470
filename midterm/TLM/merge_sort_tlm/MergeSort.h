#ifndef MERGE_SORT_H_
#define MERGE_SORT_H_
#include <systemc>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

class MergeSort : public sc_module {
public:
  tlm_utils::simple_target_socket<MergeSort> t_skt;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<int> o_result;

  SC_HAS_PROCESS(MergeSort);
  MergeSort(sc_module_name n);
  ~MergeSort();

private:
  void do_merge();
  unsigned int sort_array[SIZE];
  unsigned int tmp_array[SIZE];


  unsigned int base_offset;
  void blocking_transport(tlm::tlm_generic_payload &payload,
                          sc_core::sc_time &delay);
};
#endif
