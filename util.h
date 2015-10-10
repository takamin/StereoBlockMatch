#include <opencv2/opencv.hpp>
int toInt(char const* s, int dfl);
void saveStereoImages(
        const cv::Mat& imageR, const cv::Mat& imageL,
        char const* prefix, int seqno, int quality = 100);
bool loadStereoImages(
        cv::Mat& imageR, cv::Mat& imageL,
        char const* prefix, int seqno);
