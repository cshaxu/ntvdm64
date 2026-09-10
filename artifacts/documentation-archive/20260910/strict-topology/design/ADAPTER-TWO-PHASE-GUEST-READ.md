# Adapter Two-Phase Guest Read

## Problem

Some source-derived host capabilities register state from a guest boundary and
must inspect a bounded value in ordinary guest RAM before they can faithfully
complete that boundary.  The adapter cannot receive a Bochs mapping or call a
Bochs memory API.  The existing profile observation facility deliberately
handles only profile-declared passive evidence ranges and cannot be repurposed
as a dynamic host-service reader.

## One reusable protocol

The adapter may return one of three generic outcomes at an already copied
exception boundary:

```text
pass through  -> native Bochs exception delivery
resume        -> apply checked generic CPU result, then native resume
need read     -> copy one checked ordinary-RAM range, complete in adapter,
                 then either resume or use native exception delivery
```

`need read` contains only a versioned physical range with a non-zero bounded
length.  It contains no service identifier, selector, register, profile name,
pointer, device term, callback or output result.  The completion receives a
copy of exactly those bytes together with the original immutable event, CPU
state and instruction window.  It cannot request another read.

The Bochs participant performs exactly one all-or-nothing
`copy_from_ordinary_ram` call after validation. It holds no mapping, does not
decode the bytes, and has no resume result until adapter completion succeeds.
Existing `pass through` and `resume` behavior remains byte-for-byte unchanged.

## Ownership

| Owner | Responsibility |
| --- | --- |
| CLI | Admit immutable artifact/profile inputs only; no guest address or read request. |
| Adapter | Derive a request from copied CPU state, constrain it to the real-mode aperture, interpret copied bytes, own service/session metadata, and form a generic result. |
| Bochs | Validate/copy ordinary RAM and apply only a generic accepted result. |
| Guest OpenNT | Supplies the source caller, register convention and data layout. |
| Host capability | Consumes adapter-owned descriptors only after its own source closure is admitted. |

## Safety gates

1. One request and one completion per intercepted boundary; no retry or nested
   read.
2. Full range preflight before copying. ROM, VGA, PCI, A20 remapping,
   monitored/device pages and overflow fail closed through native exception
   delivery.
3. Fixed-width records only. No Bochs object, host pointer, CRT allocation,
   raw function pointer or borrowed guest span crosses the boundary.
4. A completion may alter only fields already allowed by the generic CPU-result
   ABI. It cannot write guest memory or modify machine/device state.
5. Each semantic consumer requires its own source contract and focused tests;
   this mechanism does not itself admit DOS files, WOW, DEM or a host API.

## First completed consumer

`SVC_DEMSETDTALOCATION` is the first completed consumer. OpenNT's NTDOS caller
invokes it as `C4 C4 50 1B`; `demSetDTALocation` registers `DS:AX`, `DS:DX`,
and `DS:CX`, then follows `DS:SI` to the low word of
`DOSWOWDATA.lpSftAddr` (offset 32). The adapter therefore admits precisely one
two-byte ordinary-RAM read at `DS:(SI+32)`, with real-mode aperture and
non-wrapping checks. On successful completion it stores the four resulting
physical guest locators only inside adapter state and returns generic `RIP+4`.

This is a locator-registration seam, not a filesystem, DTA, PDB, SFT, WOW, or
DOS service implementation. No locator is exported to the CLI or host plane,
and no subsequent capability is admitted by this result alone.

## Implemented mechanism evidence

The C11 action ABI, extended single-pending-operation session test, and the
focused `DemSetDTALocation` test pass. The r20 private native container
incrementally rebuilt `cpu/exception.o`, the DTA service and adapter runtime,
then linked `runner-observation-fixture.exe`; no complete `bochs.exe` build
was run. Its bounded source-built NTIO trace records accepted `50:1b` at
`9346:7484` with `next=7488`, followed by the next unhandled source boundary
`50:32` at `9346:7490`. That is runtime evidence for this narrow registration
slice, not evidence of a filesystem or runnable DOS command.
