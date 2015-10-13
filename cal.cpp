#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "cal.h"
#include "util.h"

//
//
// class ChessboardInfo
//
//
ChessboardInfo::ChessboardInfo()
{
}
ChessboardInfo::ChessboardInfo(
        const cv::Mat& image,
        const cv::Size& patternSize,
        const cv::Size& gridSize,
        int flags)
{
    this->image = image.clone();
    this->patternSize = patternSize;
    bool patternFound = cv::findChessboardCorners(
            this->image, this->patternSize,
            this->corners, flags);
    if(patternFound) {
        cv::cornerSubPix(
                this->image, this->corners,
                cv::Size(11, 11), cv::Size(-1, -1),
                cv::TermCriteria(
                    CV_TERMCRIT_EPS + CV_TERMCRIT_ITER,
                    30, 0.1));
    }
    for(int row = 0; row < this->patternSize.height; row++) {
        for(int col = 0; col < this->patternSize.width; col++) {
            this->objectPoints.push_back(cv::Point3f(
                        (float)col * gridSize.width,
                        (float)row * gridSize.height,
                        (float)0));
        }
    }
}
ChessboardInfo& ChessboardInfo::operator=(const ChessboardInfo& that)
{
    this->image = that.image.clone();
    this->patternSize = that.patternSize;
    this->corners = that.corners;
    this->objectPoints = that.objectPoints;
    return *this;
}
bool ChessboardInfo::IsDetectionValid() const
{
    return image.rows != 0 && image.cols != 0
        && patternSize.height != 0 && patternSize.width != 0
        && !corners.empty();
}

//
//
// class CalibrationInfo
//
//
CalibrationInfo::CalibrationInfo()
    :
        cameraMatrix(cv::Mat::zeros(3, 3, CV_32F)),
        calibrateFlags(
            //CV_CALIB_USE_INTRINSIC_GUESS +
            //CV_CALIB_FIX_PRINCIPAL_POINT +
            //CV_CALIB_FIX_ASPECT_RATIO +
            //CV_CALIB_ZERO_TANGENT_DIST +
            0)
{
}
CalibrationInfo::~CalibrationInfo()
{
    std::vector<ChessboardInfo*>::iterator it = chessboards.begin();
    for(; it != chessboards.end(); it++) {
        delete (*it);
        *it = 0;
    }
}

void CalibrationInfo::addChessboardInfo(const ChessboardInfo& chessboardInfo)
{
    if(chessboardInfo.IsDetectionValid()) { 
        if(chessboards.size() == 0) {
            this->imageSize = cv::Size(
                    chessboardInfo.Image().cols,
                    chessboardInfo.Image().rows);
            this->patternSize = chessboardInfo.PatternSize();
        }
        if(chessboards.size() == 0
            || this->imageSize.height == chessboardInfo.Image().rows
            && this->imageSize.width == chessboardInfo.Image().cols
            && this->patternSize.height == chessboardInfo.PatternSize().height
            && this->patternSize.width == chessboardInfo.PatternSize().width)
        {
            this->objectPoints.push_back(chessboardInfo.ObjectPoints());
            ChessboardInfo* p = new ChessboardInfo();
            *p = chessboardInfo;
            imagePoints.push_back(p->Corners());
            chessboards.push_back(p);
        }
    }
}
bool CalibrationInfo::calibrate()
{
    if(this->objectPoints.size() == 0 || this->imagePoints.size() == 0) {
        std::cerr << "no calibration data." << std::endl;
        return false;
    }
    double reprojectionError = cv::calibrateCamera(
            this->objectPoints, this->imagePoints, this->imageSize,
            this->cameraMatrix, this->distCoeffs, this->rvecs, this->tvecs,
            this->calibrateFlags);
    std::cerr << "reprojection error: " << reprojectionError << std::endl;
	std::cerr << "cameraMatrix : " << this->cameraMatrix << std::endl;
	std::cerr << "distCoeffs : " << this->distCoeffs << std::endl;
	std::cerr << "rvecs.size: " << this->rvecs.size() << std::endl;
	std::cerr << "tvecs.size: " << this->tvecs.size() << std::endl;
    return true;
}

