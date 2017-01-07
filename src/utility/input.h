#ifndef __INPUT_H__
#define __INPUT_H__

#include <cstdint>

namespace blastbeat
{
    typedef struct
    {
        int16_t left_stick_x;
        int16_t left_stick_y;
        bool    dpad_up;
        bool    dpad_down;
        bool    dpad_left;
        bool    dpad_right;
        bool    start;
        bool    back;
        bool    lb;
        bool    rb;
        bool    a;
        bool    b;
        bool    x;
        bool    y;
        bool    is_connected;
    } gamepad_state;

    void init_input_system (void);
    gamepad_state get_gamepad_state (int gamepad_num);
    void set_gamepad_state (int gamepad_num, uint16_t left_motor, uint16_t right_motor);
    void inject_key (uint64_t vkey, bool keydown, bool new_press);
}

#endif //__INPUT_H__
