# M0 T372 S1 — Original Redirector runtime-topology revalidation

## Scope and inputs

This is a source/build/topology audit only. It enables no `57:xx` selector and
does not claim a guest Redirector execution.

Inputs re-read on 2026-09-02:

- `src/mvdm-host/vdmredir/vrdisp.c:54-152`, the original 50-entry
  `VrDispatchTable` and its bounds/failure path;
- the complete current original `src/mvdm-host/vdmredir` package (15 provider
  translation units, five declarations/build carriers);
- [the prior selector ledger](../operations/m0-t290-s1-redirector-service-abi-ledger.tsv),
  which contains 50 selectors plus three non-selector broker support entries;
- [the prior package-path ledger](../operations/m0-t290-s1-redirector-path-interface-ledger.tsv),
  which covers the fifteen provider bodies, six declaration carriers and four
  historical build forms;
- `src/mvdm-host/softpc.new/host/src/nt_bop.c:404-584`, the original BOP-7
  load/dispatch path; and
- `tools/build/New-T310OriginalSoftpcNinja.ps1:800-824` plus the generated
  `build/M0-T371/S2/formal-x86` graph.

The selected mirror identity is already recorded in the MVDM union manifest:
`vrdisp.c` SHA-256 `030a3a2f2baaeb36bb58eb459cc91a0f6bf15bfdec777fc2998e3c3d69b2a389`,
`vrmslot.c` SHA-256 `70294b72780d3287137310dd4b018b7e572328e1b8bcf9bf4178c32734b95c7c`,
and `vrnmpipe.c` SHA-256 `fbb09851b3b1174e37b8cfa49f67b9667bea4262ad5e60cc9fb1d0917bf60e40`.
The prior complete-package manifest records these as dual-identical selected
OpenNT/OpenNT-4.5 inputs. No new source import or mirror divergence is
introduced by S1.

## Original control topology

`VrDispatchTable` has exactly 50 entries, selector `57:00` through `57:31`.
The original body validates the service byte, calls `VrUnsupportedFunction`
for an out-of-range value, otherwise invokes the table entry in its source
order. The existing selector ledger remains the normative per-entry map; its
53 rows comprise those 50 selectors and the three necessary DEM/COMMAND
broker support functions.

The owner groups, retained as package groups rather than leaf tasks, are:

- lifecycle/local broker (`VrInitialize`, `VrUninitialize`, process teardown
  and the two DEM/COMMAND consumers);
- synchronous named pipes and their helper exports;
- mailslots;
- asynchronous named-pipe/completion;
- public NetAPI/remote/mode services;
- NetBIOS; and
- DLC/LLC, window and interrupt completion.

All guest spans in these groups must use the existing session mapping-manager
lease/copy contract. Native handles and asynchronous completion state remain
session-owned; the Redirector mirror receives only its source-shaped
historical interface through `adapter-mvdm-host-out/redir`.

## Current formal and runtime state

The current formal CPU40/x86 graph compiles all fifteen original provider
bodies into `original-mvdm-redir.lib`; `ninja -C
build/M0-T371/S2/formal-x86 -t query original-mvdm-redir.lib` lists all fifteen
objects. `original-softpc-candidate` depends on that library and on
`redirector-bindings.lib`.

That is intentionally not product runtime composition. The same generated
graph's `original-softpc-process.exe` link line excludes both
`original-mvdm-redir.lib` and `redirector-bindings.lib`. The original
`MS_bop_7` does not expect a statically linked provider: it calls
`SafeLoadLibrary("VDMREDIR")`, then resolves these nine original exports:

- `VrDispatch`, `VrInitialized`;
- `VrReadNamedPipe`, `VrWriteNamedPipe`;
- `VrIsNamedPipeName`, `VrIsNamedPipeHandle`;
- `VrAddOpenNamedPipeInfo`, `VrConvertLocalNtPipeName`,
  `VrRemoveOpenNamedPipeInfo`.

The fixed `O:\\ntvdm64` stage has no `VDMREDIR.dll`. If BOP-7 is reached in
that state, its own one-time load state sets `CF=1` and `AX=ERROR_INVALID_FUNCTION`;
it does not grant a fallback selector route. This is the current earliest
Redirector runtime gate.

## Recovery ladder and S2 boundary

1. **Original reuse:** retain all original provider bodies, dispatcher and
   dynamic-load contract.
2. **Smallest same-shaped facade:** retain existing named redir, session,
   SoftPC and public-Win32 interfaces only where each original call cannot be
   directly composed; do not add Redirector policy to an adapter.
3. **External intrusion:** none admitted by S1.
4. **New behavior:** none admitted by S1.

S2 must therefore freeze a source-shaped `VDMREDIR` product composition and
export contract, then classify the local/named-pipe, mailslot/async and
public-network external interfaces. It must preserve the original failed-load
result above until that composition is ready. It may not statically bind the
provider into `nt_bop.c`, create a second COMMAND broker, or enable a single
selector to evade the package order.

## Verification and conclusion

The audit used `rg` against the dispatcher, `nt_bop.c`, current consumers and
Ninja generator; queried the existing formal graph; and checked the fixed
stage for `VDMREDIR.dll`. The existing T290 focused fixtures remain evidence
of isolated original-provider owner groups, not proof of shipped DLL loading
or guest execution.

S1 is closed: the prior static package closure is current, while the missing
source-defined runtime composition is precisely identified. S2 is the next
bounded task.
