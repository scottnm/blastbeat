#include "rect_converter.h"

using blastbeat::utility::lrtb_rect;
using blastbeat::utility::xywh_rect;

xywh_rect blastbeat::utility::lrtb_to_xywh(lrtb_rect rect)
{
    xywh_rect new_rect;

    new_rect.x = rect.left;
    new_rect.y = rect.top;
    new_rect.width = rect.right - rect.left;
    new_rect.height = rect.bottom - rect.top;

    return new_rect;
}

lrtb_rect blastbeat::utility::xywh_to_lrtb(xywh_rect rect)
{
    lrtb_rect new_rect;

    new_rect.left = rect.x;
    new_rect.right = rect.x + rect.width;
    new_rect.top = rect.y; 
    new_rect.bottom = rect.y + rect.height;

    return new_rect;
}
