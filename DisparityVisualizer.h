#include <opencv2/opencv.hpp>
#include <cvImagePipeline.h>

using namespace cvImagePipeline::Filter;
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
