/*****************************
 * Author: Scott Munro
 * File:   blastbeat_main.cpp
 *****************************/

#include "blastbeat_main.h"
#include "utility/input.h"
#include "utility/rect_converter.h"
#include "utility/render_buffer.h"

#include <cmath>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <xinput.h>

using blastbeat::gamepad_state;
using blastbeat::init_input_system;
using blastbeat::render_buffer;
using blastbeat::utility::lrtb_rect;
using blastbeat::utility::xywh_rect;
using blastbeat::utility::lrtb_to_xywh;

#define global static
#define local_persist static
#define internal static

/******************
 * Internal funcs *
 ******************/
internal void       init_render_buffer (render_buffer* rbuf, int bytes_per_pixel);
internal xywh_rect  get_window_rect (HWND window);
internal void       resize_dib_section (render_buffer* rbuf, int width, int height);
internal void       update_window (render_buffer* src_buf, HDC dest_dc, int dest_width, int dest_height);
internal void       render_crazy_gradient (render_buffer* rbuf, int x_offset, int y_offset);

// TODO (scott): fix global scope later
global bool g_game_running;
global render_buffer g_backbuffer;

#define WINDOW_REG_FAILED 0
int CALLBACK
WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd)
{
    init_input_system();
    init_render_buffer(&g_backbuffer, 4);

    WNDCLASS window_class = {};
    // TODO (scott) determine if these actually matter
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = blastbeat_window_message_router;
    window_class.hInstance = instance;
    // window_class.hIcon = 
    window_class.lpszClassName = "BlastbeatWindowClassName";

    ATOM window_registration_result = RegisterClass(&window_class);
    if (window_registration_result == WINDOW_REG_FAILED)
    {
        OutputDebugStringA("Window class registration failed\n");
        return -1;
    }

    HWND window = CreateWindowEx(0, window_class.lpszClassName, "Blastbeat",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, /* TODO (scott): check if this is necessary */
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            0, 0, instance, 0);
    if (window == NULL)
    {
        OutputDebugStringA("Window creation failed\n");
        return -1;
    }
    
    auto _win_rect = get_window_rect(window);
    resize_dib_section(&g_backbuffer, _win_rect.width, _win_rect.height);

    g_game_running = true;

    int x_offset = 0;
    int y_offset = 0;

    while (g_game_running)
    {
        MSG message;
        while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                g_game_running = false;
            }

            TranslateMessage(&message);
            DispatchMessage(&message); 
        }

        bool any_gamepad_connected = false;
        for (int gamepad_id = 0; gamepad_id < XUSER_MAX_COUNT; ++gamepad_id)
        {
            gamepad_state gamepad = blastbeat::get_gamepad_state(gamepad_id);
            if (gamepad.is_connected)
            {
                any_gamepad_connected = true;
                if (gamepad.b)
                {
                    ++x_offset;
                }
                if (gamepad.x)
                {
                    --x_offset;
                }
                if (gamepad.a)
                {
                    ++y_offset;
                }
                if (gamepad.y)
                {
                    --y_offset;
                }

                if (gamepad.left_stick_x > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                {
                    blastbeat::set_gamepad_state(gamepad_id, 0, gamepad.left_stick_x);
                }
                else if (gamepad.left_stick_x < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                {
                    blastbeat::set_gamepad_state(gamepad_id, -gamepad.left_stick_x, 0);
                }
                else
                {
                    blastbeat::set_gamepad_state(gamepad_id, 0, 0);
                }
            }
        }

        if (!any_gamepad_connected)
        {
            ++x_offset;
            ++y_offset;
        }

        render_crazy_gradient(&g_backbuffer, x_offset, floor(500 * cos(y_offset / 70.0)));

        auto device_context = GetDC(window);
        auto win_rect = get_window_rect(window);
        update_window(&g_backbuffer, device_context, win_rect.width, win_rect.height);
        ReleaseDC(window, device_context);
    }

    return 0;
}

