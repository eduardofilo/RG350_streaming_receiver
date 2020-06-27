[Fuente](https://gist.github.com/NickHu/95e8e5e1b8b326d2cb46ce461d3ec701)

## Compilación

#### receiver

Desde máquina Linux donde se va a recibir el streaming:

```
$ cd ~/git/git/streaming_receiver/receiver
$ make
```

#### streaming

Desde [Docker con toolchain para RG350](/2020-05-25-rg350_docker_buildroot.html#compilacion-de-distribucion-od-contrib):

```
$ docker container start RG350_buster_buildroot
$ docker exec -it RG350_buster_buildroot /bin/bash
# ~/git/streaming_receiver/streaming
# make
```

## Streaming con Gambatte y FCeux

Emiten en 320x240 y 16bit en pixel_format rgb565le.

```
$ ssh root@10.1.1.2 -- ./streaming | ./receiver | ffplay -vcodec rawvideo -f rawvideo -pixel_format rgb565le -video_size 320x240 -framerate 30 -i -
```

## Streaming con GMenu2X

Emite en 320x240 y 32bit en pixel_format bgr0.

```
$ ssh root@10.1.1.2 -- ./streaming | ./receiver | ffplay -vcodec rawvideo -f rawvideo -pixel_format bgr0 -video_size 320x240 -framerate 30 -i -
```

## Información sobre el FrameBuffer

Se puede obtener la metainformación del framebuffer con el ejecutable screeninfo. La metainformación varía según el modo de pantalla:

#### En GMenu2X

```
Vscreen Info:-
 Xres   =  320 | Yres   =  240
 BPP    =   32 | Height =   -1 | Width =   -1
 Xres_V =   -1 | Yres_V =   -1
 Pixel format : RGBX_8888
 Begin of bitfields(Byte ordering):-
  Red    : 16
  Blue   : 0
  Green  : 8
  Transp : 24

Fscreen Info:-
 Device ID : jz-lcd
 Start of FB physical address : 205520896
 Length of FB : 3686400
 Length of Line : 1280
 Start of MMIO physical address : 0
 Length of MMIO : 0
```

#### En FCeux

```
Vscreen Info:-
 Xres   =  320 | Yres   =  240
 BPP    =   16 | Height =   -1 | Width =   -1
 Xres_V =   -1 | Yres_V =   -1
 Pixel format : RGBX_5650
 Begin of bitfields(Byte ordering):-
  Red    : 11
  Blue   : 0
  Green  : 5
  Transp : 0

Fscreen Info:-
 Device ID : jz-lcd
 Start of FB physical address : 205520896
 Length of FB : 3686400
 Length of Line : 640
 Start of MMIO physical address : 0
 Length of MMIO : 0
```

## Captura y conversión del framebuffer a PNG

Según vemos en la información que obtiene screeninfo el framebuffer es bastante más grande de lo que sería necesario para contener toda la pantalla. Por ejemplo en el caso de GMenu2X la pantalla tiene 320x240 y 4 bytes por pixel. Por tanto sería suficiente 320*240*4 = 307200 bytes de buffer, pero sin embargo tiene 3686400, es decir suficiente para contener 3686400/307200 = 12 pantallas.

Podemos capturar el buffer entero ejecutando lo siguiente en la RG:

```
# cp /dev/fb0 buffer.bin
```

Transfiriendo el fichero `buffer.bin` al ordenador y convirtiéndolo a PNG con el pequeño programa `convert.py` en Python (necesita que tengamos instalado el módulo `ffpyplayer`), vemos en la imagen resultante que en el buffer se utilizan sólo el espacio correspondiente a los dos primeros frames de los 12 posibles. De hecho se dibujan en esas posiciones los distintos cambios en la pantalla alternativamente, es decir, primero en la posición 0 y luego en la 1, volviendo a la 0 a continuación.

Para convertir el dump del framebuffer anterior ejecutamos `convert.py` así:

```
$ python3 convert.py buffer.bin
```
