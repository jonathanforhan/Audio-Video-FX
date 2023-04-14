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

enum decode_error {
    NO_ERROR,
    FILE_ERROR,
    STREAM_NOT_FOUND,
    UNSUPPORTED_CODEC,
    CODEC_OPEN_ERROR,
    MALLOC_ERROR,
};

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
public:
    explicit Video(const char* path) noexcept;
    ~Video() noexcept;

    void stream_decode();
    inline void lock_stream() { m_mutex.lock(); }
    inline void unlock_stream() { m_mutex.unlock(); }
    inline bool stream_done() { return m_done; }
    void log_decode();

    const std::vector<Frame*>& frames();
    const Frame* frame(int i);
    size_t frame_count();
    double frame_rate();

private:
    static void _stream_decode(
            const char* path,
            std::vector<Frame*>* frames,
            std::mutex* mutex,
            bool* done,
            decode_error* error,
            double* frame_rate);

    const char* m_path;
    std::vector<Frame*> m_frames;
    double m_frame_rate;

    std::mutex m_mutex;
    bool m_done;
    enum decode_error m_error;
};

} // namespace avfx

#endif // AVFX_SRC_CORE_VIDEO_HPP
