# T95 S6 Native Configuration Reconciliation 001

## Decision

The CPU3 `config.h` projection route is rejected as the implementation route
for the retained native container.  It remains valid negative evidence: the
first target observation proved that a partial feature-header overlay cannot
stand in for Bochs's original configure output.

The selected route is now an **intact original MSVC configuration/build graph
plus a minimal runtime machine profile**.  This is the smallest faithful
boundary: Bochs keeps its own configure-derived mechanics and product-shell
ownership; the profile controls what is enabled or assembled at runtime.

## Evidence

The 105-substitution audit shows that original configuration controls both
`config.h` and every generated archive/object/link list.  The CPU3 projection
changed only the former.  Its first build attempted a denied `gameport.o`
because the retained makefile was generated from a configuration where
`GAME_OBJS=gameport.o`; compilation then stopped on declarations correctly
removed by `BX_SUPPORT_GAMEPORT=0`.

Recreating a matching 105-variable configure output by overlays would be a new
Bochs configuration implementation.  Running the old autoconf script with a
different host compiler is also not a neutral shortcut: it probes host ABI
facts before its late MSVC-target rewrite, so an x64 host probe can create an
x86-incompatible `SIZEOF_INT_P` result.  Neither path narrows the real
Bochs/adapter boundary.

## Meaning of “minimal”

This decision does not accept all hardware as an NTDOS64 capability.  It
separates two levels:

| Level | Owner and policy |
| --- | --- |
| Compiled native support | Retained original Bochs 2.6 MSVC configuration and source/build graph. It is not a guest capability declaration. |
| Enabled machine profile | Bochs configuration only: `nogui`, one CPU/runtime selection, declared RAM and ROM inputs, and existing `plugin_ctrl` denial. This is the minimal machine claim. |
| Guest/host meaning | Adapter and OpenNT only. No Bochs compile option or profile line adds BOP, DEM, DOS/WOW, host filesystem or CLI semantics. |

The existing profile's `plugin_ctrl` gate is particularly important: its
implementation deliberately works with `BX_PLUGINS=0` by controlling Bochs's
built-in optional plugin registry.  No external plugin loader is needed or
admitted merely to reduce static objects.

## Consequences

1. The failed CPU3-projected build root is retained and never repaired.
2. Future fresh roots copy the imported `config.h`, `config.status` and all
   generated makefiles together, with hash verification; no projection is
   applied.
3. A separately named native-container target may still be a mechanical
   transform of the original executable target, so it never creates or runs
   `bochs.exe`.
4. A successful target build would prove only Bochs mechanics closure.  It
   still requires a later reset/controlled-stop fixture and an independent
   adapter ABI admission before any CLI or guest claim.
