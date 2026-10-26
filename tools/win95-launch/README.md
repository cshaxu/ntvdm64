# Win95 add-on installation tools

These opt-in tools operate on an already prepared installation. They are not
runtime dependencies of Run16, NTSRV, NTVDM or WOW32. Keep the original disk
image read-only; the scripts use extracted files and explicit backups.

`Apply-Compat95.ps1 -WindowsRoot <installed-tree> -BackupRoot <fresh-backup>
-Logo <local-logo.sys>` checks the released compatibility-driver hash and
updates the installation's single existing CONFIG.NT driver entry. Select
`-NoLogo` instead of `-Logo` to disable artwork. The backup records previous
files, hashes and attributes; a failed installation restores them. See the
[driver source and build contract](../../src/addon/win95-compat-sys/README.md).

`Prepare-StartMenu.ps1 -OutputRoot <repo-build-stage> -Inventory <extracted-name-inventory>
-WindowsRoot <installed-tree> -ProgramFilesRoot <extracted-program-files>`
prepares copies and a hashed plan. It uses the released LNK/PIF utilities,
preserves original shortcut metadata and maps the supplied Win95 menu to
stable DOS paths. Inspect the generated `plan.json` before installation.

`Apply-StartMenu.ps1 -Plan <stage/plan.json> -BackupRoot <fresh-backup>` checks
source/candidate hashes and exact destinations before writing STRTMENU and
APPS95. A nonempty APPS95 is refused. Existing menu files and original input
files are saved in the backup; failure restores changed files and removes
only empty directories created by that attempt. These tools require VDM
sessions to be stopped and never edit original hives or executable bytes.

Keep the successful installation's `install.json` and backup together for
explicit recovery. `Restore-StartMenu.ps1 -BackupRoot <installation-backup>`
checks every installed identity before restoring saved files and removing
unchanged owned additions; it refuses user-modified files. There is no
background service or automatic uninstall. Actual guest menu usability is a separate acceptance
gate from offline shortcut parsing and installation safety.
