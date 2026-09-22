# M0 T420 S45 Static Registry Admission Audit

## Question

Can the product recover selected original static configuration readers through
`NTVDM.REG` overrides and admitted read-only host Registry snapshots, while
preserving original caller algorithms and never writing the Windows registry?

## Inputs

- Pinned selected original callers under `src/mvdm/`.
- S44's package-local VDD `NTVDM.REG` binding.
- Fresh current-source `build/M0-T420/S45/shadow-registry-r1/ntvdm.exe`.

## Actual formal import baseline

`dumpbin /imports` on the x86 worker reports exactly these registry imports:

| Import | Selected caller class |
| --- | --- |
| `RegOpenKeyExA`, `RegQueryInfoKeyA` | worker-local provider host-baseline snapshots for the admitted static readers |
| `RegEnumValueA` | serial-port discovery (`dpmi32/vxd.c`) |

`CpuEnv` reaches the provider's private native-shaped API, not an NTDLL
Registry import. `nt_umb.c`'s Registry ROM walk is in its original
`#ifdef MONITOR` branch. The selected CCPU40 product is `!MONITOR`, so the
compiled original body uses its fixed EGA/BIOS ROM ranges and never opens the
modern host Registry.

The prior "no registry imports" baseline is superseded by the owner-approved
layered model: the provider, and only its explicit root allow-list, may import
read-only Registry APIs. `ADVAPI32.dll` elsewhere does not license an
unreviewed Registry consumer.

## Receiver classification

| Original owner | Classification | S45 route |
| --- | --- | --- |
| `softpc.new/host/src/nt_msscs.c` | static optional VDD list | Replace the S44 VDD-special binding with the common read facade. |
| `dos/command/cmdexec.c` | static WOW compatibility value | Common ANSI open/query/close facade; original absent-value branch remains the behavior. |
| `dos/command/cmdkeyb.c` | static keyboard/ID values | Common ANSI open/query/close facade. |
| `dos/dem/demgset.c` | static boot-drive value | Common ANSI open/query/close facade. |
| `softpc.new/host/src/config.c` | static WOW profile values | Common ANSI open/query/close facade. |
| `softpc.new/obj.vdm/ntvdm.c` | static `CpuEnv` value enumeration | Common native-shaped open/enumerate/close facade; retain original value-to-CPUENV conversion. |
| `dpmi32/vxd.c` | host serial hardware discovery | Finite public-device provider: enumerate the public DOS-device namespace, then retain the original VCD enumeration loop. |
| `softpc.new/host/src/nt_umb.c` | ROM hardware discovery | Not selected: original `#ifdef MONITOR` only. The selected `!MONITOR` CCPU40 body retains its fixed original EGA/BIOS ranges and has no Registry route. |
| `wow32/*`, WOW Shell, Registry Editor and OLE | mutable per-Win16 registry semantics | Excluded from S45; owned by the WOW32 successor's shadow-tree work. |

## Minimal design selected

`ntvdm-exe` owns one worker-local layered provider and a finite read facade.
The facade is scoped per selected original translation unit by its include
binding; it does not globally redefine `Reg*` for the product.  It provides
only the historical shapes actually reached: ANSI `open`, `query`, `close`,
and native `open`/value enumeration/close for `CpuEnv`.  ROM discovery retains
the original unavailable result; it does not enumerate modern hardware or
synthesize a zero-ROM description.  Opaque local key identities are validated in a worker-local
handle list before dereference, never as Windows handles.

The provider loads `NTVDM.REG` once before original worker initialization.
For an admitted key, it snapshots all host values read-only, closes the native
HKEY, and returns only a validated private key identity to original code.
`NTVDM.REG` values take precedence; a missing file value falls back to that
worker-local host snapshot. Missing values in both sources and malformed file
values reach the original caller's existing fallback or failure branch. The
one non-static exception is VCD `SERIALCOMM`: its
same-shaped `open/query-info/enumerate/close` facade snapshots only `COM1`
through `COM9` from public `QueryDosDeviceA`, never from `NTVDM.REG` or a
Registry handle. S45 tests each receiver through its original caller with
file override, host baseline, absent and malformed inputs, then proves all
Registry imports are confined to this provider and no system Registry write
API is linked.

