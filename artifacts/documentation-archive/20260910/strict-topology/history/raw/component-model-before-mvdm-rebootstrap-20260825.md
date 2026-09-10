# Superseded component model before MVDM package rebootstrap

## Status

This record preserves the component model superseded on 2026-08-25. It is
historical evidence only. Current ownership is defined by the design and rules
authorities.

## Former model

The transition-era design split original MVDM material conceptually into
`opennt-abi`, `opennt-bop`, `opennt-host`, `opennt-softpc` and
`opennt-utils`, alongside `bochs-core`, DOS/WOW guest mirrors, four/five
adapters, `session` and `app`. Its intended direction was:

```text
app -> adapter-bochs -> bochs-core
app -> adapter-bop -> opennt-bop
opennt-bop -> opennt-host / adapter-win32 / adapter-softpc
opennt-host -> adapter-win32 / adapter-softpc / session
```

The model correctly established selector-blind BOP transport, a Bochs-only
machine boundary, same-shaped SoftPC/Win32 recovery, load-only guest inputs,
neutral session state and mirror-private overlays. It was superseded because
the conceptual BOP/host/SoftPC/utility split did not follow the actual upstream
MVDM package topology and encouraged copied headers, reverse dependencies and
partial package imports.

## Successor decision

The successor keeps all selected non-guest/non-tool MVDM packages under one
canonical `opennt-mvdm-host` mirror, moves outside-MVDM declarations to
`opennt-platform-abi`, preserves separate complete load-only DOS/WOW16 guest
mirrors, and formalizes `adapter-vdm-monitor`. Subsequent review added a
multi-instance-safe `session`, a distinct cross-process `broker`, and a shared
x86/x64 32-bit compatibility object space. The complete current model is in
`docs/design/ARCHITECTURE.md`.

The repository commit immediately before the final cleanup retains the full
transition-era authority text for exact comparison: `05872803`.
