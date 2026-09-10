# T204 S3 — configuration transform and contained-provider source contract

## Decision

The historical `cmdconf.c` implementation is the semantic reference, but its
temporary-file product composition is not the modern CLI provider.  A later
`bx-vdm` startup-configuration provider must:

1. consume one copied T204 S2 source policy and copied input/facts supplied at
   CLI admission;
2. transform CONFIG and AUTOEXEC into session-owned contained images;
3. publish those images and their short guest-visible paths through one
   provider-owned namespace capability;
4. feed the associated autoexec environment merge data into the existing
   copied COMMAND context without ambient re-capture; and
5. tear down generated images and merge state with the session.

It must not recreate NT4's PIF composition, create/delete host temporary
files, modify selected source files, read ambient process state from a BOP,
or make `54:0C`, `54:0D`, `54:0F`, `5E`, or `50:3D` an independently patched
endpoint.

## Original owner and ordered contract

| Stage | Original source and contract | Modern provider disposition |
| --- | --- | --- |
| Source selection | `softpc.new/host/src/nt_pif.c::GetPIFConfigFiles` consumes one PIF override when present; otherwise it builds `GetSystemDirectory()` plus `config.nt` or `autoexec.nt`. A PIF string is OEM environment-expanded, then freed. The routine itself can return an invalid name. | Replace PIF policy with explicit CLI-admitted copied input. A selected host source is ordinary user-mode input, not a host-installation dependency. |
| Raw input | `dos/command/cmdconf.c::ExpandConfigFiles` obtains Windows-directory short `SystemRoot`, opens the chosen source read-only, requires a nonzero finite size, then reads it fully. | Copy bounded input bytes before transform. Source reading may use selected normal user-mode APIs in a later provider; no BOP may open it. |
| CONFIG line pass | It skips leading non-graphical bytes, skips complete `country=` and `shell=` lines case-insensitively, expands each case-insensitive `%SystemRoot%` occurrence until EOL, writes every retained line with CRLF, then appends a locale/OEM-derived `country=` line and a controlled `shell=` line. | Preserve this ordering and line/error behavior in a source-derived transform. Locale, OEM code page and resolved root are copied host facts; do not query them at BOP time. |
| Shell rule | A SystemRoot `\\System32\\command.com` shell has its `/e:` option retained; another shell becomes `/c <third-party-shell>` and calls `cmdInitConsole`. The final shell is `%SystemRoot%\\System32\\command.com /p %SystemRoot%\\system32`, followed by either `/c` or `/e:`. | Map third-party-shell/console as a complete COMMAND capability disposition before enabling it. The first contained profile must not silently claim console support. |
| AUTOEXEC line pass | It writes retained lines with `%SystemRoot%` expansion. It also records `PROMPT`, `PATH`, and well-formed `SET` name/value pairs in a double-NUL name/value sequence; they remain in the emitted AUTOEXEC image. | Preserve the dual output: image plus copied merge sequence. The later COMMAND environment package owns applying it. |
| Environment merge | `cmdenv.c::cmdCreateVDMEnvironment` builds a DOS environment from copied 32-bit data, removes `=`, `COMSPEC`, malformed, and normally `WINDIR` entries, then overlays the AUTOEXEC tuple sequence after expansion. `cmdGetInitEnvironment` separately creates first-command OEM data and copies it to guest memory. | Existing copied COMMAND context remains the consumer boundary. A later provider may replace its boot-source provenance, not reintroduce ambient `GetEnvironmentStrings` access. |
| Guest path publication | `cmdGetConfigSys` / `cmdGetAutoexecBat` call the transform, convert the temporary ANSI path to OEM and copy it to `DS:DX` with maximum length 64; conversion failure displays `ED_INITMEMERR` then terminates the VDM. | Later bind both endpoints together to the same contained-image provider. Retain only their checked guest-RAM copy mechanics; the provider supplies a bounded OEM guest-visible path. |
| Cleanup | `DeleteConfigFiles` deletes the two host temporary files and frees their names. `cmdGetNextCmd` first call, WOW startup, reset and termination call it. | Teardown zeroes/releases only the session-owned images, paths and merge data. It never deletes a selected host source. |

## Original failure ledger

