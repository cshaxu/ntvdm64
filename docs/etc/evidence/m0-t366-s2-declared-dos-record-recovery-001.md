# M0 T366 S2 — Declared DOS record recovery

## Change

`app/launch_declaration.c::app_launch_declaration_publish` now sets the
existing `base_vdm_command.command_owner` to `BASE_VDM_COMMAND_DOS` before it
uses the existing BaseVDM broker. This restores the original BaseSrv
DOS-versus-WOW record selection at the sole app-owned initial declaration
boundary. No original MVDM/OpenNT body, adapter ABI, guest byte or mapping
route changed.

## Focused verification

```text
node tools/build/New-T366DeclaredDosRecordNinja.mjs \
  O:\repos.hobby\ntvdm64 \
  O:\repos.hobby\ntvdm64\build\M0-T366\S2\declared-dos-record-x86
VsDevCmd -arch=x86 -host_arch=x64
ninja -C build\M0-T366\S2\declared-dos-record-x86 -j 6 test
```

Observed result: the twelve-action graph compiled the current app,
BaseVDM client/local/broker, session and test, linked
`declared_dos_record_fixture.exe`, and printed:

```text
PASS: local Base VDM broker contract
```

The fixture supplies `--command T365TEST.COM`, requires the delivered record
owner to be `BASE_VDM_COMMAND_DOS`, checks the source-shaped
`/C T365TEST.COM\r\n\0` payload, and retains existing malformed/empty/newline/
duplicate declaration rejection.

## Formal product verification

```text
VsDevCmd -arch=x86 -host_arch=x64
ninja -C build\M0-T365\S2\x86 -j 8 original-softpc-process.exe
```

Observed result: only `app/launch_declaration.c` and
`app-machine-shell.lib` rebuilt; `original-softpc-process.exe` linked
successfully with `basesrv-bindings.lib`, `session.lib`, `broker.lib`,
`original-mvdm-command.lib` and the selected CPU40 libraries.

## Disposition

This is source-shaped command-record delivery only. It proves neither guest
`$Exec`, file/JFN/SFT access, a `.COM` child, MZ relocation nor parent return.
Those remain S4/runtime or later owner work.
