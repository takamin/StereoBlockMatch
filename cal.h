#include <vector>
#include <opencv2/opencv.hpp>
class ChessboardInfo {
    public:
        ChessboardInfo();
        ChessboardInfo(const cv::Mat& image, const cv::Size& patternSize, int flags);
        const cv::Mat& Image() const { return image; }
        const cv::Size& PatternSize() const { return patternSize; }
        const std::vector<cv::Point2f>& Corners() const { return corners; }
        bool IsDetectionValid() const;
        ChessboardInfo& operator=(const ChessboardInfo& that);
    private:
        cv::Mat image;
        cv::Size patternSize;
        std::vector<cv::Point2f> corners;
};
class CalibrationInfo {
    public:
        CalibrationInfo();
        virtual ~CalibrationInfo();
        void addChessboardInfo(const ChessboardInfo& chessboardInfo);
        bool calibrate();

        const cv::Size& ImageSize() const { return imageSize; }
        const std::vector< std::vector<cv::Point3f> >& ObjectPoints() const
        { return objectPoints; }
        const std::vector< std::vector<cv::Point2f> >& ImagePoints() const
        { return imagePoints; }
        const cv::Mat& CameraMatrix() const { return cameraMatrix; }
        const cv::Mat& DistCoeffs() const { return distCoeffs; }
        const std::vector<cv::Mat>& Rvects() const { return rvecs; }
        const std::vector<cv::Mat>& Tvects() const { return tvecs; }
    private:
        std::vector<ChessboardInfo*> chessboards;
        // for calibrateCamera
        cv::Size imageSize;
        cv::Size patternSize;
        std::vector< std::vector<cv::Point3f> > objectPoints;
        std::vector< std::vector<cv::Point2f> > imagePoints;
        cv::Mat cameraMatrix;
        cv::Mat distCoeffs;
        std::vector<cv::Mat> rvecs;
        std::vector<cv::Mat> tvecs;
        int calibrateFlags;
};
class StereoCalibrationInfo {
    public:
        StereoCalibrationInfo();
        void FromImages(char const* prefix);
        const cv::Mat& MapRight1() const { return this->mapR1; }
        const cv::Mat& MapRight2() const { return this->mapR2; }
        const cv::Mat& MapLeft1() const { return this->mapL1; }
        const cv::Mat& MapLeft2() const { return this->mapL2; }
        const cv::Mat& MapQ() const { return this->Q; }
    private:
        void addImages(const cv::Mat& imageR, const cv::Mat& imageL);
    private:
        cv::Size patternSize;
        int calibrateFlag;
        CalibrationInfo r;
        CalibrationInfo l;

        // for stereoCalibrate
        cv::Mat cameraMatrixR;
        cv::Mat distCoeffsR;
        cv::Mat cameraMatrixL;
        cv::Mat distCoeffsL;
        cv::Mat R;
        cv::Mat T;
        cv::Mat E;
        cv::Mat F;
        cv::Mat R1;
        cv::Mat R2;
        cv::Mat P1;
        cv::Mat P2;
        cv::Mat Q;
        cv::Mat mapR1,mapR2;
        cv::Mat mapL1,mapL2;
};
