#ifndef COMMON_WINDOW_STYLE_H
#define COMMON_WINDOW_STYLE_H
/* Copied launch policy, never a HWND, guest flag or process-global WOW mode. */
#define WINDOW_STYLE_OFF 0u
#define WINDOW_STYLE_WIN16 1u
#define WINDOW_STYLE_ALL 2u
#define WINDOW_STYLE_VALID(value) ((unsigned)(value) <= WINDOW_STYLE_ALL)
#endif
