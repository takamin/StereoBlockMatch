#include <iostream>
#include <opencv2/opencv.hpp>
#include <cvImagePipeline.h>

using namespace cvImagePipeline::Filter;
class StereoBlockMatcher : public ImageProcessor {
    cv::StereoSGBM stereo;
public:
    DECLARE_CVFILTER;
    StereoBlockMatcher()
        : stereo(5, 16, 21, 0,0,0,0,0,0,true)
    {
        defInputMat("right");
        defInputMat("left");
        undefInputMat("");
    }
    void execute() {
        stereo(getInputMat("left"), getInputMat("right"), refOutputMat());
    }
};
class DisparityVisualizer : public ImageProcessor {
public:
    DECLARE_CVFILTER;
    DisparityVisualizer(){}
    void execute() {
        const cv::Mat& in = getInputMat();
        if(in.empty()) {
            return;
        }
        cv::Mat& out = refOutputMat();
        out = cv::Mat::zeros(in.rows, in.cols, CV_8UC1);
        short min = 0;
        short max = 0;
        for(int row = 0; row < in.rows; row++) {
            short* p = (short*)(in.data + in.step * row);
            for(int col = 0; col < in.cols; col++) {
                short value = *p;
                p++;
                if(value < min) { min = value; }
                if(value > max) { max = value; }
            }
        }
        for(int row = 0; row < in.rows; row++) {
            short* src = (short*)(in.data + in.step * row);
            unsigned char* dst = (unsigned char*)(out.data + out.step * row);
            for(int col = 0; col < in.cols; col++) {
                short value = *src;
                src++;
                *dst = (unsigned char)(255.0 * (value - min) / (max - min));
                dst++;
            }
        }
    }
};
IMPLEMENT_CVFILTER(StereoBlockMatcher);
IMPLEMENT_CVFILTER(DisparityVisualizer);

int main(int argc, char* argv[]) {
    cvImagePipeline::Filter::ImgProcSet proc;
    
    proc.add("VideoCapture", "capR", false).property("deviceIndex", 1);
    proc.add("VideoCapture", "capL", false).property("deviceIndex", 2);
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
