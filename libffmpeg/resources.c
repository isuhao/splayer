
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "libxplayer.h"
#include "resources.h"

typedef struct {
    unsigned char* data;
    unsigned int len;
    unsigned int pos;
} memimage_t;

static int feofmem(memimage_t* memimage)
{
    return memimage->pos==memimage->len;
}

#if 0
static long ftellmem(memimage_t* memimage)
{
    return memimage->pos;
}

static long fseekmem(memimage_t* memimage, long offset, int whence)
{
    switch(whence) {
    case SEEK_SET:
        memimage->pos=offset;
        break;
    case SEEK_END:
        memimage->pos=memimage->len-offset;
        break;
    case SEEK_CUR:
        memimage->pos+=offset;
        break;
    default:
        return -1;
    }
    if(memimage->pos<0)
        memimage->pos=0;
    if(memimage->pos>memimage->len)
        memimage->pos=memimage->len;
    return 0;
}
#endif

static size_t freadmem(void* ptr, size_t size, size_t nmemb, memimage_t* memimage)
{
    size_t i;

    for(i=0;i<nmemb;i++)
    {
        if(memimage->pos+size>memimage->len)
            break;
        memcpy(ptr,&memimage->data[memimage->pos],size);
        ptr+=size;
        memimage->pos+=size;
        if(memimage->pos+size==memimage->len)
            break;
    }
    return i;
}

static char* fgetsmem(char* s, int len, memimage_t* memimage)
{
    int i;

    memset(s,0,len);
    for(i=0;i<len;i++)
    {
        s[i]=memimage->data[memimage->pos++];
        if(!s[i] || s[i]=='\n' || feofmem(memimage))
            break;
    }
    return s;
}

int xplayer_API_getresourceimage(const char* name, mp_image_t** mpi)
{
    memimage_t memimage;
    char buffer[8192];
    char *p;
    unsigned char *dst;
    int w,h,fmt,i,r;
    mp_image_t* img = NULL;

    if(!mpi) {
        return -1;
    }
    for(i=0;resources[i].name;i++) {
        if(!strcmp(resources[i].name,name))
            break;
    }
    if(!resources[i].name) {
        return -1;
    }
    memimage.data=resources[i].data;
    memimage.len=resources[i].size;
    if(!memimage.data || !memimage.len) {
        return -1;
    }
    fgetsmem(buffer,sizeof(buffer),&memimage);
    if((p=strchr(buffer,'\n'))) *p=0;
    if(!strcmp(buffer,"P5")) {
        fmt=IMGFMT_Y8;
    } else if(!strcmp(buffer,"P6")) {
        fmt=IMGFMT_RGB24;
    } else {
        return 0;
    }
    fgetsmem(buffer,sizeof(buffer),&memimage);
    if((p=strchr(buffer,'\n'))) *p=0;
    if(!(p=strchr(buffer,' '))) {
        return 0;
    }
    *p++=0;
    w=atoi(buffer);
    h=atoi(p);
    if(!w || !h) {
        return 0;
    }
    fgetsmem(buffer,sizeof(buffer),&memimage);
    if((p=strchr(buffer,'\n'))) *p=0;
    if(atoi(buffer)!=255) {
        return 0;
    }
    img=alloc_mpi(w, h, fmt);
    dst=img->planes[0];
    for(i=0;i<img->h;i++)
    {
        freadmem(dst,1,img->stride[0],&memimage);
        dst+=img->stride[0];
    }
    if(*mpi)
        free_mp_image(*mpi);
    *mpi=img;
    return 1;
}
