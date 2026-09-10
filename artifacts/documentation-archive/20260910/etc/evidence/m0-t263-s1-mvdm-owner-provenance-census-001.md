# M0 T263 S1 — OpenNT MVDM owner and provenance census

Date: 2026-08-24  
Mode: single-person dual-role review  
Source policy: `docs/etc/operations/policy/source-policy.md`

## Source identities

- OpenNT source root: `O:\repos.external\opennt`, Git `5c5b979e`.
- OpenNT 4.5 comparison root: `O:\repos.external\opennt-4.5`, Git
  `98bd914d2`.
- Census root: `base/mvdm/` in the first source root.  It contains 24
  top-level packages and 3,555 regular files at census time.

This is an owner/provenance decision record, not an import record. No source
body is admitted merely by appearing below `base/mvdm`.

## Top-level owner census

| Original package | Files | Target owner / disposition | Reason |
| --- | ---: | --- | --- |
| `bde` | 13 | `opennt-host` optional debugger package | Historical debug extension product. |
| `bin86` | 10 | `opennt-guest` candidate | Guest-facing configuration/image companions; S2 must classify each input. |
| `dbg` | 3 | `opennt-host` optional debugger package | Host debug product code. |
| `dos` | 656 | `opennt-guest` / existing `opennt-bop` split | DOS/V86 image source stays guest; host BOP bodies retain their established owner. |
| `dpmi` | 58 | `opennt-bop` with guest/adapter prerequisites | DPMI owner package, not an automatic guest import. |
| `dpmi32` | 25 | `opennt-bop` with WOW/DPMI prerequisites | 32-bit DPMI host composition. |
| `fax` | 21 | `opennt-host` optional provider package | Historical host extension, not debugger core. |
| `ieuvddex` | 23 | `opennt-host` optional VDD extension package | Host-side VDD extension. |
| `inc` | 94 | split by declared consumer | Interface headers follow their original consumer package; no header-only catch-all. |
| `oemuni` | 6 | deferred owner audit | OEM-specific source; no current reached caller. |
| `sim16` | 8 | `adapter-softpc` evidence / deferred | Historical simulator contract, not a second runtime. |
| `sim32` | 5 | `adapter-softpc` evidence / deferred | Historical simulator contract, not a second runtime. |
| `softpc.new` | 998 | split: `opennt-softpc`, `opennt-host`, `opennt-bop`, `adapter-softpc` evidence | BIOS/ROM inputs differ from historical host/BOP callers and old machine APIs. |
| `suballoc` | 8 | `opennt-utils` candidate | Reusable allocation utility; requires original-caller and build audit. |
| `tools16` | 7 | `tools/opennt` candidate | Historical build-tool closure, never a product component. |
| `v86` | 35 | `opennt-guest` / `adapter-softpc` evidence split | Guest monitor content and historical machine-interface evidence must not be conflated. |
| `vdd` | 47 | `opennt-host` optional VDD provider package | Host VDD package; no automatic link admission. |
| `vdmdbg` | 6 | `opennt-host` optional debugger package | Host debugger support. |
| `vdmexts` | 25 | `opennt-host` optional debugger package | Host debugger extensions. |
| `vdmredir` | 25 | `opennt-host` / existing BOP contract | Reached redirector provider follows current original-host ownership. |
| `vdmutils` | 28 | per-product split: `opennt-guest` or `opennt-utils` candidate | `forcedos`/`graftabl` and `pifedit`/`win` cannot share one automatic owner. |
| `wow16` | 1201 | `opennt-guest` | Original WOW16 guest sources and image inputs. |
| `wow32` | 201 | `opennt-host` optional WOW broker package | Host-side WOW composition, deferred until admitted. |
| `xms.486` | 15 | `opennt-bop` / `adapter-softpc` prerequisites | Original XMS owner body and its machine seam remain distinct. |

## Initial component decisions

- `opennt-guest` expands only through source-identical, guest-owned MVDM
  packages. `bin86` is the first candidate; its configuration and binary
  assets must retain their source-relative paths and hashes.
- `opennt-softpc` is a new original-code mirror for firmware, ROM and
  machine-contract assets. Its initial candidate boundary is
  `softpc.new/bios` and `softpc.new/roms`. It neither displaces Bochs nor
  grants `bx-mantle` knowledge of OpenNT.
- `opennt-utils` is a new original-code mirror, initially empty. `suballoc`
  and portions of `vdmutils` are candidates, not pre-approved imports.
- `tools/opennt` is a non-component source/tool closure, initially empty.
  `tools16` is its first candidate and must never enter a modern runtime link.
- `vdd`, `ieuvddex`, `fax`, `bde`, `dbg`, `vdmdbg`, `vdmexts`, and `wow32`
  remain original `opennt-host` package candidates. Their preservation does
  not assert runtime capability or cause a default link.

## S1 exit result

The project now has a source-supported 12-component ownership model and empty
production-only roots for the three newly named locations. Subsequent S
packets must add a per-file source identity and local exception registration
before importing any body or binary input.