## Layered baseline evidence

On 2026-09-22, the current x86 worker was rebuilt after adding the root
allow-list and snapshot provider. With no `O:\winnt\NTVDM.REG`, real
`run16 MEM.EXE` exited `0` and recorded:

```text
HOST_SNAPSHOT_OK:HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Setup
OPEN_OK
QUERY_HOST_OK
CLOSE
```

This is the original `demgset.c::demGetBootDrive` route reading the current
host `Setup\\BootDir` through its private key handle. The provider closed the
native host key before the original reader received its handle. Repeating the
same run with `S45-BOOTDRIVE-NTVDM.REG` produced `QUERY_OK`, not
`QUERY_HOST_OK`, proving file precedence. Both runs completed `MEM.EXE`
successfully; the fixture was removed afterwards. No Registry modification
occurred.

The rebuilt worker's import table names only `RegOpenKeyExA`, `RegEnumValueA`
and `RegQueryInfoKeyA`. Those three imports occur only inside
`mvdm_shadow_registry.c`'s allow-listed snapshot code; it imports no
`RegCreate*`, `RegSet*`, `RegDelete*`, `NtSetValueKey` or equivalent system
Registry mutation API.

The final rerun against the deployed S45 artifact passed both the no-file
`CpuEnv` control (`S45_SHADOW_REGISTRY_REAL_WORKER_OK`) and the `BootDir`
file-precedence control (`S45_LAYERED_FILE_PRECEDENCE_OK`).  The latter's
real `MEM.EXE` trace contained `QUERY_OK` and did not contain
`QUERY_HOST_OK`; the former retains the original absent `CpuEnv` route.  This
is deliberately a read contract: mutable create/set/delete and tombstone
serialization remain a future WOW shadow-tree obligation, rather than an
unimplemented behavior being claimed by S45.

## Interpretation

This expands the S44 approach from one VDD-only exception to six selected
static original readers without moving their algorithms out of the mirror.
It replaces one VDD-special adapter rather than adding a second policy layer.
It does not claim support for mutable WOW registry APIs or fabricate host
hardware configuration.

## Earlier file-only production evidence

The following evidence was recorded before the owner approved the layered
host-baseline design.  It remains evidence that original callers consume the
private facade and file parser; its old assertion that the worker imported no
Registry API is superseded by the import audit above.  The current worker may
import only `RegOpenKeyExA`, `RegQueryInfoKeyA`, and `RegEnumValueA`, and only
inside the allow-listed snapshot provider.  It imports no system-Registry
mutation API.

The existing real S44 VDD/MEM probe passed against the earlier file-only
worker:

```text
S44_VDD_SHADOW_END_TO_END_OK
LOAD_OK
OPEN_OK
QUERY_OK
CLOSE
```

The trace proves the worker loaded package-local `NTVDM.REG` and the original
`nt_msscs.c` caller reached its open/query/close sequence.  During this test,
the provider also safely rejected foreign/stale native handles rather than
dereferencing them.  Per-receiver configured/absent/malformed acceptance is
still required before S45 can close.

`tests/observation/verify-s45-cpuenv-shadow.ps1` additionally deploys a
controlled `CpuEnv` fixture and runs real `MEM.EXE`.  Its required trace is
`LOAD_OK → NT_OPEN_OK → NT_ENUM_OK → NT_ENUM_END → NT_CLOSE`, proving that
the original `obj.vdm/ntvdm.c::CpuEnvInit` value-enumeration loop, rather than
a test-only substitute, consumed the worker-local configuration.  The test
removes its temporary `NTVDM.REG` and worker processes in `finally`.

The same test has passed all three CpuEnv conditions: configured value,
malformed non-string value (`NT_ENUM_MALFORMED`), and no `NTVDM.REG`
(`LOAD_ABSENT → NT_OPEN_ABSENT`).  In each case the original caller's
success/failure/default branch completed and `MEM.EXE` retained its expected
guest output.