#define MESSAGE_HANDLED 0
LRESULT CALLBACK
blastbeat_window_message_router(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result = MESSAGE_HANDLED;

    switch (message)
    {
        case WM_DESTROY:
            OutputDebugStringA("WM_DESTROY\n");
            // TODO (scott): don't always immediately quit
            // handle as an error - recreate window?
            g_game_running = false;
        break;

        case WM_CLOSE:
            OutputDebugStringA("WM_CLOSE\n");
            // TODO (scott): don't always immediately quit
            // propogate to user to double check
            g_game_running = false;
        break;

        case WM_QUIT:
            OutputDebugStringA("WM_QUIT\n");
            g_game_running = false;
        break;

        case WM_SYSKEYDOWN:
            // allow ALT+Fn4 to close window
            if ((w_param == VK_F4) && (l_param & (1 << 29)) != 0) { g_game_running = false; }
        case WM_KEYDOWN:
            blastbeat::inject_key(w_param, true, ((1 << 30) & l_param) == 0);
        break;

        case WM_SYSKEYUP:
        case WM_KEYUP:
            blastbeat::inject_key(w_param, false, true);
        break;

        case WM_ACTIVATEAPP:
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC device_context = BeginPaint(window, &paint);
            auto win_rect = get_window_rect(window);
            update_window(&g_backbuffer, device_context, win_rect.width, win_rect.height);
            EndPaint(window, &paint);
        }
        break;

        default:
            result = DefWindowProc(window, message, w_param, l_param);
        break;
    }

    return result;
}

internal void
init_render_buffer (render_buffer* rbuf, int bytes_per_pixel)
{
    rbuf->bmpinfo.bmiHeader.biSize = sizeof(rbuf->bmpinfo.bmiHeader);
    rbuf->bmpinfo.bmiHeader.biPlanes = 1;
    rbuf->bmpinfo.bmiHeader.biBitCount = 32;
    rbuf->bmpinfo.bmiHeader.biCompression = BI_RGB;
    rbuf->bytes_per_pixel = bytes_per_pixel;
}

internal xywh_rect
get_window_rect(HWND window)
{
    lrtb_rect new_rect;
    GetClientRect(window, &new_rect);
    return lrtb_to_xywh(new_rect);
}

internal void
resize_dib_section(render_buffer* rbuf, int width, int height)
{
    if (rbuf->pixel_buf != nullptr)
    {
        VirtualFree(rbuf->pixel_buf, 0, MEM_RELEASE);
        rbuf->pixel_buf = nullptr;
    }

    rbuf->bmpinfo.bmiHeader.biWidth = width;
    rbuf->bmpinfo.bmiHeader.biHeight = -height; // negative for topdown buffer

    rbuf->width = width;
    rbuf->height = height;

    // NOTE (scott): no more DC because we can just allocate our
    // own memory since stretchdibits takes a pointer to raw memory
    //
    // NOTE (scott): don't have to worry about clearing to black because
    // memory returned by virtual alloc is 0 initialized by default
    rbuf->pixel_buf = VirtualAlloc(NULL, width * height * rbuf->bytes_per_pixel,
                                   MEM_COMMIT, PAGE_READWRITE);
    assert (rbuf->pixel_buf != nullptr);
}

internal void
update_window(render_buffer* src_buf, HDC dest_dc, int dest_width, int dest_height)
{
    StretchDIBits(dest_dc,
                  0, 0, dest_width, dest_height, 
                  0, 0, src_buf->width, src_buf->height,
                  src_buf->pixel_buf, &src_buf->bmpinfo,
                  DIB_RGB_COLORS, SRCCOPY); 
}

internal void
render_crazy_gradient(render_buffer* rbuf, int x_offset, int y_offset)
{
    uint32_t* pixel_buffer = (uint32_t*)rbuf->pixel_buf;
    int buffer_height = rbuf->height;
    int buffer_width = rbuf->width;

    for (int y = 0; y < buffer_height; ++y)
    {
        for (int x = 0; x < buffer_width; ++x)
        {
            // warpy twist cylinder
            uint8_t r = (x+y_offset)*7%(y+1) - (y+x_offset)*100/(x+1); //RR 
            uint8_t g = y_offset + y;                                  //GG
            uint8_t b = x_offset + x;                                  //BB

            /* Rainbow bowser tie dye
            int new_y = y+x_offset;
            if ( y == 0 )
            {
                *pixel_ptr++ = (uint8_t)(255); //RR
            }
            else
            {
                *pixel_ptr++ = (uint8_t)((x+y_offset)*1000/(new_y)); //RR
            }
            *pixel_ptr++ = 0;              //XX
            */

            *(pixel_buffer++) = (r << 16) | (g << 8) | b;
        }
    }
}
