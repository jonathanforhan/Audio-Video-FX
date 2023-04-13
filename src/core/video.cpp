#include "video.hpp"
#include <cassert>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

namespace avfx {

bool Video::init() {
    AVFormatContext *format_ctx = NULL;
    AVCodecParameters  *codec_params = NULL;
    AVCodecContext  *codec_ctx = NULL;
    AVCodec         *codec = NULL;
    AVFrame         *frame_yuv = NULL;
    AVFrame         *frame_rgb = NULL;
    AVPacket        *packet = NULL;
    uint8_t         *buffer = NULL;
    SwsContext      *sws_ctx = NULL;

    format_ctx = avformat_alloc_context();
    assert(format_ctx != NULL);

    // open file
    if (avformat_open_input(&format_ctx, m_path, NULL, NULL) != 0) {
        fprintf(stderr, "Could not open file\n");
        return false;
    }

    // retreive stream info
    if (avformat_find_stream_info(format_ctx, NULL) < 0) {
        fprintf(stderr, "Stream info not found\n");
        return false;
    }

    // dump file info to console
#if !defined NDEBUG || defined _DEBUG
    av_dump_format(format_ctx, 0, m_path, 0);
#endif

    // find first video stream
    int video_stream_index = -1;
    for (size_t i = 0; i < format_ctx->nb_streams; ++i) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1)
        return false;

    codec_params = format_ctx->streams[video_stream_index]->codecpar;
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

    while (av_read_frame(format_ctx, packet) >= 0) {
        if (packet->stream_index != video_stream_index) {
            av_packet_unref(packet);
            continue;
        }
        int res = 0;
        res = avcodec_send_packet(codec_ctx, packet);
        while (res >= 0) {
            res = avcodec_receive_frame(codec_ctx, frame_yuv);
            if (res == AVERROR(EAGAIN) || res == AVERROR_EOF) {
                break;
            }
            else if (res < 0) {
                fprintf(stderr, "error while receiving frame from decoder\n");
            }
            sws_scale(sws_ctx, (uint8_t const* const*)frame_yuv->data,
                    frame_yuv->linesize, 0, codec_ctx->height,
                    frame_rgb->data, frame_rgb->linesize);

            Frame *avfx_frame = new Frame;
            size_t data_len = 3 * codec_ctx->width * codec_ctx->height;
            buffer = (uint8_t*)malloc(data_len);
            memcpy(buffer, frame_rgb->data, data_len);
            avfx_frame->data = buffer;
            avfx_frame->width = codec_ctx->width;
            avfx_frame->height = codec_ctx->height;
            m_frames.push_back(avfx_frame);
        }
        av_packet_unref(packet);
    }

    avformat_close_input(&format_ctx);
    av_packet_free(&packet);
    av_frame_free(&frame_yuv);
    av_frame_free(&frame_rgb);
    avcodec_free_context(&codec_ctx);

    return true;
}

Video::~Video() noexcept {
    for (auto& frame : m_frames) {
        delete frame->data;
    }
}

std::unique_ptr<Video> Video::import(const char* p_path) {
    Video *ret = new Video(p_path);
    if (ret->init()) {
        return std::unique_ptr<Video>(ret);
    }
    delete ret;
    return std::unique_ptr<Video>(nullptr);
} 

const uint8_t* Video::get_frame_bytes(uint32_t p_index) {
    if (p_index < m_frames.size()) {
        return m_frames[p_index]->data;
    }
    return nullptr;
}

int32_t Video::get_frame_width(uint32_t p_index) {
    if (p_index < m_frames.size()) {
        return m_frames[p_index]->width;
    }
    return -1;
}

int32_t Video::get_frame_height(uint32_t p_index) {
    if (p_index < m_frames.size()) {
        return m_frames[p_index]->height;
    }
    return -1;
}

const std::vector<Frame*>& Video::get_frames() {
    return m_frames;
}

} // namespace avfx

