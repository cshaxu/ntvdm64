# T199 S39: package-complete native integration baseline

## Inputs and method

One and only one guest execution was run through
`tools/probe/Invoke-T198S97PostBootstrapNativeProbe.ps1` into
`artifacts/build/t199-s39-global-native-r1/`.  It rebuilt the current
manifest under MSVC x64 `/MT`, using the locked source-built COMMAND/SHARE
fixture inputs and the CPU5/Pentium-MMX native core recipe.

The fixture now binds both package sessions before entry: the boot namespace
session (DEM/COMMAND/redirector/debugger/top-level/machine) and native
XMS/DPMI session.  Its external observation bridge mirrors production order:
boot namespace first, native XMS/DPMI only after a decline.  The fixture was
also recompiled without guest execution in
`artifacts/build/t199-s39-fixture-compile-r1/`.

## Observed result

The one guest run completed its controlled fixture stop (process exit `2`)
after existing DEM traffic and reported:

```text
observed-5011=1 observed-503b-resume=1 observed-dta-resume=1
observed-hard-error-resume=1 observed-drive-resume=1
observed-stop=1 next=54:01 ... error-lock=1 gset=1
```

It did not execute a generic non-BOP #UD (`observed-first-generic-ud=0`): the
test's canonical BOP observer records the boundary and deliberately stops at
the first unhandled BOP.  This is expected for this bounded integration
fixture, not evidence that the CPU exception-to-bridge mechanics are absent.

## Frontier classification

The frontier is `54:01`, owned by the **COMMAND bootstrap/launch/lifecycle
package**, not by a new single-service task.  Source review confirms that
`bx_ntvdm_command_package_session_v1_dispatch` routes service `01` through
the package's `get_next` lifecycle, which requires the copied `54:05`
registration (`launch.valid`) before it can prepare CMDINFO.  Original source
authority is `src/opennt/base/mvdm/dos/v86/cmd/cmdmisc.c` (`cmdGetNextCmd` and
`cmdSetInfo`); the package matrix is
`t199-s5-command-source-abi-failure-matrix-001.md`.

The trace does **not** authorize a `54:01` patch.  It demonstrates that the
COMMAND package's entry lifecycle must be reassessed as one unit: bootstrap,
`54:05` registration, `54:01` CMDINFO/return lifecycle, execution
environment and the source-proven failure path when registration is absent.
The next work must be an owner-package S with that scope.

## Limits

No XMS/DPMI selector was reached in this run; their bound-session statement is
from S38's source-built composition test, not inferred from this trace.  No
Bochs selector knowledge, DOS semantics in bx-core/bx-mantle, host filesystem
access or second guest run was introduced.
