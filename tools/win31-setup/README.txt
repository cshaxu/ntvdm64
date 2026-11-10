Windows 3.1 Setup patch

Run APPLY.CMD against a copy of the installation media, then run its
PATCH\SETUP.CMD. After graphical Setup finishes, enter the installed directory.
Both .386 drivers and PIF.EXE are always copied to the installed PATCH.

Start the installed PATCH\WIN.CMD. Select Standard or 386 Enhanced, DOSONLY
0 or 1, and (Enhanced only) whether to enable the two drivers. WIN.CMD
regenerates CONFIG.NT, AUTOEXEC.NT and WIN.PIF; it toggles the two owned
SYSTEM.INI entries by commenting/uncommenting them. Other entries are kept.
run16 must be on PATH. Do not reconfigure a running Windows installation.
No PowerShell or repository path is needed by the installed launcher.
