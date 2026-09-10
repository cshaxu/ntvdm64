# M0 T318 S2 P10 — Original NTIO and ordinary-child counterfactuals

## Question

Do the staged original NTIO media and the one admitted Base VDM ordinary-child
declaration affect the real selected CPU40 startup path, rather than only
formal source composition?

## Procedure

Using the P9 console-owning x86 observer, three disposable copies of the same
V7 VGA runtime input were compared for five seconds:

1. intact DOS media with `--ordinary-child`;
2. the same copy with only `dos/NTIO.SYS` removed;
3. the same copy with only `dos/COMMAND.COM` removed, once with and once
   without `--ordinary-child`.

The observer supplied valid `CONIN$`/`CONOUT$`; no product source, guest
image, BOP, or machine state was modified.

## Observations and original ownership

- The intact profile remained `STILL_ACTIVE` at the five-second bound.
- Missing `NTIO.SYS` exited with code `1`.
  `host/src/nt_msscs.c::InitialiseDosEmulation` first calls original
  `mvdm_softpc_dos_find_file("ntio.sys", ...)`; its exact failure branch calls
  `host_error`, `TerminateVDM`, and returns `-1`. This identifies the
  counterfactual's owner and proves the selected path reaches the original
  NTIO load decision.
- Missing `COMMAND.COM` with `--ordinary-child` exited with code `1`.
  The identical missing-media profile without that app declaration remained
  `STILL_ACTIVE` at the same bound. The declaration therefore reaches and is
  consumed by the source-shaped Base VDM/COMMAND route; it is not only an app
  local record.
- Both original `-f -o` and default `-f` Stream-I/O forms remained alive for
  the intact profile, but the bounded console-buffer read contained no
  observable `/C VER` text.

## Disposition

The original NTIO load decision and ordinary-child command input are now
runtime-sensitive in the selected CPU40 x86 profile. Guest command execution,
ordinary EXEC, `54:0B`, and parent restoration remain unproven: no visible
COMMAND completion may be inferred from liveness or input sensitivity. The
next work must establish that guest-owned continuation using original ownership
or transfer the first concrete cross-owner blocker; it must not substitute a
new app loader, guest executor, or BOP route.
