#ifndef __SOUND_H__
#define __SOUND_H__

#include <cstdint>

#pragma warning(push, 0)
#include <windows.h>
#pragma warning(pop)

namespace blastbeat
{
    namespace sound
    {
        void init(HWND window, uint32_t buffer_size, uint32_t samples_per_sec);
    }
}

#endif // __SOUND_H__
