[Fuente](https://gist.github.com/NickHu/95e8e5e1b8b326d2cb46ce461d3ec701)

# Compilación

## receiver

Desde máquina Linux donde se va a recibir el streaming:

```
$ cd ~/git/git/streaming_receiver/receiver
$ make
```

### streaming

Desde [Docker con toolchain para RG350](/2020-05-25-rg350_docker_buildroot.html#compilacion-de-distribucion-od-contrib):

```
$ docker container start RG350_buster_buildroot
$ docker exec -it RG350_buster_buildroot /bin/bash
# ~/git/streaming_receiver/streaming
# make
```

# Streaming con Gambatte y FCeux

Emiten en 320x240 y 16bit en pixel_format rgb565le.

```
$ ssh root@10.1.1.2 -- ./streaming | ./receiver | ffplay -vcodec rawvideo -f rawvideo -pixel_format rgb565le -video_size 320x240 -framerate 30 -i -
```

# Streaming con GMenu2X

Emite en 320x240 y 32bit en pixel_format bgr0.

```
$ ssh root@10.1.1.2 -- ./streaming | ./receiver | ffplay -vcodec rawvideo -f rawvideo -pixel_format bgr0 -video_size 320x240 -framerate 30 -i -
```
