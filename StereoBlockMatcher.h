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
