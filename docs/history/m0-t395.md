# m0-t395 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t395-cpu40-dpmi32-provider-closure-20260906.md

# M0 T395 — CPU40 DPMI32 provider closure

Date: 2026-09-06

## Scope and owner acceptance

T395 was admitted to recover the CPU40 DPMI32 provider and its original DOSX
bootstrap.  The owner subsequently accepted closure of this package after the
complete host-provider surface and reachable bootstrap were evidenced, with
the real Win16 client start/return proof explicitly transferred to the next
WOW32 provider package.

## Delivered result

- The CPU40 formal DPMI32 cohort is correctly 15 C bodies: thirteen portable
  `sources` members and `dpmimemr.c`/`dpmimscr.c`; three `i386/` kernel-VDM
  bodies remain historical-only.
- All 25 `53:00`–`53:18` dispatch rows have an original owner and a CPU40
  composition disposition.  `53:0E` is the deliberate original CPU40
  unsupported debug-register path, not a missing provider.
- The CPU40 `53:01` source-shaped transition, `53:0F` LDT binding, `53:00`
  descriptor synchronization, shared-XMS services, and `53:11` guest-linear
  PM-stack projection were recovered with registered divergences
  `MVDM-HOST-DIV-222` through `DIV-226`.
- The formal x86 product linked, the DPMI mirror audit passed, and the
  immutable DOSX bootstrap crossed `53:01` and executed downstream original
  DPMI services (`00`, `02`, `03`, `04`, `07`, `0A`, `0F`, `11`).

The detailed S records are [S1 admission audit](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t395-s1-dpmi32-runtime-admission-audit-001.md),
[S2 protected-mode bridge](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t395-s2-cpu40-protected-mode-entry-bridge-001.md),
and [S3 dispatch/lifecycle audit](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t395-s3-cpu40-dpmi-dispatch-lifecycle-audit-001.md).

## Explicit limitation and successor

The available original client images (`KRNL386.EXE`, `WINHELP.EXE`, and
`WOWEXEC.EXE`) require the original WOW32 provider/callback/task-lifetime
route.  Naked DOSX therefore cannot prove normal client termination through
`53:13`/`53:0D`.  This is not represented as a completed Win16 runtime row.

M0 T396 owns that successor boundary: complete WOW32 provider admission,
bootstrap and an original Win16 NE start/return workload.  It must not treat
T395's bootstrap evidence as a substitute for client runtime proof.

