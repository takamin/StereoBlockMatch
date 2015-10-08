#include <iostream>
#include <opencv2/opencv.hpp>
#include <cvImagePipeline.h>
#include "StereoBlockMatcher.h"
#include "DisparityVisualizer.h"
#include "getopt.h"
#include "util.h"

using namespace cvImagePipeline::Filter;
IMPLEMENT_CVFILTER(StereoBlockMatcher);
IMPLEMENT_CVFILTER(DisparityVisualizer);

int main(int argc, char* argv[]) {
    char const* optstring = "L:R:";
    int opt = 0;
    char const* optR = 0;
    char const* optL = 0;
    while((opt = getopt(argc, argv, optstring)) != -1) {
        switch(opt) {
            case 'R':
                optR = optarg;
                break;
            case 'L':
                optL = optarg;
                break;
        }
    }
    int deviceIndexR = toInt(optR, 1);
    int deviceIndexL = toInt(optL, 2);

    cvImagePipeline::Filter::ImgProcSet proc;
    
    proc.add("VideoCapture", "capR", false).property("deviceIndex", deviceIndexR);
    proc.add("VideoCapture", "capL", false).property("deviceIndex", deviceIndexL);
    proc.add("ColorConverter", "gryR", false).property("cvtCode", CV_BGR2GRAY);
    proc.add("ColorConverter", "gryL", false).property("cvtCode", CV_BGR2GRAY);
    proc.add("StereoBlockMatcher", "stereo", false);

    proc["capR"] >> proc["gryR"] >> proc["stereo"].input("right");
    proc["capL"] >> proc["gryL"] >> proc["stereo"].input("left");

    proc.add("DisparityVisualizer", "dis");
    proc.add("Convert", "cvtReal").property("rtype", CV_32FC1);
    proc.add("RunningAvg", "runavg").property("averageCount", 15);
    proc.add("Convert", "avg").property("rtype", CV_8UC1);

    proc.add("FitInGrid", "fig", false)
        .property("cols", 2).property("rows", 2)
        .property("width", 960).property("height", 720);
    proc["gryR"] >> proc["fig"].input("0");
    proc["gryL"] >> proc["fig"].input("1");
    proc["dis"] >> proc["fig"].input("2");
    proc["avg"] >> proc["fig"].input("3");
    
    proc.add("ImageWindow", "wnd").property("windowName", "disparity map");
    
    while(true) {
        proc.execute();
        int c = cvWaitKey(1);
        if(c == 'Q') {
            break;
        }
    }
    return 0;
}
