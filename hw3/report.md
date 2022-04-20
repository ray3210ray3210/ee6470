# **Homework 3** 

電機所 許睿哲 110061592

## **Github repo link**
[Hw3 repo](https://github.com/ray3210ray3210/ee6470.git)


## **1.Gaussian Blur with High level synthesis of Gaussian Blur:**



#### **Optimization:**
    <!-- 我的pipeline 1cycle 與pipline 2cycle 或是pipeline 1cycle + datapath Out都是分別加在最外面的while迴圈，因為裡面的loop會自動unroll，還會同時做運算，因此電路面積變得很大，不過latency也會變得最少，throughput變最大。pipeline in for loop則是加在最裡面的迴圈，讓三個channel做運算時pipeline，不會一個作完之後，才做另一個。

![圖片載入錯誤 檔案存在github ->ee6470/hw3/image/stratus_optimization](https://github.com/ray3210ray3210/ee6470/raw/main/hw3/image/stratus_optimization.png)



## **2.Gaussian Blur with Timing annotation of Gaussian Blur module:**
    下圖右邊的是我在GaussianBlurFilter的blocking tranport function加了6ns(cycle)的delay。因為我output每個pixel輸出了3次，因為輸出還需要檢查有沒有東西輸出，他一共用了6次的read_from_socket，所以output多增加了36(6x6)個delay(43 into 79)。
    pipeline的部分因為我沒做出來，我用了兩個thread來做。不過我做了之後都會發生程式跑不完的問題，可能因為我read write的數量沒有用好，我的想法是先在GaussianBlurFilter，read完9個pixel之後，那個thread做繼續運算，另一個thread 從程式一開始 wait 9個cycle後開始讀取新資料，這樣他們的input port就不會增加，throughput還能增加。
![圖片載入錯誤 檔案存在github ->ee6470/hw3/image/tlm_delay0](https://github.com/ray3210ray3210/ee6470/raw/main/hw3/image/tlm_delay0.PNG)![圖片載入錯誤 檔案存在github ->ee6470/hw3/image/tlm_delay6](https://github.com/ray3210ray3210/ee6470/raw/main/hw3/image/tlm_delay6.PNG)
    


