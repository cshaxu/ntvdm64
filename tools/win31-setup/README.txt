Windows 3.1 Setup
=================

Run the tool's APPLY.CMD and select a copy of the installation media. Then run
that media copy's PATCH\SETUP.CMD, select an installation directory in Setup,
and provide the same directory when asked. The installed directory receives
its own PATCH\WINSTD.CMD and PATCH\WIN386.CMD launchers.

UNAPPLY.CMD restores the compressed media files directly from adjacent .BAK
files and deletes the entire media PATCH directory, including user files.
Installed launch profiles are separate. SETUP.CMD does not patch installed
guest binaries again. Never use original media as the target; use a copy.
