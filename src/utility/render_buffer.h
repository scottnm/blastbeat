#ifndef __RENDER_BUFFER_H__
#define __RENDER_BUFFER_H__

#pragma warning(push, 0)
#include <windows.h>
#pragma warning(pop)

namespace blastbeat
{
    typedef struct
    {
        BITMAPINFO bmpinfo;
        void* pixel_buf;
        int width;
        int height;
        int bytes_per_pixel;
    } render_buffer;
}

#endif //__RENDER_BUFFER_H__
