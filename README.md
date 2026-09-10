# NTVDM64

## Documentation Governance

Current policy and task state are organized under [docs/](docs/README.md). Start
with the [project goal](docs/design/GOAL.md), then read the
[architecture](docs/design/ARCHITECTURE.md),
[source policy](docs/etc/operations/policy/source-policy.md), and
[current status](docs/states/CURRENT.md). Historical supporting records are
indexed from [docs/etc/README.md](docs/etc/README.md).

NTVDM64 is a self-contained, non-invasive Windows NTVDM recovery project. Its
sole product and acceptance executable is the Win32/x86 `ntvdm32.exe`, built
from the original OpenNT SoftPC CCPU40 path and staged under `build/output/`.
Native x64 compilation is retired. External projects, including historical
Bochs material, are comparison evidence only and never a runtime, link, build,
or acceptance dependency.

## Current Build Entrypoint

The supported build graph is the manifest-driven Win32/x86 MSVC `/MT` Ninja
route selected by the active packet. Build trees, compiler output and fixture
results belong below `build/<task-id>/<run-id>/`; only a verified final product
is staged as `build/output/ntvdm32.exe`. The deployment exception is an
intentional copy to `O:\ntvdm64\ntvdm32.exe`; runtime observations belong under
`O:\ntvdm64\logs\`.
## Current Boundary

The product directly composes selected original MVDM source and preserves its
historical call shapes through the smallest same-shaped adapter or registered
private overlay. It does not install software, replace Windows files, rebuild
private subsystems, or mutate host configuration.

## Source Policy

Source, firmware, guest-media and Microsoft-component admission is controlled
by the [source policy](docs/etc/operations/policy/source-policy.md). Public
Win32 APIs may bind an admitted original-owner contract; a private API cannot
become a product dependency without an owner-approved exception.

## External Comparison Material

Historical `runner-run` research records may remain as evidence, but do not
describe the current executable or acceptance boundary. The governed recovery
direction is defined by the documentation authorities above.
