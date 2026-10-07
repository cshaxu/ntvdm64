# Windows 3.1 isolated launch profile

Generate with explicit InstallRoot and OutputDirectory; no fixed machine path
or drive mapping is used. Deploy the four outputs to the installation's PATCH
directory and invoke its win.cmd with run16 available on PATH. PIF/config paths
refer to that installed PATCH, not the staging directory. Regenerate after
relocation. Standard mode /S is set in both PIF argument fields; dedicated
config uses HIMEM and DOSONLY, and autoexec does not preload NT/WOW DOSX.
Windows uses its existing SYSTEM/DOSX.EXE. Default product profiles, existing
launchers and guest binaries are not modified. This generator preserves a
preexisting installation; it neither installs Windows nor patches KRNL386.
An already patched experimental installation is not original-media acceptance.

## Owner-approved enhanced-mode experiment

adapt-retail-dosmgr.ps1 accepts only the exact reviewed retail WIN386 and NT DOS
hashes, checks original LE pages/fixups/instruction bytes and creates a new
build-owned image with one checked ten-byte substitution. It supplies the
verified NT DOS33-byte SFT entry size before the unsupported discovery probe
acquires any buffer or CON handles. System-VM/client-register positioning,
client-state push/pop and the original size commit remain. The original
temporary-allocation field is verified zero, so cleanup has nothing to free.
The caller consumes carry only and overwrites EAX immediately afterwards.
Outside-region bytes, object extents, page maps, fixups and file length remain
unchanged. The larger exhausted-scan fallback is superseded research evidence.
It does not modify the input installation, NT DOS or any host runtime.
Use only a recoverable separately approved retail installation copy; runtime
compatibility is not implied by successful image construction.

Enhanced profiles support /3, optional /B boot logging and the original PIF's
DisableIdleDetection option. The latter may substantially increase CPU usage.
Do not apply these to the standard-mode profile implicitly. VGA uses the
original paired VGA.DRV/VGA.3GR and built-in VDDVGA, not a mixed Video7 VDD.
Actual /3 desktop and Notepad have been observed, but later independent replays
still expose startup instability. This is a research candidate, not a reliable
general release or proof of normal shutdown, mouse or DOS-VM interoperability.
The current output matches the candidate that produced those observations.
Independent failures remain recorded; a stale BOOTLOG is not current startup
evidence. This is not a claim of reliable general compatibility.
