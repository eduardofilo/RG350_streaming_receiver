#include <stdio.h>
#include <stdlib.h>

#include "lz4.h"

enum {
  HEIGHT = 240,
  WIDTH = 320,
  BYTES_PER_PIXEL = 4,
  WINDOW_BYTES = WIDTH*HEIGHT*BYTES_PER_PIXEL
};

int main(int argc, char const* argv[]) {
  LZ4_streamDecode_t lz4StreamDecode_body;
  LZ4_streamDecode_t* lz4StreamDecode = &lz4StreamDecode_body;

  LZ4_setStreamDecode(lz4StreamDecode, NULL, 0);

  char compressed[LZ4_COMPRESSBOUND(WINDOW_BYTES)];
  char *decompressed = malloc(WINDOW_BYTES);
  int compressed_size = 0;
  for (;;) {
    const size_t read_int = fread(&compressed_size, sizeof(compressed_size), 1, stdin);
    if (read_int != 1 || compressed_size <= 0)
      break;

    const size_t read_bytes = fread(compressed, 1, compressed_size, stdin);
    if (read_bytes != compressed_size)
      break;

    LZ4_decompress_safe_continue(lz4StreamDecode, compressed, decompressed, compressed_size, WINDOW_BYTES);
    /* puts("Decompressed a block"); */
    fwrite(decompressed, 1, WINDOW_BYTES, stdout);
  }

  puts("Error in decompression\n");
  free(decompressed);
  return 1;
}