| Condition | Original result | First-profile disposition |
| --- | --- | --- |
| Source missing/unreadable, zero length, invalid file size, no short system root | `ED_BADSYSFILE` with source path, then `TerminateVDM`. | Map as source-input failure to the existing typed fatal startup terminal; no fake empty configuration. |
| Raw/input or merge allocation failure; OEM path conversion failure | `ED_INITMEMERR`, then `TerminateVDM`. `cmdGetAutoexecBat` passes `pchTmpConfigFile` in its error diagnostic, which is retained as a historical detail rather than an API requirement. | Bounded provider allocation failure uses the same fatal configuration category. |
| Temporary path/create/read/write/short-name failure | `ED_INITTMPFILE`, then `TerminateVDM`. `cmdCreateTempFile` uses `GetTempPath`, `GetTempFileName`, an inheritable handle and a Windows-directory fallback. | The APIs are available to a modern user-mode process but are deliberately **not selected**: contained-image allocation replaces this product-shell failure family. Its own bounded-image failure maps to the fatal category; no host temp side effect occurs. |
| Third-party shell | Calls console initialization before the generated `/c` shell. | Explicit deferred/no-console capability until COMMAND console package proves a selected profile. |
| `5E` zero notification | `nt_bop.c::MS_bop_E` calls `UMBNotify(0)` and `demDasdInit()`. | Separate XMS/machine and DEM raw-media owner actions; provider success does not turn this notification into a no-op closure. |
| Guest signals configuration fatal | `demmisc.c::demExitVDM` reports bad `config.nt` and calls `TerminateVDM`. | Retain the typed controlled stop as the non-GUI fatal fallback until a contained success path is fully regressed. |

No deleted or unavailable normal Win32 API blocks this package.  The actual
historical composition gaps are PIF ownership, process/VDM environment
capture, guest pointer/SAS access, guest-visible host temporary paths, UI,
and cross-owner console/UMB/DASD actions.  They require copied ABI and owner
composition, not reconstruction of a host kernel feature.

## Provider shape and lifetime

The following is the only admitted implementation shape for the next S;
field sizes remain to be selected against the existing checked transaction
limits.

```text
CLI selected source + copied host facts
             |
             v
T204 S2 policy -- authorizes one copied mode
             |
             v
bx-vdm startup-configuration session
  - copied CONFIG input / AUTOEXEC input
  - copied SystemRoot, country and OEM-codepage facts
  - generated CONFIG image / AUTOEXEC image
  - generated bounded guest-visible paths
  - AUTOEXEC name/value merge sequence
             |
             +--> one contained namespace/image backend
             +--> COMMAND boot-path and environment-context binders
             +--> explicit fatal result / session teardown
```

The provider may use a session-private contained image in every mutation
mode: the profile controls host-backed mutation policy, while transform
output must never modify its selected source.  `direct` therefore permits the
explicit source capability; it does not permit overwriting `config.nt` or
`autoexec.nt`.  `readonly`, `overlay`, and `virtual` must route through the
same S2/T201 authorization rather than grow independent substitute logic.

## Current-code reconciliation

| Existing surface | Required action in the future complete provider |
| --- | --- |
| `bx_ntvdm_readonly_namespace` fixed `REM NTVDM64` / empty arrays | Migrate only as named deterministic fixture input; remove from default startup composition. |
| `bx_ntvdm_command_boot_input_v1` and boot-file service | Preserve the checked two-path write mechanics, but bind them to provider-owned generated paths instead of profile constants. |
| `bx_ntvdm_boot_namespace_provider_v1` | Split generic command/target namespace responsibilities from startup configuration image ownership; no duplication of file state. |
| `bx_ntvdm_command_host_context_v1` | Retain its copied directory/environment/processor context. Add only a provider-owned, copied merge handoff; never re-read ambient environment. |
| `bx_ntvdm_config_done_service` | Do not change here. Its zero-notification actions remain separate owner prerequisites. |
| Typed `50:3D` STOP | Retain as failure fence until the provider has full image/path/environment and lifecycle regressions. |

## Required implementation regression

The next implementation S must run one whole-provider MSVC x64 `/MT`
regression, without a trace, covering at minimum:

1. selection rejection and copied-policy/mode mismatch;
2. CONFIG filtering, `%SystemRoot%` expansion, controlled country/shell output;
3. AUTOEXEC image retention plus PROMPT/PATH/SET tuple extraction and merge
   input lifetime;
4. empty/missing/oversize/malformed input and bounded allocation failure;
5. source immutability, no host temporary file creation, and teardown zeroing;
6. paired `54:0C/0D` path publication only after the provider image is valid;
   and
7. all four shared profile modes through the one policy gate.

Only after that regression may a later S bind the COMMAND and namespace
consumers, then run a single native integration observation.  This S changes
no source and runs no trace.
