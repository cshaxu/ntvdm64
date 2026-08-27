# mvdm-support-overlay

This overlay is called only by the matching `mvdm-support` static
library build. It preserves the four original OEM exports that contain x86
pointer truncation, while the original bodies remain byte-exact evidence.

## Registered divergences

| ID | Original purpose | Reason | Implementation | Files |
| --- | --- | --- | --- | --- |
| SUPPORT-OVERLAY-001 | OEM find-data header copy and file-part output reconstruction. | The original casts native pointers to `ULONG`/`DWORD`; that is unsafe on x64. | Preserve the four original APIs and use `offsetof` or a converted buffer-relative offset. | `source/oemuni_pointer_width.c` |
