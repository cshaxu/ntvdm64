# M0 T263 S7 — MVDM owner-expansion closure

Date: 2026-08-24

## Delivered owner layout

T263 establishes the following source-identical original-package additions:

| Owner/root | Original package(s) | Exact files |
| --- | --- | ---: |
| `opennt-guest` | `base/mvdm/bin86` | 10 |
| `opennt-softpc` | `base/mvdm/softpc.new/bios`, `roms` | 35 |
| `opennt-utils` | `base/mvdm/suballoc` | 8 |
| `opennt-host` | `bde`, `dbg`, `fax`, `ieuvddex`, `vdd`, `vdmdbg`, `vdmexts` | 138 |
| `tools/opennt` (non-component) | `base/mvdm/tools16` | 7 |

Each imported file was checked against `O:\repos.external\opennt`, Git
`5c5b979e`, by original-relative path and SHA-256 in its packet. There are no
new local divergence IDs in the six original-code component READMEs.

## Component and build-boundary checks

- The architecture/rules/code layout identify exactly 12 source components:
  six original-code mirrors (`bx-core`, `opennt-guest`, `opennt-bop`,
  `opennt-host`, `opennt-softpc`, `opennt-utils`), four mechanical adapters,
  and `app`/`session` composition.
- All three new roots are production-only: their initial content is an
  original package plus the component README exception register. No test,
  example, comparison copy or generic compatibility root was introduced.
- `opennt-softpc` remains opaque firmware/ROM/machine-contract input. It does
  not cause a Bochs/OpenNT reverse static dependency.
- `opennt-utils` contains only the original `suballoc` package; it is not a
  project common layer and has no new static library.
- `tools/opennt` is explicitly non-production and non-runtime.
- `ninja -C build/M0-T262-S5/001 -n` completed with exit code zero and showed
  445 existing graph edges. Neither `opennt-softpc`, `opennt-utils` nor
  `tools/opennt` appeared as an object, archive or link owner. Ninja emitted
  the pre-existing `premature end of file; recovering` warning for that old
  disposable build graph; this packet did not regenerate or modify it.

## Deliberate deferrals

- `dpmi`, `dpmi32`, `xms.486`, `wow32`, `vdmredir`, and BOP callers remain
  with their existing/future owner packages; this preservation task does not
  declare their runtime contracts recovered.
- `v86`, `sim16`, `sim32`, `oemuni`, and each mixed `vdmutils` product retain
  the owner dispositions from the S1 census until a reached original consumer
  justifies a separate admission.
- The preserved VDD/debugger/fax packages are not claimed runnable until their
  own host-product contracts have source/ABI/failure admission.

## Closure conclusion

The requested component expansion is complete at its approved ownership and
source-preservation boundary. Future work begins from named original packages,
not from an undifferentiated `mvdm` tree or a new generic adapter.
