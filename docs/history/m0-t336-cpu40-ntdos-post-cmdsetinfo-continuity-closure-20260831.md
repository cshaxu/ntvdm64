# M0 T336 closure — CPU40/NTDOS post-`54:05` startup continuity

T336 closes at an exact original terminal boundary, not at DOS execution.

- S1 established that NTIO/NTDOS guest media had already loaded and that the
  observed post-`54:05` fault was not a missing loader or BOP provider.
- S2 restored the existing original CCPU40 `host_simulate_func` initialization
  call required by `floppy.c::wait_int`; the former null native target no
  longer occurs.
- S3 selected the complete original FDC/INT15/ICA/heartbeat cohort and
  rejected a speculative floppy, BIOS, or loader repair.
- S4 attributes the subsequent live state to original `50:3D -> demExitVDM`
  and its error-dialog wait.  It identifies guest drive-root/media projection
  as the next product boundary while preserving the distinction between the
  two original guest callers of that terminal service.

Paired x86/x64 formal CCPU40 build/link evidence remains valid for the
selected product.  The bounded runtime observation is x86-only.  No claim is
made that COMMAND.COM started, that a DOS child executed, or that parent
return works.

The successor must be an explicitly admitted portable boot-media/guest-drive
composition package.  It may not solve the gap by inventing a BOP result,
rewriting guest media, installing files on the host system, or reviving a
Bochs route.
