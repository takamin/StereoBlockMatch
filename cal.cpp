#include <opencv2/opencv.hpp>
#include "cal.h"
#include "util.h"
ChessboardInfo::ChessboardInfo()
{
}
ChessboardInfo::ChessboardInfo(
        const cv::Mat& image,
        const cv::Size& patternSize,
        int flags)
{
    this->image = image.clone();
    bool patternFound = cv::findChessboardCorners(
            this->image, patternSize,
            this->corners, flags);
    if(patternFound) {
        cv::cornerSubPix(
                this->image, this->corners,
                cv::Size(11, 11), cv::Size(-1, -1),
                cv::TermCriteria(
                    CV_TERMCRIT_EPS + CV_TERMCRIT_ITER,
                    30, 0.1));
    }
}
ChessboardInfo& ChessboardInfo::operator=(const ChessboardInfo& that)
{
    this->image = that.image.clone();
    this->corners = that.corners;
    return *this;
}
CalibrationInfo::CalibrationInfo()
{
}
CalibrationInfo::~CalibrationInfo()
{
    std::vector<ChessboardInfo*>::iterator it = chessboard.begin();
    for(; it != chessboard.end(); it++) {
        delete (*it);
        *it = 0;
    }
}
void CalibrationInfo::addChessboardInfo(const ChessboardInfo& chessboardInfo)
{
    ChessboardInfo* p = new ChessboardInfo();
    *p = chessboardInfo;
    chessboard.push_back(p);
}
StereoCalibrationInfo::StereoCalibrationInfo()
    : patternSize(10, 8),
    flags(
        CV_CALIB_CB_ADAPTIVE_THRESH |
        CV_CALIB_CB_NORMALIZE_IMAGE |
        //CV_CALIB_CB_FILTER_QUADS |
        CV_CALIB_CB_FAST_CHECK)
{
}
void StereoCalibrationInfo::FromImages(char const* prefix)
{
    int i = 0;
    StereoCalibrationInfo calInfo;
    cv::Mat imageR, imageL;
    while(loadStereoImages(imageR, imageL, prefix, i++)) {
        calInfo.addImages(imageR, imageL);
    }
}
void StereoCalibrationInfo::addImages(const cv::Mat& imageR, const cv::Mat& imageL)
{
    ChessboardInfo cbR(imageR, patternSize, flags);
    ChessboardInfo cbL(imageL, patternSize, flags);
    if(cbR.Corners().empty() && cbL.Corners().empty()) {
        r.addChessboardInfo(cbR);
        l.addChessboardInfo(cbL);
    }
}
