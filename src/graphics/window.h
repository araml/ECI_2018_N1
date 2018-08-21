#pragma once

#include <windows.h>
#include <windowsx.h>

class window {
public:
    window(int width, int height, bool fullscreen);
    ~window();
    void* native_window() { return (void*)m_window;  }
    int width() const { return m_width; }
    int height() const { return m_height; }
private:
    int m_width, m_height;
    HWND m_window;
    WNDCLASSEX wc;
};

LRESULT CALLBACK window_proc(HWND hWnd, UINT message,
                             WPARAM wParam, LPARAM lParam);