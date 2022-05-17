# **Homework 4** 

電機所 許睿哲 110061592

## **Github repo link**
[Hw4 repo](https://github.com/ray3210ray3210/ee6470.git)

## **Original Image**
![圖片載入錯誤 檔案存在github ->ee6470/hw04/image/lena_std_short.bmp](https://github.com/ray3210ray3210/ee6470/raw/main/hw04/image/lena_std_short.bmp)

#### **Result:**

5x5
![圖片載入錯誤 檔案存在github ->ee6470/hw04/image/lena_std_out.bmp](https://github.com/ray3210ray3210/ee6470/raw/main/hw04/image/lena_std_out.bmp)

## **Cross-compile Gaussian Blur to RISC-V VP platform:**

這次因為需要更改的地方跟之前TLM module是一樣的，所以做的很快，將gaussianblur.h輸出的FIFOchannel改成3個，各自輸出RGB不同的值； `blocking_transport()` 內的buffer則放入對應的RGB值。SoftWare的部分則是從 `read_data_from_ACC()` 取出對應的RGB做成圖片。
下面是我跑不同傳送資料的方式，可以看到因為DMA不用透過CPU來傳送資料，不需要像memcpy經過cache.cpu，可以直接讓memory的資料傳到gaussiblurfilter裡面。


| way    | execution cycles | instructions |
| :----: | :----:           | :----:       |
| memcpy |   344714029      |86640407      |
| DMA    |   322431711      |74188552       |



    


