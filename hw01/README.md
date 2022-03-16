execution:
  1. $ mkdir build
  2. $ cd build
  3. $ cmake ..
  4. $ make
  5. $ make run


change filter:

  1.open filter_def.cpp and change filter_size (MASK_X and MASK_Y)
  2.open GaussianblurFilter.cpp and change the mask[MASK_Y][MASK_Y]
   (i have written the 5x5 and 3x3 gaussianblur's kernel ,and comment one of them.you can replace your filter size can be bigger. )
