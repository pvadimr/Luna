#ifndef OGGDEC_H
#define OGGDEC_H

#include "../config.h"
#ifdef OPENAL
/* OggDec
 *
 * This program is distributed under the GNU General Public License, version 2.
 * A copy of this license is included with this source.
 *
 * Copyright 2002, Michael Smith <msmith@xiph.org>
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <locale.h>

#if defined(_WIN32) || defined(__EMX__) || defined(__WATCOMC__)
#include <fcntl.h>
#include <io.h>
#endif

#include <vorbis/vorbisfile.h>



    //#include "i18n.h"     // put the header into the source file
    #ifdef HAVE_CONFIG_H
    #include <config.h>
    #endif

    #ifdef ENABLE_NLS
    #include <locale.h>
    #include <libintl.h>
    #define _(String) gettext (String)
    #else
    #define _(String) (String)
    #endif

    #ifndef HAVE_RPMATCH
    #define rpmatch(line) \
        ( (line == NULL)? -1 : \
          (*line == 'y' || *line == 'Y')? 1 : \
          (*line == 'n' || *line == 'N')? 0 : \
          -1 )
    #endif


struct option long_options[] = {
    {"quiet", 0,0,'Q'},
    {"help",0,0,'h'},
    {"version", 0, 0, 'V'},
    {"bits", 1, 0, 'b'},
    {"endianness", 1, 0, 'e'},
    {"raw", 0, 0, 'R'},
    {"sign", 1, 0, 's'},
    {"output", 1, 0, 'o'},
    {NULL,0,0,0}
};

static int quiet = 0;
static int bits = 16;
static int endian = 0;
static int raw = 0;
static int sign = 1;
unsigned char headbuf[44]; /* The whole buffer */
char *outfilename = NULL;

#define WRITE_U32(buf, x) *(buf)     = (unsigned char)((x)&0xff);\
                          *((buf)+1) = (unsigned char)(((x)>>8)&0xff);\
                          *((buf)+2) = (unsigned char)(((x)>>16)&0xff);\
                          *((buf)+3) = (unsigned char)(((x)>>24)&0xff);

#define WRITE_U16(buf, x) *(buf)     = (unsigned char)((x)&0xff);\
                          *((buf)+1) = (unsigned char)(((x)>>8)&0xff);

/* Some of this based on ao/src/ao_wav.c */
int write_prelim_header(OggVorbis_File *vf, FILE *out, ogg_int64_t knownlength) {
    unsigned int size = 0x7fffffff;
    int channels = ov_info(vf,0)->channels;
    int samplerate = ov_info(vf,0)->rate;
    int bytespersec = channels*samplerate*bits/8;
    int align = channels*bits/8;
    int samplesize = bits;

    if(knownlength && knownlength*bits/8*channels < size)
        size = (unsigned int)(knownlength*bits/8*channels+44) ;

    memcpy(headbuf, "RIFF", 4);
    WRITE_U32(headbuf+4, size-8);
    memcpy(headbuf+8, "WAVE", 4);
    memcpy(headbuf+12, "fmt ", 4);
    WRITE_U32(headbuf+16, 16);
    WRITE_U16(headbuf+20, 1); /* format */
    WRITE_U16(headbuf+22, channels);
    WRITE_U32(headbuf+24, samplerate);
    WRITE_U32(headbuf+28, bytespersec);
    WRITE_U16(headbuf+32, align);
    WRITE_U16(headbuf+34, samplesize);
    memcpy(headbuf+36, "data", 4);
    WRITE_U32(headbuf+40, size - 44);

    if(fwrite(headbuf, 1, 44, out) != 44) {
        fprintf(stderr, _("ERROR: Failed to write Wave header: %s\n"), strerror(errno));
        return 1;
    }

    return 0;
}

int rewrite_header(FILE *out, unsigned int written)
{
    unsigned int length = written;

    length += 44;

    WRITE_U32(headbuf+4, length-8);
    WRITE_U32(headbuf+40, length-44);
    if(fseek(out, 0, SEEK_SET) != 0)
        return 1;

    if(fwrite(headbuf, 1, 44, out) != 44) {
        fprintf(stderr, _("ERROR: Failed to write Wave header: %s\n"), strerror(errno));
        return 1;
    }
    return 0;
}

static FILE *open_input(char *infile)
{
    FILE *in;

    if(!infile) {
#ifdef __BORLANDC__
        setmode(fileno(stdin), O_BINARY);
#elif _WIN32
        _setmode(_fileno(stdin), _O_BINARY);
#endif
        in = stdin;
    }
    else {
        in = fopen(infile, "rb");
        if(!in) {
            fprintf(stderr, _("ERROR: Failed to open input file: %s\n"), strerror(errno));
            return NULL;
        }
    }

    return in;
}

static FILE *open_output(char *outfile)
{
    FILE *out;
    if(!outfile) {
#ifdef __BORLANDC__
        setmode(fileno(stdout), O_BINARY);
#elif _WIN32
        _setmode(_fileno(stdout), _O_BINARY);
#endif
        out = stdout;
    }
    else {
        out = fopen(outfile, "wb");
        if(!out) {
            fprintf(stderr, _("ERROR: Failed to open output file: %s\n"), strerror(errno));
            return NULL;
        }
    }

    return out;
}

