# T408 S4 original RTL status restoration

## Question

Can the reached `RtlNtStatusToDosError` behavior be restored from OpenNT
rather than retaining the adapter's three-case mapping, while preserving the
one source-observable TEB side effect?

## Inputs and procedure

- Compared `O:\repos.external\OpenNT\base\ntos\rtl\error.c` and `error.h`.
  OpenNT-4.5 does not contain this source path, so there is no competing
  edition to select.
- Imported the original translation unit and generated status table under
  `src/opennt-host/base/ntos/rtl/`.  A content comparison ignoring the
  repository's terminal newline reports no source/table difference.
- Replaced the adapter's `STATUS_SUCCESS`/`STATUS_NO_MEMORY`/
  `STATUS_BUFFER_OVERFLOW` fallback with the imported source in a private
  `OpenNtRtl*` symbol domain.  The only retained binding is the existing
  private TLS TEB carrier extended with `LastStatusValue`.
- Generated the x86 graph in
  `build/M0-T408/S4/r001-status-restoration`.  Native Ninja scheduling was
  repeatedly observed idle with no target output, so its 442 generated
  commands were executed sequentially from that same build root.  The full
  worker linked as `original-softpc-process.exe` (3,241,984 bytes).

## Observations

| Verification | Result |
| --- | --- |
| Focused D12 fixture | `rtl-x86-fixture.exe` exited 0. It proves no-memory mapping, Win32 HRESULT low-word mapping, customer status pass-through, Redirector's `C001` low-word convention, unknown mapping to `ERROR_MR_MID_NOT_FOUND`, TEB write, and `NoTeb` non-write behavior. |
| Full x86 link | The worker map resolves both `OpenNtRtlNtStatusToDosError@4` and `OpenNtRtlNtStatusToDosErrorNoTeb@4` from `original-opennt-rtl-x86:error.obj`. No adapter function definition remains. |
| MEM regression | Deployed candidate SHA-256 `37d57df9c1048ffa27c29e95c6fb1e3a10aa69e1fd26a4abdfbf0f6027a83077` ran `MEM.EXE` from `O:\ntvdm64` with exit 0; stdout/stderr are under `O:\ntvdm64\logs\m0-t408-s4-mem-*.log`. |
| COMMAND regression | The same deployed candidate ran `COMMAND.COM /c exit` with exit 0; logs are under `O:\ntvdm64\logs\m0-t408-s4-command-*.log`. |

## Interpretation and follow-up

D12 no longer has a project-defined status-selection algorithm or table.  The
new source-facing macros exist solely to avoid collision with modern NTDLL
exports; they direct all selected callers to the original source owner.
`LastStatusValue` is finite thread-local state, never a guest pointer or host
TEB cast.  S4 is complete.  S5 owns the distinct USER `MBToWCSEx` conversion
body and must not reuse this status work as permission to change Redirector
OEM conversion.
