# OpenNT Guest Build Inventory

Status: source-graph inventory reconciled with retained local build evidence,
2026-08-08. No guest binary is a default project input or distributable output.

The fixed OpenNT tree contains distinct historical DOS/V86 build entry points:

| Guest artifact | Historical build entry |
| --- | --- |
| `NTIO.SYS` | `base/mvdm/dos/v86/doskrnl/bios/makefile` |
| `NTDOS.SYS` | `base/mvdm/dos/v86/doskrnl/dos/makefile` |
| `COMMAND.COM` | `base/mvdm/dos/v86/cmd/command/makefile` |
| `HIMEM.SYS` | `base/mvdm/dos/v86/dev/himem/makefile` |
| `DOSX.EXE` | `base/mvdm/dpmi/makefile` |
| `REDIR.EXE` | `base/mvdm/dos/v86/redir/makefile` |

The same tree also carries a 32-bit `dos/command` source target, host-side DEM,
and the VDM redirector DLL source. These do not prove that a guest artifact is
reproducible: the DOS makefiles expect their original assembler, linker,
message/resource generators, country data, and build-layout variables.

The research build has two separate admission tracks:

1. Historical host-stack and machine-provider object closure under isolated
   clang-cl/i686 gates.
2. A completed reproducible guest-toolchain profile for the 22 direct
   unchanged-source outputs recorded in `NT4-GUEST-ARTIFACT-CLOSURE.md`, with
   per-stage provenance and a BYOB-only output policy.

Four direct payload files remain original `binplace` inputs rather than source
builds: `BACKUP.EXE`, `QBASIC.EXE`, `MSHERC.COM`, and `EDIT.COM`. The detailed
current classification is `M76-NT4-GUEST-CLOSURE-EVIDENCE-AUDIT.md`.

A successful host object gate remains independent from guest source-to-image
evidence and from runtime execution support.