static void
permute_channels(char *in, char *out, int len, int channels, int bytespersample)
{
    int permute[6][6] = {{0}, {0,1}, {0,2,1}, {0,1,2,3}, {0,1,2,3,4},
        {0,2,1,5,3,4}};
    int i,j,k;
    int samples = len/channels/bytespersample;

    /* Can't handle, don't try */
    if (channels > 6)
        return;

    for (i=0; i < samples; i++) {
        for (j=0; j < channels; j++) {
            for (k=0; k < bytespersample; k++) {
                out[i*bytespersample*channels +
                    bytespersample*permute[channels-1][j] + k] =
                    in[i*bytespersample*channels + bytespersample*j + k];
            }
        }
    }
}

static int decode_file(FILE *in, FILE *out, char *infile, char *outfile)
{
    OggVorbis_File vf;
    int bs = 0;
    char buf[8192], outbuf[8192];
    char *p_outbuf;
    int buflen = 8192;
    unsigned int written = 0;
    int ret;
    ogg_int64_t length = 0;
    ogg_int64_t done = 0;
    int size = 0;
    int seekable = 0;
    int percent = 0;
    int channels;
    int samplerate;

    if (ov_open_callbacks(in, &vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0) {
        fprintf(stderr, _("ERROR: Failed to open input as Vorbis\n"));
        fclose(in);
        return 1;
    }

    channels = ov_info(&vf,0)->channels;
    samplerate = ov_info(&vf,0)->rate;

    if(ov_seekable(&vf)) {
        int link;
        int chainsallowed = 0;
        for(link = 0; link < ov_streams(&vf); link++) {
            if(ov_info(&vf, link)->channels == channels &&
                    ov_info(&vf, link)->rate == samplerate)
            {
                chainsallowed = 1;
            }
        }

        seekable = 1;
        if(chainsallowed)
            length = ov_pcm_total(&vf, -1);
        else
            length = ov_pcm_total(&vf, 0);
        size = bits/8 * channels;
        if(!quiet)
            fprintf(stderr, _("Decoding \"%s\" to \"%s\"\n"),
                    infile?infile:_("standard input"),
                    outfile?outfile:_("standard output"));
    }

    if(!raw) {
        if(write_prelim_header(&vf, out, length)) {
            ov_clear(&vf);
            return 1;
        }
    }

    while((ret = ov_read(&vf, buf, buflen, endian, bits/8, sign, &bs)) != 0) {
        if(bs != 0) {
            vorbis_info *vi = ov_info(&vf, -1);
            if(channels != vi->channels || samplerate != vi->rate) {
                fprintf(stderr, _("Logical bitstreams with changing parameters are not supported\n"));
                break;
            }
        }

        if(ret == OV_HOLE) {
            if(!quiet) {
                fprintf(stderr, _("WARNING: hole in data (%d)\n"), ret);
            }
            continue;
        }
        else if(ret < 0) {
            if(!quiet) {
                fprintf(stderr, _("=== Vorbis library reported a stream error.\n"));
            }
            ov_clear(&vf);
            return 1;
        }

        if(channels > 2 && !raw) {
          /* Then permute! */
          permute_channels(buf, outbuf, ret, channels, bits/8);
          p_outbuf = outbuf;
        }
        else {
          p_outbuf = buf;
        }

        if(fwrite(p_outbuf, 1, ret, out) != ret) {
            fprintf(stderr, _("Error writing to file: %s\n"), strerror(errno));
            ov_clear(&vf);
            return 1;
        }

        written += ret;
        if(!quiet && seekable) {
            done += ret/size;
            if((double)done/(double)length * 200. > (double)percent) {
                percent = (int)((double)done/(double)length *200);
                fprintf(stderr, "\r\t[%5.1f%%]", (double)percent/2.);
            }
        }
    }

    if(seekable && !quiet)
        fprintf(stderr, "\n");

    if(!raw)
        rewrite_header(out, written); /* We don't care if it fails, too late */

    ov_clear(&vf);

    return 0;
}

int convert(char* infilename,char* outfilename)
{
    int i;

  //  setlocale(LC_ALL, "");
  //  bindtextdomain(PACKAGE, LOCALEDIR);
  //  textdomain(PACKAGE);

        FILE *infile, *outfile;
//        char *infilename;
	//char * outfilename="out.wav";

        if(!strcmp(outfilename, "-")) {
            free(outfilename);
            outfilename = NULL;
        }
      //  outfile = open_output(outfilename);

        if(!outfile)
            return 1;


                infilename = "test.ogg";
                infile = open_input(infilename);


            if(!infile) {
                fclose(outfile);
                free(outfilename);
                return 1;
            }
            if(decode_file(infile, outfile, infilename, outfilename)) {
                fclose(outfile);
                return 1;
            }

        fclose(outfile);

    if(outfilename)
        free(outfilename);

    return 0;
}
#endif
#endif // OGGDEC_H
