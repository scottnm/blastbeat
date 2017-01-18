#ifndef __RENDER_BUFFER_H__
#define __RENDER_BUFFER_H__

#pragma warning(push, 0)
#include <windows.h>
#pragma warning(pop)

#pragma warning(disable : 4820)

namespace blastbeat
{
    namespace rendering
    {
        typedef struct
        {
            BITMAPINFO bmpinfo;
            void* pixel_buf;
            int width;
            int height;
            int bytes_per_pixel;
        } render_buffer;

        void init_render_buffer (render_buffer* rbuf, int bytes_per_pixel);
    }
}

#endif //__RENDER_BUFFER_H__
