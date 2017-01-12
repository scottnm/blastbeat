#include "utility/input.h"
#include "utility/unused.h"

#pragma warning(push, 0)
#include <cassert>
#include <windows.h>
#include <xinput.h>
#pragma warning(pop)

namespace blastbeat
{

    static bool input_system_initialized = false;

    #define XINPUT_GET_STATE(name) DWORD WINAPI name(DWORD user_index, XINPUT_STATE* state)
    typedef XINPUT_GET_STATE(xinput_get_state_delegate);
    XINPUT_GET_STATE(xinput_get_state_stub)
    {
        (void)user_index;
        (void)state;
        assert(input_system_initialized);
        return ERROR_DEVICE_NOT_CONNECTED;
    }
    static xinput_get_state_delegate* xinput_get_state = xinput_get_state_stub;

    #define XINPUT_SET_STATE(name) DWORD WINAPI name(DWORD user_index, XINPUT_VIBRATION* state)
    typedef XINPUT_SET_STATE(xinput_set_state_delegate);
    XINPUT_SET_STATE(xinput_set_state_stub)
    {
        (void)user_index;
        (void)state;
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

    gamepad_state get_gamepad_state(DWORD gamepad_num)
    {
        assert(input_system_initialized);
        gamepad_state input_state = {};
        XINPUT_STATE xi_state= {};
        if (xinput_get_state(gamepad_num, &xi_state) == ERROR_SUCCESS)
        {
            input_state.left_stick_x = xi_state.Gamepad.sThumbLX;
            input_state.left_stick_y = xi_state.Gamepad.sThumbLY;
            input_state.dpad_up = (xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
            input_state.dpad_down = (xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
            input_state.dpad_left = (xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
            input_state.dpad_right = (xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
            input_state.start = (xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
            input_state.back = (xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
            input_state.lb = (xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
            input_state.rb = (xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
            input_state.a = (xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
            input_state.b = (xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
            input_state.x = (xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
            input_state.y = (xi_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
            input_state.is_connected = true;
        }
        else
        {
            input_state.is_connected = false;
        }
        return input_state;
    }

    void set_gamepad_state (DWORD gamepad_num, uint16_t left_motor_vibration, uint16_t right_motor_vibration)
    {
        /* set the state */
        assert(input_system_initialized);
        XINPUT_VIBRATION vibration = {};
        vibration.wLeftMotorSpeed = left_motor_vibration;
        vibration.wRightMotorSpeed = right_motor_vibration;
        xinput_set_state(gamepad_num, &vibration);
    }

    void inject_key (uint64_t vkey, bool keydown, bool new_press)
    {
        UNUSED_PARAM(vkey);
        UNUSED_PARAM(keydown);
        UNUSED_PARAM(new_press);
    }
}

