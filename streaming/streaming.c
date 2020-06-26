#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>

#include "lz4.h"

enum {
  HEIGHT = 240,
  WIDTH = 320,
  BYTES_PER_PIXEL = 4,
  WINDOW_BYTES = WIDTH*HEIGHT*BYTES_PER_PIXEL
};

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

int main(int argc, char const* argv[]) {
  int fb0 = open("/dev/fb0", O_RDONLY);
  char *fbp = mmap(NULL, WINDOW_BYTES, PROT_READ, MAP_SHARED, fb0, 0);

  // handle failed to open

  LZ4_stream_t lz4Stream_body;
  LZ4_stream_t* lz4Stream = &lz4Stream_body;

  LZ4_initStream(lz4Stream, sizeof(*lz4Stream));

  char compressed[LZ4_COMPRESSBOUND(WINDOW_BYTES)];
  int compressed_size = 0;

  long long beforeTs = current_timestamp(); //TS in MS
  int delay = 0;

  for (;;) {
    delay = ((1000/30) - (current_timestamp() - beforeTs)) * 1000;
    if (delay > 0)
      usleep(delay);
    beforeTs = current_timestamp();
    compressed_size = LZ4_compress_fast_continue(lz4Stream, fbp, compressed, WINDOW_BYTES, sizeof(compressed), 1);
    if (compressed_size == 0) {
      break;
    }

    fwrite(&compressed_size, sizeof(compressed_size), 1, stdout);
    fwrite(compressed, 1, compressed_size, stdout);
  };

  puts("Error in compression\n");
  return 1;
}
