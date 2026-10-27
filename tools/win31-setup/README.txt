Windows 3.1 Setup
=================

Run the tool's APPLY.CMD and select a copy of the installation media. Then run
that media copy's PATCH\SETUP.CMD, select an installation directory in Setup,
and provide the same directory when asked. The installed directory receives
its own PATCH\WINSTD.CMD and PATCH\WIN386.CMD launchers.

UNAPPLY.CMD restores the exact original compressed media inputs from the
verified backups in the target PATCH. Unknown files are preserved. Never use
the supplied original media as the target; use a recoverable copy.
