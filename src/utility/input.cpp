#include "input.h"
#include <cassert>
#include <windows.h>
#include <xinput.h>

namespace blastbeat
{

    static bool input_system_initialized = false;

    #define XINPUT_GET_STATE(name) DWORD WINAPI name(DWORD user_index, XINPUT_STATE* state)
    typedef XINPUT_GET_STATE(xinput_get_state_delegate);
    XINPUT_GET_STATE(xinput_get_state_stub)
    {
        assert(input_system_initialized);
        return ERROR_DEVICE_NOT_CONNECTED;
    }
    static xinput_get_state_delegate* xinput_get_state = xinput_get_state_stub;

    #define XINPUT_SET_STATE(name) DWORD WINAPI name(DWORD user_index, XINPUT_VIBRATION* state)
    typedef XINPUT_SET_STATE(xinput_set_state_delegate);
    XINPUT_SET_STATE(xinput_set_state_stub)
    {
        assert(input_system_initialized);
        return ERROR_DEVICE_NOT_CONNECTED;
    }
    static xinput_set_state_delegate* xinput_set_state = xinput_set_state_stub;

    void init_input_system(void)
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
            xinput_get_state = (xinput_get_state_delegate*)GetProcAddress(x_input_library,
                    "XInputGetState");
            xinput_set_state = (xinput_set_state_delegate*)GetProcAddress(x_input_library,
                    "XInputSetState");
        }

        input_system_initialized = true;
    }

    gamepad_state get_gamepad_state(int gamepad_num)
    {
        assert(input_system_initialized);
        gamepad_state input_state = {};
        XINPUT_STATE xi_state= {};
        if (xinput_get_state(gamepad_num, &xi_state) == ERROR_SUCCESS)
        {
            input_state.left_stick_x = xi_state.Gamepad.sThumbLX;
            input_state.left_stick_y = xi_state.Gamepad.sThumbLY;
            input_state.dpad_up = xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
            input_state.dpad_down = xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
            input_state.dpad_left = xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
            input_state.dpad_right = xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
            input_state.start = xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_START;
            input_state.back = xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK;
            input_state.lb = xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
            input_state.rb = xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
            input_state.a = xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_A;
            input_state.b = xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_B;
            input_state.x = xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_X;
            input_state.y = xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y;
            input_state.is_connected = true;
        }
        else
        {
            input_state.is_connected = false;
        }
        return input_state;
    }

    void set_gamepad_state(int gamepad_num)
    {
        /* set the state */
        assert(input_system_initialized);
    }
}

