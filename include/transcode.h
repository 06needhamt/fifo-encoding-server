#pragma once
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <pthread.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>

 static enum AVCodecID enc_video_codec_id = AV_CODEC_ID_H264;
 static enum AVCodecID enc_audio_codec_id = AV_CODEC_ID_AAC;

typedef struct FilteringContext {
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
} FilteringContext;

typedef struct StreamContext {
    AVCodecContext *dec_ctx;
    AVCodecContext *enc_ctx;
} StreamContext;

typedef struct FormatContext {
	AVFormatContext *ifmt_ctx;
	AVFormatContext *ofmt_ctx;
} FormatContext;

typedef struct TranscodingContext {
	FormatContext * format_ctx;
	FilteringContext *filter_ctx;
	StreamContext *stream_ctx;
} TranscodingContext;

 int open_input_file(const char *filename, TranscodingContext* tra_ctx);
 int open_output_file(const char *filename, TranscodingContext* tra_ctx);
 int init_filter(FilteringContext* fctx, AVCodecContext *dec_ctx,
        AVCodecContext *enc_ctx, const char *filter_spec, TranscodingContext* tra_ctx);
 int init_filters(TranscodingContext* tra_ctx);
 int encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame, TranscodingContext* tra_ctx);
 int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index, TranscodingContext* tra_ctx);
 int flush_encoder(unsigned int stream_index, TranscodingContext* tra_ctx);
int transcode_video(char* input_file, char* outputFile, TranscodingContext* tra_ctx);
