#include <string>

class Channel {
public:
    Channel(std::string& path) : path_(path) { }
    ~Channel() = default;

    std::string& path() { return path_; }
    std::string& run();
private:
    std::string path_;
};
