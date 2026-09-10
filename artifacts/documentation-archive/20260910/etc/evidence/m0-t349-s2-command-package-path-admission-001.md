# M0 T349 S2 — COMMAND package-path admission

Date: 2026-09-01

## Change

`app_package_layout_validate_command_configuration_root` is an app-owned
preflight executed after executable-relative media-root derivation and before
original host startup.  It requires:

- an existing `mvdm` root whose public `GetShortPathNameA` result is 1–63
  bytes, exactly matching the unchanged `cmdconf.c` `achSysRoot[64]` form;
- capacity for the original default `config.nt`, `autoexec.nt` and `ntio.sys`
  path callers in `MAX_PATH+12` buffers.

It deliberately does not test those files' existence.  PIF can override the
two configuration names, and original MVDM retains all later missing-file
paths.  The preflight returns the existing app media-rejected result before
host startup; it does not fabricate an 8.3 name, create a directory, copy
media, replace `%SystemRoot%`, or alter an imported source file.

## Local evidence

A disposable Ninja Win32/x86 target compiled `session`, app package layout and
the existing package-layout fixture with `/MT /W4`.

- Its invented nonexistent `C:\\installed\\mvdm` root was rejected while
  retaining ordinary root derivation.
- The S2 fixture then received the existing staged executable
  `build/M0-T348/S2/runtime-x86-r1/original-softpc-process.exe`; derivation of
  its adjacent `mvdm` directory passed the new admission check.

The next S performs the selected full CPU40/x86 formal link and governance
closure only.  Neither result is a guest-execution claim.
