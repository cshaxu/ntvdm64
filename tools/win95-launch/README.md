# Win95 launch tools

The direct launcher is `APPLY.CMD <installed-Windows-95-directory>`. It works
only on a user-selected, recoverable installed tree: original disk media is
never mounted or changed. It refuses pre-existing tool-owned `PATCH` content,
uses the paired Win95 path migrator, identity-checks the one supported VMM32
consumer image, and creates `PATCH\\WIN.CMD`/`WIN.PIF` plus private NTVDM
configuration. The wrapper resolves `run16` through `PATH`; it has no fixed
repository or media path.

`UNAPPLY.CMD <directory>` requires the exact journal created by a successful
Apply. It restores only journaled adjacent backups and removes only known
tool-owned files. A missing journal is a refusal, never an instruction to
delete `PATCH` content.

`PATCH95.EXE` and the freshly built `compat95.sys` must be beside the wrapper;
released `PIF.EXE` and `MOUSE31.DRV` are read from `assets/release`. The tool
does not install a background service or modify the global NTVDM installation.

The lower-level `Apply-Compat95.ps1` / `Restore-StartMenu.ps1` tools remain
explicit opt-in installation helpers. They require fresh external backups,
record installed identities, and refuse restoration over later user changes.
`Prepare-StartMenu.ps1` produces a build-owned hashed plan; it never edits a
guest tree. Their source and recovery contract are covered by the component
fixture tests, not by a claim of general Win95 runtime compatibility.
