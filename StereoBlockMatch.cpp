#include <iostream>
#include <opencv2/opencv.hpp>
#include <cvImagePipeline.h>
#include "StereoBlockMatcher.h"
#include "DisparityVisualizer.h"
#include "Remapper.h"
#include "getopt.h"
#include "cal.h"
#include "util.h"

using namespace cvImagePipeline::Filter;
IMPLEMENT_CVFILTER(StereoBlockMatcher);
IMPLEMENT_CVFILTER(DisparityVisualizer);
IMPLEMENT_CVFILTER(Remapper);

int main(int argc, char* argv[]) {
    char const* optstring = "L:R:C:";
    int opt = 0;
    char const* optR = 0;
    char const* optL = 0;
    char const* calPrefix = 0;
    while((opt = getopt(argc, argv, optstring)) != -1) {
        switch(opt) {
            case 'R':
                optR = optarg;
                break;
            case 'L':
                optL = optarg;
                break;
            case 'C':
                calPrefix = optarg;
                break;
        }
    }
    int deviceIndexR = toInt(optR, 1);
    int deviceIndexL = toInt(optL, 2);

    StereoCalibrationInfo cal;
    if(calPrefix != 0) {
        cal.FromImages(calPrefix);
    }

    cvImagePipeline::Filter::ImgProcSet proc;
    
    proc.add("VideoCapture", "capR", false).property("deviceIndex", deviceIndexR);
    proc.add("VideoCapture", "capL", false).property("deviceIndex", deviceIndexL);
    proc.add("ColorConverter", "gryR", false).property("cvtCode", CV_BGR2GRAY);
    proc.add("ColorConverter", "gryL", false).property("cvtCode", CV_BGR2GRAY);
    if(calPrefix != 0) {
        proc.add("Remapper", "remapR", false);
        proc["remapR"].setInputMat("map1", cal.MapRight1());
        proc["remapR"].setInputMat("map2", cal.MapRight2());
        proc.add("Remapper", "remapL", false);
        proc["remapL"].setInputMat("map1", cal.MapLeft1());
        proc["remapL"].setInputMat("map2", cal.MapLeft2());
    }
    proc.add("ImagePoint", "viewR", false);
    proc.add("ImagePoint", "viewL", false);
    proc.add("StereoBlockMatcher", "stereo", false);
    if(calPrefix != 0) {
        proc["capR"] >> proc["gryR"] >> proc["remapR"]
             >> proc["viewR"] >> proc["stereo"].input("right");
        proc["capL"] >> proc["gryL"] >> proc["remapL"]
            >> proc["viewL"] >> proc["stereo"].input("left");
    } else {
        proc["capR"] >> proc["gryR"]
            >> proc["viewR"] >> proc["stereo"].input("right");
        proc["capL"] >> proc["gryL"]
            >> proc["viewL"] >> proc["stereo"].input("left");
    }
    proc.add("DisparityVisualizer", "dis");
    if(calPrefix != 0) {
        proc["dis"].setInputMat("Q", cal.MapQ());
    }
    proc.add("Convert", "cvtReal").property("rtype", CV_32FC1);
    proc.add("RunningAvg", "runavg").property("averageCount", 15);
    proc.add("Convert", "avg").property("rtype", CV_8UC1);

    proc.add("FitInGrid", "fig", false)
        .property("cols", 2).property("rows", 2)
        .property("width", 960).property("height", 720);
    proc["viewR"] >> proc["fig"].input("0");
    proc["viewL"] >> proc["fig"].input("1");
    proc["dis"] >> proc["fig"].input("2");
    proc["avg"] >> proc["fig"].input("3");
    
    proc.add("ImageWindow", "wnd").property("windowName", "StereoBlockMatch");
    
    while(true) {
        proc.execute();
        int c = cvWaitKey(1);
        if(c == 'Q') {
            break;
        }
    }
    return 0;
}
