#include <opencv2/opencv.hpp>
#include <cvImagePipeline.h>

using namespace cvImagePipeline::Filter;
class StereoBlockMatcher : public ImageProcessor {
    cv::StereoSGBM* stereo;
public:
    TParam<int> minDisparity;
    TParam<int> numDisparity;
    TParam<int> sadWindowSize;
    TParam<int> disp12maxDiff;
    TParam<int> prefilterCap;
    TParam<int> uniquenessRatio;
    TParam<int> speckleWindowSize;
    TParam<int> speckleRange;
public:
    DECLARE_CVFILTER;
    StereoBlockMatcher()
        :
            stereo(0),
            minDisparity("minDisparity", 0),
            numDisparity("numDisparity", 16),
            sadWindowSize("sadWindowSize", 3),
            disp12maxDiff("disp12maxDiff", 0),
            prefilterCap("prefilterCap", 0),
            uniquenessRatio("uniquenessRatio", 0),
            speckleWindowSize("speckleWindowSize", 0),
            speckleRange("speckleRange", 0)
    {
        defParam(minDisparity);
        defParam(numDisparity);
        defParam(sadWindowSize);
        defParam(disp12maxDiff);
        defParam(prefilterCap);
        defParam(uniquenessRatio);
        defParam(speckleWindowSize);
        defParam(speckleRange);
        defInputMat("right");
        defInputMat("left");
        undefInputMat("");
    }
    ~StereoBlockMatcher()
    {
        delete stereo;
        stereo = 0;
    }
    void execute() {
        if(stereo == 0) {
            create();
        }
        (*stereo)(
                getInputMat("left"), getInputMat("right"),
                refOutputMat());
    }
    void onPropertyChange(Property& property) {
        if(stereo) {
            delete stereo;
            stereo = 0;
        }
        create();
    }
private:
    void create() {
        stereo = new cv::StereoSGBM(
                minDisparity, numDisparity, sadWindowSize,
                8 * sadWindowSize * sadWindowSize,
                32 * sadWindowSize * sadWindowSize,
                disp12maxDiff, prefilterCap, uniquenessRatio,
                speckleWindowSize, speckleRange,false);
    }
};
