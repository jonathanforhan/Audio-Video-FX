#ifndef AVFX_SRC_CORE_VIDEO_HPP
#define AVFX_SRC_CORE_VIDEO_HPP

#include <cstdint>
#include <memory>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

namespace avfx {

struct Frame {
    uint8_t *data = nullptr;
    int32_t width = -1;
    int32_t height = -1;
};

class Video {
protected:
    Video(const char* path) noexcept
        : m_path(path) {}
    bool init();

public:
    ~Video() noexcept;
    static std::unique_ptr<Video> import(const char* path); 

    // get data from frame at index n
    const uint8_t* get_frame_bytes(uint32_t index);
    int32_t get_frame_width(uint32_t index);
    int32_t get_frame_height(uint32_t index);
    const std::vector<Frame*>& get_frames();

private:
    const char *m_path;
    std::vector<Frame*> m_frames{};
};

} // namespace avfx

#endif // AVFX_SRC_CORE_VIDEO_HPP

