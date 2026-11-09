# Windows multitask addon

Close Windows, then run `APPLY.CMD` and enter the installed Windows directory,
or pass that path as its first argument. The command detects a prepared Win3.1
or Win95 installation, copies the two released `.386` drivers into `PATCH`,
and adds their entries under `[386Enh]` in `SYSTEM.INI`. On Win3.1 it also
adds `device=*vmd` if absent, preserving an existing matching entry. It does
remove `mouse=*vmd` in the same section: the tested production host could
start with the device loaded, while the additional mouse binding caused an
early Win3.1 startup failure. It does not patch original Windows executables.

`UNAPPLY.CMD` restores the byte-identical configuration backup only if the
installed INI, backup hash and drivers still match. It removes only the two
drivers and three records created by this installer. Existing files or driver
entries cause installation to refuse; configuration edited afterward causes
removal to refuse. `!` in SYSTEM.INI is unsupported and rejected before writes.
Both commands use CMD and the existing released `HASH.EXE` only.

Win95 owner manual acceptance remains pending. A Win3.1 diagnostic control
without `device=*vmd` reached EDIT.COM but did not open its mouse menu; adding
the entry made the corresponding mouse-menu sequence pass. An earlier startup
failure involving a different test copy remains unattributed. Production-host
and visible-window validation are still required.

`test/integration/multitask/verify-win31-installer.ps1` tests the actual
installer on an isolated Win3.1 copy, then runs the production host's four
standard/enhanced and DOSONLY combinations. When the source already has the
drivers, the fixture removes only its own copied driver files and entries to
create a pre-install state; it verifies source file hashes afterward. This
startup/exit matrix does not establish editor or fullscreen interaction.
