#pragma once

#include <windows.h>
#include <windowsx.h>

class window {
public:
    window(int width, int height, bool fullscreen);
    ~window();

private:
    HWND m_window;
    WNDCLASSEX wc;
};

LRESULT CALLBACK window_proc(HWND hWnd, UINT message,
                             WPARAM wParam, LPARAM lParam);