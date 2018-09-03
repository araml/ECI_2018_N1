#include <chrono>
#include <thread>
#include <iostream>

#include <window.h>
#include <renderer.h>

using namespace std::chrono;

bool process_events() {
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT || msg.message == WM_CLOSE)
            return true;
        if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP) {
            puts("KEY");
           // handle_key(msg);
        }

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

    using fps = duration<long, std::ratio<1, 60>>; // Define 1/60th of a second

    // main loop
    while (true) {
        time_point<steady_clock> t_new = steady_clock::now();
        if (process_events())
            break;
        r.clear(); 
        // Do rendering stuff
        r.render();
        r.present();
        std::this_thread::sleep_until(t_new + fps{ 1 });
    }

    // return 0 to Windows
    return 0;
}

