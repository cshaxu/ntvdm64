# T423 S39: native packet bounds and startup client ownership

## Question and baseline

The owner approved three audit repairs and explicitly excluded the existing
30 ms NTW32 presentation sampling. Baseline: S38 `2cf3cec60`, application/RPC
revision 28 and its published coherent eight-file package. T423 stays open.

## Source and ownership ledger

| Source and previous location | Disposition and final owner | Preserved boundary |
| --- | --- | --- |
| Project-added `ntw32-exe/launch_packet.c` | Move to `run16-exe/native_launch_packet.c`; existing frontend-client archive. | Copied native launch strings/handles; no execution or frontend policy. |
| Project-added `ntw32-exe/launch.c` | Move to `run16-exe/native_launch.c`; separate object linked by run16 and NTW32. | Same restricted handle materialization and CreateProcess body. NTKVM does not link it. |
| Project-added NTKVM bootstrap/request client and transfer | Move existing three C files to run16-exe, retaining frontend-client.lib. | Startup/request/receipt client only; no rendering, hidden Console, input pump or worker state. |
| Public bootstrap/request/transfer declarations | Move three headers to interface. | Declarations only; no transport implementation or wire layout change. |
| NTW32 request executor | Retain NTW32-owned. | Worker execution, target binding, broker completion and cleanup remain unchanged apart from validation/allocation order. |
| NTVDM/NTW32 common worker client | Retain worker-base unchanged. | Consumers do not acquire worker-base dependencies. |

This is a relocation of existing project-owned startup clients, using the
same owner-library model as the NTSRV client; no new source directory, process,
helper or generic shared component is introduced. Native launch, bootstrap,
request and transfer bodies were compared against HEAD and are identical after
the required include-path changes. The packet codec alone adds bounded scans
and validation. No original mirror or immutable guest is changed. Original
in-process Console/VDM semantics remain their original owners; this existing
project cross-process startup contract has no directly composable historical
translation unit to relocate. No original algorithm is replaced.

## Repairs

1. `NATIVE_LAUNCH_MAX_BYTES` is one shared 1 MiB product limit. It is not a
   claimed Windows Unicode-environment limit. It allows the existing three
   32767-character fields and substantially larger environments than the old
   test peer's arbitrary 64 KiB cap. Pack bounds its string/environment scan
   and returns ERROR_BUFFER_OVERFLOW before allocation. Unpack rejects an
   oversized extent or invalid destination. Production NTW32 rejects an
   oversized header with ERROR_INVALID_DATA before allocation/body read;
   request cleanup closes its channel. Valid resume requests remain unchanged.
2. `ntw32_execution_start` validates owner/command before HeapAlloc. Invalid
   arguments leave caller-owned resources untouched and allocate nothing.
3. Shared startup implementations no longer originate in NTKVM/NTW32-private
   roots; the old files are removed, public include sites and formal source/
   link manifests are updated. No parallel old implementation remains.

Wire record layouts, application/RPC revision 28, launch syntax, native
completion semantics, Console ownership and the 30 ms sampling are unchanged.

## Build and focused proof

