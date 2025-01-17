#include <transcode.h>

int open_input_file(const char *filename, TranscodingContext* tra_ctx)
{
    int ret;
    unsigned int i;

	tra_ctx->format_ctx = (FormatContext*) malloc(sizeof(FormatContext));
    tra_ctx->format_ctx->ifmt_ctx = avformat_alloc_context();
    
    av_register_all();
    avcodec_register_all();

    if ((ret = avformat_open_input(&tra_ctx->format_ctx->ifmt_ctx, filename, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(tra_ctx->format_ctx->ifmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    tra_ctx->stream_ctx = av_mallocz_array(tra_ctx->format_ctx->ifmt_ctx->nb_streams, sizeof(*tra_ctx->stream_ctx));
    if (!tra_ctx->stream_ctx)
        return AVERROR(ENOMEM);

    for (i = 0; i < tra_ctx->format_ctx->ifmt_ctx->nb_streams; i++) {
        AVStream *stream = tra_ctx->format_ctx->ifmt_ctx->streams[i];
        AVCodec *dec = avcodec_find_decoder(stream->codecpar->codec_id);
        AVCodecContext *codec_ctx;
        if (!dec) {
            av_log(NULL, AV_LOG_ERROR, "Failed to find decoder for stream #%u\n", i);
            return AVERROR_DECODER_NOT_FOUND;
        }
        codec_ctx = avcodec_alloc_context3(dec);
        if (!codec_ctx) {
            av_log(NULL, AV_LOG_ERROR, "Failed to allocate the decoder context for stream #%u\n", i);
            return AVERROR(ENOMEM);
        }
        ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Failed to copy decoder parameters to input decoder context "
                   "for stream #%u\n", i);
            return ret;
        }
        /* Reencode video & audio and remux subtitles etc. */
        if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
                || codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
                codec_ctx->framerate = av_guess_frame_rate(tra_ctx->format_ctx->ifmt_ctx, stream, NULL);
            /* Open decoder */
            ret = avcodec_open2(codec_ctx, dec, NULL);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to open decoder for stream #%u\n", i);
                return ret;
            }
        }
        tra_ctx->stream_ctx[i].dec_ctx = codec_ctx;
		tra_ctx->stream_ctx[i].dec_ctx->thread_count = 1; // Force the decoder to only use one thread
    }

    av_dump_format(tra_ctx->format_ctx->ifmt_ctx, 0, filename, 0);
    return 0;
}

