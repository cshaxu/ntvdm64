# M0 T418 S2 run16 and product ownership

## Disposition

This is a no-behaviour-change ownership move:

| Former path | Final owner | Reason |
| --- | --- | --- |
| `app/run16_entry.c` | `run16/main.c` | public launcher process entry |
| `app/console_probe.c` | `run16/console_probe.c` | launcher-only Console probe child |
| `app/version.h` | `product-abi/version.h` | fixed metadata read by all three executables |
| `app/package_layout.{c,h}` | `product-package/package_layout.{c,h}` | stateless installed-media layout calculation |

No generic shared Win32 component was added. The package module remains a
worker link input; it has no process state. The service IDL and protocol were
not moved, because they remain BaseSrv-owned for S3. `APP_VERSION` is corrected
to the admitted task identity `0.0.418`; protocol remains 3.

## Verification

The fresh `build/M0-T418/S2/run16-owner-x86-001` graph selects only
`src/run16/main.c`, `src/run16/console_probe.c` and
`src/product-package/package_layout.c`; a source/build sweep finds no selected
old app placement. The sequential MSVC Win32/x86 build completed all 506
commands. Its only final unresolved names belong to the intentional
non-runnable `/FORCE` original-closure audit DLL.

The dedicated x86 package-layout fixture passed. The same fresh graph's CCPU
HALT/RESET, C-VID vector and original external-memory/EMS/DIB fixtures all
passed. Strict mirror roots were not changed.

## Handoff

S3 may now move the service entry, protocol and authenticated transport into
`src/basesrv` while keeping `srvvdm.c` in `opennt-host` as the original
DOS/WOW record-policy owner.
