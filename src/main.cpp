#include <chrono>
#include <thread>
#include <iostream>

#include <window.h>
#include <player.h>
#include <renderer.h>
#include <mesh.h>
player p;

using namespace std::chrono;

bool process_events() {
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT || msg.message == WM_CLOSE)
            return true;
        if (msg.message == WM_KEYDOWN) {
            switch (msg.wParam) {
            case 0x57 : // w
                p.update_position(0, 0.1);
                break;
            case 0x53 : // S
                p.update_position(0, -0.1);
                break;
            case 0x41 : // A 
                p.update_position(-0.1, 0);
                break;
            case 0x44:
                p.update_position(0.1, 0);
                break;
            } 
        }
            puts("KEY");
        if (msg.message == WM_KEYUP)
            puts("hola");   // handle_key(msg);
        

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
    //  data/cube.obj data/bunny.obj
    //# OBJ file format with ext.obj
    //    # vertex count = 2503
    //    # face count = 4968
    mesh m("data/bunny.obj", r.get_device(), r.get_device_context());
    // main loop
    while (true) {
        time_point<steady_clock> t_new = steady_clock::now();
        if (process_events())
            break;
        r.clear(); 

        r.update_camera(p.get_camera());
        // Do rendering stuff
        r.render(m);
        r.present();
        std::this_thread::sleep_until(t_new + fps{ 1 });
    }

    // return 0 to Windows
    return 0;
}

