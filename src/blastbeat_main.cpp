/*****************************
 * Author: Scott Munro
 * File:   blastbeat_main.cpp
 *****************************/

#include "blastbeat_main.h"

#define WINDOW_REG_FAILED 0
int CALLBACK
WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd)
{
    WNDCLASS window_class = {};
    // TODO (scott) determine if these actually matter
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = blastbeat_window_callback;
    window_class.hInstance = instance;
    // window_class.hIcon = 
    window_class.lpszClassName = "BlastbeatWindowClassName";

    ATOM window_registration_result = RegisterClass(&window_class);
    if (window_registration_result == WINDOW_REG_FAILED)
    {
        OutputDebugStringA("Window class registration failed\n");
        return -1;
    }

    HWND window_handle = CreateWindowEx(0, window_class.lpszClassName, "Blastbeat",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, /* TODO (scott): check if this is necessary */
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            0, 0, instance, 0);
    if (window_handle == nullptr)
    {
        OutputDebugStringA("Window creation failed\n");
        return -1;
    }

    MSG message;
    for (;;)
    {
        auto message_result = GetMessage(&message, 0, 0, 0);
        if (message_result > 0)
        {
            TranslateMessage(&message);
            DispatchMessage(&message); 
        }
        else
        {
            // TODO (scott): logging
        }
    }

    return 0;
}

#define MESSAGE_HANDLED 0
LRESULT CALLBACK
blastbeat_window_callback(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result = MESSAGE_HANDLED;

    switch (message)
    {
        case WM_SIZE:
            OutputDebugStringA("WM_SIZE\n");
        break;

        case WM_DESTROY:
            OutputDebugStringA("WM_DESTROY\n");
        break;

        case WM_CLOSE:
            OutputDebugStringA("WM_CLOSE\n");
            exit(0);
        break;

        case WM_ACTIVATEAPP:
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        break;

        case WM_PAINT:
            paint_window(window);
        break;

        default:
            result = DefWindowProc(window, message, w_param, l_param);
        break;
    }

    return result;
}

void paint_window(HWND window)
{
    PAINTSTRUCT paint;
    auto device_context = BeginPaint(window, &paint);
    auto x = paint.rcPaint.left;
    auto y = paint.rcPaint.top;
    auto width = paint.rcPaint.right - x;
    auto height = paint.rcPaint.bottom - y;
    PatBlt(device_context, x, y, width, height, WHITENESS);
    EndPaint(window, &paint);
}

