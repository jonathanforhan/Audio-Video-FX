#include "video.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

namespace avfx {

bool Video::init() {
    // Initalizing these to NULL prevents segfaults!
    AVFormatContext   *pFormatCtx = NULL;
    int               i, videoStream;
    AVCodecContext    *pCodecCtxOrig = NULL;
    AVCodecContext    *pCodecCtx = NULL;
    AVCodec           *pCodec = NULL;
    AVFrame           *pFrame = NULL;
    AVFrame           *pFrameRGB = NULL;
    AVPacket          packet;
    int               frameFinished;
    int               numBytes;
    uint8_t           *buffer = NULL;
    struct SwsContext *sws_ctx = NULL;

    // Open video file
    if (avformat_open_input(&pFormatCtx, m_path, NULL, NULL) != 0) {
        return false;
    }

    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        return false;
    }

    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, m_path, 0);

    // Find the first video stream
    videoStream = -1;
    for(i = 0; i < (int)pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }

    if (videoStream == -1) {
        return false;
    }

    // Get a pointer to the codec context for the video stream
    pCodecCtxOrig = pFormatCtx->streams[videoStream]->codec;
    // Find the decoder for the video stream
    pCodec = avcodec_find_decoder(pCodecCtxOrig->codec_id);
    if(pCodec == NULL) {
        fprintf(stderr, "Unsupported codec!\n");
        return false; // Codec not found
    }
    // Copy context
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if(avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
        fprintf(stderr, "Couldn't copy codec context");
        return false; // Error copying codec context
    }

    // Open codec
    if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
        return false; // Could not open codec

    // Allocate video frame
    pFrame = av_frame_alloc();

    // Allocate an AVFrame structure
    pFrameRGB = av_frame_alloc();
    if(pFrameRGB == NULL) {
        return false;
    }

    // Determine required buffer size and allocate buffer
    numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
            pCodecCtx->height);
    buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24,
            pCodecCtx->width, pCodecCtx->height);

    // initialize SWS context for software scaling
    sws_ctx = sws_getContext(pCodecCtx->width,
            pCodecCtx->height,
            pCodecCtx->pix_fmt,
            pCodecCtx->width,
            pCodecCtx->height,
            AV_PIX_FMT_RGB24,
            SWS_BILINEAR,
            NULL,
            NULL,
            NULL
            );

    // Read frames and save first five frames to disk
    i = 0;
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        // Is this a packet from the video stream?
        if (packet.stream_index == videoStream) {
            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // Did we get a video frame?
            if (frameFinished) {
                int width = pCodecCtx->width;
                int height = pCodecCtx->height;
                // Convert the image from its native format to RGB
                sws_scale(sws_ctx, (uint8_t const* const*)pFrame->data,
                        pFrame->linesize, 0, pCodecCtx->height,
                        pFrameRGB->data, pFrameRGB->linesize);

                Frame *frame = new Frame;
                uint8_t *data = (uint8_t*)malloc(3 * width * height);
                memcpy(data, *pFrameRGB->data, 3 * width * height);
                frame->data = data;
                frame->width = width;
                frame->height = height;
                m_frames.push_back(frame);
                i++;
            }
        }

        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }

    sws_freeContext(sws_ctx);

    // Free the RGB image
    av_free(buffer);
    av_frame_free(&pFrameRGB);

    // Free the YUV frame
    av_frame_free(&pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);
    avcodec_close(pCodecCtxOrig);

    // Close the video file
    avformat_close_input(&pFormatCtx);

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

