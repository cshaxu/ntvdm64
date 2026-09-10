# M0 T360 S2 — app/staging original-layout binding

`app_package_layout_set_media_roots` now selects the executable directory
unchanged as the session SystemRoot and `\\softpc` as the independent firmware
root. `app_package_layout_set_firmware_root` uses the same firmware child.

`Stage-OriginalSoftpcRuntime.mjs` retains the exact same sixteen source assets
and hashes, but changes only their destinations from an invented `mvdm/` child
to the source-defined package shape: root `NTIO.SYS`, `NTDOS.SYS`,
`COMMAND.COM`, `config.nt`, `autoexec.nt`; `system32/` host/COMMAND assets;
and `softpc/` firmware resources.

No MVDM, DOS, firmware or guest byte was changed. No alias, virtual drive,
host C copy or new mapping was introduced.
