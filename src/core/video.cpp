#include "video.hpp"
#include <cassert>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 28, 1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

namespace avfx {

bool Video::init() {
    AVFormatContext* fmt_ctx        = NULL;
    AVCodecParameters* codec_params = NULL;
    AVCodecContext* codec_ctx       = NULL;
    AVCodec* codec                  = NULL;
    AVFrame* frame_yuv              = NULL;
    AVFrame* frame_rgb              = NULL;
    AVPacket* packet                = NULL;
    uint8_t* buffer                 = NULL;
    SwsContext* sws_ctx             = NULL;


    fmt_ctx = avformat_alloc_context();
    assert(fmt_ctx != NULL);

    // open file
    if (avformat_open_input(&fmt_ctx, m_path, NULL, NULL) != 0) {
        fprintf(stderr, "Could not open file\n");
        return false;
    }

    // retreive stream info
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Stream info not found\n");
        return false;
    }

    // dump file info to console
#if !defined NDEBUG || defined _DEBUG
    av_dump_format(fmt_ctx, 0, m_path, 0);
#endif

    // find first video stream
    int video_stream_index = -1;
    for (size_t i = 0; i < fmt_ctx->nb_streams; ++i) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1)
        return false;

    codec_params = fmt_ctx->streams[video_stream_index]->codecpar;
    // find decoder for video stream
    codec = avcodec_find_decoder(codec_params->codec_id);
    if (codec == NULL) {
        fprintf(stderr, "Unsupported codec\n");
        return false;
    }

    // copy context
    codec_ctx = avcodec_alloc_context3(codec);
    assert(codec_ctx != NULL);
    if (avcodec_parameters_to_context(codec_ctx, codec_params) < 0) {
        fprintf(stderr, "Copy codec failed\n");
        return false;
    }

    // assign threads
    codec_ctx->thread_count = 8;
    codec_ctx->thread_type  = FF_THREAD_FRAME;

    // open codec
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        return false;
    }

    // alloc video frame
    frame_yuv = av_frame_alloc();
    frame_rgb = av_frame_alloc();
    assert(frame_rgb != NULL && frame_yuv != NULL);
    if (frame_rgb == NULL) {
        return false;
    }

    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
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
        int res = 0;
        res     = avcodec_send_packet(codec_ctx, packet);
        while (res >= 0) {
            res = avcodec_receive_frame(codec_ctx, frame_yuv);
            if (res == AVERROR(EAGAIN) || res == AVERROR_EOF) {
                break;
            } else if (res < 0) {
                fprintf(stderr, "error while receiving frame from decoder\n");
            }

            size_t data_len = 3 * codec_ctx->width * codec_ctx->height;

            sws_scale(sws_ctx, (uint8_t const* const*)frame_yuv->data,
                    frame_yuv->linesize, 0, codec_ctx->height,
                    frame_rgb->data, frame_rgb->linesize);

            int w = codec_ctx->width;
            int h = codec_ctx->height;
            m_frames.emplace_back(new Frame(*frame_rgb->data, data_len, w, h));
        }
        av_packet_unref(packet);
    }

    avformat_close_input(&fmt_ctx);
    av_packet_free(&packet);
    av_frame_free(&frame_yuv);
    av_frame_free(&frame_rgb);
    avcodec_free_context(&codec_ctx);

    return true;
}

Video::~Video() noexcept {
    for (auto& frame : m_frames) {
        delete frame;
    }
}

std::unique_ptr<Video> Video::import(const char* p_path) {
    std::unique_ptr<Video> v(new Video(p_path));
    if (v->init())
        return v;
    else
        return nullptr;
}

const std::vector<Frame*>& Video::frames() {
    return m_frames;
}

const Frame* Video::frame(int i) {
    return m_frames[i];
}

} // namespace avfx