The retained S44 VDD probe now accepts an explicit `-ShadowFixture`.  It has
passed both the former one-line multi-string and
`S45-VDD-MULTILINE-NTVDM.REG`, whose standard `hex(7)` continuation crosses a
physical line.  Both actual runs reached `OPEN_OK → QUERY_OK → CLOSE` and
reported `S44_VDD_SHADOW_END_TO_END_OK`; the parser therefore does not reduce
the package configuration format to the test's former one-line accident.

The immutable `COMMAND.COM` shipped in the package is byte-identical to the
selected guest mirror and contains the original `C4 C4 54 0E` keyboard-layout
BOP at file offset 10601.  The normal US-layout, fast-paste profile reaches
the original `cmdGetKbdLayout` early no-install branch before either keyboard
registry key is opened.  A direct synthetic BOP probe is not valid evidence:
it lacks the original COMMAND initialization context and faulted.  S45 retains
the common read binding, but does not count the keyboard configured-value row
as passed until a real non-US or non-fast-paste COMMAND profile drives the
original branch.

The same real-worker harness has also passed controlled `Control\\WOW`
(`size` and `CommsAutoClose`) and Setup `BootDir` fixtures.  Their section
traces identify the original receiver keys and show `OPEN_OK → QUERY_OK →
CLOSE`; in both cases `MEM.EXE` kept its expected guest result.  A malformed
`Control\\WOW` value produces `QUERY_MALFORMED` and still reaches the
original default-size path without destabilizing the worker.

## 2026-09-22 ROM-boundary correction and rebuild

An intermediate S45 provider draft accepted the absent NT4
`HARDWARE\\DESCRIPTION\\SYSTEM` key and synthesized a zero-ROM
`Configuration Data` value.  That was removed before acceptance: the source
matrix classifies ROM as current-host discovery, not package policy, and a
modern host without an equivalent NT4 tree must retain `NtOpenKey`/
`NtQueryValueKey` unavailable behavior.  The provider now returns the
original unavailable status for native value queries it does not explicitly
support; it does not admit the ROM key for a host snapshot and does not create
a fictitious ROM inventory.

After that removal, the x86 provider object, complete `softpc-bindings.lib`
member set and `ntvdm.exe` were freshly rebuilt.  `Verify-VdmTibStorage.mjs`
passed (`mvdm_vdm_tib.obj`, 4208 bytes, no overlap).  Fresh deployment then
passed both the standard multi-line VDD fixture
(`S44_VDD_SHADOW_END_TO_END_OK`) and the configured `Control\\WOW` real
worker fixture (`S45_SHADOW_REGISTRY_REAL_WORKER_OK`).  The later layered
rebuild supersedes this artifact's old no-Registry-import claim; its direct
`MEM.EXE` host-baseline and file-precedence evidence is recorded above.

## 2026-09-22 VCD public serial-discovery recovery

`vxd.c::VcdPmGetPortArray` is not a configuration reader: its original
`SERIALCOMM` registry walk supplied a bit mask of currently named COM ports to
the Win16 Virtual Communications Device.  Treating that key as `NTVDM.REG`
would let a text file invent hardware, which violates the worker boundary.

The selected production binding instead calls `QueryDosDeviceA(NULL, ...)`,
grows its temporary buffer through the documented insufficient-buffer result,
and accepts only exact public names `COM1` through `COM9`.  It exposes those
names as finite `REG_SZ` values to the **unchanged original**
`RegOpenKeyEx → RegEnumValue → RegCloseKey` loop.  The open fails when there
are no matching public names; it does not create an empty synthetic registry
key.  The existing original nine-bit clamp remains the final authority.

`tests/observation/verify-s45-vcd-serial-discovery.ps1` builds a disposable
DOSX probe whose expected value is derived from the same public namespace at
test time.  It passed direct and twice-nested `COMMAND.COM` routes on the
fresh S45 x86 worker with `mask=0x4` (COM3), and recorded
`SERIAL_DISCOVERY_OK → SERIAL_ENUM_OK → CLOSE` from the real worker.  The
probe is removed from `O:\winnt\tests` in `finally`.  This is real guest VCD
evidence, not a COM I/O success claim; actual serial open/read/write remains
owned by the unchanged original `nt_com.c` and its S30 hardware-medium ledger.

