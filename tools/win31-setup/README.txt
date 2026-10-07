Windows 3.1 original-Setup PATCH
================================

This PATCH adds only authored setup support and MOUSE31.DRV. It does not
replace original media or default NT config files.

1. Run PATCH\SETUP.CMD from a short physical path. It runs original Setup
   directly from this package's media root and asks for the
   actual directory selected by Setup.
2. After postconfiguration, use exactly one installed entry point:
     PATCH\WINSTD.CMD   standard mode (/S)
     PATCH\WIN386.CMD   386 enhanced mode (/3)
3. Both resolve run16 from PATH. PATCH\CONFIG.NT and PATCH\AUTOEXEC.NT are
   shared because their current, checked semantics are identical.

The installed PATCH is self-contained. Do not keep a dependency on this media
directory, a repository path, or a hard-coded drive. The enhanced
mode remains an accepted instability-limited experiment; there is no fallback
from /3 to /S.
