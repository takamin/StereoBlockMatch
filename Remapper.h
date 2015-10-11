#include <opencv2/opencv.hpp>
#include <cvImagePipeline.h>

using namespace cvImagePipeline::Filter;
class Remapper : public ImageProcessor {
public:
    TParam<int> interpolation;
public:
    DECLARE_CVFILTER;
    Remapper(): interpolation("interpolation", (int)cv::INTER_LINEAR) {
        defParam(this->interpolation);
        defInputMat("map1");
        defInputMat("map2");
	}
    void execute() {
        const cv::Mat& in = getInputMat();
        const cv::Mat& map1 = getInputMat("map1");
        const cv::Mat& map2 = getInputMat("map2");
        if(in.empty()) {
            return;
        }
        cv::Mat& out = refOutputMat();
        cv::remap(in, out, map1, map2, interpolation);
    }
};
