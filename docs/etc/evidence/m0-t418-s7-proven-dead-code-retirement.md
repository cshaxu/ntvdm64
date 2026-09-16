# M0 T418 S7 — proven non-mirror dead-code retirement

## Question

Can the source-proven non-mirror dead-code subset be removed without changing
the three-program protocol, original DOS/WOW record policy, or worker runtime
behaviour?

## Inputs and source-first disposition

- S6 baseline: 74 selected non-mirror implementation inputs and 12,427
  physical lines in the formal x86 product closure.
- Fresh formal root: `build/M0-T418/S7/formal-x86-001`.
- Runtime package: `O:\winnt`; raw runtime observations remain under
  `O:\winnt\logs`.

The removed classifier was a second, private implementation of the launcher
image classification operation.  A whole-tree source sweep found no caller,
and its object had no final `ntvdm.exe` map contribution.  Retaining it could
not recover original OpenNT behaviour because the live classifier remains with
its product entry owner; deletion is therefore the first applicable recovery
rung.

The removed `HostIdleNoActivity` body was an unselected duplicate.  The fresh
maps resolve the live symbol only to original
`src/mvdm/softpc.new/host/src/nt_unix.c` (`original-softpc-host-roots:nt_unix.obj`).
The unavailable Redirector-thread error stub had no source caller or selected
product input.  Neither removal substitutes a modern implementation for an
original one.

## Retired material

| Material | Disposition | Product effect |
| --- | --- | --- |
| `ntvdm/command/{include,mvdm_image_classification.*}` | Deleted duplicate classifier and removed its stale formal-generator input. | One selected C input removed; 264 physical C lines retired. |
| `ntvdm/monitor/{include/host_idle.h,source/host_idle.c}` | Deleted unselected duplicate. | Original `nt_unix.c` remains the sole live provider. |
| `ntvdm/win32/mvdm_redirector_thread.c` and `opennt-abi/host-compat/include/mvdm_redirector_thread.h` | Deleted unreferenced unavailable error stub. | No product source/map provider removed. |

The three deleted C bodies total 292 physical lines.  The selected formal
non-mirror closure falls from 74 / 12,427 to **73 / 12,163**: the 264-line
classifier was the only selected body.  The remaining 28 C lines and the
paired declarations were tree hygiene, not a claimed executable-size change.

`mvdm_a20.c` and the legacy execution/presentation helpers remain available to
their historical fixture only; they were not made product inputs.  The
1,676-line mixed termination/observation source is deliberately deferred: it
contains a small lifecycle binding interleaved with mirror observers and does
not meet this S's zero-caller/zero-map deletion rule.

## Build and verification

1. Regenerated the x86 graph with Git Bash Node 22.22.1 and MSVC x86 `/MT`.
   The complete 443-step default graph succeeded.  Explicit product/fixture
   linking succeeded for `run16.exe`, `basesrv.exe`, `ntvdm.exe`,
   `VDMREDIR.dll`, BaseSrv reservation/original-service fixtures, descriptor,
   RTL, environment, CCPU reset, original external-memory and C-VID fixtures.
   The only diagnostics were existing original-source warnings and the known
   VDMREDIR DESCRIPTION linker warning.
2. All eight explicit fixtures passed.  The original-memory fixture covers
   disk alignment, EMS cross-window copying, SAS access, allocation resize,
   DIB sharing and backing recovery.
3. Source sweeps found no code reference to any deleted name.  Fresh final
   maps contain `HostIdleNoActivity` only from original `nt_unix.obj`; no final
   map or selected input contains the retired classifier or Redirector-thread
   stub.
4. Deployed the formal `run16.exe`, `basesrv.exe`, `ntvdm.exe` and
   `VDMREDIR.dll` to `O:\winnt`; each deployed SHA-256 matched its formal
   source artifact.  The deployed package passed automatic direct `MEM.EXE`,
   repeat direct `MEM.EXE`, and `COMMAND.COM /c ver` observations.  The product
   version-negative verifier passed and left only its expected result markers
   under `O:\winnt\logs\m0-t418-s7-version-negative`.

The legacy hidden Console observer did not reach its visible-prompt gate for
scripted interactive input in this environment, so its `COMMAND -> MEM ->
EDIT -> MEM` injection result is recorded as unavailable rather than passing.
It is not evidence of a deleted-path regression: all S7 deletions were proven
unreachable before the build, and the non-interactive deployed regressions
above passed.  Owner Terminal/RDP acceptance remains the final interactive
check.

## Conclusion

S7 closes the admitted dead-code subset without a protocol, mirror, lifecycle
or Console semantic change.  It removes one selected product C input and 292
physical C lines from the non-mirror tree, preserves the original live idle
provider, and publishes the tested x86 package to `O:\winnt`.
