# M0 T324 S2 — Local Redirector guest-span and ICA binding

## Result

The selected local Redirector cohort cannot be made runnable by selecting its
original translation units plus a cdecl thread thunk.  This is not an
unbounded new subsystem: the original source already supplies the dispatcher,
request queue, completion ordering and ICA call.  Its one missing modern
binding is a private, source-owned conversion at every guest-memory access.

## Original boundary

- `vrdisp.c` maps a real-mode pair through `_inlinePointerFromWords`; on the
  historical x86 product this delegated to `GetVDMAddr`.
- `vrnmpipe.c` uses that form for immediate request records and buffers, then
  retains `Buffer`, `pBytesTransferred`, `pErrorCode`, `ANR`, and `Semaphore`
  beyond the BOP call in `DOS_ASYNC_NAMED_PIPE_INFO`.
- `vrmslot.c` has the same immediate real-mode input/output form.
- `vrinit.c` owns the original serialized completion queue and calls the
  already mirrored SoftPC `call_ica_hw_interrupt(NETWORK_ICA, NETWORK_LINE, 1)`.
  It is therefore neither a missing Win32 notification nor a license to
  synthesize an interrupt in Redirector code.

## Binding decision

`mvdm_guest_location` and the session's existing guest-memory lease are the
only admissible guest-span facility.  A local Redirector overlay must:

1. retain original 16:16 values in its original request structures;
2. acquire a fresh bounded lease for each immediate request decode or
   completion write-back;
3. stage async read/write bytes in host-private memory, never in a retained
   guest alias;
4. leave `VrQueueCompletionHandler`, `VrRaiseInterrupt`,
   `VrNmPipeInterrupt`, cancellation and cleanup order in their original
   owners; and
5. bind a worker to its creating session only via the existing cdecl-thread
   compatibility binding.

The overlay is private to `mvdm-host/vdmredir`; it has no adapter-facing
public ABI and introduces no mapping manager or redirector provider.

## Consequence for source selection

The existing forced `mvdm_redirector_create_thread` decline remains correct
until the above private overlay replaces every retained-pointer access in the
local cohort.  Selecting `vrnmpipe.c` unchanged would otherwise make an
ordinary 16:16 numeric value appear to be a persistent native pointer.
`call_ica_hw_interrupt` is a real selected SoftPC source capability, but the
current formal product generator does not yet select `vdmredir`; adding the
cohort is permitted only together with its private overlay and all declared
original dependencies.

## Non-results

This does not enable a 57:xx BOP, remote/RAP/NetBIOS/DLC execution, VDD hook
success, or a guest interrupt claim.  It identifies the bounded work required
for original local-source recovery.
