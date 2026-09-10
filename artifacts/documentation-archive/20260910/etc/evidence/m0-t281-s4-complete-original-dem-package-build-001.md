# M0 T281 S4 — complete original DEM static package evidence

On 2026-08-26, formal external MSVC/Ninja graphs built
`original-dem-package.lib` for x64 and x86. Each archive contains exactly the
sixteen original DEM object members listed in the S4 member ledger.

The formal verifier passed on both architectures. It rejects executable and
run rules, BOP ingress, Bochs inputs, `nt_bop.c`, and the
`adapter-vdm-monitor` implementation body. The same-shaped VDM control
declaration remains compile-only. No DEM provider was called and no `50:xx`
route was enabled.

Compiler warnings originate in retained original source (CRT deprecation,
historical width forms, unused values and shadowing). No warning suppression,
mirror edit, or new provider behavior was introduced by this S.
