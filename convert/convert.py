from ffpyplayer.pic import Image, SWScale
from ffpyplayer.tools import get_supported_pixfmts
from ffpyplayer.writer import MediaWriter



file = open("/home/edumoreno/snap.bin", "rb")
buf = bytearray(file.read())


# create image
w, h = 320, 2880
fmt = 'bgr0'
size = w * h * 4
#buf = bytearray([int(x * 255 / size) for x in range(size)])
img = Image(plane_buffers=[buf], pix_fmt=fmt, size=(w, h))
codec = 'png'  # we'll encode it using the tiff codec

# make sure the output codec supports the input pixel format type
# otherwise, convert it to the best pixel format
ofmt = get_supported_pixfmts(codec, fmt)[0]
if ofmt != fmt:
    sws = SWScale(w, h, fmt, ofmt=ofmt)
    img = sws.scale(img)
    fmt = ofmt

out_opts = {'pix_fmt_in': fmt, 'width_in': w, 'height_in': h,
            'frame_rate': (30, 1), 'codec': codec}
writer = MediaWriter('output.png', [out_opts])
writer.write_frame(img=img, pts=0, stream=0)
writer.close()

file.close()
