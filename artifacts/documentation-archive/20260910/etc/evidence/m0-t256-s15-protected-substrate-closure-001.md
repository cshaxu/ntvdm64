# M0 T256 S15 — Protected substrate closure

## Scope audited

T256 was admitted to recover the selector-blind protected virtual-access and
guest-linear mapping substrate before the OpenNT DPMI owner package.  Its
completed parts are:

| Contract | Closure evidence | Result |
| --- | --- | --- |
| Typed protected context expression/application | `BX-ABI-094`, T255 S2 | Closed mechanical context ABI; no DPMI policy. |
| Fault-contained non-paged protected copied access | `BX-MANTLE-095`, T256 S1--S3 | Closed bounded copy action; no pointer return or paging claim. |
| Guest RAM capacity/geometry | `BX-MANTLE-096`, T256 S5--S6 | Closed explicit capacity/configuration contract. |
| Ordinary-RAM reservation lifecycle | `BX-MANTLE-097`, T256 S7--S8 | Closed selector-blind reserve/release lifecycle with opaque mantle IDs. |
| OpenNT XMEM identity and reallocation | T256 S9--S11 | Closed bx-vdm record adapter preserves one record ID across copied ordinary-RAM reallocation; no host pointer crosses ABI. |
| Descriptor/LDT identity | T256 S12--S13 | Closed classification: original NT4 process-LDT composition cannot become a mantle descriptor API; native Bochs retains architectural descriptor ownership. |
| Interrupt/fault/IRET identity | T256 S14 | Closed classification: OpenNT hook/locked-stack lifecycle is whole DPMI/DOSX work; native Bochs retains architectural fault/IRET ownership. |

## Fresh formal verification

A new graph was generated under `build/M0-T256-S15/formal-r1` from
`tools/build/t225-s7-full-module-manifest.json`.  It uses MSVC x64 `/MT`, the
pinned CPU5/P-MMX projection and the formal Ninja graph; it has no Bochs
product target or prebuilt adapter input.

```
ninja -C build/M0-T256-S15/formal-r1 \
  bin/t256-s10-xmem-record-adapter-fixture.exe
build graph: 278/278 completed
t256-s10-xmem-record-adapter-fixture.exe: exit 0
```

The fixture covers inactive rejection, explicit reservation placement,
record allocation/release/owner cleanup/reset, copied growth reallocation,
record-ID preservation and stale-ID rejection.  It is a local substrate
verification only; it does not activate `53:xx` or claim a DPMI guest run.

## Boundary and handoff

No selected T256 code creates a host pointer, imports an NT4 process-LDT API,
duplicates a CPU descriptor cache, adds BOP/DOS vocabulary to bx-core or
bx-mantle, or leaves a v1 DPMI route active.

The exact successor is the queued **OpenNT DPMI protected-mode owner-package
recovery**.  It must import/recover DOSX and `dpmi32` as one source package:
table publication/selector policy, protected frame/IRET lifecycle, IDT/fault
handling, and then the already-prepared XMEM records.  It may use the closed
generic contracts above, but must not reinterpret their ownership.

## S15 and T256 closure

S15 passes the final evidence/tracker/formal-build audit.  T256 closes the
generic prerequisite, not the DPMI provider.  The following T257 S1 admission
is therefore a source-package inventory/ABI/failure map rather than a `53:xx`
implementation task.
