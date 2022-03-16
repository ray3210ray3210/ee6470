execution:
  1. download the folder(gaussianblur_fifo or data_movement)
  2. go into the folder
  3. $ mkdir build
  4. $ cd build
  5. $ cmake ..
  6. $ make
  7. $ make run


change filter:

  1.open filter_def.cpp and change filter_size (MASK_X and MASK_Y)
  2.open GaussianblurFilter.cpp and change the mask[MASK_Y][MASK_Y]
   (i have written the 5x5 and 3x3 gaussianblur's kernel ,and comment one of them.you can replace your filter size can be bigger. )
