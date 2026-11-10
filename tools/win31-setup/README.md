# Windows 3.1 Setup

Use `APPLY.CMD` and enter the path of a **copy** of the Windows 3.1 installation media. The original supplied media must remain unchanged. The command prepares that target's `PATCH`, adapts the checked kernel inputs and substitutes the released mouse driver. Exact original compressed files are retained beside their inputs as `KRNL386.EX_.BAK`, `WIN386.EX_.BAK` and `MOUSE.DR_.BAK`.

If a media copy still contains an older package, use its `UNAPPLY.CMD` recovery workflow before applying this version; APPLY preserves unrecognized existing helper files.

Run `<target>\PATCH\SETUP.CMD` from CMD with `run16` on PATH. After Setup succeeds, enter the selected Windows installation directory. Finalization always copies `INSIGNIA.386`, `NTVDM64.386`, `PIF.EXE` and the single `WIN.CMD` launcher into its `PATCH`. The two device entries in `[386Enh]` start commented out. Original installed guest binaries are not patched again.

Run the installed `PATCH\WIN.CMD` to select Standard or 386 Enhanced, then DOSONLY (`0` host commands, `1` guest DOS). Enhanced mode additionally offers the two-driver enable switch. Standard mode leaves both entries commented. The launcher generates `CONFIG.NT`, `AUTOEXEC.NT` and one `WIN.PIF` each time, and enables/disables only its two device entries. There is no template PIF, second installer, PowerShell runtime dependency or permanent configuration backup. Other SYSTEM.INI entries are preserved. Invalid choices or configuration errors stop before launch; temporary configuration staging is removed. Do not reconfigure an installation while its Windows instance is running.

The installation is self-contained except for `run16` on PATH. `PATCH\TEMP` holds runtime scratch. The generated NT files and PIF are owned outputs and are replaced on the next launch; customize unrelated SYSTEM.INI entries rather than those generated files. Six configurations retain the S14 execution/scheduling contracts. This does not resolve the separately deferred EDIT mouse transition defect.

`patch31.exe` applies hash-checked kernel adaptations to staging copies.
`SZDD.EXE` writes the SZDD compressed files consumed by original Setup.
Both are independent utilities in assets/release, used by APPLY.CMD.

`UNAPPLY.CMD` prompts for the media-copy directory, restores the three
compressed files directly from their adjacent backups, and deletes the entire
media `PATCH` directory, including user files. Installed profiles are separate.

Build each utility like PIF/GRP/LNK: `src\addon\szdd\BUILD.CMD output-exe` and `src\addon\patch31\BUILD.CMD output-exe`. Run from a build-owned output directory; publish through `tools\build\Stage-AddonUtilityRelease.ps1`. These tools contain no Microsoft guest binaries. Standard and enhanced modes depend on the actual installed NTVDM runtime; no enhanced-mode fallback is performed.
