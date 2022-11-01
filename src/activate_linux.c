#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "i18n.h"
#include "options.h"
#include "log.h"

#ifdef GDI
  #include <windows.h>
  #include "gdi/gdi.h"
#endif

#ifdef WAYLAND
  #include "wayland/wayland.h"
#endif

#ifdef X11
  #include "x11/x11.h"
#endif

#if !defined(WAYLAND) && !defined(X11) && !defined(GDI)
  #error "One of Wayland, X11 or GDI backend must be enabled."
#endif

int main(int argc, char *const argv[]) {
  // options with their default values are in global variable `options'
  i18n_set_info(NULL);
  parse_options(argc, argv);

  if (options.kill_running) {
    __info__("Killing running instances\n");
#ifdef WAYLAND
    wayland_backend_kill_running();
#endif
#ifdef X11
    x11_backend_kill_running();
#endif
#ifdef GDI
    gdi_backend_kill_running();
#endif
    __debug__("Exit because of -K option\n");
    exit(EXIT_SUCCESS);
  }

  if (options.daemonize) {
    __info__("Forking to background\n");
#ifdef GDI
    if (GetConsoleWindow()) {
      STARTUPINFOW si = {0};
      si.cb = sizeof(si);
      PROCESS_INFORMATION pi = {0};
      bool daemon_started = !CreateProcessW(NULL, GetCommandLineW(), NULL, NULL,
                            FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi);
      if (!daemon_started) {
        __error__("Cannot start daemon process, error 0x%08lX. Continuing as non-daemon\n", GetLastError());
      } else {
        exit(daemon_started);
      }
    }
#else
    int pid = -1;
    pid = fork();
    if (pid > 0) {
      __debug__("Exiting as daemon started\n");
      exit(EXIT_SUCCESS);
    } else if (pid == 0) {
      setsid();
    }
#endif
  }

#ifdef GDI
  CreateMutex(NULL, TRUE, "activate-linux");
  if (GetLastError() == ERROR_ALREADY_EXISTS) {
      __error__("activate-linux is already running. You may kill it using -K option\n");
      return 0;
  }
#endif

  // if one backend fails, we'll try next one
  int try_next = 1;
  __info__("Starting backend\n");
#ifdef WAYLAND
  if (try_next) try_next = wayland_backend_start();
#endif
#ifdef X11
  if (try_next) try_next = x11_backend_start();
#endif
#ifdef GDI
  if (try_next) try_next = gdi_backend_start();
#endif

  return try_next;
}