The complementary test-only
`tests/mvdm-host/s45_serial_discovery_absent_fixture.c` compiles the same
product provider directly with an empty `QueryDosDeviceA` namespace.  It
passed `S45_VCD_PUBLIC_SERIAL_ABSENT_OK`: `RegOpenKeyEx` returns
`ERROR_FILE_NOT_FOUND` and publishes no key.  This mock proves the
unavailable-medium direction without changing the product or replacing the
real DOSX success probe.

## 2026-09-22 PIF keyboard-path admission correction

The first non-fast-paste keyboard fixture exposed a separate launch defect:
direct `run16 tests\\S45KEY.PIF` returned `ERROR_BAD_EXE_FORMAT` (193) before
the worker began.  This was not a PIF parser, keyboard, or Registry failure.
The selected original `BaseCheckVDM` explicitly rejects a DOS PIF submitted
from an existing Console unless its caller supplied `CREATE_NEW_CONSOLE`.
`run16` had retained the PIF subtype correctly but passed zero creation flags.

`run16::launch_vdm` now supplies `CREATE_NEW_CONSOLE` only for the original
`BINARY_TYPE_DOS_PIF` subtype at that `BaseCheckVDM` call.  It does not alter
CheckDOS, the source-owned PIF record, or worker subtype handling.  On the
fresh x86 run16, the same PIF reached the worker's original
`ASKING_FOR_PIF | ASKING_FOR_DOS_BINARY` query (`get-state 260`) instead of
returning 193.  The direct `MEM.EXE` regression passed with exit zero and
normal conventional-memory output.

The first fixture was not a valid one-variable keyboard profile: it cleared
the whole 386 PIF extension, including the original editor's XMS/EMS and
priority defaults, and then nested `COMMAND.COM /c MEM.EXE`.  That profile
can fail before the intended reader, and the nested COMMAND route is a
separate re-entry concern.  The fixture was corrected to preserve the
original default resource profile, change only `fINT16Paste`, and let the
initial COMMAND consume a PIF whose target is `MEM.EXE` directly.

The existing detached Console observer then recorded a successful real
worker: exit `0` and normal `MEM` text.  Its trace has the original initial
PIF request followed by COMMAND's `state=5` request and the original ExitVDM
completion.  The same run recorded both keyboard-table sections through the
private facade:

```text
HOST_SNAPSHOT_OK:...Keyboard Layout\\DosKeybCodes
OPEN_OK
QUERY_OK
CLOSE
HOST_SNAPSHOT_OK:...Keyboard Layout\\DosKeybIDs
OPEN_OK
QUERY_OK
CLOSE
```

The accompanying x86 `s45_cmdkeyb_shadow_fixture` calls the selected original
`cmdkeyb.c` directly while controlling only the Console layout-name result;
it changes neither guest media nor the system Registry.  It passed the three
remaining deterministic source branches: configured file value
(`S45_CMDKEYB_CONFIGURED_OK`), malformed `hex:zz`
(`S45_CMDKEYB_MALFORMED_OK`), and an absent layout value
(`S45_CMDKEYB_ABSENT_OK`).  Its trace contains respectively `QUERY_OK`,
`QUERY_MALFORMED`, and `QUERY_ABSENT`; the original no-install return remains
the result for the latter two.  Together with the real PIF route, this closes
the `cmdkeyb.c` reader rather than treating a host-US early exit as coverage.

## Live receiver matrix

