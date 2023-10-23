#include "../options.h"
#include <windows.h>

void draw_text_gdi(HWND hwnd, HDC windowDC) {
    // create abstract surface (device context, DC)
    HDC buffDC = CreateCompatibleDC(windowDC);
    // create bitmap compatible (by bit depth and so on) with surface
    HBITMAP buffBitmap = CreateCompatibleBitmap(windowDC,
        options.overlay_width * options.scale,
        options.overlay_height * options.scale
    );
    // assign one to another
    DeleteObject(SelectObject(buffDC, buffBitmap));

    #define GetTextColorComponent(component) (int)(options.text_color.component*255)
    SetTextColor(buffDC, RGB(GetTextColorComponent(r), GetTextColorComponent(g), GetTextColorComponent(b)));
    // don't fill background when drawing
    SetBkMode(buffDC, TRANSPARENT);

    // logical font is a base for "real" font
    LOGFONT logfont;
    GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &logfont);
    logfont.lfHeight = 24 * options.scale;
    logfont.lfWeight = options.bold_mode ? FW_BOLD : FW_NORMAL;
    logfont.lfItalic = options.italic_mode;
    // Doesn't work :(
    // _tcscpy(logfont.lfFaceName, _T("Comic Sans"));
    DeleteObject(SelectObject(buffDC, CreateFontIndirectA(&logfont)));

    // text in options is in UTF-8, Windows wants UTF-16. Let's convert this
    #define UTF16CONVERTED_LEN 666
    WCHAR utf16converted[UTF16CONVERTED_LEN];
    MultiByteToWideChar(CP_UTF8, 0, options.title, -1, utf16converted, UTF16CONVERTED_LEN);

    // select place and draw text
    RECT rect = {20*options.scale, 30*options.scale, 0, 0};
    DrawTextW(buffDC, utf16converted, -1, &rect, DT_NOCLIP);

    // same for 2nd line
    MultiByteToWideChar(CP_UTF8, 0, options.subtitle, -1, utf16converted, UTF16CONVERTED_LEN);
    logfont.lfHeight = 16 * options.scale;
    HFONT font = CreateFontIndirect(&logfont);
    DeleteObject(SelectObject(buffDC, font));
    rect.top = 55 * options.scale;
    DrawTextW(buffDC, utf16converted, -1, &rect, DT_NOCLIP);

    // copy pixels from our surface with text to window's surface
    SIZE wsize = {options.overlay_width * options.scale, options.overlay_height * options.scale};
    POINT zero = {0};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, options.text_color.a*255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, windowDC, NULL, &wsize, buffDC, &zero, 0, &blend, ULW_ALPHA);

    // free resources
    DeleteObject(font);
    DeleteObject(buffBitmap);
    DeleteDC(buffDC);
    return;
}
