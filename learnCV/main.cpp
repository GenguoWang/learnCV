//
//  main.cpp
//  learnCV
//
//  Created by Kingo Wang on 15/8/22.
//  Copyright (c) 2015年 Kingo Wang. All rights reserved.
//

#include <iostream>
#include <sstream>
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
std::string TmpPrefix = "/Users/kingo/Documents/Projects/learnCV/learnCV/tmp/";
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

void drawTest(Mat &img, std::string text)
{
    int fontFace = FONT_HERSHEY_PLAIN;//FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 2;
    int thickness = 2;
    
    
    int baseline=0;
    int margin = 10;
    Size textSize = getTextSize(text, fontFace,
                                fontScale, thickness, &baseline);
    baseline += thickness;
    
    // center the text
    std::cout << img.cols << " " << img.rows << std::endl;
    Point textOrg(img.cols - textSize.width-margin,
                  textSize.height+margin);
    
    // draw the box
    rectangle(img, textOrg+Point(-margin,margin),
              textOrg + Point(textSize.width+margin, -textSize.height-margin),
              Scalar(0,0,255),-1);
    
    // then put the text itself
    putText(img, text, textOrg, fontFace, fontScale,
            Scalar::all(255), thickness, CV_AA);
}
void handleCard(string cardName)
{
    string filename = DataPrefix + cardName;
    Mat org = imread(filename);
    Mat dst[5];
    Size dstSize(25, 36);
    int len = 91;
    string label = "";
    for (int i = 0; i < 5; ++i)
    {
        getRectSubPix(org, dstSize, Point2f(355 + i*len, 257), dst[i]);
        Mat gray;
        cvtColor(dst[i], gray, CV_RGB2GRAY);
        threshold(gray, gray, 80, 255, THRESH_BINARY);
        resize(gray, gray, Size(), 0.5, 0.5);
        imshow(string("out")+char('0'+i),gray);
        string outFileName = TmpPrefix + string("out") + char('0'+i) +".jpg";
        imwrite(outFileName,gray);
        int idx = getLabel(outFileName.c_str());
        if(idx >=1 && idx <= 14)
        {
            std::cout << labelName[idx-1] << std::endl;
            label += labelName[idx-1];
        }
        else{
            std::cout << "unknow" << std::endl;
        }
    }
    drawTest(org, label);
    imshow("org", org);
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
    /* do the call (1 arguments, 1 result) */
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
    lua_State *L = luaL_newstate();
    g_L = L;
    luaopen_io(L);
    luaopen_base(L);
    luaopen_table(L);
    luaopen_string(L);
    luaopen_math(L);
    luaL_openlibs(L);
    int s = luaL_dofile(L, (DataPrefix+"getLabel.lua").c_str());
    if(s){
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);  /* pop error message from the stack */
    }
    int idx = 0;
    while (true) {
        std::ostringstream ostr;
        ostr << "card" << idx << ".png";
        handleCard(ostr.str());
        int key = cv::waitKey();
        std::cout << key << std::endl;
        if(key == 63234){
            idx--;
        }
        else if(key == 63235)
        {
            idx++;
        }
        else{
            break;
        }
        if(idx < 0) idx = 0;
        if(idx > 2) idx = 2;
    }
    lua_close(L);
    return 0;
}
