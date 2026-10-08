# Windows 1.01 Setup additions

Open `APPLY.CMD`, enter the original Windows 1.01 media directory once, then
run that media directory's `PATCH\SETUP.CMD` from CMD. The only files copied
into media `PATCH` are its batch orchestrator, documentation, `MOUSE.DRV`,
`SETVER.EXE`, `PIF.EXE`, and `HASH.EXE`.

`SETUP.CMD` creates a temporary private copy of the flat media, substitutes
only that copy's mouse driver, generates a PIF with `PIF.EXE`, and starts
original Setup through plain `run16`. Original Setup remains responsible for
assembling `WIN100.BIN`. After Setup, enter its actual installed directory to
generate the self-contained installed `PATCH\WIN.CMD` profile.

No PowerShell script, template, JSON manifest, or persistent `WORK` directory
is copied into the media or installed PATCH. Use a short physical DOS-compatible
path without spaces or shell metacharacters; PIF string fields are bounded.
`run16` is resolved through `PATH` and no drive path is hard-coded.
