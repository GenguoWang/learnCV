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
#include <fstream>
extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

using namespace cv;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::string;

std::string DataPrefix = "/Users/kingo/Documents/Projects/learnCV/learnCV/data/";
char labelName[] = {'2','3','4','5','6','7','8','9','T','J','Q','K','A','B'};
int getLabel(const char *name);
int getLabel(lua_State *L,const char *name);
void getPicture(Mat &src, Mat &dst, int index)
{
    Size dstSize(25, 36);
    int len = 91;
    Mat tmp;
    getRectSubPix(src, dstSize, Point2f(355 + index*len, 257), tmp);
    cvtColor(tmp, dst, CV_RGB2GRAY);
}

/*
 void test()
 {
 string pre = "C:\\Users\\Kingo\\Documents\\Workshop\\holdem\\raw\\2015-08-15 ";
 string name = "111306.png";
 Mat org = imread(pre+name);
 Mat dst;
 getPicture(org, dst, 2);
 g_mat = &dst;
 imshow("img", org);
 imshow("dst",dst);
 createTrakerInWin("img");
 waitKey(0);
 }
 
 */
void testCrop()
{
    string filename = DataPrefix + "card.png";
    Mat org = imread(filename);
    imshow("org", org);
    //cout << pre+filename << " " << org.size().height << " " << org.size().width<<" "<<label << endl;
    Mat dst[5];
    Size dstSize(25, 36);
    int len = 91;
    for (int i = 0; i < 5; ++i)
    {
        getRectSubPix(org, dstSize, Point2f(355 + i*len, 257), dst[i]);
        Mat gray;
        cvtColor(dst[i], gray, CV_RGB2GRAY);
        threshold(gray, gray, 80, 255, THRESH_BINARY);
        resize(gray, gray, Size(), 0.5, 0.5);
        imshow(string("out")+char('0'+i),gray);
        string outFileName = DataPrefix + string("out") + char('0'+i) +".jpg";
        imwrite(outFileName,gray);
        int idx = getLabel(outFileName.c_str());
        if(idx >=1 && idx <= 14)
        {
            std::cout << labelName[idx-1] << std::endl;
        }
        else{
            std::cout << "unknow" << std::endl;
        }
    }
}
lua_State *g_L;
int getLabel(const char *name){
    return getLabel(g_L,name);
}
int getLabel(lua_State *L, const char *name) {
    double z;
    /* push functions and arguments */
    lua_getglobal(L, "getLabel");  /* function to be called */
    lua_pushstring(L,name);
    /* do the call (2 arguments, 1 result) */
    if (lua_pcall(L, 1, 1, 0) != 0)
    {
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  /* pop error message from the stack */
    }
    /* retrieve result */
    z = lua_tonumber(L, -1);
    lua_pop(L, 1);  /* pop returned value */
    return z;
}

int main(int argc, const char * argv[]) {
    // insert code here...
//     std::cout << "Hello, World!\n";
//     cv::Mat mat = cv::imread(DataPrefix + "test.jpg");
//     cv::blur(mat, mat, cv::Size(5,5));
//     cv::Mat gray;
//     cv::cvtColor(mat, gray, CV_RGB2GRAY);
//     cv::imshow("test", mat);
//     cv::imshow("gray", gray);
    char buff[256];
    int error;
    lua_State *L = luaL_newstate();
    g_L = L;
    luaopen_io(L); // provides io.*
    luaopen_base(L);
    luaopen_table(L);
    luaopen_string(L);
    luaopen_math(L);
    luaL_openlibs(L);        /* opens the standard libraries */
    int s = luaL_dofile(L, (DataPrefix+"getLabel.lua").c_str());
    //luaL_dofile(<#L#>, <#fn#>)
    if(s){
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  /* pop error message from the stack */
    }
//    std::cout << getLabel(L,"wgg") << std::endl;
//    while (fgets(buff, sizeof(buff), stdin) != NULL) {
//        error = luaL_loadstring(L, buff) || lua_pcall(L, 0, 0, 0);
//        if (error) {
//            fprintf(stderr, "%s\n", lua_tostring(L, -1));
//            lua_pop(L, 1);  /* pop error message from the stack */
//        }
//    }
    testCrop();
    cv::waitKey();
    lua_close(L);
    return 0;
}
