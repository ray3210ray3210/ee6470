#include <cmath>

#include "GaussianblurFilter.h"

GaussianblurFilter::GaussianblurFilter(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
}

// GaussianblurFilter mask
const double mask[MASK_X][MASK_Y] = {{ 0.00390625, 0.015625, 0.0234375, 0.015625, 0.00390625},            //-------5x5
                                              { 0.015625  , 0.0625  , 0.09375,   0.0625,    0.015625}, 
                                              { 0.0234375,  0.09375, 0.140625, 0.09375, 0.0234375}, 
                                              { 0.015625  , 0.0625  , 0.09375,   0.0625,    0.015625}, 
                                              { 0.00390625, 0.015625, 0.0234375, 0.015625, 0.00390625}};
                                              
//{{0.077847, 0.123317, 0.077847},                          //-----3x3
//{0.123317, 0.195346, 0.123317}, 
//{0.077847, 0.123317, 0.077847}};


void GaussianblurFilter::do_filter() {
  int width = 256;
  int height = 256;
  int adjustX, adjustY, xBound, yBound;
  for (int v = 0; v < MASK_Y; ++v) { 
    for (int x = 0; x < width; ++x) {
      rowbuffer_red[x][v] = 0;
      rowbuffer_green[x][v] = 0;
      rowbuffer_blue[x][v] = 0;
    }
  }
  int count_col=0;
  while (true) {
      
  //-----------------------modify---------------------------
    for (int x = 0; x < width; ++x) {
      val_red[x] = 0;
      val_green[x] = 0;
      val_blue[x] = 0;
    }
    //----shift row----
    for (int x = 0; x < width; ++x) {
      for (int count_exchange =OFFSET_SHIFT_ROW; count_exchange > 0; --count_exchange)
      {
        rowbuffer_red[x][(count_exchange)] = rowbuffer_red[x][(count_exchange-1)];
        rowbuffer_green[x][(count_exchange)] = rowbuffer_green[x][(count_exchange-1)];
        rowbuffer_blue[x][(count_exchange)] = rowbuffer_blue[x][(count_exchange-1)];
      }
    }
    //---------------

    //----read row----
    if(count_col<height) {                   //-------------------------------------   back row no need to read
      for (int x = 0; x < width; ++x) {
        unsigned char red = i_r.read();
        unsigned char green = i_g.read();
        unsigned char blue = i_b.read();
        rowbuffer_red[x][0] = red;
        rowbuffer_green[x][0] = green;
        rowbuffer_blue[x][0] = blue;
        //if(count_col>250 && x==153 )  printf("---rowbuffe:rpixel= %d, col = %d---", red, count_col);
      }
    }
    //----------------

    //----compute----
    if(count_col>=OFFSET_SHIFT_ROW) {
      for (int x = 0; x < width; ++x) {
        adjustX = (MASK_X % 2) ? 1 : 0; // 1
        adjustY = (MASK_Y % 2) ? 1 : 0; // 1
        xBound = MASK_X / 2;            // 1
        yBound = MASK_Y / 2;            // 1

        for (int u = -xBound; u != xBound + adjustX; ++u) { //-1, 0, 1
          for (int v = -yBound; v != yBound + adjustY; ++v) {   //-1, 0, 1
            int red_reg, green_reg, blue_reg;
            if(x+u<0 || x+u>=width || (count_col>=height && count_col-v==height-1)) {//fore row not to inject 0 
              red_reg = 0; 
              green_reg = 0; 
              blue_reg = 0;
            } 
            else { 
              red_reg = rowbuffer_red[x+u][yBound+v];
              green_reg = rowbuffer_green[x+u][yBound+v];
              blue_reg = rowbuffer_blue[x+u][yBound+v];
            }
            val_red[x] += red_reg * mask[u+xBound][v+yBound];
            val_green[x] += green_reg * mask[u+xBound][v+yBound];
            val_blue[x] += blue_reg * mask[u+xBound][v+yBound];
          }
        }

      }
    }
    //---------------
    
    
    if(count_col >= OFFSET_SHIFT_ROW) {            //-------------------------------------   fore row no need to write
      for (int x = 0; x < width; ++x) {
        double total_red = 0, total_green = 0, total_blue = 0;
        total_red += val_red[x] * val_red[x];
        total_green += val_green[x] * val_green[x];
        total_blue += val_blue[x] * val_blue[x];
        int result_red = (int)(std::sqrt(total_red));
        int result_green = (int)(std::sqrt(total_green));
        int result_blue = (int)(std::sqrt(total_blue));
        o_result.write(result_red);
        o_result.write(result_green);
        o_result.write(result_blue);
        //if (count_col>250  && x ==153) printf("***pixel= %d, col = %d***", result_red, count_col);
      }
    }

    count_col++;
    

  //--------------------------------------------------------
    wait(10); //emulate module delay
  }
}
