#include "../options.h"
#include "../log.h"
#include "gdi.h"
#include "gdi_draw.h"
#include <stdio.h>
#include <windows.h>

// Le classique https://stackoverflow.com/a/17387176
void PrintLastError(void) {
    LPSTR messageBuffer = "";
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, GetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), messageBuffer, 0, NULL);
    __error__("%s", messageBuffer);
    LocalFree(messageBuffer);
}

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType) {
    if (dwCtrlType == CTRL_C_EVENT)
        ExitProcess(EXIT_SUCCESS);
    return FALSE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        case WM_PAINT: {
            __info__("Got WM_PAINT message. Starting (re)drawing text\n");
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            draw_text_gdi(hwnd, hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        // would be nice to implement https://learn.microsoft.com/en-us/windows/win32/hidpi/wm-dpichanged
        case WM_DESTROY:
            __info__("Got WM_DESTROY message. Shutting down\n");
            exit(EXIT_SUCCESS);
        default:
            // https://wiki.winehq.org/List_Of_Windows_Messages
            __debug__("Got message 0x%04X (%4d) wParam 0x%08lX (hi: %d lo: %d), lParam 0x%08llX\n", Message, Message, (long unsigned int)wParam, HIWORD(wParam), LOWORD(wParam), lParam);
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
}

int gdi_backend_start(void) {
    __info__("GDI backend starting\n");
    SetConsoleCtrlHandler(HandlerRoutine, TRUE);

    __debug__("Registering window class\n");
    WNDCLASS wc = {0, WndProc, 0, 0, NULL, NULL, NULL, NULL, "", WINDOW_CLASS};
    if (!RegisterClass(&wc)) {
        __error__("Failed to register class \"" WINDOW_CLASS "\"\n");
        PrintLastError();
        exit(EXIT_FAILURE);
    }

    __debug__("Determining monitor resolution\n");
    MONITORINFO monitorinfo = {sizeof(monitorinfo), {0, 0, 0, 0}, {0, 0, 0, 0}, 0};
    GetMonitorInfo(MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY), &monitorinfo);
    __debug__("Full resolution: %ldx%ld\n", monitorinfo.rcMonitor.right, monitorinfo.rcMonitor.bottom);
    __debug__("Work resolution: %ldx%ld\n", monitorinfo.rcWork.right, monitorinfo.rcWork.bottom);

    __debug__("Creating window\n");
    HWND hwnd = CreateWindowEx(WS_EX_LAYERED|WS_EX_TOPMOST|WS_EX_NOACTIVATE, WINDOW_CLASS, WINDOW_CLASS, WS_VISIBLE,
        monitorinfo.rcWork.right  - (options.overlay_width  * options.scale),
        monitorinfo.rcWork.bottom - (options.overlay_height * options.scale),
        options.overlay_width,
        options.overlay_height,
        NULL, NULL, NULL, NULL);
    if (hwnd == NULL) {
        __error__("Failed to CreateWindowEx\n");
        PrintLastError();
        exit(EXIT_FAILURE);
    }

    __debug__("Sent redraw signal to the window\n");
    InvalidateRect(hwnd, NULL, TRUE);

    __debug__("Starting window message loop\n");
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        DispatchMessage(&msg);
    }

    return 0;
}

int gdi_backend_kill_running(void) {
    HWND hwnd = FindWindow(WINDOW_CLASS, NULL);
    if (hwnd != NULL) {
        __debug__("Sent WM_CLOSE to window with HWND %p\n", (void*)hwnd);
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
    return 0;
}
