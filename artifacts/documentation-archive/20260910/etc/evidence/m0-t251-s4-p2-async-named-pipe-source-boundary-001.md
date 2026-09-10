# M0 T251 S4 P2 — async named-pipe source-boundary closure

## Original continuation chain

The retained guest source makes the ownership boundary exact:

1. `redir/namepipe.asm:426+` accepts a packed `DOS_ASYNC_NAMED_PIPE_STRUCT`
   at `DS:SI`, first calls `MapNtHandle`, and invokes `57:23` for both read
   and write forms.
2. `MapNtHandle` obtains a 32-bit value from the guest DOS SFT field
   `sf_NtHandle` and places it in `BP:BX`; historical code stores a raw NT
   handle there.
3. The absent VDMREDIR body would retain `DOS_ASYNC_NAMED_PIPE_INFO` and a
   serialized `VR_ASYNC_DISPOSITION`.
4. Completion is not a host callback: `redir/int5c.asm:318+` receives the
   redirected network interrupt, obtains the copied ANR/buffer/semaphore
   addresses from the provider, and makes a guest far call to the ANR.

## Current source-shaped disposition

`57:23` and `57:24` are explicit `ERROR_INVALID_FUNCTION` / CF terminals.
Their formal fixture proves the typed resume and failure value.  They do not
attempt `ReadFile`/`WriteFile`, retain a guest pointer, start a worker, or
expose a host handle before the two required owner contracts exist.

This is not a claim that asynchronous named pipes are implemented.  It is a
complete failure disposition for this T251 sub-package and prevents a false
synchronous success.

## Required subsequent owner contracts

| Need | Correct owner | Why it cannot be recreated here |
| --- | --- | --- |
| SFT token seam | guest DOS/NTDOS + shared handle-manager composition | OpenNT's guest `sf_NtHandle` stores a raw 32-bit HANDLE.  bx-vdm must instead receive an opaque token through a checked, owner-defined SFT seam. |
| Interrupt/ANR delivery seam | selector-blind machine/BIOS composition | The actual guest `int 5Ch` path sets the call frame and executes the ANR.  Redirector may provide a copied completion record, but must not modify CPU/IVT/PIC state or call a guest address. |
| Serialized completion queue | later Redirector continuation package, after both seams | `DOS_ASYNC_NAMED_PIPE_INFO` and `VR_ASYNC_DISPOSITION` can be recovered with public overlapped I/O/events, without CCPU, CSR or BaseSrv, only once a typed completion delivery contract exists. |

The later package must retain the OpenNT ordering and `MAXIMUM_ASYNC_PIPES`
bound, but use session-owned opaque handles and copied 16:16 values.  It must
not revive raw guest callbacks or historical NT4 product infrastructure.
