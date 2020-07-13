## Enlaces

* [reStream lz4 streaming](https://gist.github.com/NickHu/95e8e5e1b8b326d2cb46ce461d3ec701)
* [The Frame Buffer Device API](https://www.kernel.org/doc/Documentation/fb/api.txt)
* [Tutorial 1: Intro to Low-Level Graphics on Linux](http://betteros.org/tut/graphics1.php#fbdev)
* [Cómo programar el framebuffer en Linux](https://rastersoft.com/articulos/fbuffer.html)

## Compilación

#### Instalación código

```
$ cd ~/git
$ git clone https://github.com/eduardofilo/streaming_receiver.git
```

Contiene los siguientes módulos:

* `receiver`: Recoge el streaming de frames comprimodos que genera `streaming` por la entrada estándar y genera por la salida estándar un streaming de rawvideo.
* `streaming`: Lee el framebuffer y emite por la salida estándar un streaming de frames comprimidos.
* `screeninfo`: Muestra los parámetros del framebuffer actual. Sirve para averiguar la resolución de la pantalla y la profundidad de color de cada píxel.
* `convert.py`: Pequeño script Python para convertir un volcado completo del framebuffer a PNG. Sirve para visualizar el contenido completo del framebuffer en un momento dado para ayudarnos a interpretarlo (por ejemplo para ver si estamos utilizando el pixel_format correcto).

#### receiver

Lo compilamos desde máquina Linux donde se va a recibir el streaming:

```
$ cd ~/git/git/streaming_receiver/receiver
$ make
```

#### streaming

Necesitamos el contenedor [Docker con toolchain para RG350](/2020-05-25-rg350_docker_buildroot.html#compilacion-de-distribucion-od-contrib). La primera vez lo instalamos:

```
$ docker run -it -v ~/git:/root/git --name RG350_buster_buildroot eduardofilo/rg350_buster_buildroot
```

Posteriormente sólo necesitamos arrancarlo:

```
$ docker container start RG350_buster_buildroot
$ docker exec -it RG350_buster_buildroot /bin/bash
```

Finalmente compilamos:

```
# cd ~/git/streaming_receiver/streaming
# make
```

Y transferimos a la RG ya que se ejecutará en ella:

```
# scp streaming root@10.1.1.2:
```

#### screeninfo

Compilamos también desde el contenedor [Docker con toolchain para RG350](/2020-05-25-rg350_docker_buildroot.html#compilacion-de-distribucion-od-contrib) y transferimos a la RG:

```
# cd ~/git/streaming_receiver/screeninfo
# make
# scp screeninfo root@10.1.1.2:
```

Ejecutando por ejemplo este binario en la RG (por SSH por ejemplo) mientras se está ejecutando por ejemplo GMenu2X obtenemos la siguiente salida:

```
Vscreen Info:-
 Xres         =  320 | Yres         =  240
 Xres_virtual =  320 | Yres_virtual =  720
 Xoffset      =    0 | Yoffset      =    0
 BPP          =   32 | Height       =   -1 | Width =   -1
 Xres_V       =   -1 | Yres_V       =   -1
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
 XPan step for hardware panning : 1
 YPan step for hardware panning : 1
 YWrap step for hardware ywrap : 0
 Length of Line : 1280
 Start of MMIO physical address : 0
 Length of MMIO : 0
```

Los siguientes elementos son importantes se interpretan así:

* Xres: Resolución horizontal de cada frame.
* Yres: Resolución vertical de cada frame.
* Xres_virtual: Resolución horizontal del framebuffer.
* Yres_virtual: Resolución horizontal del framebuffer.
* Yoffset: Primera linea vertical del frame activo en el buffer (será Yres * el número de frame activo empezando a contar en 0).
* BPP: Bits por pixel.
* Byte ordering: Nos indica en qué orden llegan los bits de cada uno de los tres colores y la transparencia.
* Length of FB: Tamaño total del framebuffer en bytes.
* Length of Line: Tamaño de cada linea horizontal del framebuffer en bytes.

Ejecutando este binario en distintos programas ejecutándose en la RG se han encontrado los siguientes parámetros:

|Programa|Xres|Yres|BPP|pixel_format|Frames en buffer|
|:-------|:---|:---|:--|:-----------|:---------------|
|GMenu2X|320|240|32|bgr0|2|
|DinguxCmdr|320|240|16|rgb565le|1|
|FCeux|320|240|16|rgb565le|3|
|PCSX4All|640|480|15|bgr555le|3|
|Gambatte-M|640|480|16|rgb565le|3|

Para conocer todos los pixel_format disponibles, ejecutar el comando:

```
$ ffplay -pix_fmts
```

## Sesión de streaming

Por ejemplo con FCeux en ejecución. Como vemos en la tabla anterior el framebuffer en este caso trabaja a 320x240 y pixel_format rgb565le. Para visualizar al vuelo el streaming desde la consola ejecutaremos así desde el directorio donde se encuentre el binario `receiver` en el ordenador que recibirá el streaming (cambiaremos los parámetros anteriores en `pixel_format` y `video_size`):

```
$ ssh root@10.1.1.2 -- ./streaming | ./receiver | ffplay -vcodec rawvideo -f rawvideo -pixel_format rgb565le -video_size 320x240 -framerate 30 -i -
```

## Conversión a video

Si en lugar de visualizar al vuelo preferimos capturar el streaming ejecutaremos así:

```
$ ssh root@10.1.1.2 -- ./streaming | ./receiver > dump.bin
```

Finalmente convertiremos a vide ejecutando:

```
$ ffmpeg -vcodec rawvideo -f rawvideo -pixel_format rgb565le -video_size 320x240 -framerate 30 -i dump.bin -vcodec h264 out.mp4
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

Hay que adaptar el tamaño del buffer y el pixel_format en el script `convert.py`.

En lugar de utilizar el script convert.py, podemos utilizar [esta](https://rawpixels.net/) utilidad online. Otra opción es abrir con Gimp seleccionando el formato `Datos de imagen en bruto`. Aparecerá un cuadro de diálogo donde podremos ajustar la resolución y el pixel_format.
