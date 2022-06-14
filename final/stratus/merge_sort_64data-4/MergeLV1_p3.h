
#ifndef MERGE_LV1_P3_H
#define MERGE_LV1_EIGHT_p3_H

#include <cynw_p2p.h>
#include "defines.h"

SC_MODULE( MergeLV1_p3 )
{
	// Declare the clock and reset ports
	sc_in< bool >	clk;
	sc_in< bool >	rst;

	// Declare the input port and the output port.
	// The template specializations <DT_*> configure the
	// modular interfaces to carry the desired datatypes.
	cynw_p2p < input_t >::in	i_data_lv0_p3;	// The input port
	cynw_p2p < output_t >::out	o_data_lv1_p3;	// The output port



	SC_CTOR( MergeLV1_p3 )
	: clk( "clk" )
	, rst( "rst" )
	, i_data_lv0_p3( "i_data_lv0_p3" )
	, o_data_lv1_p3( "o_data_lv1_p3" )
	{
#if defined(FLAT)
		HLS_FLATTEN_ARRAY(sort_array);
		HLS_FLATTEN_ARRAY(tmp_array);
#endif
#if defined(MEM)
    	HLS_MAP_TO_MEMORY( sort_array );
    	HLS_MAP_TO_MEMORY( tmp_array );
#endif
#if defined(REG)
    	HLS_MAP_TO_REG_BANK( sort_array );
    	HLS_MAP_TO_REG_BANK( tmp_array );

#endif
		SC_CTHREAD( merge_lv1_p3, clk.pos() );
		reset_signal_is( rst, 0 );
		// Connect the P2P ports to clk and rst
		i_data_lv0_p3.clk_rst( clk, rst );
		o_data_lv1_p3.clk_rst( clk, rst );
	}

	// Declare the array
	sc_uint< 32 > sort_array[DATA_NUM];		//SIZE x32
	sc_uint< 32 > tmp_array[DATA_NUM];

	void merge_lv1_p3();	// the thread function

};

#endif // MERGE_SORT_H
