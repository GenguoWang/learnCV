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
#include <vector>
using std::vector;
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
using std::cout;
using std::endl;

std::string DataPrefix = "/Users/kingo/Documents/Projects/learnCV/learnCV/data/";
std::string TmpPrefix = "/Users/kingo/Documents/Projects/learnCV/learnCV/tmp/";
char labelName[] = {'2','3','4','5','6','7','8','9','T','J','Q','K','A','B'};
int getLabel(const char *name);
int getLabel(lua_State *L,const char *name);
RNG rng(12345);
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

void cardRecognizer()
{
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
}

void fdivp2()
{
    std::string filename = "/Users/kingo/workshop/learning/FDIVP/digital_images_week2_quizzes_lena.gif";
    Mat org = imread(filename);
    std::cout << org.size[0] << std::endl;
    //imshow("org", org);
    //waitKey();
}

bool is_border(cv::Mat& edge, uchar color)
{
    cv::Mat im = edge.clone().reshape(0,1);
    
    bool res = true;
    for (int i = 0; i < im.cols; ++i)
        res &= (color == im.at<uchar>(0,i));
    
    return res;
}

/**
 * Function to auto-cropping image
 *
 * Parameters:
 *   src   The source image
 *   dst   The destination image
 */
void autocrop(cv::Mat& src, cv::Mat& dst)
{
    cv::Rect win(0, 0, src.cols, src.rows);
    
    std::vector<cv::Rect> edges;
    edges.push_back(cv::Rect(0, 0, src.cols, 1));
    edges.push_back(cv::Rect(src.cols-2, 0, 1, src.rows));
    edges.push_back(cv::Rect(0, src.rows-2, src.cols, 1));
    edges.push_back(cv::Rect(0, 0, 1, src.rows));
    
    cv::Mat edge;
    int nborder = 0;
    uchar color = 255;
    
    bool next;
    
    do {
        edge = src(cv::Rect(win.x, win.height-2, win.width, 1));
        if (next = is_border(edge, color))
            win.height--;
    }
    while (next && win.height > 0);
    
    do {
        edge = src(cv::Rect(win.width-2, win.y, 1, win.height));
        if (next = is_border(edge, color))
            win.width--;
    }
    while (next && win.width > 0);
    
    do {
        edge = src(cv::Rect(win.x, win.y, win.width, 1));
        if (next = is_border(edge, color))
            win.y++, win.height--;
    }
    while (next && win.y <= src.rows);
    
    do {
        edge = src(cv::Rect(win.x, win.y, 1, win.height));
        if (next = is_border(edge, color))
            win.x++, win.width--;
    }
    while (next && win.x <= src.cols);
    
    dst = src(win);
}

void cropEmpty()
{
    std::string filename = TmpPrefix+"out4.jpg";
    Mat org = imread(filename);
    imshow("org", org);
    Mat larger = org.clone();
    resize(org, larger, Size(),3,3);
    cvtColor(larger, larger, COLOR_RGB2GRAY);
    threshold(larger, larger, 150, 255, THRESH_BINARY);
    imshow("larger", larger);
    cout << "type " << org.depth() << " " << org.channels() << endl;
    
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    /// Detect edges using canny
    int thresh = 100;
    Canny( larger, canny_output, thresh, thresh*2, 3 );
    imshow("canny", canny_output);
    /// Find contours
    findContours( canny_output, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0) );
    imshow("larger",larger);
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( size_t i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, (int)i, color, 2, 8, hierarchy, 0, Point() );
    }
    
    /// Show in a window
    namedWindow( "Contours", WINDOW_AUTOSIZE );
    imshow( "Contours", drawing );
    cout << contours.size() << endl;
    Rect bound = boundingRect(contours[0]);
    Mat croped;
    autocrop(larger, croped);
    imshow("croped",croped);
    
    waitKey();
}

int main(int argc, const char * argv[]) {
    cropEmpty();
    //cardRecognizer();
    return 0;
}