int open_output_file(const char *filename, TranscodingContext* tra_ctx)
{
    AVStream *out_stream;
    AVStream *in_stream;
    AVCodecContext *dec_ctx, *enc_ctx;
    AVCodec *encoder;
    int ret;
    unsigned int i;

    tra_ctx->format_ctx->ofmt_ctx = NULL;
    avformat_alloc_output_context2(&tra_ctx->format_ctx->ofmt_ctx, NULL, NULL, filename);
    if (!tra_ctx->format_ctx->ofmt_ctx) {
        av_log(NULL, AV_LOG_ERROR, "Could not create output context\n");
        return AVERROR_UNKNOWN;
    }


    for (i = 0; i < tra_ctx->format_ctx->ifmt_ctx->nb_streams; i++) {
        out_stream = avformat_new_stream(tra_ctx->format_ctx->ofmt_ctx, NULL);
        if (!out_stream) {
            av_log(NULL, AV_LOG_ERROR, "Failed allocating output stream\n");
            return AVERROR_UNKNOWN;
        }

        in_stream = tra_ctx->format_ctx->ifmt_ctx->streams[i];
        dec_ctx = tra_ctx->stream_ctx[i].dec_ctx;

        if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {

            encoder = avcodec_find_encoder(enc_video_codec_id);
            if (!encoder) {
                av_log(NULL, AV_LOG_FATAL, "Necessary encoder not found\n");
                return AVERROR_INVALIDDATA;
            }
            enc_ctx = avcodec_alloc_context3(encoder);
            if (!enc_ctx) {
                av_log(NULL, AV_LOG_FATAL, "Failed to allocate the encoder context\n");
                return AVERROR(ENOMEM);
            }

			enc_ctx->thread_count = 1; // Force The Encoder to only use one thread
			
			enc_ctx->height = dec_ctx->height;
			enc_ctx->width = dec_ctx->width;
			enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
			/* take first format from list of supported formats */
			if (encoder->pix_fmts)
				enc_ctx->pix_fmt = encoder->pix_fmts[0];
			else
				enc_ctx->pix_fmt = dec_ctx->pix_fmt;
			/* video time_base can be set to whatever is handy and supported by encoder */
			enc_ctx->time_base = av_inv_q(dec_ctx->framerate);

            if (tra_ctx->format_ctx->ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            /* Third parameter can be used to pass settings to encoder */
            ret = avcodec_open2(enc_ctx, encoder, NULL);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
                return ret;
            }
            ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to copy encoder parameters to output stream #%u\n", i);
                return ret;
            }

            out_stream->time_base = enc_ctx->time_base;

            tra_ctx->stream_ctx[i].enc_ctx = enc_ctx;

		  } else if(dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
			  encoder = avcodec_find_encoder(enc_audio_codec_id);
            if (!encoder) {
                av_log(NULL, AV_LOG_FATAL, "Necessary encoder not found\n");
                return AVERROR_INVALIDDATA;
            }
            enc_ctx = avcodec_alloc_context3(encoder);
            if (!enc_ctx) {
                av_log(NULL, AV_LOG_FATAL, "Failed to allocate the encoder context\n");
                return AVERROR(ENOMEM);
            }

			enc_ctx->thread_count = 1; // Force The Encoder to only use one thread

			enc_ctx->sample_rate = dec_ctx->sample_rate;
                enc_ctx->channel_layout = dec_ctx->channel_layout;
                enc_ctx->channels = av_get_channel_layout_nb_channels(enc_ctx->channel_layout);
                /* take first format from list of supported formats */
                enc_ctx->sample_fmt = encoder->sample_fmts[0];
                enc_ctx->time_base = (AVRational){1, enc_ctx->sample_rate};

				 if (tra_ctx->format_ctx->ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            /* Third parameter can be used to pass settings to encoder */
            ret = avcodec_open2(enc_ctx, encoder, NULL);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
                return ret;
            }
            ret = avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Failed to copy encoder parameters to output stream #%u\n", i);
                return ret;
            }

            out_stream->time_base = enc_ctx->time_base;
            tra_ctx->stream_ctx[i].enc_ctx = enc_ctx;

        } else if (dec_ctx->codec_type == AVMEDIA_TYPE_UNKNOWN) {
            av_log(NULL, AV_LOG_FATAL, "Elementary stream #%d is of unknown type, cannot proceed\n", i);
            return AVERROR_INVALIDDATA;
        } else {
            /* if this stream must be remuxed */
            ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Copying parameters for stream #%u failed\n", i);
                return ret;
            }
            out_stream->time_base = in_stream->time_base;
        }

    }
    av_dump_format(tra_ctx->format_ctx->ofmt_ctx, 0, filename, 1);

    if (!(tra_ctx->format_ctx->ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&tra_ctx->format_ctx->ofmt_ctx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Could not open output file '%s'", filename);
            return ret;
        }
    }

    /* init muxer, write output file header */
    ret = avformat_write_header(tra_ctx->format_ctx->ofmt_ctx, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error occurred when opening output file\n");
        return ret;
    }

    return 0;
}

