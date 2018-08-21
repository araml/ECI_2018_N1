#include <iostream>

#include <window.h>
#include <renderer.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

bool process_events() {
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT)
            return true;

        TranslateMessage(&msg);
        DispatchMessage(&msg); 
    }

    return false;
}

// the entry point for any Windows program
int main() {
    //FreeConsole();
    
    window w(500, 400, false);
    renderer r(w);

    // main loop
    while (true) {
        if (process_events())
            break;
        r.clear(); 
        // Do rendering stuff
        r.render();
        r.present();
    }

    // return 0 to Windows
    return 0;
}

