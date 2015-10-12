#include <string>
#include <ios>
#include <sstream>

class TextOverlay : public ImageProcessor {
public:
    std::vector<std::string> textlines;
public:
    DECLARE_CVFILTER;
    TextOverlay() { }
    void setText(const std::string& text) {
        textlines.clear();
        addLines(text);
    }
    void execute() {
        const cv::Mat& in = getInputMat();
        if(in.empty()) {
            return;
        }
        cv::Mat& out = refOutputMat();
        in.copyTo(out);
        int y = 15;
        int dy = 15;
        for(std::string textline : textlines) {
            cv::putText(out, textline, cv::Point(15, y),
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 0.6,
                            cv::Scalar(0,200,0), 1);
            y += dy;
        }
    }
private:
    void addLines(const std::string& text) {
        std::string textline;
        for (char c: text) {
            if (c == '\n') {
                textlines.push_back(textline);
                textline.clear();
            }
            else {
                textline += c;
            }
        }
        if (!textline.empty()) {
            textlines.push_back(textline);
        }
    }
};

