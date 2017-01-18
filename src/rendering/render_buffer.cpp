#include "render_buffer.h"

namespace blastbeat
{
    namespace rendering
    {
        void init_render_buffer(render_buffer* rbuf, int bytes_per_pixel)
        {
            rbuf->bmpinfo.bmiHeader.biSize = sizeof(rbuf->bmpinfo.bmiHeader);
            rbuf->bmpinfo.bmiHeader.biPlanes = 1;
            rbuf->bmpinfo.bmiHeader.biBitCount = 32;
            rbuf->bmpinfo.bmiHeader.biCompression = BI_RGB;
            rbuf->bytes_per_pixel = bytes_per_pixel;
        }
    }
}