| Original reader | Configured | Absent / malformed | Current disposition |
| --- | --- | --- | --- |
| `nt_msscs.c` VDD list | real BOP VDD load passes, including multi-line `REG_MULTI_SZ` | absent VDD list preserves no-load path | complete S44 evidence, rechecked on S45 artifact |
| `ntvdm.c::CpuEnvInit` | real `MEM.EXE` reaches native enumeration | absent file and malformed value both retain original completion | complete S45 receiver evidence |
| `config.c` Control\\WOW | real `MEM.EXE` consumes configured values | malformed size takes original default | complete S45 receiver evidence |
| `demgset.c` Setup `BootDir` | real `MEM.EXE` consumes configured boot drive | keys-without-`BootDir` and malformed `hex:zz` both take the original default-drive route | complete S45 receiver evidence |
| `cmdkeyb.c` keyboard layout/ID | real non-fast-paste PIF/MEM route opens and queries both tables | selected original caller's configured/malformed/absent branches pass in the controlled layout fixture | complete S45 receiver evidence |
| `cmdexec.c` WOW Compatibility | needs an actual WOW-classified executable | absent value has no effect on ordinary DOS `MEM` | transferred runtime consumer is WOW successor; static binding remains unclaimed |
| `vxd.c` SERIALCOMM | real DOSX VCD returns the public `QueryDosDeviceA` COM1--COM9 mask (`0x4` on this host) | no named COM ports makes the original open/failure route observable; no Registry fallback | complete S45 discovery receiver; serial I/O remains separately constrained by S30's external peer evidence |
| `nt_umb.c` ROM description | not applicable | not applicable | selected original `!MONITOR` CCPU40 body has fixed EGA/BIOS ranges; its `MONITOR`-only Registry branch is not linked |

The `cmdexec.c` row is a production binding but not a T420 WOW pass; its real
WOW-classified consumer is expressly owned by the queued WOW32 successor.
Every other selected non-WOW S45 row is complete. The unselected `MONITOR`
ROM branch is not a missing CCPU40 capability.

## Current-source rebuild and focused regression

On 2026-09-22, every dirty object selected by the formal x86 Ninja graph was
rebuilt using that graph's exact MSVC command: `demgset`, `cmdexec`,
`cmdkeyb`, `vxd`, `nt_msscs`, `config`, `nt_umb`, `ntvdm` entry, the layered
provider, and the worker binding. The five dependent archives and `ntvdm.exe`
were then rebuilt from the same graph command lines. `Verify-VdmTibStorage`
passed for the resulting map.

The deployed `O:\winnt` artifact then passed the no-file `CpuEnv` worker
route, real COMMAND/PIF keyboard route plus configured/malformed/absent
original-reader fixture, public serial-discovery route (`mask=0x4` on this
host), and original VDD BOP load/dispatch/cleanup route. The deployed import
table still contains only `RegOpenKeyExA`, `RegQueryInfoKeyA`, and
`RegEnumValueA`; no Registry mutation import appears.

After the final no-op `nt_umb.c` include removal and relink, the same deployed
artifact passed all 17 transcript- and exit-gated product routes under
`O:\winnt\logs\s45-final-product-r2-summary.json`: interactive
and direct COMMAND, missing/native child cases, stdout/stderr/EOF, ordinary
and repeated MEM, two-level nested COMMAND with three MEM reports,
`COMMAND.COM /c`, direct and nested guest exit status, and EDIT return.

The final `run16.exe` was then rebuilt from its currently modified source with
the formal graph's exact x86 compile and link commands; `VDMREDIR.DLL` was
rebuilt from that same graph as well.  The complete deployed set (`run16.exe`,
`basesrv.exe`, `dtmgr.exe`, `ntvdm.exe`, `VDMREDIR.DLL`) passed the same 17
routes again under `O:\winnt\logs\s45-final-product-r3-summary.json`.  Every
record has `Actual == Expected`; the runner left no package `run16`, `basesrv`,
or `ntvdm` process behind.

## Final footprint accounting

The mirror source changes are intentionally small: six selected original
translation units receive only the local facade include (+6), while
`nt_msscs.c` is +6/-12 because its prior special sequence is restored to the
original open/query/close shape.  Together this is **+12/-12 production lines
in the MVDM mirror**; `nt_umb.c` is byte-identical to its pre-S45 source and
is not a change.  The mirror README changes one DIV record only.

The adapter replaces the old VDD-only configuration provider with the named
worker-local layered-read provider.  It adds no mirror file and no overlay;
the adapter delta is **+861/-135 lines** across the renamed header/source,
or **+726 net**, because it now owns one parser, private-handle validation,
the finite allow-list, and host snapshot boundary for all admitted readers.
This is not a duplicate of any original caller algorithm.  The remaining
worker/run16 binding change is +10/-2.  Test fixtures and observation scripts
are separate test-only additions and do not enter the package or guest media.
