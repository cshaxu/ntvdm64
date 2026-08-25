/* DIVERGENCE(HOST-DIV-024,HOST-DIV-025): private static-provider selector and
 * pure declarations extracted from unavailable NT4 CCPU/VDM product headers. */
#ifndef OPENNT_HOST_OVERLAY_VRNMPIPE_PRODUCT_SEAMS_H
#define OPENNT_HOST_OVERLAY_VRNMPIPE_PRODUCT_SEAMS_H

#define VDMREDIR_DLL
#define IS_ASCII_PATH_SEPARATOR(ch) (((ch) == '/') || ((ch) == '\\'))
#define LOCAL_DEVICE_PREFIX "\\\\."
#define ARGUMENT_PRESENT(value) ((value) != NULL)
#define ASSERT(value) ((void)0)
#define ROUND_UP_COUNT(count, alignment) \
    (((count) + ((alignment) - 1u)) & ~((alignment) - 1u))

#define NAMED_PIPE_TIMEOUT INFINITE

typedef struct _OVERLAPPED_PIPE_IO {
    struct _OVERLAPPED_PIPE_IO* Next;
    DWORD Thread;
    BOOL Cancelled;
    OVERLAPPED Overlapped;
} OVERLAPPED_PIPE_IO, *POVERLAPPED_PIPE_IO;

#endif
