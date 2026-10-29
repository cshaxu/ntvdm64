# Windows 3.1 Setup

Use `APPLY.CMD` and enter the path of a **copy** of the Windows 3.1 installation media. The original supplied media must remain unchanged. The command prepares that target's `PATCH`, adapts the checked kernel inputs and substitutes the released mouse driver. Exact original compressed files are retained beside their inputs as `KRNL386.EX_.BAK`, `WIN386.EX_.BAK` and `MOUSE.DR_.BAK`.

Run `<target>\PATCH\SETUP.CMD` from CMD with `run16` on PATH. Select the actual Windows installation directory in Setup, then enter it when the script asks. SETUP.CMD generates that installation's own `PATCH\WINSTD.CMD`, `PATCH\WIN386.CMD`, PIFs and configuration using PIF.EXE; it does not patch installed guest binaries again. Setup scratch uses the target's `PATCH\TEMP`. Default NT profiles are not edited.

`patch31.exe` applies hash-checked kernel adaptations to staging copies.
`SZDD.EXE` writes the SZDD compressed files consumed by original Setup.
Both are independent utilities in assets/release, used by APPLY.CMD.

`UNAPPLY.CMD` prompts for the media-copy directory, restores the three
compressed files directly from their adjacent backups, and deletes the entire
media `PATCH` directory, including user files. Installed profiles are separate.

Build each utility like PIF/GRP/LNK: `src\addon\szdd\BUILD.CMD output-exe` and `src\addon\patch31\BUILD.CMD output-exe`. Run from a build-owned output directory; publish through `tools\build\Stage-AddonUtilityRelease.ps1`. These tools contain no Microsoft guest binaries. Standard and enhanced modes depend on the actual installed NTVDM runtime; no enhanced-mode fallback is performed.
