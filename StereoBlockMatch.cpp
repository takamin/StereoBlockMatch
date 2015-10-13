#include <iostream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <cvImagePipeline.h>
#include "StereoBM.h"
#include "StereoSGBM.h"
#include "DisparityVisualizer.h"
#include "Remapper.h"
#include "TextOverlay.h"
#include "getopt.h"
#include "cal.h"
#include "util.h"

using namespace cvImagePipeline::Filter;
IMPLEMENT_CVFILTER(StereoBM);
IMPLEMENT_CVFILTER(StereoSGBM);
IMPLEMENT_CVFILTER(DisparityVisualizer);
IMPLEMENT_CVFILTER(Remapper);

struct EditParam {
    int idchar;
    char const* name;
    int value;
    int min_value;
    int max_value;
    int m;
};
struct EditParam paratbl[] {
    {'m', "minDisparity", 3,0,16,1,},
    {'n', "numDisparity", 16,16,512,16,},
    {'w', "sadWindowSize", 3,1,25,1,},
    {'d', "disp12maxDiff", 0,0,256,16,},
    {'p', "prefilterCap", 0,0,256,2,},
    {'u', "uniquenessRatio", 0,0,100,1,},
    {'s', "speckleWindowSize", 0,0,200,50,},
    {'r', "speckleRange", 0,0,256,16,},
};
static std::string createParamStr(int paraidx) {
    std::stringstream ss;
    ss << "StereoBlockMatch" << std::endl;
    ss << "================" << std::endl;
    ss << std::endl;
    ss << " --- PARAMETERS ---" << std::endl;
    for(int i = 0; i < sizeof(paratbl)/sizeof(paratbl[0]); i++) {
        ss << (i == paraidx ? ">" : " ")
            << "[" << (char)paratbl[i].idchar << "]:"
            << paratbl[i].name << "=" << paratbl[i].value << std::endl;
    }
    ss << "----" << std::endl
        << " [+] / [-]: change value" << std::endl
        << " [SPC]: set value to Min or Max" << std::endl
        << " [Q]: Quit" << std::endl;
    return ss.str();
}
int main(int argc, char* argv[]) {
    char const* optstring = "L:R:C:W:H:m:n:w:d:p:u:s:r:";
    int opt = 0;
    char const* optR = 0;
    char const* optL = 0;
    char const* calPrefix = 0;
    cv::Size chessboardGridSize(18, 18);//[mm]
    int minDisparity = 3;
    int numDisparity = 16;
    int sadWindowSize = 3;
    int disp12maxDiff = 0;
    int prefilterCap = 0;
    int uniquenessRatio = 0;
    int speckleWindowSize = 0;
    int speckleRange = 0;
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
            case 'W': chessboardGridSize.width = toInt(optarg, 18); break;
            case 'H': chessboardGridSize.height = toInt(optarg, 18); break;
            case 'm': minDisparity = toInt(optarg, 3); break;
            case 'n': numDisparity = toInt(optarg, 16); break;
			case 'w': sadWindowSize = toInt(optarg, 3); break;
			case 'd': disp12maxDiff = toInt(optarg, 0); break;
			case 'p': prefilterCap = toInt(optarg, 0); break;
			case 'u': uniquenessRatio = toInt(optarg, 0); break;
			case 's': speckleWindowSize = toInt(optarg, 0); break;
			case 'r': speckleRange = toInt(optarg, 0); break;
        }
    }
    int deviceIndexR = toInt(optR, 1);
    int deviceIndexL = toInt(optL, 2);

    StereoCalibrationInfo cal(chessboardGridSize);
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
    proc.add("StereoSGBM", "stereo", false)
        .property("minDisparity", minDisparity)
        .property("numDisparity", numDisparity)
        .property("sadWindowSize", sadWindowSize)
        .property("disp12maxDiff", disp12maxDiff)
        .property("prefilterCap", prefilterCap)
        .property("uniquenessRatio", uniquenessRatio)
        .property("speckleWindowSize", speckleWindowSize)
        .property("speckleRange", speckleRange);
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
    
    TextOverlay textoverlay;
    proc.add(textoverlay, "textoverlay");
    proc.add("ImageWindow", "wnd").property("windowName", "StereoBlockMatch");
    
    int paraidx = -1;
    for(int i = 0; i < sizeof(paratbl)/sizeof(paratbl[0]); i++) {
        paratbl[i].value = toInt(
                proc["stereo"].property(paratbl[i].name).getString().c_str(), 0);
    }
    textoverlay.setText(createParamStr(paraidx));
    while(true) {
        proc.execute();
        int c = cvWaitKey(1);
        if(c == 'Q') {
            break;
        }
        switch(c) {
            case '+':
                if(paraidx >= 0) {
                    if(paratbl[paraidx].value < paratbl[paraidx].max_value) {
                        paratbl[paraidx].value += paratbl[paraidx].m;
                        proc["stereo"].property(
                                paratbl[paraidx].name,
                                paratbl[paraidx].value);
                    }
                    textoverlay.setText(createParamStr(paraidx));
                }
                break;
            case '-':
                if(paraidx >= 0) {
                    if(paratbl[paraidx].value > paratbl[paraidx].min_value) {
                        paratbl[paraidx].value -= paratbl[paraidx].m;
                        proc["stereo"].property(
                                paratbl[paraidx].name,
                                paratbl[paraidx].value);
                    }
                    textoverlay.setText(createParamStr(paraidx));
                }
                break;
            case ' ':
                if(paraidx >= 0) {
                    if(paratbl[paraidx].value == paratbl[paraidx].min_value) {
                        paratbl[paraidx].value = paratbl[paraidx].max_value;
                    } else {
                        paratbl[paraidx].value = paratbl[paraidx].min_value;
                    }
                    proc["stereo"].property(paratbl[paraidx].name, paratbl[paraidx].value);
                    textoverlay.setText(createParamStr(paraidx));
                }
                break;
            default:
                for(int i = 0; i < sizeof(paratbl)/sizeof(paratbl[0]); i++) {
                    if(c == paratbl[i].idchar) {
                        paraidx = i;
                        textoverlay.setText(createParamStr(paraidx));
                        break;
                    }
                }
                break;
        }
    }
    return 0;
}
