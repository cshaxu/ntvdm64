# Windows 3.1 Setup

Use `APPLY.CMD` and enter the path of a **copy** of the Windows 3.1 installation media. The original supplied media must remain unchanged. The command prepares that target's `PATCH` and adapts the two checked compressed Setup inputs; the exact original compressed files are retained as `PATCH\KRNL386.EX_.BAK` and `PATCH\WIN386.EX_.BAK`.

Run `<target>\PATCH\SETUP.CMD` from CMD with `run16` on PATH. Select the actual Windows installation directory in Setup, then enter it when the script asks. PATCHSET creates that installation's own `PATCH\WINSTD.CMD`, `PATCH\WIN386.CMD`, PIFs, configuration and mouse driver. Setup scratch uses the target's `PATCH\TEMP`. Default NT profiles are not edited.

`UNAPPLY.CMD` accepts the same media-copy directory, checks the current images and backups, restores the exact original compressed files and removes only verified tool files. Unknown files are retained. A conflicting file or missing recovery backup stops the operation.

The helper executables can be rebuilt with `src\addon\win31-setup\BUILD-TOOLS.CMD`, which takes separate output and build directories. The packaged executable components do not contain Microsoft guest binaries. Standard and enhanced modes depend on the actual installed NTVDM runtime; no enhanced-mode fallback is performed.
