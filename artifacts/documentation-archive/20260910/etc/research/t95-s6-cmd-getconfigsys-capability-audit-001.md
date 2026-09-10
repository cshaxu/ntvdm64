# T95 S6 CMD `54:0C` CONFIG.SYS capability audit 001

## Question

What does the first post-`54:04` BOP, COMMAND `54:0C`
(`SVC_GETCONFIGSYS`), require, and may it be resumed by the present
Bochs/adapter closure?  Does its paired COMMAND `54:0D`
(`SVC_GETAUTOEXECBAT`) have the same ownership and closure requirements?

## Inputs

- Immutable listener observation
  `artifacts/analysis/t95-s6-r30-cli-root-release-023-20260811-001`, which
  reaches `C4 C4 54 0C` at `CS:IP=8DC8:0732`.
- OpenNT `base/mvdm/inc/cmdsvc.h:28`, which assigns service ID 12.
- OpenNT guest caller `base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm:1158-1170`.
- Historical host owner `base/mvdm/dos/command/cmdconf.c:51-82` and its
  expansion path `cmdconf.c:187-269`; temporary-file helper
  `base/mvdm/dos/command/cmdredir.c:99-154`.
- OpenNT COMMAND caller `base/mvdm/dos/v86/cmd/command/init.asm:1401-1421`
  for `SVC_GETAUTOEXECBAT`, and original selector source
  `softpc.new/host/src/nt_pif.c:29-86`.

## Observations

1. The real-mode guest saves DS, makes DS point at its `config` descriptor,
   loads `DX=offset config`, invokes `CMDSVC SVC_GETCONFIGSYS`, restores DS,
   then immediately calls `doconf`.  Thus the output is an input filename for
   the existing NTDOS CONFIG.SYS parser, not merely a status acknowledgement.
2. `cmdGetConfigSys` calls `ExpandConfigFiles(TRUE)`, converts the resulting
   temporary host filename to OEM, and copies it to guest `DS:DX` with a
   maximum output length of 64 bytes.  Its documented failure behavior is
   `TerminateVDM`, not a CF/AX DOS error return.
3. `ExpandConfigFiles(TRUE)` obtains a selected CONFIG.SYS name, opens and
   reads it, creates a temporary file, expands `%SystemRoot%`, adds a `SHELL`
   line, and requires a short temporary pathname of at most 63 characters.
   It owns temporary-file cleanup through `DeleteConfigFiles`.
4. The current adapter has only an immutable admitted-drive snapshot and
   opaque root handles.  It intentionally has neither selected input-file
   policy, bounded file read, temp-file lifecycle, OEM conversion, nor a
   guest-visible path mapping.  The `54:04` root string facility cannot supply
   any of these requirements.
5. COMMAND later invokes `54:0D`, then immediately opens the returned name
   through its ordinary DOS `INT 21h` path.  The historical host routine is
   the same `ExpandConfigFiles(FALSE)` pipeline; it additionally retains
   environment lines for later COMMAND initialization.  The original
   `GetPIFConfigFiles` chooses PIF overrides or the host system directory's
   `config.nt`/`autoexec.nt`; those ambient NT/PIF choices are not part of the
   CLI profile and cannot be inherited unchanged.
6. `ExpandConfigFiles(TRUE)` is not an empty-file fallback.  After filtering
   input it unconditionally appends `COUNTRY=` from the host locale/OEM code
   page and a `SHELL=` line containing the host `%SystemRoot%` and
   `System32\\command.com` (`cmdconf.c:462-542`).  Thus a project-authored
   “minimal built-in CONFIG” would be a new ntdos64 profile semantic, not a
   source-equivalent absence case.

## Interpretation

`54:0C`/`54:0D` are one coherent **CLI/adapter configuration-materialization
capability**, with lifecycle cleanup; neither is a one-BOP fixed register
result.  Replacing either with a synthetic guest path would make `doconf` or
COMMAND's ordinary DOS open parse a file that does not exist.  Opening the
ambient host `C:\CONFIG.SYS`, system `config.nt`/`autoexec.nt`, PIF choice or
process temp directory would also bypass the CLI's contained namespace policy.

More fundamentally, current work has only root admission and search-oriented
namespace evidence; it has no admitted DOS open/read projection or guest path
mapping.  The returned temporary pathname cannot become reachable merely by a
RAM write.  Any implementation therefore depends on a prior bounded
read-only DOS-file projection (or an explicitly selected guest-media owner),
not on a standalone configuration-file BOP shim.

For the same reason, the CLI cannot silently select an empty CONFIG.SYS as a
faithful default.  It must either require explicit configuration/command
profile input, or deliberately define and label a new ntdos64 configuration
profile with its own guest COMMAND path and country/code-page policy.  That is
a product/CLI decision, not an adapter or Bochs implementation detail.

There is an earlier guest-composition prerequisite: the current BYOB session
identity-validates `COMMAND.COM`, but the first guest-startup descriptor and
source-built NTIO observations explicitly classify it as identity-only, not
placed or started.  It therefore supplies neither a guest pathname nor a
`SHELL=` target.  S7 must first receive a source-derived guest artifact
placement/loader contract that makes the selected COMMAND payload reachable
under a declared guest path.  Reusing `--byob-root` as that path is forbidden:
it is an immutable artifact-identity authority, not a DOS namespace root.

The current listener therefore remains the correct behavior: it records the
call and leaves the native Bochs `#UD` path unchanged.  No Bochs change,
adapter dispatch, or host file access is admitted by this audit.

## Follow-up

Before implementing either `54:0C` or `54:0D`, first establish the owner of
the bounded DOS open/read projection and the guest COMMAND placement required
by the returned name.  Only then may a paired capability be admitted whose
CLI contract chooses immutable CONFIG/AUTOEXEC inputs and a contained
materialization location, and whose adapter contract specifies bounded
content, OEM filename output, failure termination/controlled-stop mapping and
session cleanup.  It may extend the same future read-only namespace projection
after its complete contract is admitted, but may not create a parallel one-off
path facility or put file semantics into Bochs.