//
//
// class StereoCalibrationInfo
//
//
StereoCalibrationInfo::StereoCalibrationInfo(cv::Size chessboardGridSizeMM)
    :
        cameraMatrixR(cv::Mat::zeros(3,3,CV_32F)),
        cameraMatrixL(cv::Mat::zeros(3,3,CV_32F)),
        patternSize(10, 8),
        gridSize(chessboardGridSizeMM),
        calibrateFlag(
            CV_CALIB_CB_ADAPTIVE_THRESH |
            CV_CALIB_CB_NORMALIZE_IMAGE |
            //CV_CALIB_CB_FILTER_QUADS |
            CV_CALIB_CB_FAST_CHECK)
{
}
void StereoCalibrationInfo::FromImages(char const* prefix)
{
    int i = 0;
    cv::Mat imageR, imageL;
    while(loadStereoImages(imageR, imageL, prefix, i++)) {
        addImages(imageR, imageL);
    }
	std::cerr << "*** right result of calibrateCamera:" << std::endl;
    bool calR = r.calibrate();
	std::cerr << "*** left result of calibrateCamera:" << std::endl;
    bool calL = l.calibrate();
    if(calR && calL) {
        cameraMatrixR = r.CameraMatrix().clone();
        distCoeffsR = r.DistCoeffs().clone();
        cameraMatrixL = l.CameraMatrix().clone();
        distCoeffsL = l.DistCoeffs().clone();

        std::cerr << "*** result of stereoCalibrate:" << std::endl;
        double reprojectionError = cv::stereoCalibrate(r.ObjectPoints(),
                this->l.ImagePoints(), this->r.ImagePoints(),
                this->cameraMatrixL, this->distCoeffsL,
                this->cameraMatrixR, this->distCoeffsR,
                this->r.ImageSize(),
                this->R, this->T, this->E, this->F);
        std::cerr << "reprojection error: " << reprojectionError << std::endl;
        std::cerr << "cameraMatrixR : " << std::endl << this->cameraMatrixR << std::endl;
        std::cerr << "distCoeffsR : " << std::endl << this->distCoeffsR << std::endl;
        std::cerr << "cameraMatrixL : " << std::endl << this->cameraMatrixL << std::endl;
        std::cerr << "distCoeffsL : " << std::endl << this->distCoeffsL << std::endl;
        std::cerr << "R: " << std::endl << this->R << std::endl;
        std::cerr << "T: " << std::endl << this->T << std::endl;
        std::cerr << "E: " << std::endl << this->E << std::endl;
        std::cerr << "F: " << std::endl << this->F << std::endl;

        cv::stereoRectify(
                this->cameraMatrixL, this->distCoeffsL,
                this->cameraMatrixR, this->distCoeffsR,
                this->r.ImageSize(), this->R, this->T,
                this->R1, this->R2, this->P1, this->P2, this->Q);
        std::cerr << "*** result of stereoRectify:" << std::endl;
        std::cerr << "R1: " << std::endl << this->R1 << std::endl;
        std::cerr << "R2: " << std::endl << this->P2 << std::endl;
        std::cerr << "P1: " << std::endl << this->R1 << std::endl;
        std::cerr << "P2: " << std::endl << this->P2 << std::endl;
        std::cerr << "Q: " << std::endl << this->Q << std::endl;

        cv::initUndistortRectifyMap(
                this->cameraMatrixR, this->distCoeffsR, this->R2,
                this->P2, this->r.ImageSize(), CV_32FC1,
                this->mapR1, this->mapR2);
        std::cerr << "*** right result of initUndistortRectifyMap:" << std::endl;
        std::cerr << "mapR1.cols: " << this->mapR1.cols << std::endl;
        std::cerr << "mapR1.rows: " << this->mapR1.rows << std::endl;
        std::cerr << "mapR2.cols: " << this->mapR2.cols << std::endl;
        std::cerr << "mapR2.rows: " << this->mapR2.rows << std::endl;

        cv::initUndistortRectifyMap(
                this->cameraMatrixL, this->distCoeffsL, this->R1,
                this->P1, this->l.ImageSize(), CV_32FC1,
                this->mapL1, this->mapL2);
        std::cerr << "*** left result of initUndistortRectifyMap:" << std::endl;
        std::cerr << "mapL1.cols: " << this->mapL1.cols << std::endl;
        std::cerr << "mapL1.rows: " << this->mapL1.rows << std::endl;
        std::cerr << "mapL2.cols: " << this->mapL2.cols << std::endl;
        std::cerr << "mapL2.rows: " << this->mapL2.rows << std::endl;
    }
}
void StereoCalibrationInfo::addImages(const cv::Mat& imageR, const cv::Mat& imageL)
{
    ChessboardInfo cbR(imageR, patternSize, gridSize, calibrateFlag);
    ChessboardInfo cbL(imageL, patternSize, gridSize, calibrateFlag);
    if(cbR.IsDetectionValid() && cbL.IsDetectionValid()) {
        r.addChessboardInfo(cbR);
        l.addChessboardInfo(cbL);
    }
}
