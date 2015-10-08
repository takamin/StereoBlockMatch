#include <sstream>
int toInt(char const* s, int dfl) {
    int n = dfl;
    if(s) {
        std::stringstream ss(s);
        ss >> n;
    }
    return n;
}

