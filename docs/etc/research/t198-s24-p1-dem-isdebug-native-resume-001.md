# T198 S24 P1 DEM IsDebug Native Resume 001

## Question

Can the next original NTIO DEM boundary, `C4 C4 50 3B`, be selected through
the common adapter ingress/registry/DEM component path and resumed under the
MSVC x64 `/MT` CPU5 process?

## Source Contract

`dossvc.h` defines `SVC_ISDEBUG` as `3Bh`; the original `demdisp.c` table
selects `demIsDebug`; `demmisc.c` calls `IsDebuggee` and returns `AL=0` when
the host has no debug port.  The contained CLI has no host-debugger relation,
so its source-derived result is that original false branch, retaining AH and
resuming after all four bytes.

## Implementation And Focused Verification

`bx_ntvdm_dem_misc_plane_v1` accepts an already-classified common ingress
and provider selection, proves the DEM-MISC component, and then dispatches
only its admitted `3Bh` endpoint to the pre-existing source-derived provider.
The outer composition does not inspect service bytes.  The focused
`Invoke-T198S16BootNamespaceVdmX64Probe.ps1` r14 closure is PE32+ MSVC x64
`/MT` and verifies `EAX=ABCDh -> AB00h`, `RIP+4`; the distinct BIOS `15h`
negative remains covered.

## Native Observation

The r14 adapter objects, newly compiled observation fixture, and unchanged
S23 CPU5/mantle objects link and run as one process.  Its output is:

```
t198-s23 status=0 observed-5011=1 observed-503b-resume=1 observed-stop=0 next=00:00
```

The default-off observer saw both DEM `50:11` and the accepted `50:3B`
`RIP+4` resume.  No later `#UD` BOP arrived in the fixed budget; execution
ended at the existing `HLT instruction with IF=0` bounded-run state.

## Limit

This proves a real guest continuation through the global DEM-MISC plane, not
DOS boot or a later-boundary identity.  The HLT state's original guest
control-flow predecessor must be audited before changing budget, CPU state,
firmware, or any provider.
