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
		o_data_lv0_p2.reset();
		o_data_lv0_p3.reset();
		wait();
	}

	while( true )
	{
		// get the sorted value in array
		for(int n=0 ; n<DATA_NUM ; n++)
        {
            HLS_DEFINE_PROTOCOL("input");
			input_t din = i_data.get();
            if( n < DATA_NUM_L1 )
			    p0_array[n] = din;
            else if( DATA_NUM_L1<=n && n <(2*DATA_NUM_L1) )
                p1_array[n-DATA_NUM_L1] = din;
			else if( (2*DATA_NUM_L1)<=n && n <(3*DATA_NUM_L1) )
                p2_array[n-DATA_NUM_L1*2] = din;
				else
                p3_array[n-DATA_NUM_L1*3] = din;
            wait();
		}
        
		
        // output the result
		for(int n=0 ; n<DATA_NUM_L1 ; n++)
        {
            HLS_DEFINE_PROTOCOL("output");
			output_t din_p0 = p0_array[n];
			wait(2);
			output_t din_p1 = p1_array[n];
			wait(2);
			output_t din_p2 = p2_array[n];
			wait(2);
			output_t din_p3 = p3_array[n];
			o_data_lv0_p0.put( din_p0 );
			o_data_lv0_p1.put( din_p1 );
			o_data_lv0_p2.put( din_p2 );
			o_data_lv0_p3.put( din_p3 );
            wait();
		}
		wait();
	}
}
