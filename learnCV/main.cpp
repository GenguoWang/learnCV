//
//  main.cpp
//  learnCV
//
//  Created by Kingo Wang on 15/8/22.
//  Copyright (c) 2015年 Kingo Wang. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
std::string DataPrefix = "/Users/kingo/Documents/Projects/learnCV/learnCV/data/";
int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    cv::Mat mat = cv::imread(DataPrefix + "test.jpg");
    cv::imshow("test", mat);
    cv::waitKey();
    return 0;
}
