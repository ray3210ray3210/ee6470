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

#include "tb.h"
#include <esc.h>		// for the latency logging functions
#include <string>
#include <iostream>

using namespace std;

// The source thread reads data from a file and sends it to the MergeSort
void tb::source()
{
	dout.reset();		// reset the outputs and cycle the design's reset
	rst.write( 0 );		// assert reset (active low)
	wait( 2 );		// wait 2 cycles
	rst.write( 1 );		// deassert reset
	wait();

	string stim_file = "./stimulus_dir/stimulus0.dat";
	char count_f_char[32];

	for( int count_f = 0 ,fail = 1 ;count_f<FILE_NUM ; count_f++ ){

			//update stimulus file name
		sprintf(count_f_char, "%d", count_f);
		stim_file.erase(23, 1);							//offset 23 to modify filename
		stim_file.insert(23,count_f_char);
			// Open the stimulus file
	    char const *stim_file_pointer = stim_file.c_str();
		infp = fopen( stim_file_pointer, "r" );
		if( infp == NULL )
		{
			if(fail == 1){
				cout << "Couldn't open stimulus.dat for reading.5206" << endl;
				cout << stim_file << endl;

				exit(0);
			}
		}
		else{
			fail = 0;
		}
		// Read stimulus values from file and send to MergeSort
		for( int i = 0; i < DATA_NUM; i++ )
		{
			int value;
			fscanf(infp, "%u\n", &value);	
			dout.put( (input_t)value );			// send the stimulus value
			if(i==0)
			pre_sample_time = sc_time_stamp();
		}
		fclose( infp );
	}
		// Guard condition: after 100000 cycles the sink() thread should have ended the simulation.
		// If we're still here, timeout and print error message.
		wait( 200000 );
			//fclose( infp );
		cout << "Error! TB source thread timed out!" << endl;
		esc_stop();
}

// The sink thread reads all the expected values from the design
void tb::sink()
{
	din.reset();
	wait();     // to synchronize with reset

	unsigned long total_latency = 0;

    char output_file_char[256];

	// write to file in sequence response0, reponse1, response2...
	char count_f_char[32];
	char const *output_file_pointer;
	sprintf( output_file_char, "%s/response0.dat", getenv("BDW_SIM_CONFIG_DIR") );
	outfp = fopen(output_file_char, "wb");

	int file_name_offset = strlen(output_file_char)-5;

	for( int count_f = 0 ;count_f < FILE_NUM ; count_f++ ){

		unsigned long sample_latency = 0;								//latency for sorting this file

		sprintf(count_f_char, "%d", count_f);
		string output_file(output_file_char);
		output_file.erase(file_name_offset, 1);								// offset 23 to modify filename
		output_file.insert(file_name_offset,count_f_char);
			// Open the stimulus file
	    output_file_pointer = output_file.c_str();
		//cout <<  "******file name is " <<output_file_char << "******" << endl;
		outfp = fopen(output_file_pointer, "wb");
		//cout << "------------------open the file ------------------." << output_file_char << endl;

		if (outfp == NULL)
		{
			cout << "Couldn't open output.dat for writing." << endl;
			exit(0);
		}
	// Read outputs from MergeSort
		for( int i = 0; i < DATA_NUM; i++ )
		{
			output_t inVal = din.get();

			fprintf( outfp, "%u\n", (int)inVal );	// write value to response file

			// Calculate latency for this particular output

			unsigned long latency = clock_cycle( sc_time_stamp() - pre_sample_time );
			pre_sample_time = sc_time_stamp();


			//cout << "Latency for sample " << i << " is " <<  latency << endl;
			//cout << "NOW time is " << sc_time_stamp() << "." << endl;

			// Keep running total of all latency cycles for later
			total_latency += latency;
			sample_latency += latency;
		}
			// Close the results file
			cout << "latency for sorting this file " << count_f << " is " << sample_latency << endl;
			//cout << "------------------close the file ------------------." << endl;

			fclose( outfp );
	}
	// Calculate, log and print average latency
	unsigned long average_latency = total_latency / FILE_NUM;
	esc_log_latency( "MergeSort", average_latency, "average_latency" );
	cout << "Average latency " << average_latency << "." << endl;

	// End the simulation
	esc_stop();
}


////
// Convert a time in simulation time units to clock cycles
////
int tb::clock_cycle( sc_time time )
{
    sc_clock * clk_p = DCAST < sc_clock * >( clk.get_interface() );
    sc_time clock_period = clk_p->period(); // get period from the sc_clock object.
    return ( int )( time / clock_period );

}