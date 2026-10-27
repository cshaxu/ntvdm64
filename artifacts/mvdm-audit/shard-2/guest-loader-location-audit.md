# S10 shard 2: guest archive and installed loader path

## Read-only observations

`assets/winnt-guest-manifest.json` names 62 files. All 62 named entries in
`assets/winnt.zip` match their declared hashes; archive SHA-256 is
`7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7`.
The installed O:/winnt direct paths have 15 missing members and two different
existing configuration files (`autoexec.nt`, `config.nt`), recorded in
`guest-archive-install-observation.json`. These two configs may be intentional
user state and were not changed. A zip inventory is a packaging baseline; it
is not proof that the current O:/winnt installation contains every member.

In particular, the archive supplies original MMSYSTEM.DLL
`080432D0CAD660B163D0D231D37603010F7D9366F58570AC7C74D171C518211A`,
TOOLHELP.DLL
`DDEECCC103F7E3C105602A6F791792BAA7778A796709BE63983A8A921DF80CFE`,
and TIMER.DRV; their direct O:/winnt/system32 paths are absent. O:/winnt
itself has no matching copy and O:/winnt/system does not exist. A recursive
read-only inventory found alternative TOOLHELP/MMSYSTEM/TIMER files under
O:/winnt/tests/w31p and other test installations, but they are separate
application/test directories, not the regular O:/winnt/system32 location.
No claim is made about inherited DOS PATH, arbitrary application directory,
already-loaded module, KnownDLL override or another search location for an
individual run without observing that run.

## Original/project search chain

- `src/common/system_root.c::common_system_root_w` derives the product root
  by removing the image and system32 directories from its own executable
  path; for O:/winnt/system32/run16.exe that root is O:/winnt.
- `src/run16-exe/main.c::begin_worker_win16_directory` supplies the derived
  WIN16DIR to BaseCheckVDM's environment. Separately
  `mvdm_softpc_project_wow_initial_environment` projects product SYSTEMROOT
  into the finished guest environment before the initial guest PDB copy.
- The retained original `wow16/kernel31/ldboot.asm` says the WOW guest uses
  the loaded KERNEL module's directory for system32, and derives the 16-bit
  system path from projected SYSTEMROOT plus `\system`. `get_windir`
  separately honors WIN16DIR. Its `ldopen.asm` default source table searches
  current directory, Windows directory, system16 directory, system32, Wx86,
  application directory, then DOS/Novell PATH; the special BootOrder is
  different. This is a source-derived contract. The actual installed
  KRNL386.EXE SHA-256 `D404576899CC11D23F368F2B30554E6108657CD37809837B341FD37E6B1A7AF4`
  is not the same bytes as the shard's
  `wow16/kernel31/retail3/krnl386.exe` (`BCED7650A5B5D7AC25955AB2A21BC53FF06DE1C35B25FAD0690390135B093A03`),
  so source inspection alone does not prove all deployed search instructions.
- `wow16/kernel31/ld.asm` asks `WowIsKnownDLL` after boot; `wow32/wkman.c`
  uses the worker's `KnownDLLs` registry value, and a hit constructs a full
  path using `pszSystemDirectory`, which changes how fallback is applied.
  The current machine has neither direct host HKLM
  `SYSTEM/CurrentControlSet/Control/WOW` key nor
  `O:/winnt/system32/NTVDM.REG` at inspection time. The registry facade may
  combine environment or in-memory sources; the effective value of a specific
  worker was not captured and must not be assumed.

## Ordinary apps versus optional/dynamic modules

The guest-manifest NE import lists show SOL imports KERNEL/GDI/USER/SHELL;
WINMINE adds SOUND; WRITE adds KEYBOARD/OLECLI/COMMDLG; WOWEXEC adds SHELL
and KEYBOARD. None statically imports MMSYSTEM, TOOLHELP or TIMER. Each of
these four installed binaries and the installed SOUND.DRV matches the archive
manifest. This explains why their basic startup can succeed without the
three modules, but does not exclude a later dynamic load or demonstrate
all functionality.

When a program actually imports or dynamically loads MMSYSTEM, or a timer
driver resolves its MMSYSTEM dependency, a matching guest module must be
found through the applicable guest loader path. The retained original
`wow16/mmsystem/init.c::ThunkInit` requests native WINMM and its private
`aux32Message`, `tid32Message` and `joy32Message` entrypoints via the
original WOW bridge; T466 S2/S3 demonstrate bounded source-shaped calls and
export availability, not full physical sound/joystick use. The archive
MMSYSTEM source-binary relationship and the actual selected DLL for a given
run are not proved merely by its filename.

A client importing TOOLHELP needs a matching guest DLL on its own applicable
search path. T470 S2 did not rely on the installed system32: its runner copied
an independently pinned `O:/winnt/tests/w31p/SYSTEM/TOOLHELP.DLL` alongside
the test app. The guest executed, but TaskFirst and valid ModuleFirst both
returned 0; that was accepted only as a negative compatibility witness.
The test copy SHA-256 is
`274568F7B811539C2D85700617D3624FD63AD6724419295174BD73FD72A1C291`,
different from the archive TOOLHELP SHA above. T470 also recorded an
independently pinned prebuilt source ToolHelp probe with the same negative
result. Neither demonstrates success of queries or availability at the
installed system path.

## Verification needed and boundaries

Use a disposable staged package or non-mutating observer and a read-only
independent Win16 module-load witness to capture actual resolved path/hash
for MMSYSTEM/TOOLHELP, default-path absence and app-local fallback, plus
KnownDLLs state and explicit errors. Compare identical guest input against
the installed route and the archive-staged route; do not use existing basic
SOL/WINMINE success as proof of those dynamic imports. No test was run here.
No guest, installed configuration, registry or product file was changed.
