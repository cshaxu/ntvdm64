#ifndef NTVDM_VM_TEXT_H
#define NTVDM_VM_TEXT_H
#include <windows.h>
#include "common/protocol/console_io.h"

/* Copied VMM BIOS snapshot. Each following cell is one OEM byte/attribute
 * pair. Geometry, code page and font height come from the actual guest. */
typedef struct ntvdm_vm_text_header {
    uint32_t columns, rows, x, y, shape, mode, page, codepage, font_height;
} ntvdm_vm_text_header;
typedef struct ntvdm_vm_text ntvdm_vm_text;

DWORD ntvdm_vm_text_create(const void *, uint32_t, ntvdm_vm_text **);
void ntvdm_vm_text_destroy(ntvdm_vm_text *);
/* Serving thread after channel EOF: abandon uncommitted publication/snapshot,
 * retaining only the last guest-acknowledged state for the next channel. */
void ntvdm_vm_text_disconnect(ntvdm_vm_text *);
/* Single channel owner; no internal waits or guest pointers. Text publication
 * changes only the private candidate until its exact bytes are guest-applied.
 * Unsupported operations are errors, not a claim of a complete I/O provider. */
DWORD ntvdm_vm_text_dispatch(ntvdm_vm_text *, const console_io_request *, console_io_reply *);
/* Implicit DOS publications share the same candidate/acknowledgment owner as
 * explicit native batches. Pending=0/1/2: none/explicit/implicit. */
DWORD ntvdm_vm_text_begin(ntvdm_vm_text *, BOOL implicit);
unsigned ntvdm_vm_text_pending(const ntvdm_vm_text *);
BOOL ntvdm_vm_text_ready(const ntvdm_vm_text *);
/* Borrowed until finish/abort; the application owner must apply BOTH text and
 * any video payload before reporting success. Video includes fonts/palette,
 * not merely duplicate character cells. No raw guest pointers are exposed. */
typedef struct ntvdm_vm_text_publication {
    const void *text;
    uint32_t text_bytes, video_serial;
    const console_video_description *video;
    const void *video_bytes;
} ntvdm_vm_text_publication;
DWORD ntvdm_vm_text_candidate(ntvdm_vm_text *, ntvdm_vm_text_publication *);
/* Copied guest application: BIOS header/cells, optionally followed by this
 * versioned trailer and description.bytes bytes of the complete VIDEO frame.
 * No pointers, native packing assumptions or host handles cross the bridge. */
#define NTVDM_VM_VIDEO_SIGNATURE 0x31564d56u
#define NTVDM_VM_VIDEO_VERSION 1u
typedef struct ntvdm_vm_video_application {
    uint32_t signature, version;
    console_video_description description;
} ntvdm_vm_video_application;
typedef char ntvdm_vm_video_style_size[
    sizeof(console_text_style) == 16420 && offsetof(console_text_style, fonts) == 36 ? 1 : -1];
typedef char ntvdm_vm_video_application_size[
    sizeof(ntvdm_vm_video_application) == 1056 ? 1 : -1];
/* Allocates one immutable wire copy; caller HeapFree's it after the existing
 * continuation publication has copied it. Does not commit or acknowledge. */
DWORD ntvdm_vm_text_copy_application(ntvdm_vm_text *, void **, uint32_t *);
DWORD ntvdm_vm_text_finish(ntvdm_vm_text *, DWORD application_error);
#endif
