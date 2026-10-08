Windows 3.1 original-Setup PATCH
================================

This derived Setup package keeps the original media layout, with only checked
KRNL386.EX_ and WIN386.EX_ replacements needed before Setup's first Windows
load.  PATCH adds a batch entry point, one setup finalizer, MOUSE31.DRV, the
checked candidates and their checked retail recovery images.  It does not
write to the supplied original media or default NT config files.

1. Run PATCH\SETUP.CMD from a short physical path. It launches original Setup
   through DOS COMMAND.COM from this package's media root, then asks for the
   actual directory selected by Setup.  PATCHSET.EXE creates the installed
   profiles and PIFs from an embedded template; it never needs a PowerShell
   script or template file at install time.
2. After postconfiguration, use exactly one installed entry point:
     PATCH\WINSTD.CMD   standard mode (/S)
     PATCH\WIN386.CMD   386 enhanced mode (/3)
3. Both resolve run16 from PATH. PATCH\CONFIG.NT and PATCH\AUTOEXEC.NT are
   shared because their current, checked semantics are identical.

The installed PATCH is self-contained and contains only the two launchers,
their PIFs, one shared profile pair, MOUSE31.DRV and recoverable KRNL386/WIN386
originals.
Do not keep a dependency on this media directory, a repository path, or a
hard-coded drive. The enhanced mode remains an accepted instability-limited
experiment; there is no fallback from /3 to /S.
