# Normal real WOW32 loader boundary

An isolated copy of the current sealed package ran SOL through Run16. The
process-bound observer reported the loaded module at the isolated runtime's
`system32/WOW32.DLL`; its pre-launch package manifest identifies that file as
SHA-256 `14473D50F5A9525A7046F3E90813EEFDD4E4913147455955DA94E7698140F0D4`,
matching the current `assets/release/WOW32.DLL`. The Options dialog opened,
closed and returned to SOL, and the launcher exited zero. The manifest, module
path, probe output and result are retained here. This observes the loaded
module path and the staged file's identity; it does not independently hash the
process's mapped pages after load.

The corrected BOOLEAN ABI is the native host WOW32.DLL process initializer,
not an arbitrary Win16 DLL entry. This successful live case and the separate
controlled false-return loader fixture do not prove a real WOW32 allocation
failure, notification ordering or unload cleanup. Win16 guest DLL loading and
unloading are separate obligations. No guest or installed image was changed.
