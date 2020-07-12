#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <linux/fb.h>

#include "lz4.h"

#define FPS 30


long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}


int main(int argc, char const* argv[]) {

    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;

    int fb_fd = open("/dev/fb0", O_RDONLY);

    //Get variable screen information
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        //printf("Error reading variable information\n");
        exit(0);
    }
    //Get fixed screen information
    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        //printf("Error reading fixed information\n");
        exit(0);
    }

    int buffersize = vinfo.yres_virtual * finfo.line_length;
    //printf("buffersize: %d\n", buffersize);
    int screensize = vinfo.yres * finfo.line_length;
    //printf("screensize: %d\n", screensize);
    uint8_t *fbp = mmap(0, buffersize, PROT_READ, MAP_SHARED, fb_fd, (off_t)0);
    uint8_t *bbp;
    //printf("Start of FB physical address : %ld\n",fbp);
    uint8_t *buffer = malloc(screensize);
    //printf("Start of buffer physical address : %ld\n",buffer);


    LZ4_stream_t lz4Stream_body;
    LZ4_stream_t* lz4Stream = &lz4Stream_body;

    LZ4_initStream(lz4Stream, sizeof(*lz4Stream));

    char compressed[LZ4_COMPRESSBOUND(screensize)];
    int compressed_size = 0;

    long long beforeTs = current_timestamp(); //TS in MS
    int delay = 0;

    //for (int i=0;i<=2;i++) {
    for (;;) {
        delay = ((1000/FPS) - (current_timestamp() - beforeTs)) * 1000;
        if (delay > 0)
            usleep(delay);
        beforeTs = current_timestamp();

        //Get variable screen information
        if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
            //printf("Error reading variable information\n");
            exit(0);
        }

        bbp = fbp+vinfo.yoffset*finfo.line_length;
        //printf("yoffset: %d\n", vinfo.yoffset);
        //printf("Pointer to fbp physical address : %ld\n",fbp);
        //printf("Pointer to bbp physical address : %ld\n",bbp);

        memcpy(buffer, bbp, screensize);
        compressed_size = LZ4_compress_fast_continue(lz4Stream, buffer, compressed, screensize, sizeof(compressed), 1);
        if (compressed_size == 0) {
            exit(0);
        }

        //printf("screensize: %d\n", sizeof(screensize));
        //printf("compressed_size: %d\n", sizeof(compressed_size));
        fwrite(&screensize, sizeof(screensize), 1, stdout);
        fwrite(&compressed_size, sizeof(compressed_size), 1, stdout);
        fwrite(compressed, 1, compressed_size, stdout);
    };

    puts("Error in compression\n");
    return 1;
}