int init_filter(FilteringContext* fctx, AVCodecContext *dec_ctx,
        AVCodecContext *enc_ctx, const char *filter_spec, TranscodingContext* tra_ctx)
{
    char args[512];
    int ret = 0;
    const AVFilter *buffersrc = NULL;
    const AVFilter *buffersink = NULL;
    AVFilterContext *buffersrc_ctx = NULL;
    AVFilterContext *buffersink_ctx = NULL;
    
    avfilter_register_all();

    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    AVFilterGraph *filter_graph = avfilter_graph_alloc();

    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
        buffersrc = avfilter_get_by_name("buffer");
        buffersink = avfilter_get_by_name("buffersink");
        if (!buffersrc || !buffersink) {
            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        snprintf(args, sizeof(args),
                "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
                dec_ctx->time_base.num, dec_ctx->time_base.den,
                dec_ctx->sample_aspect_ratio.num,
                dec_ctx->sample_aspect_ratio.den);

        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                args, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
            goto end;
        }

        ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                NULL, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "pix_fmts",
                (uint8_t*)&enc_ctx->pix_fmt, sizeof(enc_ctx->pix_fmt),
                AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
            goto end;
        }
    } else if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        buffersrc = avfilter_get_by_name("abuffer");
        buffersink = avfilter_get_by_name("abuffersink");
        if (!buffersrc || !buffersink) {
            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        if (!dec_ctx->channel_layout)
            dec_ctx->channel_layout =
                av_get_default_channel_layout(dec_ctx->channels);
        snprintf(args, sizeof(args),
                "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%"PRIx64,
                dec_ctx->time_base.num, dec_ctx->time_base.den, dec_ctx->sample_rate,
                av_get_sample_fmt_name(dec_ctx->sample_fmt),
                dec_ctx->channel_layout);
        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                args, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
            goto end;
        }

        ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                NULL, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer sink\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "sample_fmts",
                (uint8_t*)&enc_ctx->sample_fmt, sizeof(enc_ctx->sample_fmt),
                AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "channel_layouts",
                (uint8_t*)&enc_ctx->channel_layout,
                sizeof(enc_ctx->channel_layout), AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "sample_rates",
                (uint8_t*)&enc_ctx->sample_rate, sizeof(enc_ctx->sample_rate),
                AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
            goto end;
        }
    } else {
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if (!outputs->name || !inputs->name) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_spec,
                    &inputs, &outputs, NULL)) < 0)
        goto end;

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;

    /* Fill FilteringContext */
    fctx->buffersrc_ctx = buffersrc_ctx;
    fctx->buffersink_ctx = buffersink_ctx;
    fctx->filter_graph = filter_graph;

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

