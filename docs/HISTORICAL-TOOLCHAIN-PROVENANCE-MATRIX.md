# Historical Toolchain Provenance Matrix

Status: historical matrix superseded by `HISTORICAL-TOOLCHAIN-REBASE-2026-08-07.md`, 2026-08-07.

> This file preserves the pre-rebase comparison record. Its references to
> source-built MASM386, WLink, locally rebuilt relocation tools, and host
> compatibility message generators are not active build policy.

## Purpose

OpenNT real-mode and DOS build descriptions name a historical toolchain, but
the current research stages use a deliberately mixed, evidence-recorded
toolchain. This matrix prevents a source-to-image result from being described
as a retail or historical-tool reproduction merely because it assembled or
linked successfully.

It applies to local research stages only. It neither distributes an artifact
nor changes the BYOB rule.

## Terminology

| Term | Meaning in this project |
| --- | --- |
| historical expectation | Tool named or implied by the fixed OpenNT makefile/link description. |
| compatible tool | Modern or reconstructed tool used only after its input, invocation, and limitations are recorded. |
| source-built MASM386 | A host executable rebuilt from the managed OpenNT `sdktools/masm` text-source closure. It is not an original Microsoft-distributed assembler binary. |
| structural result | A stage image whose source, object, link, and conversion evidence has been recorded. It is not runtime or retail-identity evidence. |

## Tool Roles

| Historical role | Current research substitute or state | Admission boundary |
| --- | --- | --- |
| MASM / MASM386 | source-built MASM386 is preferred where it accepts the original source; JWasm remains diagnostic-only for any retired compatibility stage. | Do not claim global interchangeability. Each object records its selected assembler. |
| LINK16 | OpenWatcom WLink in a named DOS/OMF profile. | It is not LINK16; unsupported OMF features, header-layout differences, warnings, and EXEPACK behavior remain material. |
| `reloc` | Locally owned, narrowly tested `opennt-reloc`; `COMMAND.COM` conversion is independently cross-checked with OpenWatcom `exe2bin`. | This does not recreate the unavailable historical `reloc` executable. |
| `stripz` / `stripdd` | Source-built local compatibility tools, with the source format rule recorded per artifact. | Only the documented strip behavior is established. |
| `dosmsg`, `buildidx`, message tools | Selected host compatibility stages that retain the historical parsing body while replacing unavailable DOS host services. | Generated EN-US inputs must be recorded and checked; host-ABI corrections are not guest-source edits. |
| `mapsym`, `rc16`, `EXEPACK` | Not recovered for the relevant target paths. | A missing tool or unproven output format remains a blocker, not permission to substitute an unrelated layout. |

## Selected Artifact Evidence

| Artifact/family | Historical build expectation | Current recorded route | Evidence level and limit |
| --- | --- | --- | --- |
| `NTIO.SYS` | real-mode assembly/C, message generation, `LINK16`, `mapsym`, `reloc` | All 12 original modules pass pristine MASM386; WLink comparison loses LIDATA relocations | `masm386-link-compared`; LINK16-compatible linker blocked |
| `NTDOS.SYS` | MASM-family assembly, `LINK16`, `mapsym`, `reloc`, `stripz` | All 38 original modules pass pristine MASM386; WLink comparison loses `msdata.obj` LIDATA relocations | `masm386-link-compared`; LINK16-compatible linker blocked |
| `COMMAND.COM` | MASM-family assembly, `LINK16`, historical `reloc` | all 34 original objects assemble through managed-source-built MASM386; WLink is rejected when it loses `rdata.obj` LIDATA relocations | historical-LINK-compatible final link remains required; no COMMAND image candidate |
| `EXE2BIN.EXE` | MASM-family assembly and `LINK16` | managed-source-built MASM386 for its admitted source path, then WLink-compatible link | structural build record only; runtime remains pending |
| `HIMEM.SYS`, `COUNTRY.SYS`, console and utility stages | varies by makefile, generally MASM-family / `LINK16` / packaging tools | per-artifact MASM386, linker, and conversion choices are recorded in their own build cards; retired JWasm/UASM stages remain diagnostic evidence only | no family-wide assembler substitution is implied |
| `DOSX.EXE` | x86 assembly, `LINK16`, `EXEPACK` directive | unchanged-source OpenNT 4.5 MASM/LINK16 rebuild in `dosx-tools16-historical-v6` | source-built artifact; DPMI runtime remains unvalidated |
| `GRAPHICS.COM`, `NLSFUNC.EXE`, `MEM.EXE`, `LOADFIX.COM` | historical assembler/linker/packer or C16 environment | unchanged-source OpenNT tools16 stages recorded in the rebase ledger | source-built artifacts; runtime remains unvalidated |

The authoritative detailed cards are `OPENNT-BUILD-BOOTSTRAP-AUDIT.md`,
`NTDOS-ASSEMBLY-COMPATIBILITY-LEDGER.md`,
`OPENNT-COMMAND-LAYOUT-RECOVERY.md`, and the per-artifact build records.

## MASM386-First Reprocessing Rule

All attempted or built guest artifacts now require a pristine MASM386 audit
before they can be used as current evidence. Retain the original object order,
record the exact assembler and host path, and compare any legacy result only
after the MASM386 result exists. See the MASM386-first work order for the
per-object, host-ABI, linker, relocation, and resource-tool gates.

## Prohibitions

Do not infer any of the following from this matrix:

- that a rebuilt host MASM386 is the original historical tool binary;
- that WLink is equivalent to LINK16, EXEPACK, RC16, or MAPSYM;
- that a source-to-image result is retail-identical or distributable; or
- that a structural stage has executed through the historical NTIO, BIOS,
  BOP, DEM, and NTDOS chain.
