///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2019 Cadence Design Systems, Inc. All rights reserved worldwide.
//
// The code contained herein is the proprietary and confidential information
// of Cadence or its licensors, and is supplied subject to a previously
// executed license and maintenance agreement between Cadence and customer.
// This code is intended for use with Cadence high-level synthesis tools and
// may not be used with other high-level synthesis tools. Permission is only
// granted to distribute the code as indicated. Cadence grants permission for
// customer to distribute a copy of this code to any partner to aid in designing
// or verifying the customer's intellectual property, as long as such
// distribution includes a restriction of no additional distributions from the
// partner, unless the partner receives permission directly from Cadence.
//
// ALL CODE FURNISHED BY CADENCE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, AND CADENCE SPECIFICALLY DISCLAIMS ANY WARRANTY OF NONINFRINGEMENT,
// FITNESS FOR A PARTICULAR PURPOSE OR MERCHANTABILITY. CADENCE SHALL NOT BE
// LIABLE FOR ANY COSTS OF PROCUREMENT OF SUBSTITUTES, LOSS OF PROFITS,
// INTERRUPTION OF BUSINESS, OR FOR ANY OTHER SPECIAL, CONSEQUENTIAL OR
// INCIDENTAL DAMAGES, HOWEVER CAUSED, WHETHER FOR BREACH OF WARRANTY,
// CONTRACT, TORT, NEGLIGENCE, STRICT LIABILITY OR OTHERWISE.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MERGE_SORT_H
#define MERGE_SORT_H

#include <cynw_p2p.h>
#include "defines.h"

SC_MODULE( MergeSort )
{
	// Declare the clock and reset ports
	sc_in< bool >	clk;
	sc_in< bool >	rst;

	// Declare the input port and the output port.
	// The template specializations <DT_*> configure the
	// modular interfaces to carry the desired datatypes.
	cynw_p2p < input_t >::in	din;	// The input port
	cynw_p2p < output_t >::out	dout;	// The output port



	SC_CTOR( MergeSort )
	: clk( "clk" )
	, rst( "rst" )
	, din( "din" )
	, dout( "dout" )
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
		SC_CTHREAD( thread1, clk.pos() );
		reset_signal_is( rst, 0 );
		// Connect the P2P ports to clk and rst
		din.clk_rst( clk, rst );
		dout.clk_rst( clk, rst );
	}

	// Declare the array
	sc_uint< 32 > sort_array[SIZE];		//SIZE x32
	sc_uint< 32 > tmp_array[SIZE];

	void thread1();	// the thread function

};

#endif // MERGE_SORT_H
