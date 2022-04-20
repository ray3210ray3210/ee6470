#ifndef GAUSSIANBLUR_FILTER_H_
#define GAUSSIANBLUR_FILTER_H_
#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "filter_def.h"

class GaussianBlurFilter: public sc_module
{
public:
	sc_in_clk i_clk;
	sc_in < bool >  i_rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<24> >::in i_rgb;
	cynw_p2p< sc_dt::sc_uint<32> >::out o_result;
#else
	sc_fifo_in< sc_dt::sc_uint<24> > i_rgb;
	sc_fifo_out< sc_dt::sc_uint<32> > o_result;
#endif

	SC_HAS_PROCESS( GaussianBlurFilter );
	GaussianBlurFilter( sc_module_name n );
	~GaussianBlurFilter();
private:
	void do_filter();	// it is a thread define in GaussianBlurFilter(constructor).
  	int val_red;
	int val_green;
	int val_blue;
};
#endif
