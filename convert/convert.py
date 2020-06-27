import sys
from ffpyplayer.pic import Image, SWScale
from ffpyplayer.tools import get_supported_pixfmts
from ffpyplayer.writer import MediaWriter


# Params
w = 320                 # get from fb_var_screeninfo.xres
line_length = 1280      # get from fb_fix_screeninfo.line_length
fmt = 'bgr0'

buffer_size = 3686400   # get form fb_fix_screeninfo.smem_len
h = buffer_size / line_length
codec = 'png'


file = open(sys.argv[1], "rb")
buf = bytearray(file.read())

img = Image(plane_buffers=[buf], pix_fmt=fmt, size=(w, h))

# make sure the output codec supports the input pixel format type
# otherwise, convert it to the best pixel format
ofmt = get_supported_pixfmts(codec, fmt)[0]
if ofmt != fmt:
    sws = SWScale(w, h, fmt, ofmt=ofmt)
    img = sws.scale(img)
    fmt = ofmt

out_opts = {'pix_fmt_in': fmt, 'width_in': w, 'height_in': h,
            'frame_rate': (30, 1), 'codec': codec}
writer = MediaWriter('output.'+codec, [out_opts])
writer.write_frame(img=img, pts=0, stream=0)
writer.close()

file.close()
