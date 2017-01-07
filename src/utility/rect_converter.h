#ifndef __RECT_CONVERTER_H__
#define __RECT_CONVERTER_H__

#include <windows.h>

namespace blastbeat
{
    namespace utility
    {
        typedef struct
        {
            int x;
            int y;
            int width;
            int height;
        } 
        xywh_rect;

        typedef RECT lrtb_rect;

        xywh_rect lrtb_to_xywh(lrtb_rect rect);
        lrtb_rect xywh_to_lrtb(xywh_rect rect);
    }
}

#endif //__RECT_CONVERTER_H__
