[Fuente](https://gist.github.com/NickHu/95e8e5e1b8b326d2cb46ce461d3ec701)

# Con Gambatte

ssh root@10.1.1.2 -- ./streaming | ./receiver | ffplay -vcodec rawvideo -f rawvideo -pixel_format rgb565 -video_size 320x240 -framerate 60 -i -

# Con GMenu2X

ssh root@10.1.1.2 -- ./streaming | ./receiver | ffplay -vcodec rawvideo -f rawvideo -pixel_format bgr0 -video_size 320x240 -framerate 60 -i -