int init_filters(TranscodingContext* tra_ctx)
{
    const char *filter_spec;
    unsigned int i;
    int ret;

    tra_ctx->filter_ctx = av_malloc_array(tra_ctx->format_ctx->ifmt_ctx->nb_streams, sizeof(*tra_ctx->filter_ctx));
    
	if (!tra_ctx->filter_ctx)
        return AVERROR(ENOMEM);

    for (i = 0; i < tra_ctx->format_ctx->ifmt_ctx->nb_streams; i++) {
        tra_ctx->filter_ctx[i].buffersrc_ctx  = NULL;
        tra_ctx->filter_ctx[i].buffersink_ctx = NULL;
        tra_ctx->filter_ctx[i].filter_graph   = NULL;
        if (!(tra_ctx->format_ctx->ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO
                || tra_ctx->format_ctx->ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO))
            continue;


        if (tra_ctx->format_ctx->ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            filter_spec = "null"; /* passthrough (dummy) filter for video */
        else
            filter_spec = "anull"; /* passthrough (dummy) filter for audio */
        ret = init_filter(&tra_ctx->filter_ctx[i], tra_ctx->stream_ctx[i].dec_ctx,
                tra_ctx->stream_ctx[i].enc_ctx, filter_spec, tra_ctx);
        if (ret)
            return ret;
    }
    return 0;
}

int encode_write_frame(AVFrame *filt_frame, unsigned int stream_index, int *got_frame, TranscodingContext* tra_ctx) {
    int ret;
    int got_frame_local;
    AVPacket enc_pkt;
    int (*enc_func)(AVCodecContext *, AVPacket *, const AVFrame *, int *) =
        (tra_ctx->format_ctx->ifmt_ctx->streams[stream_index]->codecpar->codec_type ==
         AVMEDIA_TYPE_VIDEO) ? avcodec_encode_video2 : avcodec_encode_audio2;

    if (!got_frame)
        got_frame = &got_frame_local;

    // av_log(NULL, AV_LOG_INFO, "Encoding frame\n");
    /* encode filtered frame */
    enc_pkt.data = NULL;
    enc_pkt.size = 0;
    av_init_packet(&enc_pkt);
    ret = enc_func(tra_ctx->stream_ctx[stream_index].enc_ctx, &enc_pkt,
            filt_frame, got_frame);
    av_frame_free(&filt_frame);
    if (ret < 0)
        return ret;
    if (!(*got_frame))
        return 0;

    /* prepare packet for muxing */
    enc_pkt.stream_index = stream_index;
    av_packet_rescale_ts(&enc_pkt,
                         tra_ctx->stream_ctx[stream_index].enc_ctx->time_base,
                         tra_ctx->format_ctx->ofmt_ctx->streams[stream_index]->time_base);

    av_log(NULL, AV_LOG_DEBUG, "Muxing frame\n");
    /* mux encoded frame */
    ret = av_interleaved_write_frame(tra_ctx->format_ctx->ofmt_ctx, &enc_pkt);
    return ret;
}

int filter_encode_write_frame(AVFrame *frame, unsigned int stream_index, TranscodingContext* tra_ctx)
{
    int ret;
    AVFrame *filt_frame;

    // av_log(NULL, AV_LOG_INFO, "Pushing decoded frame to filters\n");
    /* push the decoded frame into the filtergraph */
    ret = av_buffersrc_add_frame_flags(tra_ctx->filter_ctx[stream_index].buffersrc_ctx,
            frame, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
        return ret;
    }

    /* pull filtered frames from the filtergraph */
    while (1) {
        filt_frame = av_frame_alloc();
        if (!filt_frame) {
            ret = AVERROR(ENOMEM);
            break;
        }
        // av_log(NULL, AV_LOG_INFO, "Pulling filtered frame from filters\n");
        ret = av_buffersink_get_frame(tra_ctx->filter_ctx[stream_index].buffersink_ctx,
                filt_frame);
        if (ret < 0) {
            /* if no more frames for output - returns AVERROR(EAGAIN)
             * if flushed and no more frames for output - returns AVERROR_EOF
             * rewrite retcode to 0 to show it as normal procedure completion
             */
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                ret = 0;
            av_frame_free(&filt_frame);
            break;
        }

        filt_frame->pict_type = AV_PICTURE_TYPE_NONE;
        ret = encode_write_frame(filt_frame, stream_index, NULL, tra_ctx);
        if (ret < 0)
            break;
    }

    return ret;
}

int flush_encoder(unsigned int stream_index, TranscodingContext* tra_ctx)
{
    int ret;
    int got_frame;

    if (!(tra_ctx->stream_ctx[stream_index].enc_ctx->codec->capabilities &
                AV_CODEC_CAP_DELAY))
        return 0;

    while (1) {
        // av_log(NULL, AV_LOG_INFO, "Flushing stream #%u encoder\n", stream_index);
        ret = encode_write_frame(NULL, stream_index, &got_frame, tra_ctx);
        if (ret < 0)
            break;
        if (!got_frame)
            return 0;
    }
    return ret;
}

int transcode_video(char* input_file, char* outputFile, TranscodingContext* tra_ctx)
{
    int ret;
    AVPacket packet = { .data = NULL, .size = 0 };
    AVFrame *frame = NULL;
    enum AVMediaType type;
    unsigned int stream_index;
    unsigned int i;
    int got_frame;
    int (*dec_func)(AVCodecContext *, AVFrame *, int *, const AVPacket *);

    if ((ret = open_input_file(input_file, tra_ctx)) < 0)
        goto end;
    if ((ret = open_output_file(outputFile, tra_ctx)) < 0)
        goto end;
    if ((ret = init_filters(tra_ctx)) < 0)
        goto end;

    /* read all packets */
    while (1) {
        if ((ret = av_read_frame(tra_ctx->format_ctx->ifmt_ctx, &packet)) < 0)
            break;
        stream_index = packet.stream_index;
        type = tra_ctx->format_ctx->ifmt_ctx->streams[packet.stream_index]->codecpar->codec_type;
        av_log(NULL, AV_LOG_DEBUG, "Demuxer gave frame of stream_index %u\n",
                stream_index);

        if (tra_ctx->filter_ctx[stream_index].filter_graph) {
            av_log(NULL, AV_LOG_DEBUG, "Going to Re-encode & filter the frame\n");
            frame = av_frame_alloc();
            if (!frame) {
                ret = AVERROR(ENOMEM);
                break;
            }
            av_packet_rescale_ts(&packet,
                                 tra_ctx->format_ctx->ifmt_ctx->streams[stream_index]->time_base,
                                 tra_ctx->stream_ctx[stream_index].dec_ctx->time_base);
            dec_func = (type == AVMEDIA_TYPE_VIDEO) ? avcodec_decode_video2 :
                avcodec_decode_audio4;
            ret = dec_func(tra_ctx->stream_ctx[stream_index].dec_ctx, frame,
                    &got_frame, &packet);
            if (ret < 0) {
                av_frame_free(&frame);
                av_log(NULL, AV_LOG_ERROR, "Decoding failed\n");
                break;
            }

            if (got_frame) {
                frame->pts = frame->best_effort_timestamp;
                ret = filter_encode_write_frame(frame, stream_index, tra_ctx);
                av_frame_free(&frame);
                if (ret < 0)
                    goto end;
            } else {
                av_frame_free(&frame);
            }
        } else {
            /* remux this frame without reencoding */
            av_packet_rescale_ts(&packet,
                                 tra_ctx->format_ctx->ifmt_ctx->streams[stream_index]->time_base,
                                 tra_ctx->format_ctx->ofmt_ctx->streams[stream_index]->time_base);

            ret = av_interleaved_write_frame(tra_ctx->format_ctx->ofmt_ctx, &packet);
            if (ret < 0)
                goto end;
        }
        av_packet_unref(&packet);
    }

    /* flush filters and encoders */
    for (i = 0; i < tra_ctx->format_ctx->ifmt_ctx->nb_streams; i++) {
        /* flush filter */
        if (!tra_ctx->filter_ctx[i].filter_graph)
            continue;
        ret = filter_encode_write_frame(NULL, i, tra_ctx);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Flushing filter failed\n");
            goto end;
        }

        /* flush encoder */
        ret = flush_encoder(i, tra_ctx);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Flushing encoder failed\n");
            goto end;
        }
    }

    av_write_trailer(tra_ctx->format_ctx->ofmt_ctx);
