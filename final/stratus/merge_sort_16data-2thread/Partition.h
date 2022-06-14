#ifndef PARTITION_H
#define PARTITION_H

#include <cynw_p2p.h>
#include "defines.h"

SC_MODULE( Partition )
{
	// Declare the clock and reset ports
	sc_in< bool >	clk;
	sc_in< bool >	rst;

	// Declare the input port and the output port.
	// The template specializations <DT_*> configure the
	// modular interfaces to carry the desired datatypes.
	cynw_p2p < input_t >::in	i_data;	// The input port
	cynw_p2p < output_t >::out	o_data_lv0_p0;	// The output port
	cynw_p2p < output_t >::out	o_data_lv0_p1;	// The output port



	SC_CTOR( Partition )
	: clk( "clk" )
	, rst( "rst" )
	, i_data( "i_data" )
	, o_data_lv0_p0( "o_data_lv0_p0" )
	, o_data_lv0_p1( "o_data_lv0_p1" )
	{
#if defined(FLAT)
		HLS_FLATTEN_ARRAY(p0_array);
		HLS_FLATTEN_ARRAY(p1_array);
#endif
#if defined(MEM)
    	HLS_MAP_TO_MEMORY( p0_array );
    	HLS_MAP_TO_MEMORY( p1_array );
#endif
#if defined(REG)
    	HLS_MAP_TO_REG_BANK( p0_array );
    	HLS_MAP_TO_REG_BANK( p1_array );

#endif
		SC_CTHREAD( data_partition, clk.pos() );
		reset_signal_is( rst, 0 );
		// Connect the P2P ports to clk and rst
		i_data.clk_rst( clk, rst );
		o_data_lv0_p0.clk_rst( clk, rst );
		o_data_lv0_p1.clk_rst( clk, rst );
	}

	// Declare the array
	sc_uint< 32 > p0_array[DATA_NUM/2];		//DATA_NUM x32
	sc_uint< 32 > p1_array[DATA_NUM];

	void data_partition();	// the thread function

};

#endif // MERGE_SORT_H
