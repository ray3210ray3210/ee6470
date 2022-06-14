#include "MergeLV1_p2.h"
#include "loop_directives.h"
// The thread function for the design
void MergeLV1_p2::merge_lv1_p2()
{
	// Reset the interfaces
	// Also initialize any variables that are part of the module class here
	{
		HLS_DEFINE_PROTOCOL( "reset" );
		i_data_lv0_p2.reset();
		o_data_lv1_p2.reset();
		wait();
	}

	while( true )
	{
    #if defined (MP)
        HLS_PIPELINE_LOOP(SOFT_STALL, MP, "main_loop_pipline" );
    #endif
		// get the sorted value in array
		for(int n=0 ; n<DATA_NUM_L1 ; n++)
        {
       #if defined(MEM)
            //HLS_CONSTRAIN_LATENCY( 0, 1, "input_lat" );
        #endif
            HLS_DEFINE_PROTOCOL("input");
			input_t in_val = i_data_lv0_p2.get();
			sort_array[n] = in_val;
            wait();
		}
        
		//sort the array
        //******************************************
        int low = SORT_START_POSITION;
        int rght, wid, rend;
        int i,j,m,t;
        for (int k=1; k < DATA_NUM_L1; k *= 2 ) {       
            for (int left=low; left+k < DATA_NUM_L1; left += k*2 ) {
                rght = left + k;        
                rend = rght + k;
                if (rend > DATA_NUM_L1) rend = DATA_NUM_L1; 
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

        // printf("-----LV1p2-----\n");
        // output the result
		for(int n=0 ; n<DATA_NUM_L1 ; n++)
        {
            HLS_DEFINE_PROTOCOL("output");
			output_t out_val = sort_array[n];
			o_data_lv1_p2.put( out_val );
            // unsigned int data = out_val;
            // printf("%u ", data);
            wait();
		}
        // printf("\n----------\n");
		wait();
	}
}
