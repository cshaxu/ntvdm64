# M0 T199 S36 COMMAND Whole-Package Provider Implementation

## Question

Can every callable OpenNT COMMAND BOP (`54:00` through `54:10`) have a
single, source-governed adapter disposition without an unimplemented generic
COMMAND pass-through route, an NT4 host dependency, or Bochs knowledge?

## Inputs

- Original table and ordering: `src/opennt/base/mvdm/dos/command/cmddisp.c`;
  it maps exactly seventeen callable services and has sentinel `54:11`.
- Original providers: `cmdexit.c`, `cmdmisc.c`, `cmdredir.c`, `cmdexec.c`,
  `cmdconf.c`, `cmdkeyb.c`, and `cmdenv.c` below the same source directory.
- Existing copied COMMAND package session and bounded providers for launch,
  COMSPEC/environment, current directory, registration, boot files and the
  no-keyboard-layout response.
- `tools/Invoke-T198S16BootNamespaceVdmX64Probe.ps1`, run with an otherwise
  new build root `artifacts/build/t199-s36-command-package-r4`.

## Implementation

`src/bx-vdm/bx_ntvdm_cmd_cli_profile_service_v1` is a COMMAND-owned,
selector-local provider for the remaining profile decisions.  It accepts only
a copied real-mode `C4 C4 54 xx` event and returns a typed result; it imports
no Bochs API, host pointer, historical CCPU/SAS state, VDD, console, process,
or ambient host capability.

| Service | Original reference | Contained CLI disposition |
| --- | --- | --- |
| `54:00` | `cmdExitVDM` calls `TerminateVDM` | Typed controlled stop; an engine-owned external-status transport remains a separate package. |
| `54:01,02,04,05,0C,0D,0E,0F` | `cmdmisc`, `cmdconf`, `cmdkeyb`, `cmdenv` | Existing bounded copied provider/session paths. |
| `54:03` | inactive `cmdSaveWorld` body | Original no-op and four-byte resume. |
| `54:06,08,0A` | Win32 handle/process paths | Existing explicit unavailable controlled stop; no host handle or process is exposed. |
| `54:07` | `cmdCheckBinary` `DontCheckDosBinaryType` branch | Declared DOS-only profile: clear CF and resume, without pathname gathering. |
| `54:09` | `cmdInitConsole` changes only VDD/event-thread state | Contained no-op and four-byte resume. |
| `54:0B` | `cmdReturnExitCode` no-next-command branch | Clear CF; preserve AH and publish the profile's fixed successful child result in AL (`0`); resume. |
| `54:10` | `cmdGetStartInfo` | Preserve AH and return AL=`0`, because this profile creates no separate console. |

The COMMAND facade now classifies every non-noop/non-unavailable callable
identity as an existing bounded provider.  Its `DEFERRED` enum remains only
as an impossible defensive fallback: all valid `54:00..10` values are
explicitly enumerated.  The command session calls this provider before its
existing copied-provider switch.

The boot-namespace composition fixture is intentionally a bx-vdm-only
closure.  The current common bridge has a secondary native XMS/DPMI root;
the fixture supplies an unbound-root decline stub so the primary composition
can prove bridge order without importing mantle capabilities.  The real
native root is independently covered by the S35 P1 native composition
evidence.

## Procedure and observations

The r1 source closure revealed a missing native-root symbol after the generic
bridge acquired its deliberate fallback.  Linking the real native root into
this bx-vdm-only fixture exposed its rightful mantle capability dependencies.
The final r4 fixture instead links the explicit decline stub, preserving the
fixture boundary.

The r4 MSVC x64 `/MT` source closure compiled, linked and executed with exit
code zero.  It sweeps all seventeen COMMAND identities through common ingress
and the one bound COMMAND session, then checks:

- `54:00` and the historical unavailable group stop;
- `54:03`, `54:07`, `54:09`, `54:0B`, and `54:10` resume at `RIP+4`;
- `54:07` clears CF;
- `54:09` changes neither GPR nor flags;
- `54:0B` preserves AH, sets AL to zero and clears CF;
- `54:10` preserves AH and sets AL to zero;
- the already admitted copied providers retain their positive and negative
  contracts elsewhere in the same fixture.

The retained build manifest is
`artifacts/build/t199-s36-command-package-r4/t198-s16-bx-vdm-boot-namespace.json`.

## Interpretation

The COMMAND package no longer uses a generic deferred outcome for any
callable identity.  This does not claim that all historical NT4 capabilities
are present: handle/process services remain explicitly unavailable, and the
contained profile intentionally selects DOS-only binary checking and no
separate console.  Those are source-derived package policies with observable
guest outcomes, not hidden host substitutions.

This is source-built composition evidence, not a native guest trace.  The
current production CLI launcher still creates a separate retained engine and
does not own a live minimal-machine plus boot-namespace composition lifecycle.
Consequently, an old-product trace cannot validate this new COMMAND provider
and must not be represented as such.

## Native integration verification

`tests/bx-mantle/t199_s36_command_native_bop_fixture.cc` binds the current
boot-namespace composition, then has the native CPU execute the four bytes
`C4 C4 54 00` at real-mode `0000:0000`.  The fixture uses the finite mantle
run mechanic and requires its result `COMPLETED_UD_STOP`.

The first native attempt proved that the ordinary CPU5 r5 closure has the
registered #UD bridge gate compiled default-off: it completed the 64-tick
budget rather than claiming a provider defect.  The retained r4 integration
closure recompiles only the already-registered `src/bx-core/cpu/exception.cc`
with `BX_NTVDM_ENABLE_MANTLE_UD_BRIDGE=1`; no Bochs source was changed and no
selector/service name enters core or mantle.  Its `link.rsp`, compiler log and
link log are retained in `artifacts/build/t199-s36-command-native-bop-r4/`.
It exits zero after CPU reset/APIC/CPUID initialization and the accepted typed
controlled stop.

This is a bounded real CPU #UD integration witness, not a guest NTDOS startup
trace and not an authorization to choose another BOP endpoint from execution
output.

## Follow-up

Continue T199 with the complete DEM component package, never by whatever
single service a trace happens to reach.  A production CLI lifecycle root and
actual NTDOS startup remain separate composition work; this fixture does not
claim either.
