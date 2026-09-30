#include "window_input_queue.h"

/* Run in the native Window input callback. GetKeyState observes this UI
 * thread's message-ordered keyboard state; sampling it later from the frontend
 * input/controller thread would observe a different queue. No library edit. */
static DWORD capture_keyboard_state(void)
{
    DWORD state = 0;
    if (GetKeyState(VK_LCONTROL) & 0x8000) state |= LEFT_CTRL_PRESSED;
    if (GetKeyState(VK_RCONTROL) & 0x8000) state |= RIGHT_CTRL_PRESSED;
    if (GetKeyState(VK_LMENU) & 0x8000) state |= LEFT_ALT_PRESSED;
    if (GetKeyState(VK_RMENU) & 0x8000) state |= RIGHT_ALT_PRESSED;
    if (GetKeyState(VK_SHIFT) & 0x8000) state |= SHIFT_PRESSED;
    if (GetKeyState(VK_CAPITAL) & 1) state |= CAPSLOCK_ON;
    if (GetKeyState(VK_NUMLOCK) & 1) state |= NUMLOCK_ON;
    if (GetKeyState(VK_SCROLL) & 1) state |= SCROLLLOCK_ON;
    return state;
}

BOOL frontend_window_input_queue_create(frontend_window_input_queue *queue)
{
    ZeroMemory(queue, sizeof(*queue));
    InitializeSRWLock(&queue->lock);
    queue->ready = CreateEventW(NULL, TRUE, FALSE, NULL);
    return queue->ready != NULL;
}

static BOOL enqueue(frontend_window_input_queue *queue, const frontend_window_input *copied)
{
    BOOL ok;
    if (!queue->ready) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    AcquireSRWLockExclusive(&queue->lock);
    if (queue->closed || queue->count == FRONTEND_WINDOW_INPUT_CAPACITY) {
        DWORD error = queue->closed ? ERROR_OPERATION_ABORTED : ERROR_BUFFER_OVERFLOW;
        ReleaseSRWLockExclusive(&queue->lock);
        SetLastError(error);
        return FALSE;
    }
    queue->events[(queue->head + queue->count) % FRONTEND_WINDOW_INPUT_CAPACITY] = *copied;
    ++queue->count;
    ok = SetEvent(queue->ready);
    ReleaseSRWLockExclusive(&queue->lock);
    return ok;
}

BOOL frontend_window_input_queue_push(frontend_window_input_queue *queue,
    const kvm_input_event *event)
{
    frontend_window_input copied = {0};
    if (!event) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    copied.event = *event;
    /* The library handle is borrowed; only its stable identity crosses
     * this asynchronous boundary, including the final retirement event. */
    copied.event.source = NULL;
    if (event->type == KVM_EVENT_KEY || event->type == KVM_EVENT_TEXT || event->type == KVM_EVENT_MOUSE) {
        copied.control_state = capture_keyboard_state();
        copied.keyboard_layout = GetKeyboardLayout(0);
        copied.ui_thread_id = GetCurrentThreadId();
    }
    /* Snapshot with the UI event, not after another event or handoff changes
     * the pointer/clip. Native text uses this absolute position; DOS still
     * consumes the original relative KVM event. */
    if (event->type == KVM_EVENT_MOUSE && GetCapture() &&
        GetCursorPos(&copied.pointer_screen) && GetClipCursor(&copied.pointer_clip) &&
        copied.pointer_clip.right>copied.pointer_clip.left &&
        copied.pointer_clip.bottom>copied.pointer_clip.top)
        copied.pointer_position_valid=TRUE;
    return enqueue(queue, &copied);
}


BOOL frontend_window_input_queue_pop(frontend_window_input_queue *queue,
    frontend_window_input *event)
{
    if (!event || !queue->ready) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    AcquireSRWLockExclusive(&queue->lock);
    if (!queue->count) {
        DWORD error = queue->closed ? ERROR_OPERATION_ABORTED : ERROR_NO_DATA;
        ReleaseSRWLockExclusive(&queue->lock);
        SetLastError(error);
        return FALSE;
    }
    *event = queue->events[queue->head];
    queue->head = (queue->head + 1) % FRONTEND_WINDOW_INPUT_CAPACITY;
    --queue->count;
    if (!queue->count && !queue->closed) ResetEvent(queue->ready);
    ReleaseSRWLockExclusive(&queue->lock);
    return TRUE;
}


void frontend_window_input_queue_close(frontend_window_input_queue *queue)
{
    AcquireSRWLockExclusive(&queue->lock);
    queue->closed = TRUE;
    if (queue->ready) SetEvent(queue->ready);
    ReleaseSRWLockExclusive(&queue->lock);
}

void frontend_window_input_queue_destroy(frontend_window_input_queue *queue)
{
    if (queue->ready) CloseHandle(queue->ready);
    queue->ready = NULL;
}
