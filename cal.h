#include <opencv2/opencv.hpp>
class ChessboardInfo {
    public:
        ChessboardInfo();
        ChessboardInfo(const cv::Mat& image, const cv::Size& patternSize, int flags);
        const std::vector<cv::Point2f>& Corners() const { return corners; }
        ChessboardInfo& operator=(const ChessboardInfo& that);
    private:
        cv::Mat image;
        std::vector<cv::Point2f> corners;
};
class CalibrationInfo {
    public:
        CalibrationInfo();
        virtual ~CalibrationInfo();
        void addChessboardInfo(const ChessboardInfo& chessboardInfo);
    private:
        std::vector<ChessboardInfo*> chessboard;
};
class StereoCalibrationInfo {
    public:
        StereoCalibrationInfo();
        void FromImages(char const* prefix);
    private:
        void addImages(const cv::Mat& imageR, const cv::Mat& imageL);
    private:
        cv::Size patternSize;
        int flags;
        CalibrationInfo r;
        CalibrationInfo l;
};
