# Windows 1.01 Setup additions

Open `APPLY.CMD`, enter the original Windows 1.01 media directory once, then
run that media directory's `PATCH\SETUP.CMD` from CMD. `APPLY.CMD` renames
the media-root `MOUSE.DRV` to `MOUSE.DRV.BAK` and installs the released driver
at the original `MOUSE.DRV` path. It never puts a replacement driver in
`PATCH`.

The media `PATCH` contains exactly `SETUP.CMD`, `PIF.EXE`, `HASH.EXE`, and
`SETVER.EXE`. `SETUP.CMD` creates only transient launch configuration files,
starts original Setup through plain `run16`, and removes those files before
returning. Original Setup remains responsible for assembling `WIN100.BIN`.
Its one-shot Setup PIF and generated Windows launch PIF explicitly request
CloseOnExit, so each separate PIF session returns to its calling script when
the target terminates.
After Setup, enter its actual installed directory to generate the
self-contained installed `PATCH\WIN.CMD` profile.

Repeat application accepts only the already-installed driver plus its retained
backup; it refuses a conflicting backup/current pair. No PowerShell script,
template, JSON manifest, mouse-driver copy, or persistent work directory is
copied into media `PATCH`. Use a short physical DOS-compatible path without
spaces or shell metacharacters; PIF string fields are bounded. `run16` is
resolved through `PATH` and no drive path is hard-coded.

To restore prepared media, open this tool package's `UNAPPLY.CMD` and enter
the same media root. It first proves that the root `MOUSE.DRV` is still this
tool's replacement, restores `MOUSE.DRV.BAK` in place, then removes only the
four byte-for-byte verified helper files. Any other `PATCH` files or
directories are retained untouched; an empty `PATCH` directory is removed.
