# LNK add-on utility

`LNK.EXE` is an independently authored AMD64 offline tool for classic local
Shell Links, alongside PIF.EXE and GRP.EXE. It does not execute, resolve or
search for targets, use host COM, create installation backups or alter the
registry. The caller owns file selection, backups and rollback.

```
LNK.EXE show FILE [--codepage 936]
LNK.EXE create FILE --target O:\WINDOWS\NOTEPAD.EXE --directory O:\WINDOWS --codepage 936
LNK.EXE update FILE --target PATH [--directory PATH] [--arguments TEXT] [--description TEXT] [--icon PATH] [--icon-index N] [--show 1|3|7] [--hotkey N] [--codepage N]
```

The writer emits a 76-byte ShellLinkHeader, a classic Win95 filesystem PIDL,
28-byte LinkInfo header, local VolumeID, ANSI strings and a terminal ExtraData
DWORD. Target path components must be ASCII DOS 8.3 names; a long pathname
with no real short name is rejected. Explicitly select
936 for the supplied Chinese Win95; the default is the host ANSI codepage.
Unrepresentable text is rejected, not replaced with question marks.

Reading validates bounds, IDList items, counted strings and ExtraData blocks.
Editing canonicalizes supported local links and replaces the old opaque PIDL
(and an old relative path when replacing the target), avoiding stale C-drive
identity with the newly encoded target. Nonterminal ExtraData and unknown flags are read-only. UNC links are
unsupported. Metadata, arguments, description, working directory, icon,
show command and hotkey survive supported updates unless explicitly changed.
Create refuses to replace an existing file; updates use a flushed unique
temporary file and atomic replacement. Inputs are bounded to 1 MiB.

Build from an owned background process with `BUILD.CMD <build-output>/LNK.EXE`.
Intermediates belong under the repository build directory. The shared add-on
manifest owns publication, separately from the ten host components.

Format references: Microsoft's [MS-SHLLINK structures](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-shllink/747629b3-b5be-452a-8101-b9a2ec49978c)
and [LinkInfo](https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-shllink/6813269d-0cc8-4be2-933f-e96e8e3412dc).

Win95 filesystem-item layout is identity-bound to the owner's extracted
`/WINDOWS/Start Menu/Programs/附件/记事本.lnk`, SHA-256
`FCC713917FA8373B1AE0D83FA42EED5861383061E388401B6E9AB2008413D4AE`.
Only the observed item structure is used: My Computer GUID, 25-byte drive
item, and 14-byte file/directory header plus name and empty secondary name.
No original file/code is bundled. Actual Win95 SHELL32 class-factory and
IPersistFile/GetPath verification is separate from host parser tests.
