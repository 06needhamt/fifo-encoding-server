#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>

 AVFormatContext *ifmt_ctx;
 AVFormatContext *ofmt_ctx;

typedef struct FilteringContext {
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
} FilteringContext;

 FilteringContext *filter_ctx;

typedef struct StreamContext {
    AVCodecContext *dec_ctx;
    AVCodecContext *enc_ctx;
} StreamContext;

 StreamContext *stream_ctx;

 int open_input_file(const char *filename);
 int open_output_file(const char *filename);
 int init_filter(FilteringContext* fctx, AVCodecContext *dec_ctx,
        AVCodecContext *enc_ctx, const char *filter_spec);
 int init_filters(void);
 int encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame);
 int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index);
 int flush_encoder(unsigned int stream_index);
int transcode_video(char* input_file, char* outputFile);