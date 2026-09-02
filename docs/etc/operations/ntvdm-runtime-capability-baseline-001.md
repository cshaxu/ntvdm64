# NTVDM runtime capability baseline

## Current product facts

- The formal CPU40/x86 executable links original SoftPC/CCPU40, selected DEM,
  COMMAND, XMS, DPMI/DPMI32, Base-VDM binding and selected adapters. Link
  success is not runtime execution proof.
- App accepts only `--command <text>`. It produces a copied Base-VDM record
  for `mvdm\\system32\\COMMAND.COM` with `/C <text>\r\n`; it has no proven
  direct target grammar for DOS `.com`, DOS `.exe`, `.bat`, Win16 NE or native
  Win32 executable targets.
- T356's sole runtime observation exited before SoftPC/guest execution with
  app-owned `APP_STARTUP_MEDIA_REJECTED`: the fresh EXE was not beside its
  executable-relative `mvdm` package.
- `cmdExec32`/`cmdCreateProcess` retain original source control flow and link
  through a child-local standard-handle facade. Its local fixture proves only
  `CreateProcessA` child-stream handling; no guest `SVC_CMDEXEC`, command
  lifecycle return, or DOS-to-native program transition is observed.
- WOW/Win16 is unavailable in the production route. `adapter-mvdm-host-out/wow`
  explicitly selects no WOW provider, broker, GUI, WOWEXEC or generic Win16
  API, and the formal product selects no `wow32` body.
- XMS, DPMI/DPMI32, Redirector and graphics have source/build or bounded local
  evidence only; none has a selected-workload runtime claim.

## Capability truth table

| Requested capability | Current disposition | Missing completion proof/owner |
| --- | --- | --- |
| Boot staged CPU40 product | Not proven | Executable-relative staging and post-admission observation. |
| Run COMMAND built-in (`/C EXIT`, `DIR`) | Not proven | COMMAND bootstrap, guest execution and controlled return. |
| Run external DOS `.COM` | Not proven | Original `$Exec`, file/JFN/SFT, PSP/arena and parent return. |
| Run DOS MZ `.EXE` | Not proven | `$Exec` lifecycle plus MZ relocation/loader matrix. |
| Run `.BAT` | Not proven | COMMAND batch state, nested execution and errorlevel return. |
| Launch native 32-bit child from COMMAND | Source/fixture only | Reached `54:08/0A/0B`, re-entry/event/return lifecycle. |
| Run DOS extender / DPMI program | Source/build only | Reached DPMI/XMS/monitor workload matrix. |
| Run Win16 NE executable | Unavailable | WOWEXEC bootstrap, callback/task/monitor and host/UI provider. |
| Redirected/local or remote DOS resources | Source/build only | VDMREDIR file/pipe/network behavior and failure matrix. |
| DOS graphics workload | Unavailable as a claim | Runnable DOS workload, then SoftPC video/presentation integration. |

## Evidence basis

`build/M0-T356/S3/formal-x86/{build.ninja,source-manifest.json}`;
`src/app/launch_declaration.{c,h}`;
`src/mvdm-host/dos/command/cmdexec.c`;
`src/adapter-mvdm-host-out/win32/{source,include}/command_process_compat.*`;
`tests/adapter-mvdm-host-out/win32/command_process_compat_fixture.c`;
`src/adapter-mvdm-host-out/wow/README.md`; and T356 S4 evidence.
