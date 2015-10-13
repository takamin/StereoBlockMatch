#include <opencv2/opencv.hpp>
#include <cvImagePipeline.h>

using namespace cvImagePipeline::Filter;
class StereoBM : public ImageProcessor {
    cv::StereoBM* stereo;
public:
    TParam<int> numDisparity;
    TParam<int> sadWindowSize;
public:
    DECLARE_CVFILTER;
    StereoBM()
        :
            stereo(0),
            numDisparity("numDisparity", 16),
            sadWindowSize("sadWindowSize", 3)
    {
        defParam(numDisparity);
        defParam(sadWindowSize);
        defInputMat("right");
        defInputMat("left");
        undefInputMat("");
    }
    ~StereoBM()
    {
        destroy();
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
        destroy();
        create();
    }
private:
    void create() {
        stereo = new cv::StereoBM(
                cv::StereoBM::PREFILTER_XSOBEL,
                numDisparity, sadWindowSize);
    }
    void destroy() {
        delete stereo;
        stereo = 0;
    }
};
