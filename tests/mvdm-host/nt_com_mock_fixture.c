/*
 * Test-only host-medium contract for the original SoftPC nt_com.c body.
 *
 * This translation unit includes the selected mirror source unchanged and
 * substitutes only its Win32 serial-device calls.  It is not linked into a
 * product binary and is not a serial-device provider.  The fixture proves
 * that the original open/write/receive/read/close state machine accepts a
 * successful, CTS-capable host-medium contract when the current machine has
 * no physical or virtual paired serial endpoint.
 */

#include <windows.h>

#undef CreateFile
#undef CreateEvent
#define CreateFile mock_CreateFile
#define CloseHandle mock_CloseHandle
#define CreateEvent mock_CreateEvent
#define CreateThread mock_CreateThread
#define DeleteCriticalSection mock_DeleteCriticalSection
#define EnterCriticalSection mock_EnterCriticalSection
#define InitializeCriticalSection mock_InitializeCriticalSection
#define LeaveCriticalSection mock_LeaveCriticalSection
#define GetCommState mock_GetCommState
#define GetCommTimeouts mock_GetCommTimeouts
#define GetLastError mock_GetLastError
#define GetOverlappedResult mock_GetOverlappedResult
#define ReadFile mock_ReadFile
#define SetCommState mock_SetCommState
#define SetCommTimeouts mock_SetCommTimeouts
#define SetEvent mock_SetEvent
#define SetupComm mock_SetupComm
#define WaitForMultipleObjects mock_WaitForMultipleObjects
#define WaitForSingleObject mock_WaitForSingleObject
#define WriteFile mock_WriteFile
#define ClearCommError mock_ClearCommError

HANDLE WINAPI mock_CreateFile(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES,
                              DWORD, DWORD, HANDLE);
BOOL WINAPI mock_CloseHandle(HANDLE);
HANDLE WINAPI mock_CreateEvent(LPSECURITY_ATTRIBUTES, BOOL, BOOL, LPCSTR);
HANDLE WINAPI mock_CreateThread(LPSECURITY_ATTRIBUTES, SIZE_T,
                                DWORD (__cdecl *)(PVOID), LPVOID, DWORD, LPDWORD);
