/* Real USER queue state, independent of the active input desktop. Hidden
 * windows are distinct phase markers; stale state cannot satisfy a new phase.
 * Call only from the owning thread. No focus changes or injected input. */
typedef struct input_queue_witness {
    HWND windows[26];
} input_queue_witness;

static BOOL input_witness_set(input_queue_witness *w, int marker)
{
    HWND *window = &w->windows[marker-'A'];
    if (!*window)
        *window = CreateWindowExW(0,L"STATIC",L"Input queue witness",
            WS_OVERLAPPED,0,0,1,1,NULL,NULL,GetModuleHandleW(NULL),NULL);
    return *window && CreateCaret(*window,NULL,1,1);
}

static BOOL input_witness_read(HWND *caret)
{
    GUITHREADINFO info = {sizeof(info)};
    BOOL result = GetGUIThreadInfo(GetCurrentThreadId(),&info);
    *caret = info.hwndCaret;
    return result;
}

static void input_witness_close(input_queue_witness *w)
{
    unsigned i;
    DestroyCaret();
    for (i = 0; i < 26; ++i)
        if (w->windows[i]) DestroyWindow(w->windows[i]);
}
