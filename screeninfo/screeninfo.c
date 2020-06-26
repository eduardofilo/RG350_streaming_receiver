#include <stdio.h>
#include <linux/fb.h>
#include <fcntl.h>

int main() {
    //Structs defined in "linux/fb.h"
    struct fb_var_screeninfo vscreeninfo;
    struct fb_fix_screeninfo fscreeninfo;

    //Open the framebuffer
    int fbfd=open("/dev/fb0",O_RDONLY);
    if(fbfd==-1) {
        printf("Failed to open fb dev\n");
        exit(0);
    }

    //Fetch variable screen info
    if(ioctl(fbfd,FBIOGET_VSCREENINFO,&vscreeninfo)==-1) {
        printf("Error reading variable information\n");
        exit(0);
    }

    //Fetch fixed screen info
    if(ioctl(fbfd,FBIOGET_FSCREENINFO,&fscreeninfo)==-1) {
        printf("Error reading fixed information\n");
        exit(0);
    }

    /**********************************VSCREEN DATA************************************/
    printf("\nVscreen Info:-\n");
    printf(" Xres   = %4ld | Yres   = %4ld\n",vscreeninfo.xres,vscreeninfo.yres);
    printf(" BPP    = %4ld | Height = %4ld | Width = %4ld\n",vscreeninfo.bits_per_pixel,
                                                             vscreeninfo.height,
                                                             vscreeninfo.width);
    printf(" Xres_V = %4ld | Yres_V = %4ld\n",vscreeninfo.height,vscreeninfo.width);
    printf(" Pixel format : RGBX_%ld%ld%ld%ld\n",vscreeninfo.red.length,
                                                 vscreeninfo.green.length,
                                                 vscreeninfo.blue.length,
                                                 vscreeninfo.transp.length);
    printf(" Begin of bitfields(Byte ordering):-\n");     //In my case :
    printf("  Red    : %ld\n",vscreeninfo.red.offset);    //Red    : 16
    printf("  Blue   : %ld\n",vscreeninfo.blue.offset);   //Blue   : 0
    printf("  Green  : %ld\n",vscreeninfo.green.offset);  //Green  : 8
    printf("  Transp : %ld\n",vscreeninfo.transp.offset); //Transp : 24
              //Hence orientation is : BGRT => (BGR4)RGB32 packed format
    /********************************~VSCREEN DATA************************************/

    /*********************************FSCREEN DATA************************************/
    printf("\nFscreen Info:-\n");
    printf(" Device ID : %s\n",fscreeninfo.id);
    printf(" Start of FB physical address : %ld\n",fscreeninfo.smem_start);
    printf(" Length of FB : %ld\n",fscreeninfo.smem_len); //Size of framebuffer in bytes
    printf(" Length of Line : %ld\n",fscreeninfo.line_length);
    printf(" Start of MMIO physical address : %ld\n",fscreeninfo.mmio_start);
    printf(" Length of MMIO : %ld\n",fscreeninfo.mmio_len);
    /********************************~FSCREEN DATA************************************/

    close(fbfd);
    return 0;
}
