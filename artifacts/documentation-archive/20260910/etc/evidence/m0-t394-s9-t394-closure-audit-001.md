# M0 T394 S9 — closure audit

Date: 2026-09-06

## Ownership result

The VDD cohort has one `vdd` owner; COMMAND redirection and DEM observations
have `redir` owners; BaseVDM state/native-child and COMMAND observations have
`basesrv` owners; terminal conversion is `session`; startup report capture is
`app`; CPU/keyboard/PIC/SAS observations are named `softpc` machine ownership;
and the historical error declaration ABI is `opennt-abi`.

The residual host aggregate contains only BOP dispatcher and product-container
exception/main-return/config probes, whose owner is neither a service nor a
generic monitor. No COMMAND, DEM or machine observation body remains there.

## Closure evidence

- Direct x86/x64 compilation passed for the extracted machine and remaining
  aggregate units; S6 app capture fixture passed x86/x64.
- T310 source generation lists all final machine, BaseSrv and redirector units.
- `Verify-T280S3HostStaticDataBuild.ps1` passed after all active build and
  verifier force-includes changed to the `opennt-abi` definition header.
- Active `tools/build`, `tools/governance`, `src` and `tests` have no old
  `softpc/include/error_abi.h` reference; archival legacy evidence is excluded.
- Documentation governance verification and `git diff --check` passed.

This closes M0 T394 without a behavior, guest, BOP, media or runtime claim.
