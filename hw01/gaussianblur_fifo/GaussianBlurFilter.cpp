#include <cmath>

#include "GaussianBlurFilter.h"

GaussianBlurFilter::GaussianBlurFilter(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
}

// gaussian blur mask
const double mask[MASK_N][MASK_X][MASK_Y] = //{{{0,0,0,0,0}, {0,0,0,0,0}, {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0}}};
                                            /*{{{ 0.00390625, 0.015625, 0.0234375, 0.015625, 0.00390625},
                                              { 0.015625  , 0.0625  , 0.09375,   0.0625,    0.015625}, 
                                              { 0.0234375,  0.09375, 0.140625, 0.09375, 0.0234375}, 
                                              { 0.015625  , 0.0625  , 0.09375,   0.0625,    0.015625}, 
                                              { 0.00390625, 0.015625, 0.0234375, 0.015625, 0.00390625}}};*/
{{{0.077847, 0.123317, 0.077847}, {0.123317, 0.195346, 0.123317}, {0.077847, 0.123317, 0.077847}}};      //,
                                          //{{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}}};

void GaussianBlurFilter::do_filter() {
  while (true) {
    for (unsigned int i = 0; i < MASK_N; ++i) {
      val[i] = 0;
      //------------modify--------------
      val_red[i] = 0;
      val_green[i] = 0;
      val_blue[i] = 0;
      //----------------------------------
    }
    for (unsigned int v = 0; v < MASK_Y; ++v) {
      for (unsigned int u = 0; u < MASK_X; ++u) {
        //------------modify--------------
        //unsigned char grey = (i_r.read() + i_g.read() + i_b.read()) / 3;
        unsigned char red = (i_r.read());
        unsigned char green = (i_g.read());
        unsigned char blue = (i_b.read());


        //--------------------------------
        for (unsigned int i = 0; i != MASK_N; ++i) {
        //------------modify--------------
        //val[i] += grey * mask[i][u][v];
        val_red[i] += red * mask[i][u][v];
        val_green[i] += green * mask[i][u][v];
        val_blue[i] += blue * mask[i][u][v];

        /*wait(121576);
        wait(121576);
        wait(121576);
        wait(121576);
        wait(121576);
        wait(121576);*/

        //--------------------------------do 25 times
        }
      }
    }
    //------------modify--------------
                              /*double total = 0;
                              for (unsigned int i = 0; i != MASK_N; ++i) {
                                total += val[i] * val[i];
                              }
                              int result = (int)(std::sqrt(total));
                              o_result.write(result);*/

    double total_red = 0;
    for (unsigned int i = 0; i != MASK_N; ++i) {
      total_red += val_red[i] * val_red[i];
    }
    int result_red = (int)(std::sqrt(total_red));
    o_result.write(result_red);
    //wait(10);

    double total_green = 0;
    for (unsigned int i = 0; i != MASK_N; ++i) {
      total_green += val_green[i] * val_green[i];
    }
    int result_green = (int)(std::sqrt(total_green));
    o_result.write(result_green);

    double total_blue = 0;
    for (unsigned int i = 0; i != MASK_N; ++i) {
      total_blue += val_blue[i] * val_blue[i];
    }
    int result_blue = (int)(std::sqrt(total_blue));
    o_result.write(result_blue);
    
    //----------------------------------//do 1 times
    wait(10); //emulate module delay
  }
}
