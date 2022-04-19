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

#include "MergeSort.h"
#include "loop_directives.h"
// The thread function for the design
void MergeSort::thread1()
{
	// Reset the interfaces
	// Also initialize any variables that are part of the module class here
	{
		HLS_DEFINE_PROTOCOL( "reset" );
		din.reset();
		dout.reset();
		wait();
	}

	while( true )
	{
		// get the sorted value in array
		for(int n=0 ; n<SIZE ; n++)
        {
       #if defined(MEM)
            //HLS_CONSTRAIN_LATENCY( 0, 1, "input_lat" );
        #endif
            HLS_DEFINE_PROTOCOL("input");
			input_t in_val = din.get();
			sort_array[n] = in_val;
            wait();
		}
        
		//sort the array
        //******************************************
        int low = SORT_START_POSITION;
        int rght, wid, rend;
        int i,j,m,t;
        for (int k=1; k < SIZE; k *= 2 ) {       
            for (int left=low; left+k < SIZE; left += k*2 ) {
                rght = left + k;        
                rend = rght + k;
                if (rend > SIZE) rend = SIZE; 
                m = left; i = left; j = rght; 
                while (i < rght && j < rend) { 
                #if defined (II)
                    HLS_PIPELINE_LOOP(SOFT_STALL, II, "Loop" );
                #endif
                    FIRST_LOOP;
                    if (sort_array[i] <= sort_array[j]) {         
                        tmp_array[m] = sort_array[i]; i++;
                    } else {
                        tmp_array[m] = sort_array[j]; j++;
                    }
                    m++;
                }
                while (i < rght) { 
                #if defined (II)
                    HLS_PIPELINE_LOOP(SOFT_STALL, II, "Loop" );
                #endif
                    SECOND_LOOP;
                    tmp_array[m]=sort_array[i]; 
                    i++; m++;
                }
                while (j < rend) { 
                #if defined (II)
                    HLS_PIPELINE_LOOP(SOFT_STALL, II, "Loop" );
                #endif
                    THIRD_LOOP;
                    tmp_array[m]=sort_array[j]; 
                    j++; m++;
                }
                for (m=left; m < rend; m++) {
                #if defined (II)
                    HLS_PIPELINE_LOOP(SOFT_STALL, II, "Loop" );
                #endif           
                    FOURTH_LOOP;
                    sort_array[m] = tmp_array[m]; 

                }
            }
        }
        //******************************************

        // output the result
		for(int n=0 ; n<SIZE ; n++)
        {
            HLS_DEFINE_PROTOCOL("output");
			output_t out_val = sort_array[n];
			dout.put( out_val );
            wait();
		}
		wait();
	}
}
