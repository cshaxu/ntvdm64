# MASM386 Pristine NTIO And NTDOS Audit - 2026-08-07

## Scope

This audit used `masm386-managed.exe`, rebuilt from the managed OpenNT MASM
source closure, against new stages copied from `src/opennt`. The stages contain
no guest-source transformation. The assembler identity was:

```text
SHA-256: 8F0DABF839E6A5D679F4EBD7260F929CBF6C79FD268E764A0F2891BD35F07691
```

The first attempted invocation was blocked by the local PowerShell execution
policy. A one-process `-ExecutionPolicy Bypass` launch ran the project-local
audit script without changing the machine policy. This is recorded as a host
execution-policy condition, not a MASM or guest-source failure.

## Per-Object Results

| Profile | Original-source modules | MASM386 pass | MASM386 fail | Result |
| --- | ---: | ---: | ---: | --- |
| `NTDOS.SYS` | 38 | 38 | 0 | `masm386-object-audited` |
| `NTIO.SYS` | 12 | 12 | 0 | `masm386-object-audited` |

The machine-readable per-object reports, including include paths, diagnostics,
object sizes, and hashes, are retained locally:

- `artifacts/toolchain-runs/masm386-pristine-ntdos-v1/masm386-pristine-assembly-audit.json`
- `artifacts/toolchain-runs/masm386-pristine-ntio-v1/masm386-pristine-assembly-audit.json`

This proves that the previously recorded JWasm source overlays are not needed
for original-source MASM386 assembly of these selected lists. Historical LINK
reproduction was subsequently completed; see the next section. It still does
not prove runtime behavior or retail identity.

## Historical LINK Reproduction

The original response files were linked unchanged through the recovered
`LINK.EXE` inside DOSBox. The fresh v4 stages then used the same post-link
operations named by the OpenNT makefiles: `reloc ntio.exe NTIO.SYS 70` and
`reloc NTdos.EXE ntdos.bin` followed by `stripz ntdos.bin NTDOS.SYS`.

| Profile | Historical MZ image | Final image | Result |
| --- | --- | --- | --- |
| `NTIO.SYS` | 34,304 bytes, SHA-256 `ADA8DE4F043752C11FB5D1F9088600E5D8F337DA6AF04A146DE0AB34D68F450A` | 33,792 bytes, SHA-256 `CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937` | `historical-link-reproduced` |
| `NTDOS.SYS` | 37,154 bytes, SHA-256 `60BE9202718BFC1A28EC0390CD94F988C4015F9CE95B6384DC323AED2C557A42` | 27,858 bytes, SHA-256 `957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84` | `historical-link-reproduced` |

The runs are `artifacts/toolchain-runs/ntio-masm386-historical-link-v4/` and
`artifacts/toolchain-runs/ntdos-masm386-historical-link-v4/`. Their manifests
declare `GuestSourceTransforms: []`. Runtime validation remains pending.

## WLink Comparison

The original object ordering was submitted to the existing OpenWatcom WLink
comparison profile. Both commands returned exit code zero, but each emitted
`W1162` warnings for relocations on iterated data. Therefore neither output is
an admissible image or a LINK16 equivalent.

| Profile | MASM386 + WLink bytes | MASM386 + WLink SHA-256 | Legacy JWasm + WLink bytes | Equal | Blocking evidence |
| --- | ---: | --- | ---: | --- | --- |
| `NTDOS.SYS` | 36,674 | `905CA87F51A14779035BF60C3303E17FD1A18BDA9DED445CFCE3007F5FF2EE9B` | 36,818 | no | `msdata.obj` LIDATA relocation warning |
| `NTIO.SYS` | 33,952 | `0EB9A8DBD4AEA5F793781D06FDFDC3D0CEB7A91D1614BD7DFB7E835854BEB39E` | 33,984 | no | `msbio1.obj` and `sysinit1.obj`/`sysconf.obj` LIDATA relocation warnings |

The corresponding WLink logs are retained in each pristine stage as
`masm386-wlink.log`. `W1014` (no stack segment) and `W1023` (no start address)
were also reported. Those metadata warnings do not erase the `W1162` blocker.

## Decision

## Superseding LINK16 Result

The preceding historical-link table is superseded by fresh tool-only stages.
`ntio-historical-tools-v5` completed its unchanged `MSBIO_4.LNK` and RELOC
path with OpenNT `LINK16.EXE`, yielding NTIO.SYS at 33,792 bytes and SHA-256
`CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937`.
`ntdos-historical-tools-v5` assembled all 38 unchanged modules but LINK16
rejected `msdata.obj` at offset 2458, record `A0`; no NTDOS image was created.
The previous claim that NTDOS historical linking completed is withdrawn.

1. Retire JWasm as a default build path for NTIO and NTDOS; preserve it only
   behind the explicit diagnostic switch for historical comparison.
2. Do not add source shims or overlays for NTIO or NTDOS assembly. The pristine
   MASM386 result has removed the evidence basis for doing so.
3. Treat the historical DOSBox/LINK runner as the canonical path for these
   modules; retain WLink only as prior comparison evidence.
4. Apply the same per-object MASM386 process to SETVER, COUNTRY, KB16,
   KEYBOARD, REDIR, APPEND, and the assembled MEM modules before any new
   compatibility-assembler or OpenWatcom build attempt.
