/*****************************
 * Author: Scott Munro
 * File:   blastbeat_main.cpp
 *****************************/

#include "blastbeat_main.h"
#include "utility/rect_converter.h"

#include <cmath>
#include <cassert>
#include <cstdint>

using blastbeat::utility::lrtb_rect;
using blastbeat::utility::xywh_rect;
using blastbeat::utility::lrtb_to_xywh;

#define global static
#define local_persist static
#define internal static

/******************
 * Internal funcs *
 ******************/
internal void init_bitmap_info();
internal xywh_rect get_window_rect(HWND window);
internal void resize_dib_section(int width, int height);
internal void update_window(HDC device_context, xywh_rect client_rect, int x, int y, int width, int height);
internal void render_crazy_gradient(int x_offset, int y_offset);

// TODO (scott): fix global scope later
global bool g_game_running;
global BITMAPINFO bitmap_info;
global void* bitmap_buffer;
global xywh_rect bitmap_dimensions;

#define WINDOW_REG_FAILED 0
int CALLBACK
WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd)
{
    init_bitmap_info();

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

        render_crazy_gradient(x_offset, floor(500 * cos(y_offset / 70.0)));

        auto device_context = GetDC(window);
        lrtb_rect _cr;
        GetClientRect(window, &_cr);
        xywh_rect client_rect = lrtb_to_xywh(_cr);
        update_window(device_context, client_rect, 0, 0, client_rect.width, client_rect.height);
        ReleaseDC(window, device_context);

        x_offset += 1;
        y_offset += 1;
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

        case WM_SIZE:
        {
            xywh_rect client_rect = get_window_rect(window);
            resize_dib_section(client_rect.width, client_rect.height);
            OutputDebugStringA("WM_SIZE\n");
        }
        break;

        case WM_ACTIVATEAPP:
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC device_context = BeginPaint(window, &paint);
            xywh_rect paint_rect = lrtb_to_xywh(paint.rcPaint);
            lrtb_rect _cr;
            GetClientRect(window, &_cr);
            xywh_rect client_rect = lrtb_to_xywh(_cr);
            update_window(device_context, client_rect, paint_rect.x, paint_rect.y, paint_rect.width, paint_rect.height);
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
init_bitmap_info()
{
    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
}

internal xywh_rect
get_window_rect(HWND window)
{
    lrtb_rect new_rect;
    GetClientRect(window, &new_rect);
    return lrtb_to_xywh(new_rect);
}

internal void
resize_dib_section(int width, int height)
{
    if (bitmap_buffer != nullptr)
    {
        VirtualFree(bitmap_buffer, 0, MEM_RELEASE);
        bitmap_buffer = nullptr;
    }

    bitmap_info.bmiHeader.biWidth = width;
    // negative height used to make this a topdown buffer
    bitmap_info.bmiHeader.biHeight = -height;

    bitmap_dimensions.width = width;
    bitmap_dimensions.height = height;

    // NOTE (scott): no more DC because we can just allocate our
    // own memory since stretchdibits takes a pointer to raw memory
    bitmap_buffer = VirtualAlloc(NULL, width * height * 4, MEM_COMMIT, PAGE_READWRITE);
    assert (bitmap_buffer != nullptr);

    // TODO (scott): clear to black
}

internal void
update_window(HDC device_context, xywh_rect client_rect, int x, int y, int width, int height)
{
    StretchDIBits(device_context,
                  0, 0, bitmap_dimensions.width, bitmap_dimensions.height,
                  0, 0, client_rect.width, client_rect.height, 
                  bitmap_buffer, &bitmap_info, DIB_RGB_COLORS, SRCCOPY); 
}

internal void
render_crazy_gradient(int x_offset, int y_offset)
{
    uint32_t* pixel_bmp_buffer = (uint32_t*)bitmap_buffer;
    for (int y = 0; y < bitmap_dimensions.height; ++y)
    {
        for (int x = 0; x < bitmap_dimensions.width; ++x)
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

            *(pixel_bmp_buffer++) = (r << 16) | (g << 8) | b;
        }
    }
}
