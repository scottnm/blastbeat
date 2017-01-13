#include "utility/input.h"
#include "utility/static_defs.h"
#include "utility/unused.h"

#pragma warning(push, 0)
#include <cassert>
#include <windows.h>
#include <xinput.h>
#pragma warning(pop)

namespace blastbeat
{
    namespace input
    {
        global bool input_system_initialized = false;

        #define XINPUT_GET_STATE(name) DWORD WINAPI name(DWORD pad_index, XINPUT_STATE* state)
        typedef XINPUT_GET_STATE(xinput_get_state_delegate);
        XINPUT_GET_STATE(xinput_get_state_stub)
        {
            assert(input_system_initialized);

            UNUSED_PARAM(pad_index);
            UNUSED_PARAM(state);
            return ERROR_DEVICE_NOT_CONNECTED;
        }
        global xinput_get_state_delegate* xinput_get_state = xinput_get_state_stub;

        #define XINPUT_SET_STATE(name) DWORD WINAPI name(DWORD pad_index, XINPUT_VIBRATION* state)
        typedef XINPUT_SET_STATE(xinput_set_state_delegate);
        XINPUT_SET_STATE(xinput_set_state_stub)
        {
            assert(input_system_initialized);

            UNUSED_PARAM(pad_index);
            UNUSED_PARAM(state);
            return ERROR_DEVICE_NOT_CONNECTED;
        }
        global xinput_set_state_delegate* xinput_set_state = xinput_set_state_stub;

        void
        init(void)
        {
            /*
             * Deal with all of the input state dll pointer nonsense here
             */
            HMODULE x_input_library = LoadLibrary("XInput1_4.dll");
            if (!x_input_library)
            {
                x_input_library = LoadLibrary("xinput1_3.dll");
            }

            if (x_input_library)
            {
                #pragma warning(push)
                #pragma warning(disable : 4191)
                xinput_get_state = (xinput_get_state_delegate*)GetProcAddress(x_input_library,
                        "XInputGetState");
                xinput_set_state = (xinput_set_state_delegate*)GetProcAddress(x_input_library,
                        "XInputSetState");
                #pragma warning(pop)
            }

            input_system_initialized = true;
        }

        gamepad_state
        get_gamepad_state(DWORD gamepad_num)
        {
            assert(input_system_initialized);

            gamepad_state gp_state = {};
            XINPUT_STATE xi_state = {};
            if (xinput_get_state(gamepad_num, &xi_state) == ERROR_SUCCESS)
            {
                auto buttons = xi_state.Gamepad.wButtons;
                gp_state.left_stick_x = xi_state.Gamepad.sThumbLX;
                gp_state.left_stick_y = xi_state.Gamepad.sThumbLY;
                gp_state.dpad_up = (buttons & XINPUT_GAMEPAD_DPAD_UP) != 0;
                gp_state.dpad_down = (buttons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
                gp_state.dpad_left = (buttons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
                gp_state.dpad_right = (buttons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
                gp_state.start = (buttons & XINPUT_GAMEPAD_START) != 0;
                gp_state.back = (buttons & XINPUT_GAMEPAD_BACK) != 0;
                gp_state.lb = (buttons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
                gp_state.rb = (buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
                gp_state.a = (buttons & XINPUT_GAMEPAD_A) != 0;
                gp_state.b = (buttons & XINPUT_GAMEPAD_B) != 0;
                gp_state.x = (buttons & XINPUT_GAMEPAD_X) != 0;
                gp_state.y = (buttons & XINPUT_GAMEPAD_Y) != 0;
                gp_state.is_connected = true;
            }
            else
            {
                gp_state.is_connected = false;
            }
            return gp_state;
        }

        void
        set_gamepad_state(DWORD gamepad_num, uint16_t left_motor_vibration, uint16_t right_motor_vibration)
        {
            assert(input_system_initialized);

            XINPUT_VIBRATION vibration = {};
            vibration.wLeftMotorSpeed = left_motor_vibration;
            vibration.wRightMotorSpeed = right_motor_vibration;
            xinput_set_state(gamepad_num, &vibration);
        }

        void
        inject_key(uint64_t vkey, bool keydown, bool new_press)
        {
            assert(input_system_initialized);

            UNUSED_PARAM(vkey);
            UNUSED_PARAM(keydown);
            UNUSED_PARAM(new_press);
        }
    }
}
