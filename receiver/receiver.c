#include <stdio.h>
#include <stdlib.h>

#include "lz4.h"

int main(int argc, char const* argv[]) {
    LZ4_streamDecode_t lz4StreamDecode_body;
    LZ4_streamDecode_t* lz4StreamDecode = &lz4StreamDecode_body;

    LZ4_setStreamDecode(lz4StreamDecode, NULL, 0);

    int last_screen_size = 0;
    int screen_size = 0;
    int compressed_size = 0;
    //printf("screen_size: %d\n", sizeof(screen_size));
    //printf("compressed_size: %d\n", sizeof(compressed_size));

    char *compressed = NULL;
    char *decompressed = NULL;

    //for (int i=0;i<=2;i++) {
    for (;;) {
        const size_t read_int = fread(&screen_size, sizeof(screen_size), 1, stdin);
        if (read_int != 1 || screen_size <= 0)
            exit(0);
        const size_t read_int2 = fread(&compressed_size, sizeof(compressed_size), 1, stdin);
        if (read_int2 != 1 || compressed_size <= 0)
            exit(0);

        //printf("compressed_size: %d\n", compressed_size);
        //printf("screen_size: %d\n", screen_size);
        //printf("last_screen_size: %d\n", last_screen_size);

        if (last_screen_size != screen_size) {
            free(compressed);
            compressed = malloc(screen_size);
            free(decompressed);
            decompressed = malloc(screen_size);
            last_screen_size = screen_size;
        }
        const size_t read_bytes = fread(compressed, 1, compressed_size, stdin);
        if (read_bytes != compressed_size)
            exit(0);
        LZ4_decompress_safe_continue(lz4StreamDecode, compressed, decompressed, compressed_size, screen_size);
        /* puts("Decompressed a block"); */
        fwrite(decompressed, 1, screen_size, stdout);
    }

    puts("Error in decompression\n");
    free(decompressed);
    return 1;
}
