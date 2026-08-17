# T225 S23 P5 — `opennt-host` INT 06 Provider Admission and Fixture Result

## Decision

The formal Ninja graph now contains a fifth, deliberately narrow static library:
`opennt-host`.  Its first member is
`src/opennt/local/compat/host/opennt_int06_provider_v1.c`.  This is a
source-derived x64 rehost of exactly the OpenNT/SoftPC `illegal_op_int` plus
`unexpected_int` operation transcript; it is not the historical x86 object,
a bx-vdm substitute, or a Bochs modification.

This implements recovery-rung 3 from [P4](t225-s23-int06-x64-rehost-admission-001.md).
The owner expressly admitted this module boundary on 2026-08-17, including
future analogous source-first formal module boundaries.  The original source
remains the semantic authority.

## Fixed contract

The provider exposes only versioned, copied scalar records:

- `SS`, `ESP`, and the computed physical stack address;
- one fixed byte read/write operation at a time for ordinary RAM or port I/O;
- copied read completions and a fail-closed state result.

It preserves the original order: read four saved-frame bytes; on `IP=FFFFh`
write incremented `CS` before incremented `IP`; sample the master PIC; take
the software `FFh` BDA cause branch or the original master/slave mask/EOI
branch; then write BDA `046Bh`.  Any unexpected operation acknowledgement
makes the provider fail rather than resume or invent a fallback.

It has no callback, TLS, CCPU/SAS object, host pointer, Windows API, selector,
or BOP dependency.  `opennt-host` neither executes operations nor chooses an
INT 06 route: those remain future typed bx-vdm/mantle composition work.

## Formal fixture result

Fresh root:
`build/M0-T225-S23/opennt-host-int06-r2`

```text
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T225S7FullNinjaGraph.ps1 \
  -RepositoryRoot . -BuildRoot build/M0-T225-S23/opennt-host-int06-r2
ninja -C build/M0-T225-S23/opennt-host-int06-r2 --quiet \
  bin/t225-s23-opennt-int06-provider-fixture.exe
build/M0-T225-S23/opennt-host-int06-r2/bin/t225-s23-opennt-int06-provider-fixture.exe
```

The formal MSVC x64 `/MT` graph built `lib/opennt-host.lib`; the fixture exited
zero.  It verifies both the no-ISR software path (`046Bh = FFh`) and the
`IP=FFFFh`, slave-ISR hardware path, including exact port/mask/EOI order.

## Limits and next boundary

This proves a source-derived provider contract and its standalone regression
only.  It does not prove an INT 06 BOP route, native RAM/port execution, a
Direct/Readonly observation, or whole S23 closure.  The next admitted step is
a selector-blind typed mantle request/result seam for the already native
ordinary-RAM and PIC resources; bx-vdm may then compose it with this provider
without reusing the rejected legacy callback island.
