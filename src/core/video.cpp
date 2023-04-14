#include "video.hpp"
#include <cassert>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/timecode.h>
#include <libswscale/swscale.h>
}

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 28, 1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

namespace avfx {

Video::Video(const char* path) noexcept
  : m_path(path),
    m_frames({}),
    m_mutex(),
    m_done(false),
    m_error(NO_ERROR) {
    m_frames.reserve(1024);
}

Video::~Video() noexcept {
    for (auto& frame : m_frames) {
        delete frame;
    }
}

void Video::stream_decode() {
    std::thread t(
            _stream_decode,
            m_path,
            &m_frames,
            &m_mutex,
            &m_done,
            &m_error,
            &m_frame_rate);
    t.detach();
}

void Video::log_decode() {
    switch (m_error) {
    case NO_ERROR:
        break;
    case FILE_ERROR:
        fprintf(stderr, "FILE ERROR\n");
        break;
    case STREAM_NOT_FOUND:
        fprintf(stderr, "STREAM NOT FOUND");
        break;
    case UNSUPPORTED_CODEC:
        fprintf(stderr, "UNSUPPORTED CODEC");
        break;
    case CODEC_OPEN_ERROR:
        fprintf(stderr, "CODEC OPEN ERROR");
        break;
    case MALLOC_ERROR:
        fprintf(stderr, "MALLOC ERROR");
        break;
    }
}

void Video::_stream_decode(
        const char* path,
        std::vector<Frame*>* frames,
        std::mutex* mutex,
        bool* done,
        decode_error* error,
        double* frame_rate) {

    AVFormatContext* fmt_ctx        = NULL;
    AVCodecParameters* codec_params = NULL;
    AVCodecContext* codec_ctx       = NULL;
    AVCodec* codec                  = NULL;
    AVFrame* frame_yuv              = NULL;
    AVFrame* frame_rgb              = NULL;
    AVPacket* packet                = NULL;
    uint8_t* buffer                 = NULL;
    SwsContext* sws_ctx             = NULL;

    // thread-safe var assignment
    auto safe_assign = [&](auto* var, auto x) {
        mutex->lock();
        *var = x;
        mutex->unlock();
    };

    fmt_ctx = avformat_alloc_context();
    assert(fmt_ctx != NULL);

    // open file
    if (avformat_open_input(&fmt_ctx, path, NULL, NULL) != 0) {
        safe_assign(error, FILE_ERROR);
    }

    // retreive stream info
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        safe_assign(error, STREAM_NOT_FOUND);
    }

    // dump file info to console
#if !defined NDEBUG || defined _DEBUG
    av_dump_format(fmt_ctx, 0, path, 0);
#endif

    // find first video stream
    const int video_stream_index = av_find_best_stream(fmt_ctx,
            AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
    if (video_stream_index == -1) {
        safe_assign(error, STREAM_NOT_FOUND);
    }
    const AVStream* stream = fmt_ctx->streams[video_stream_index];
    const double fps       = (double)stream->r_frame_rate.num /
            (double)stream->r_frame_rate.den;
    safe_assign(frame_rate, fps);

    codec_params = fmt_ctx->streams[video_stream_index]->codecpar;
    // find decoder for video stream
    codec = avcodec_find_decoder(codec_params->codec_id);
    if (codec == NULL) {
        safe_assign(error, UNSUPPORTED_CODEC);
    }

    // copy context
    codec_ctx = avcodec_alloc_context3(codec);
    assert(codec_ctx != NULL);
    if (avcodec_parameters_to_context(codec_ctx, codec_params) < 0) {
        safe_assign(error, MALLOC_ERROR);
    }

    // assign threads
    codec_ctx->thread_count = 6;
    codec_ctx->thread_type  = FF_THREAD_FRAME;

    // open codec
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        safe_assign(error, CODEC_OPEN_ERROR);
    }

    // alloc video frame
    frame_yuv = av_frame_alloc();
    frame_rgb = av_frame_alloc();
    assert(frame_rgb != NULL && frame_yuv != NULL);
    if (frame_rgb == NULL) {
        safe_assign(error, MALLOC_ERROR);
    }

    const int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
            codec_ctx->width, codec_ctx->height, 16);

    buffer = (uint8_t*)av_malloc(num_bytes * sizeof(uint8_t));
    assert(buffer != NULL);

    av_image_fill_arrays(frame_rgb->data, frame_rgb->linesize, buffer,
            AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height, 1);

    // initialize SWS context for software scaling
    sws_ctx = sws_getContext(
            codec_ctx->width,
            codec_ctx->height,
            codec_ctx->pix_fmt,
            codec_ctx->width,
            codec_ctx->height,
            AV_PIX_FMT_RGB24,
            SWS_BILINEAR,
            NULL, NULL, NULL);

    packet = av_packet_alloc();
    assert(packet != NULL);

    while (av_read_frame(fmt_ctx, packet) >= 0) {
        if (packet->stream_index != video_stream_index) {
            av_packet_unref(packet);
            continue;
        }
        int res = avcodec_send_packet(codec_ctx, packet);
        while (res >= 0) {
            res = avcodec_receive_frame(codec_ctx, frame_yuv);
            if (res == AVERROR(EAGAIN) || res == AVERROR_EOF) {
                break;
            } else if (res < 0) {
                fprintf(stderr, "error while receiving frame from decoder\n");
            }

            const size_t data_len = 3 * codec_ctx->width * codec_ctx->height;

            sws_scale(sws_ctx, (uint8_t const* const*)frame_yuv->data,
                    frame_yuv->linesize, 0, codec_ctx->height,
                    frame_rgb->data, frame_rgb->linesize);

            const int w = codec_ctx->width;
            const int h = codec_ctx->height;
            mutex->lock();
            frames->emplace_back(
                    new Frame(*frame_rgb->data, data_len, w, h));
            mutex->unlock();
        }
        av_packet_unref(packet);
    }

    avformat_close_input(&fmt_ctx);
    av_packet_free(&packet);
    av_frame_free(&frame_yuv);
    av_frame_free(&frame_rgb);
    avcodec_free_context(&codec_ctx);

    safe_assign(done, true);
}

const std::vector<Frame*>& Video::frames() {
    return m_frames;
}

const Frame* Video::frame(int i) {
    return m_frames[i];
}

size_t Video::frame_count() {
    return m_frames.size();
}

double Video::frame_rate() {
    return m_frame_rate;
}

} // namespace avfx
