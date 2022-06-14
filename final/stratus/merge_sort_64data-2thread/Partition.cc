#include "Partition.h"
// The thread function for the design
void Partition::data_partition()
{
	// Reset the interfaces
	// Also initialize any variables that are part of the module class here
	{
		HLS_DEFINE_PROTOCOL( "reset" );
		i_data.reset();
		o_data_lv0_p0.reset();
		o_data_lv0_p1.reset();
		wait();
	}

	while( true )
	{
		// get the sorted value in array
		for(int n=0 ; n<DATA_NUM ; n++)
        {
            HLS_DEFINE_PROTOCOL("input");
			input_t din = i_data.get();
            if( n < (DATA_NUM/2) )
			    p0_array[n] = din;
            else
                p1_array[n-(DATA_NUM/2)] = din;
            wait();
		}
        
		
        // output the result
		for(int n=0 ; n<(DATA_NUM/2) ; n++)
        {
            HLS_DEFINE_PROTOCOL("output");
			output_t din_p0 = p0_array[n];
			wait(2);
			output_t din_p1 = p1_array[n];
			o_data_lv0_p0.put( din_p0 );
			o_data_lv0_p1.put( din_p1 );
            wait();
		}
		wait();
	}
}
