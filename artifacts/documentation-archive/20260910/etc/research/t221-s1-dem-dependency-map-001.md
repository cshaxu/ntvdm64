# T221 S1 P3: DEM module dependency and composition boundary map

## Question

For the twelve original DEM modules established in P2, which dependencies are
ordinary host capabilities, and which are historical CCPU/SAS, SoftPC, VDD,
engine, or raw-device composition dependencies that prevent an unmodified
module from becoming a direct modern x64 runtime object?

## Procedure

Run:

```powershell
powershell.exe -ExecutionPolicy Bypass -File `
  tools/governance/Export-T221S1DemDependencyMap.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -OutputPath O:\repos.hobby\ntdos64\docs\etc\research\t221-s1-dem-dependency-map-001.json
```

The exporter retains all 73 services from P2, then scans each original C
module for five explicit dependency classes: historical guest ABI/register
access, SoftPC/VDD host composition, private engine lifecycle, ordinary
filesystem APIs, and raw-device APIs.  The lower-case SoftPC register macros
are matched case-sensitively so normal Win32 `Get*` APIs are not accidentally
classified as legacy guest ABI.

## Observation

The complete machine-readable result is the
[dependency map](t221-s1-dem-dependency-map-001.json). Every module contains
the legacy SoftPC/guest-address family (`get*` register macros and/or
`GetVDMAddr`/`Sim32*`); several also contain ordinary Win32 filesystem calls.

| Original module family | Services | Ordinary-capability observation | Non-composable historical dependency | T221 recovery direction |
| --- | ---: | --- | --- | --- |
| `demdir.c`, `demgset.c` | 17 | directory/CWD, drive and volume queries | register macros and guest pointers | preserve source layout/order through checked gathers and shared profile/CWD seam |
| `demfile.c`, `demhndl.c`, `demfcb.c`, `demsrch.c` | 28 | file, handle, find and metadata APIs are ordinary user-mode capabilities | CCPU guest pointers; `Sim32*`; `demfile` JFT/SFT VDD helpers; `demsrch` VDD/process hooks | source-derived owner package, with fixed-width guest copies and opaque token/session boundary; no raw handle or JFT/SFT reconstruction |
| `demerror.c`, `demlock.c` | 3 | no independent modern host capability proven by this scan | guest error/lock packets and legacy handle/register form | retain source-shaped ABI/failure analysis before choosing a seam |
| `demmisc.c`, `demdisp.c` | 22 | selected DOS-image file read is an ordinary file operation | `TerminateVDM`, VDD hooks, host reset calls, guest state and original dispatcher | split by source-owned subfamily; retain original no-op/failure leaves and defer engine/VDD semantics rather than treating one module as one provider |
| `demdasd.c`, `demioctl.c` | 3 | raw volume APIs may be user-mode callable only with device/permission policy | SoftPC direct disk I/O, guest packets and VDD/device assumptions | deferred raw-device profile; ordinary namespace recovery must not emulate it |

## Source-recovery decision

No complete original DEM translation unit is admitted as a direct modern
runtime object today. This is not a judgement against the source: every
module's historical entry ABI assumes CCPU/SoftPC register state and direct
guest addressing, while selected modules additionally assume VDD, private
engine lifecycle or raw-device composition. Those are named unavailable
historical dependencies for the modern x64 `/MT` graph.

The first usable recovery rung is therefore a **small checked bx-vdm seam**
only where it can carry the original module's fixed-width input/output,
ordering and error contract. If the original body cannot cross that seam
without raw guest pointers, legacy handle ownership, or unavailable VDD/engine
imports, the next permitted rung is a **source-derived rehost** of the
smallest demonstrated contract. Raw media/device semantics remain deferred
until a separately admitted device profile. A convenient ordinary Win32 API
does not authorize bypassing this order.

## Follow-up

P4 will turn this module map into the required source/ABI/failure ledger:
for each source group, record the concrete input/output layout, continuation
and failure order; classify direct/readonly/overlay/virtual mutation authority;
and decide retain/migrate/replace/delete for the current bx-vdm fallback.
No BOP behavior, composition, or native trace is changed by this evidence.
