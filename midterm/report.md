# **Midterm project - Merge Sort** 

電機所 許睿哲 110061592

## **Github repo link**
[Midterm repo](https://github.com/ray3210ray3210/ee6470.git)
## **Merge sort link**
![圖片載入錯誤 檔案存在github :ee6470/midterm/image/tlm_time_delay6 ](https://github.com/ray3210ray3210/ee6470/raw/main/midterm/image/mergesort.png)

## **1. TLM Bus:**
#### **design:**
    input "stmulus.txt", output a sorting "response.txt"
    在檔案裏面有16個數字，數字範圍為[0,255]的整數。

#### **Result:**
![圖片載入錯誤 檔案存在github :ee6470/midterm/image/tlm_result ](https://github.com/ray3210ray3210/ee6470/raw/main/midterm/image/tlm_result.PNG)
#### **Time annotation**
    我在MergeSort.cpp中的blocking_transport function裡面加了6ns的latency，結果如下圖所示，左圖latency=0. 右圖latency=6。
    可以看到input 增加了6ns，output則增加了12ns。這是因為testbench輸入資料時，只用了一次write_to_socket；而從MergeSort輸出資料到testbench時，因為要確認資料是不是可以傳了，因此要多傳一次valid bit。 兩次模擬起始都為4ns，計算過程也都為1ns，
    因此計算時間相差(565-277) = 6x16 + 12x16

![圖片載入錯誤 檔案存在github :ee6470/midterm/image/tlm_time_delay0 ](https://github.com/ray3210ray3210/ee6470/raw/main/midterm/image/tlm_time_delay0.PNG)![圖片載入錯誤 檔案存在github :ee6470/midterm/image/tlm_time_delay6 ](https://github.com/ray3210ray3210/ee6470/raw/main/midterm/image/tlm_time_delay6.PNG)

## **2. Status HLS:**
#### **design:**
    Testbench會讀取"stimulus0.dat"到"stimulus7.dat"，進行Merge sort後，輸出"response0.dat"到"response7.dat"。其中每個檔案有16個數值，每個數字大小為[0,2^32-1]的unsigned bit。
![圖片載入錯誤 檔案存在github :ee6470/midterm/image/stratus_design ](https://github.com/ray3210ray3210/ee6470/raw/main/midterm/image/stratus_design.png)


#### **Result:**
![圖片載入錯誤 檔案存在github :ee6470/midterm/image/stratus_result ](https://github.com/ray3210ray3210/ee6470/raw/main/midterm/image/stratus_result.png)

#### **Optimization:**
    由下圖可以看出經過unroll後+flatten的design throughput與latency最好，雖然經過datapath optimization後面積有變小一點點。
    但是考慮到電路面積，還是Pipeline 1 cycle + flatten + datapth optimization在這個case下，感覺比較好。
![圖片載入錯誤 檔案存在github :ee6470/midterm/image/stratus_optimization1 ](https://github.com/ray3210ray3210/ee6470/raw/main/midterm/image/stratus_optimization2.png)


## 心得：**
    這次發現stratus不能用recursion，如果有data dependency也必須unroll或是flatten，不然無法合成，在design裡面也不能有指標，而且因為我這次做出來的電路也無法設計pipeline，可能因為dependency的原因，它都會跑不出來。所以我的pipeline是加在他最裡面的迴圈，也就是他在subarray比大小，及比大小後要將剩餘的array依序存進tmp_array的時候，以及最後從tmp_array放回sort_aaray。
