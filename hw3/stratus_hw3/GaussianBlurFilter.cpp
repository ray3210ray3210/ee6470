#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

#include "directives.h"
#include "GaussianBlurFilter.h"

GaussianBlurFilter::GaussianBlurFilter( sc_module_name n ): sc_module( n )
{
#ifndef NATIVE_SYSTEMC
//	HLS_FLATTEN_ARRAY(val);
#endif
	SC_THREAD( do_filter );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
#ifndef NATIVE_SYSTEMC
	i_rgb.clk_rst(i_clk, i_rst);
  o_result.clk_rst(i_clk, i_rst);
#endif
}

GaussianBlurFilter::~GaussianBlurFilter() {}

// gaussianblur mask
const int mask[MASK_X][MASK_Y] = {{77847, 123317, 77847}, {123317, 195346, 123317}, {77847, 123317, 77847}};//{{7784,12331,7784},{0,1,0},{0,0,0}};//{{0.077847, 0.123317, 0.077847}, {0.123317, 0.195346, 0.123317}, {0.077847, 0.123317, 0.077847}};

void GaussianBlurFilter::do_filter() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_rgb.reset();
		o_result.reset();
#endif
		wait();
	}
	while (true) {
		//A HLS_PIPELINE_LOOP directive that is enabled when II is set.
		#if defined (II)
      	HLS_PIPELINE_LOOP(SOFT_STALL, II, "Loop" );
    	#endif

			//HLS_CONSTRAIN_LATENCY(0, 1, "lat00");
		val_red = 0;
		val_green = 0;
		val_blue =0;
		for (unsigned int v = 0; v<MASK_Y; ++v) {
			for (unsigned int u = 0; u<MASK_X; ++u) {
				
				// LAB: Macro is used to identify this location as a potential
        		// place for directives to go.
        		MASK_LOOP;

				sc_dt::sc_uint<24> rgb;
#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input");
					rgb = i_rgb.get();
					wait();
				}
#else
				rgb = i_rgb.read();
#endif
				unsigned char red = rgb.range(7,0);// + rgb.range(15,8) + rgb.range(23, 16))/3;
				unsigned char green = rgb.range(15,8);
				unsigned char blue = rgb.range(23, 16);
					//HLS_CONSTRAIN_LATENCY(0, 1, "lat01");
				val_red += red * mask[u][v];
				val_green += green * mask[u][v];
				val_blue += blue * mask[u][v];
				
			}
		}
		int total_red = 0;
		int total_blue = 0;
		int total_green = 0;

			//HLS_CONSTRAIN_LATENCY(0, 1, "lat01");
			total_red = val_red >>20; //1000000;// * val_red;
			total_green = val_green >>20; ///1000000;		2^20=1048576
			total_blue = val_blue >>20; ///1000000;

		sc_dt::sc_uint<24> result;
		result.range(7,0)=total_red;
		result.range(15,8)=total_green;
		result.range(23,16)=total_blue;
#ifndef NATIVE_SYSTEMC
		{
			HLS_DEFINE_PROTOCOL("output");
			o_result.put(result);
			wait();
		}
#else
		o_result.write(result);
#endif
	}
}