#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <opencv2/opencv.hpp>
int toInt(char const* s, int dfl) {
    int n = dfl;
    if(s) {
        std::stringstream ss(s);
        ss >> n;
    }
    return n;
}
static std::string makeCalImgFilename(
        char const* prefix,
        int seqno,
        char const* LorR)
{
    std::stringstream fn;
    fn << prefix << "_"
        << std::setw(3) << std::setfill('0')
        << seqno << "_"
        << LorR << ".jpg";
    return fn.str();
}
static void createCalImageFilenames(
        std::string& fnR, std::string& fnL,
        char const* prefix, int seqno)
{
    fnR = makeCalImgFilename(prefix, seqno, "R");
    fnL = makeCalImgFilename(prefix, seqno, "L");
}
void saveStereoImages(
        const cv::Mat& imageR, const cv::Mat& imageL,
        char const* prefix, int seqno, int quality)
{
    std::vector<int> imwrite_params;
    imwrite_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    imwrite_params.push_back(quality);

    std::string fnR, fnL;
    createCalImageFilenames(fnR, fnL, prefix, seqno);
    cv::imwrite(fnR, imageR, imwrite_params);
    cv::imwrite(fnL, imageL, imwrite_params);
    std::cerr << "save image: " << fnL << " / " << fnR << std::endl;
}
bool loadStereoImages(
        cv::Mat& imageR, cv::Mat& imageL,
        char const* prefix, int seqno)
{
    std::string fnR, fnL;
    createCalImageFilenames(fnR, fnL, prefix, seqno);
    imageR = cv::imread(fnR);
    imageL = cv::imread(fnL);
    if(!imageR.empty()) {
        std::cerr << "load image: " << fnR << std::endl;
    }
    if(!imageL.empty()) {
        std::cerr << "load image: " << fnL << std::endl;
    }
    return !imageR.empty() && !imageL.empty();
}