VOID WINAPI mock_DeleteCriticalSection(LPCRITICAL_SECTION);
VOID WINAPI mock_EnterCriticalSection(LPCRITICAL_SECTION);
VOID WINAPI mock_InitializeCriticalSection(LPCRITICAL_SECTION);
VOID WINAPI mock_LeaveCriticalSection(LPCRITICAL_SECTION);
BOOL WINAPI mock_GetCommState(HANDLE, LPDCB);
BOOL WINAPI mock_GetCommTimeouts(HANDLE, LPCOMMTIMEOUTS);
DWORD WINAPI mock_GetLastError(void);
BOOL WINAPI mock_GetOverlappedResult(HANDLE, LPOVERLAPPED, LPDWORD, BOOL);
BOOL WINAPI mock_ReadFile(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
BOOL WINAPI mock_SetCommState(HANDLE, LPDCB);
BOOL WINAPI mock_SetCommTimeouts(HANDLE, LPCOMMTIMEOUTS);
BOOL WINAPI mock_SetEvent(HANDLE);
BOOL WINAPI mock_SetupComm(HANDLE, DWORD, DWORD);
DWORD WINAPI mock_WaitForMultipleObjects(DWORD, const HANDLE *, BOOL, DWORD);
DWORD WINAPI mock_WaitForSingleObject(HANDLE, DWORD);
BOOL WINAPI mock_WriteFile(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
BOOL WINAPI mock_ClearCommError(HANDLE, LPDWORD, LPCOMSTAT);

#include "../../src/mvdm/softpc.new/host/src/nt_com.c"

#include <stdio.h>
#include <string.h>

static DWORD mock_last_error;
static unsigned char mock_received_byte = 'R';
static unsigned char mock_written_byte;
static int mock_create_file_calls;
static int mock_get_dcb_calls;
static int mock_set_dcb_calls;
static int mock_write_calls;
static int mock_read_calls;
static int mock_close_calls;

HANDLE WINAPI mock_CreateFile(LPCSTR name, DWORD access, DWORD share,
                              LPSECURITY_ATTRIBUTES security,
                              DWORD disposition, DWORD flags,
                              HANDLE template_file)
{
    (void)access;
    (void)share;
    (void)security;
    (void)disposition;
    (void)flags;
    (void)template_file;
    if (strcmp(name, "MOCKCOM1") != 0) {
        mock_last_error = ERROR_FILE_NOT_FOUND;
        return INVALID_HANDLE_VALUE;
    }
    ++mock_create_file_calls;
    return (HANDLE)(ULONG_PTR)0x1001;
}

BOOL WINAPI mock_CloseHandle(HANDLE handle)
{
    (void)handle;
    ++mock_close_calls;
    return TRUE;
}

HANDLE WINAPI mock_CreateEvent(LPSECURITY_ATTRIBUTES security, BOOL manual,
                               BOOL initial, LPCSTR name)
{
    static ULONG_PTR next_handle = 0x2000;
    (void)security;
    (void)manual;
    (void)initial;
    (void)name;
    return (HANDLE)(next_handle++);
}

HANDLE WINAPI mock_CreateThread(LPSECURITY_ATTRIBUTES security,
                                SIZE_T stack_size,
                                DWORD (__cdecl *start)(PVOID),
                                LPVOID parameter, DWORD flags,
                                LPDWORD thread_id)
{
    (void)security;
    (void)stack_size;
    (void)start;
    (void)parameter;
    (void)flags;
    if (thread_id != NULL)
        *thread_id = 1;
    /* Close waits on this synthetic already-complete thread handle. */
    return (HANDLE)(ULONG_PTR)0x3001;
}

BOOL WINAPI mock_GetCommState(HANDLE handle, LPDCB dcb)
{
    (void)handle;
    ++mock_get_dcb_calls;
    memset(dcb, 0, sizeof(*dcb));
    dcb->DCBlength = sizeof(*dcb);
    dcb->BaudRate = CBR_9600;
    dcb->ByteSize = 8;
    dcb->Parity = NOPARITY;
    dcb->StopBits = ONESTOPBIT;
    return TRUE;
}

BOOL WINAPI mock_SetCommState(HANDLE handle, LPDCB dcb)
{
    (void)handle;
    (void)dcb;
    ++mock_set_dcb_calls;
    return TRUE;
}

BOOL WINAPI mock_SetupComm(HANDLE handle, DWORD in_queue, DWORD out_queue)
{
    (void)handle;
    (void)in_queue;
    (void)out_queue;
    return TRUE;
}

BOOL WINAPI mock_GetCommTimeouts(HANDLE handle, LPCOMMTIMEOUTS timeouts)
{
    (void)handle;
    memset(timeouts, 0, sizeof(*timeouts));
    return TRUE;
}

BOOL WINAPI mock_SetCommTimeouts(HANDLE handle, LPCOMMTIMEOUTS timeouts)
{
    (void)handle;
    (void)timeouts;
    return TRUE;
}

BOOL WINAPI mock_ReadFile(HANDLE handle, LPVOID buffer, DWORD requested,
                          LPDWORD received, LPOVERLAPPED overlapped)
{
    (void)handle;
    (void)overlapped;
    ++mock_read_calls;
    if (requested == 0) {
        *received = 0;
        return TRUE;
    }
    ((unsigned char *)buffer)[0] = mock_received_byte;
    *received = 1;
    return TRUE;
}

BOOL WINAPI mock_WriteFile(HANDLE handle, LPCVOID buffer, DWORD requested,
                           LPDWORD written, LPOVERLAPPED overlapped)
{
    (void)handle;
    (void)overlapped;
    ++mock_write_calls;
    if (requested != 1) {
        mock_last_error = ERROR_INVALID_PARAMETER;
        return FALSE;
    }
    mock_written_byte = ((const unsigned char *)buffer)[0];
    *written = 1;
    return TRUE;
}

DWORD WINAPI mock_GetLastError(void)
{
    return mock_last_error;
}

BOOL WINAPI mock_GetOverlappedResult(HANDLE handle, LPOVERLAPPED overlapped,
                                     LPDWORD transferred, BOOL wait)
{
    (void)handle;
    (void)overlapped;
    (void)wait;
    *transferred = 1;
    return TRUE;
}

BOOL WINAPI mock_ClearCommError(HANDLE handle, LPDWORD errors,
                                LPCOMSTAT stat)
{
    (void)handle;
    (void)stat;
    if (errors != NULL)
        *errors = 0;
    return TRUE;
}

BOOL WINAPI mock_SetEvent(HANDLE event)
{
    (void)event;
    return TRUE;
}

DWORD WINAPI mock_WaitForMultipleObjects(DWORD count, const HANDLE *handles,
                                         BOOL wait_all, DWORD timeout)
{
    (void)count;
    (void)handles;
    (void)wait_all;
    (void)timeout;
    return WAIT_OBJECT_0;
}

DWORD WINAPI mock_WaitForSingleObject(HANDLE handle, DWORD timeout)
{
    (void)handle;
    (void)timeout;
    return WAIT_OBJECT_0;
}

VOID WINAPI mock_InitializeCriticalSection(LPCRITICAL_SECTION section)
{
    memset(section, 0, sizeof(*section));
}

VOID WINAPI mock_DeleteCriticalSection(LPCRITICAL_SECTION section)
{
    (void)section;
}

VOID WINAPI mock_EnterCriticalSection(LPCRITICAL_SECTION section)
{
    (void)section;
}

VOID WINAPI mock_LeaveCriticalSection(LPCRITICAL_SECTION section)
{
    (void)section;
}

void *config_inquire(UTINY host_id, ConfigValues *values)
{
    static CHAR port_name[] = "MOCKCOM1";
    if (values != NULL) {
        memset(values, 0, sizeof(*values));
        strcpy(values->string, port_name);
        values->index = 1;
    }
    if (host_id == C_COM_SYNCWRITE || host_id == C_COM_TXBUFFER_SIZE)
        return NULL;
    return port_name;
}

BOOL SyncLineSettings(HANDLE file_handle, DCB *dcb,
                      DIVISOR_LATCH *divisor, LINE_CONTROL_REG *line)
{
    (void)file_handle;
    (void)dcb;
    memset(divisor, 0, sizeof(*divisor));
    memset(line, 0, sizeof(*line));
    return TRUE;
}

void SyncBaseLineSettings(int adapter, DIVISOR_LATCH *divisor,
                          LINE_CONTROL_REG *line)
{
    (void)adapter;
    (void)divisor;
    (void)line;
}

BOOL EnableMSRLSRRXmode(HANDLE file_handle, HANDLE event, unsigned char escape)
{
    (void)file_handle;
    (void)event;
    (void)escape;
    return TRUE;
}

void setup_RTSDTR(int adapter)
{
    (void)adapter;
}

void com_int_data(int adapter, int *controller, int *line)
{
    (void)adapter;
    *controller = 0;
    *line = 0;
}

void com_init(int adapter)
{
    (void)adapter;
}

void HostIdleNoActivity(void)
{
}

void tx_holding_register_empty(int adapter)
{
    (void)adapter;
}

void tx_shift_register_empty(int adapter)
{
    (void)adapter;
}

/* Unreached original branches remain in this direct-source fixture object.
 * Keep their surrounding SoftPC callbacks inert; the asserted route below
 * is open/write/GetCharsFromDriver/read/close only. */
void com_recv_char(int adapter) { (void)adapter; }
void com_modem_change(int adapter) { (void)adapter; }
void com_lsr_change(int adapter) { (void)adapter; }
SHORT host_error(int error_num, int options, char *extra)
{
    (void)error_num;
    (void)options;
    (void)extra;
    return 0;
}
VOID RcErrorBoxPrintf(UINT id, CHAR *message)
{
    (void)id;
    (void)message;
}
void ica_inb(io_addr port, IU8 *value)
{
    (void)port;
    *value = 0;
}
void ica_outb(io_addr port, IU8 value)
{
    (void)port;
    (void)value;
}
void *AddNewIOStatusBlockToList(void **first, void **last, void *value)
{
    (void)first;
    (void)last;
    return value;
}
int RemoveCompletedIOCTLs(void **first, void **last)
{
    (void)first;
    (void)last;
    return 0;
}
void *AllocStatusElement(void) { return NULL; }
int SendXOFFIoctl(HANDLE file, HANDLE event, int timeout, int count,
                  int xoff, void *status)
{
    (void)file;
    (void)event;
    (void)timeout;
    (void)count;
    (void)xoff;
    (void)status;
    return 0;
}
int FastSetCommMask(HANDLE file, HANDLE event, ULONG mask)
{
    (void)file;
    (void)event;
    (void)mask;
    return 1;
}
int FastGetCommModemStatus(HANDLE file, HANDLE event, PULONG status)
{
    (void)file;
    (void)event;
    *status = MS_CTS_ON;
    return 1;
}
BOOL FastWaitCommsOrCpuEvent(HANDLE file, PHANDLE events, int index,
                             PULONG mask, PULONG signalled)
{
    (void)file;
    (void)events;
    (void)index;
    *mask = 0;
    *signalled = 1;
    return TRUE;
}
BOOL FastCommSetBaudRate(HANDLE file, int baud)
{
    (void)file;
    (void)baud;
    return TRUE;
}
BOOL FastCommSetLineControl(HANDLE file, UCHAR stop, UCHAR parity, UCHAR bits)
{
    (void)file;
    (void)stop;
    (void)parity;
    (void)bits;
    return TRUE;
}
BOOL FastCommGetLineControl(HANDLE file, UCHAR *stop, UCHAR *parity, UCHAR *bits)
{
    (void)file;
    *stop = ONESTOPBIT;
    *parity = NOPARITY;
    *bits = 8;
    return TRUE;
}
LONG VdmUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *exception)
{
    (void)exception;
    return EXCEPTION_EXECUTE_HANDLER;
}
int DisplayErrorTerm(int error, DWORD os_error, char *file, int line)
{
    (void)error;
    (void)os_error;
    (void)file;
    (void)line;
    return 0;
}

int main(void)
{
    UTINY data = 0;
    int error = 0;

    if (!host_com_open(0)) {
        fprintf(stderr, "open failed\n");
        return 1;
    }
    host_com_write(0, 'T');
    if (mock_write_calls != 1 || mock_written_byte != 'T') {
        fprintf(stderr, "write contract failed\n");
        return 2;
    }
    if (GetCharsFromDriver(0) != 1) {
        fprintf(stderr, "receive injection failed\n");
        return 3;
    }
    host_com_read(0, &data, &error);
    if (data != 'R' || error != 0) {
        fprintf(stderr, "read contract failed: data=%u error=%d\n", data, error);
        return 4;
    }
    host_com_close(0);
    if (mock_create_file_calls != 1 || mock_get_dcb_calls != 1 ||
        mock_set_dcb_calls < 2 || mock_read_calls == 0 || mock_close_calls < 1) {
        fprintf(stderr, "lifecycle accounting failed\n");
        return 5;
    }
    puts("T420_S30_NT_COM_MOCK_OPEN_WRITE_READ_CLOSE_OK");
    return 0;
}