Working root: `build/M0-T423/S39/r001`. MSVC 14.43.34808, Windows SDK
10.0.22621.0, Win32/x86 /MT /W4 /we4013 and the CCPU40 package. Generate with:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -BuildRoot O:/repos.hobby/ntvdm64/build/M0-T423/S39/r001 -NodeExecutable O:/.nvm/versions/node/v22.22.1/bin/node.exe
```

Reuse the sealed S38 object/library cache only for unchanged inputs. Compile
the changed/moved startup sources, NTW32 executor, launcher/frontend include
consumers and affected tests, then execute the archive/link commands from the
generated Ninja graph with its x86 VS environment. Rebuilt product closure:
run16, NTW32, NTKVM. The other five published artifacts match S38 exactly.
This is incremental verified reuse, not a claimed cold full-graph rebuild.

Focused entrypoints and results:

- `ntw32-execution-lifetime-test.exe s39-execution-final.txt`: 586 checks,
  zero failures, zero remaining handles. Includes exact 1 MiB pack/unpack,
  >64 KiB valid packet, overflow, malformed counts/null outputs, and header-only
  MAXDWORD/limit+1 rejection without awaiting a body. Existing cancellation,
  preflight failure, completion fault, resume and target-survival checks remain.
- `ntw32-execution-lifetime-test.exe --invalid-arguments s39-invalid-after.txt`:
  105 checks, zero failures. The same test linked against the unchanged S38
  execution object fails the heap-growth assertion: `s39-invalid-before.txt`.
  This negative control proves the test distinguishes the actual leak.
- `frontend-request-client-test.exe`: actual native exit 37 plus rejection,
  malformed/version/EOF/partial reply, final acknowledgment and resume pass.
- `frontend-scope-lifetime-test.exe`: channel reclamation, inherited-capability
  isolation, event-driven retirement and final joins pass. Its previously
  missing park mock is now an explicit forbidden branch, not a weakened
  presentation assertion. Actual park/resume is tested separately below.
- `ntw32-next-command-test.exe`: command ownership/failure/completion passes.
- `frontend-bootstrap-test.exe`: real authenticated owner/capability/RPC,
  retirement barrier and broker-loss checks pass.
- `console-channel-lifetime-test.exe --private-desktop-full s39-console-channel-lifetime.txt`:
  real Console handoff fixture passes, retaining its two park/resume checks.
- `tests/component-integration/verify-frontend-link-ownership.ps1 -BuildRoot build/M0-T423/S39/r001`:
  passes with creation/rendering/helper/archive leakage negative controls.

The first client-pipe test was denied by the execution sandbox (error 5);
the approved unsandboxed rerun passed. A fixture link initially exposed the
pre-existing missing park mock; it was corrected and rebuilt before testing.

## Runtime and publication

Lightweight ordinary-frontend actual guest probes passed: COMMAND exit,
native-zero and native-seven. Publication initially encountered running EXE
locks; recovery hashes confirmed the complete S38 baseline before retry.
Final publication replaces only changed artifacts and checks all eight hashes;
NTVDM.REG, original guest/configuration and unchanged files are not overwritten.
The tested package is now at O:/winnt.

| File | Published SHA-256 |
| --- | --- |
| run16.exe | `299A58AB889BB53EC54315791FE4B5CC08B450CDEC2231FCE91C92AB8A1F556E` |
| ntsrv.exe | `5566B5C4B5FC86D97A54E6B0F66300AFEB33B6E7973D29DDFD67BA70D3BB76E5` |
| ntkvm.exe | `EAA7A291C11E708CF33CDC91CDB9B2C98BD2F294AD48046576A2B0E33A14FA87` |
| ntvdm.exe | `60155D9B1E8DF83F17AC407B682EFF80033A3A9584CA0CE2314DAFCEE783C0C5` |
| ntw32.exe | `4002CB09C8B732DECA4CC7BA0901359411B368BF36B8CBA4C29DA14294901CB4` |
| ntmon.exe | `3D86477B95A673B602540952805790DDFE7C4541496819030ADF16DDBD4FBA53` |
| wow32.dll | `0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A` |
| VDMREDIR.dll | `1A2418FE667348EF3C6764A85C375A53B40C00D3ECAF2FBC8F74B2D1DF4D881F` |

Console17 and Window17 reports each have 17 entries and zero result mismatches;
guest text and interaction checks are enforced by Verify-CommandExitStatus.ps1,
not inferred from exit codes alone. Commands use the S36 observer, the staged
Z:/ runtime, OrdinaryFrontend and private desktop; Window17 additionally sets
MVDM_OBSERVER_WINDOW_INPUT=1. Reports are s39-console17-summary.json and
s39-window17-summary.json under the S39 build root.

The published O:/winnt package also passes the same observer's three focused
empty/native-zero/native-seven probes (s39-published-smoke-summary.json).
All eight published hashes match the tested staging package.

observe-wow-frontiers.ps1 completed all three observations with the existing
observer/window-reader and PostExitObservationMs=5000. WINMINE retains its
visible guest window and WOWExecClass frontier. SOL/WRITE logs are observations,
not evidence of usable guest windows or gameplay; their existing limits remain.
Logs are s39-wow-frontier-{winmine,sol,write}.txt and corresponding windows.txt
files. No unrelated host window is counted as a guest success.

## Limits

No physical desktop or RDP interaction is claimed. S38's delegated reservation
fixture non-pass and SOL/WRITE gameplay/frontier limits are not repaired or
reclassified by this bounded quality task. The owner explicitly retains
30 ms sampling. T423 requires separate owner acceptance after S delivery.
