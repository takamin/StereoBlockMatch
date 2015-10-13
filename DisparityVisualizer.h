#include <opencv2/opencv.hpp>
#include <cvImagePipeline.h>

using namespace cvImagePipeline::Filter;
class DisparityVisualizer : public ImageProcessor {
public:
    DECLARE_CVFILTER;
    DisparityVisualizer(){
        defInputMat("Q");
        setInputMat("Q", cv::Mat());//set empty mat
    }
    void execute() {
        const cv::Mat& in = getInputMat();
        const cv::Mat& Q = getInputMat("Q");
        if(in.empty()) {
            return;
        }
        cv::Mat depth;
        cv::Mat& out = refOutputMat();
        out = cv::Mat::zeros(in.rows, in.cols, CV_8UC1);
        if(!Q.empty()) {
            cv::Mat _3dImage;
            cv::reprojectImageTo3D(in, _3dImage, Q, true);
            std::vector<cv::Mat> planes;
            cv::split(_3dImage, planes);
            planes[2].copyTo(depth);
            float min = 10000.0;
            float max = 0.0;
            for(int row = 0; row < depth.rows; row++) {
                float* p = (float*)(depth.data + depth.step * row);
                for(int col = 0; col < depth.cols; col++) {
                    float value = *p;
                    p++;
                    if(value < 10000.0) {
                        if(value < min) { min = value; }
                        if(value > max) { max = value; }
                    }
                }
            }
            for(int row = 0; row < depth.rows; row++) {
                float* src = (float*)(depth.data + depth.step * row);
                unsigned char* dst = (unsigned char*)(out.data + out.step * row);
                for(int col = 0; col < depth.cols; col++) {
                    float value = *src;
                    src++;
                    if(value > max) {
                        value = max;
                    }
                    *dst = (unsigned char)(255.0 * (max - value) / (max - min));
                    dst++;
                }
            }
        } else {
            in.copyTo(depth);
            short min = 0;
            short max = 0;
            for(int row = 0; row < depth.rows; row++) {
                short* p = (short*)(depth.data + depth.step * row);
                for(int col = 0; col < depth.cols; col++) {
                    short value = *p;
                    p++;
                    if(value < min) { min = value; }
                    if(value > max) { max = value; }
                }
            }
            for(int row = 0; row < depth.rows; row++) {
                short* src = (short*)(depth.data + depth.step * row);
                unsigned char* dst = (unsigned char*)(out.data + out.step * row);
                for(int col = 0; col < depth.cols; col++) {
                    short value = *src;
                    src++;
                    *dst = (unsigned char)(255.0 * (value - min) / (max - min));
                    dst++;
                }
            }
        }
    }
};
