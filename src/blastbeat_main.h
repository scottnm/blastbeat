/*****************************
 * Author: Scott Munro
 * File:   blastbeat_main.h
 *****************************/

#ifndef __BLASTBEAT_MAIN_H__
#define __BLASTBEAT_MAIN_H__

#include <windows.h>

int CALLBACK
WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd);

LRESULT CALLBACK
blastbeat_window_message_router(HWND window, UINT message, WPARAM w_param, LPARAM l_param);

#endif // __BLASTBEAT_MAIN_H__
