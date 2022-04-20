# **Homework 2** 

電機所 許睿哲 110061592

## **Github repo link**
[Hw2 repo](https://github.com/ray3210ray3210/ee6470.git)


## **Original Image**
![圖片載入錯誤 檔案存在github ->lena_std_short.bmp](https://imgur.com/gallery/snoaErQ)
## **1.Gaussian Blur with High level synthesis of Gaussian Blur:**


#### **Result:**

5x5
![圖片載入錯誤 檔案存在github ->ee6470hw1-1_5x5.png](https://imgur.com/gallery/k37W78e)



## **1.Gaussian Blur with Timing annotation of Gaussian Blur module:**
## **Additional features**
    可以換成其他size的guassian blur filter。
    


## 心得：**
    有遇到一個問題，就是會發生資料沒有讀進Testbench的問題，最後發現是因為Testbench.cpp裡面 initiator.read_from_socket 有wait 10個clock，因為要用到3個顏色，wait在read_from_socket後也用了三次，但是GaussianBlur.cpp後面沒有讓他wait，造成了TLM他們不同步，最後讓每個o_result.write各delay 10個clock就成功了。
