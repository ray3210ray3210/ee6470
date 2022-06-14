#include "MergeLV2_p1.h"
#include "loop_directives.h"
// The thread function for the design
void MergeLV2_p1::merge_lv2_p1()
{
	// Reset the interfaces
	// Also initialize any variables that are part of the module class here
	{
		HLS_DEFINE_PROTOCOL( "reset" );
		i_data_lv1_p2.reset();
		i_data_lv1_p3.reset();
		o_data_lv2_p1.reset();
		wait();
	}

	while( true )
	{
		// get the sorted value in array
		for(int n=0 ; n<DATA_NUM_L1 ; n++)
        {
       #if defined(MEM)
        #endif
            HLS_DEFINE_PROTOCOL("input");
			input_t in_val_p2 = i_data_lv1_p2.get();
			input_t in_val_p3 = i_data_lv1_p3.get();
			sort_array[n] = in_val_p2;
			sort_array[n+DATA_NUM_L1] = in_val_p3;
            wait();
		}
        
		//sort the array
        //******************************************
        int low = SORT_START_POSITION;
        int rght, wid, rend;
        int i,j,m,t;
        for (int k=DATA_NUM_L1; k < DATA_NUM_L2; k *= 2 ) {       
            for (int left=low; left+k < DATA_NUM_L2; left += k*2 ) {
                rght = left + k;        
                rend = rght + k;
                if (rend > DATA_NUM_L2) rend = DATA_NUM_L2; 
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
		for(int n=0 ; n<DATA_NUM_L2 ; n++)
        {
            HLS_DEFINE_PROTOCOL("output");
			output_t out_val = sort_array[n];
			o_data_lv2_p1.put( out_val );
            wait();
		}
		wait();
	}
}
