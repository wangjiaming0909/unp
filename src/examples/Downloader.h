#include "util/string_piece/string_piece.h"
#include <string>
#include <filesystem>
#include <boost/noncopyable.hpp>

namespace examples
{

class Downloader : boost::noncopyable
{
public:
    using string_t = std::string;
    Downloader(const string_t& url);
    ~Downloader();

public:
    void setDownloadPath(const string_t &path)
    {
    }

private:
    static constexpr char *DEFAULT_DOWNLOAD_PATH = "/tmp";
    std::filesystem::path targetPath_;
    std::string url_;
};

}