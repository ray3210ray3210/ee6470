#ifndef _SYSTEM_H_
#define _SYSTEM_H_

// Include some required files.
#include <systemc.h>            // SystemC definitions.
#include <esc.h>                // Cadence ESC functions and utilities.
#include <stratus_hls.h>        // Cadence Stratus definitions.
#include <cynw_p2p.h>           // The cynw_p2p communication channel.
#include "defines.h"
#include "tb.h"

#include "Partition_wrap.h"		// use the generated wrapper for all hls_modules
#include "MergeLV1_p0_wrap.h"
#include "MergeLV1_p1_wrap.h"
#include "MergeLV2_p0_wrap.h"

SC_MODULE( System )
{
	// clock and reset signals
	sc_clock clk_sig;
	sc_signal< bool > rst_sig;

	// cynw_p2p channels
	cynw_p2p< input_t >	data_in;	// For data going into the design
	cynw_p2p< data_t > data_lv0_p0;
	cynw_p2p< data_t > data_lv0_p1;
	cynw_p2p< data_t > data_lv1_p0;
	cynw_p2p< data_t > data_lv1_p1;
	cynw_p2p< output_t> data_out;	// For data coming out of the design
	//cynw_p2p< data_t > data_lv2_p0;
//0528
	// The testbench and MergeSort modules.
	Partition_wrapper *m_Partition;
	MergeLV1_p0_wrapper	*m_Mergelv1_p0;		// use the generated wrapper for the MergeSort hls module
	MergeLV1_p1_wrapper	*m_Mergelv1_p1;
	MergeLV2_p0_wrapper	*m_Mergelv2_p0;
	tb		*m_tb;

	SC_CTOR( System )
	: clk_sig( "clk_sig", CLOCK_PERIOD, SC_NS )
	, rst_sig( "rst_sig" )
	, data_in( "data_in" )
	, data_lv0_p0( "data_lv0_p0" )
	, data_lv0_p1( "data_lv0_p1" )
	, data_lv1_p0( "data_lv1_p0" )
	, data_lv1_p1( "data_lv1_p1" )
	, data_out( "data_out" )
	{
		// Connect the design module
		m_Partition = new Partition_wrapper( "Partition_wrapper" );
		m_Partition->clk( clk_sig );
		m_Partition->rst( rst_sig );
		m_Partition->i_data( data_in );
		m_Partition->o_data_lv0_p0( data_lv0_p0 );
		m_Partition->o_data_lv0_p1( data_lv0_p1 );

		m_Mergelv1_p0 = new MergeLV1_p0_wrapper( "MergeLV1_p0_wrapper" );
		m_Mergelv1_p0->clk( clk_sig );
		m_Mergelv1_p0->rst( rst_sig );
		m_Mergelv1_p0->i_data_lv0_p0( data_lv0_p0 );
		m_Mergelv1_p0->o_data_lv1_p0( data_lv1_p0 );

		m_Mergelv1_p1 = new MergeLV1_p1_wrapper( "MergeLV1_p1_wrapper" );
		m_Mergelv1_p1->clk( clk_sig );
		m_Mergelv1_p1->rst( rst_sig );
		m_Mergelv1_p1->i_data_lv0_p1( data_lv0_p1 );
		m_Mergelv1_p1->o_data_lv1_p1( data_lv1_p1 );

		m_Mergelv2_p0 = new MergeLV2_p0_wrapper( "MergeLV2_p0_wrapper" );
		m_Mergelv2_p0->clk( clk_sig );
		m_Mergelv2_p0->rst( rst_sig );
		m_Mergelv2_p0->i_data_lv1_p0( data_lv1_p0 );
		m_Mergelv2_p0->i_data_lv1_p1( data_lv1_p1 );
		m_Mergelv2_p0->o_data_lv2_p0( data_out );


		// Connect the testbench
		m_tb = new tb( "tb" );
		m_tb->clk( clk_sig );
		m_tb->rst( rst_sig );
		m_tb->dout( data_in );
		m_tb->din( data_out );
	}

	~System()
	{
		delete m_tb;
		delete m_Partition;
		delete m_Mergelv1_p0;
		delete m_Mergelv1_p1;
		delete m_Mergelv2_p0;
	}
};

#endif // _SYSTEM_H_
