# M0 T263 S2 — `bin86` guest-input admission

Date: 2026-08-24

## Decision

The ten-file `base/mvdm/bin86/` package is admitted below
`src/opennt-guest/bin86/` as source-identical guest image/configuration and
guest-tool companion input.  It does not become a host provider, a default
boot contract or an automatically loaded driver.

## Provenance and verification

- Source: `O:\repos.external\opennt\base\mvdm\bin86`, Git `5c5b979e`.
- Target: `src/opennt-guest/bin86`.
- Count: 10 source files and 10 target files.
- Verification: SHA-256 equality was checked for every same-named file after
  copy; no source file was changed and no local divergence is registered.

| File | Role |
| --- | --- |
| `autoexec.nt`, `config.nt`, `system.ini` | Original guest/VDM configuration inputs. |
| `lanman.drv` | Original guest-side driver input. |
| `ctl3dv2.map`, `ctl3dv2.sym` | Original debugging/build companions. |
| `pifedit.hlp`, `regedit.hlp`, `write.hlp` | Original guest/application help companions. |
| `makefile` | Original package build descriptor. |

## Result

The guest mirror now retains the whole original `bin86` package at the same
internal relative path.  Runtime admission of an individual input remains a
separate app/guest-image decision.