end:
    av_packet_unref(&packet);
    av_frame_free(&frame);
    if(tra_ctx->format_ctx->ifmt_ctx != NULL) {
        for (i = 0; i < tra_ctx->format_ctx->ifmt_ctx->nb_streams; i++) {
            avcodec_free_context(&tra_ctx->stream_ctx[i].dec_ctx);
            if (tra_ctx->format_ctx->ofmt_ctx && 
			tra_ctx->format_ctx->ofmt_ctx->nb_streams > i && 
			tra_ctx->format_ctx->ofmt_ctx->streams[i] && 
			tra_ctx->stream_ctx[i].enc_ctx)
                avcodec_free_context(&tra_ctx->stream_ctx[i].enc_ctx);
            if (tra_ctx->filter_ctx && tra_ctx->filter_ctx[i].filter_graph)
                avfilter_graph_free(&tra_ctx->filter_ctx[i].filter_graph);
        }
        avformat_close_input(&tra_ctx->format_ctx->ifmt_ctx);
    
		av_free(tra_ctx->filter_ctx);
		av_free(tra_ctx->stream_ctx);
		if (tra_ctx->format_ctx->ofmt_ctx && !(tra_ctx->format_ctx->ofmt_ctx->oformat->flags & AVFMT_NOFILE))
			avio_closep(&tra_ctx->format_ctx->ofmt_ctx->pb);
		avformat_free_context(tra_ctx->format_ctx->ofmt_ctx);
	}

    if (ret < 0)
        av_log(NULL, AV_LOG_ERROR, "Error occurred: %s\n", av_err2str(ret));

    return ret ? 1 : 0;
}
