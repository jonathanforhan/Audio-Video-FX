#ifndef AVFX_SRC_CORE_VIDEO_HPP
#define AVFX_SRC_CORE_VIDEO_HPP

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace avfx {

struct Frame {
    uint8_t* data = nullptr;
    int width     = -1;
    int height    = -1;

    explicit Frame(uint8_t* x, size_t n, int w, int h)
      : data(new uint8_t[n]), width(w), height(h) {
        memcpy(data, x, n);
    }

    ~Frame() noexcept {
        delete[] data;
    }
};

class Video {
protected:
    explicit Video(const char* path) noexcept
      : m_path(path), m_frames({}) {
        m_frames.reserve(1024);
    }

    bool init();

public:
    ~Video() noexcept;
    [[nodiscard]] static std::unique_ptr<Video> import(const char* path);

    const std::vector<Frame*>& frames();
    const Frame* frame(int i);

private:
    const char* m_path;
    std::vector<Frame*> m_frames;

    std::mutex m_mutex;
};

} // namespace avfx

#endif // AVFX_SRC_CORE_VIDEO_HPP
