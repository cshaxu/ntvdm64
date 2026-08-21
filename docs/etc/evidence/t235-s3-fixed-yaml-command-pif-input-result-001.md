# M0 T235 S3 — Fixed YAML and COMMAND/PIF Input Result 001

## Question

Can the fixed T235 runner admit a target-first COMMAND input without a runner
protocol version, while ensuring PIF-selected CONFIG/AUTOEXEC inputs are
chosen before COMMAND services `54:0C` and `54:0D`?

## Inputs and Ownership

- The runner resolves only its sibling `ntvdmcfg.yaml`; component names are
  sibling-safe, identity checked, and duplicate or unknown YAML keys fail.
  The published runner name is `build/output/ntvdm64-0235.exe`.
- The runner's first non-option remains the target.  `.bat` and `.pif` are
  classified by suffix before content inspection; no CLI BAT or PIF parser
  was added.
- `cmdmisc.c:cmdGetNextCmd` remains the initial COMMAND owner and
  `cmdpif.c:cmdCheckForPIF` remains the later PIF target owner.
- The early PIF configuration selection is the original
  `softpc.new/host/src/nt_pif.c:GetPIFData` followed by its original
  `GetPIFConfigFiles`; the wrapper merely invokes those bodies before the
  copied source reaches COMMAND and copies their selected paths into the
  session.

## Procedure and Observations

1. Fresh MSVC x64 `/MT` runner graph
   `D:\tmp\ntdos64-M0-T235-S3-runner-r2` built nine actions, including the
   new fixed-YAML loader, and emitted `ntvdm64-0235.exe`.
2. `tests/ntdos64-run-policy.cmake` and
   `tests/ntdos64-run-s1-format-policy.cmake` passed against that executable.
   They cover target-first PE handoff, COM/BAT/PIF engine disposition, fixed
   YAML sibling loading, PIF engine argument handoff, and rejected unknown
   YAML configuration.
3. Fresh full module graph
   `D:\tmp\ntdos64-M0-T235-S3-formal-r2` rebuilt the affected original
   COMMAND/PIF module.  The PIF fixture constructs a PIF carrying NT config
   and autoexec names, then observes the original parser return both selected
   names before any COMMAND configuration request.  The `54:0C/0D` fixture
   and the copied initial-command fixture also returned zero.

## Interpretation

This removes the false earlier claim that the PIF configuration choice could
wait for `cmdCheckForPIF`: that function intentionally sets
`IgnoreConfigAutoexec`, so it is too late.  The session binding now performs
the source-owned selection first and keeps only copied path bytes in its
session record.  It is not a claim of completed guest loading, child/parent
lifecycle, WOW, or a full T235 closure.

## Native-Engine Contract Correction

The first S3 result found a real end-to-end mismatch: the runner already
passed `--ntvdmcfg` and `--config-root`, while `ntdos64-native` and
`dem_v2_startup_composition` still required a versioned JSON
`--byob-profile`.  They now use the same fixed YAML loader.  The copied engine
request keeps its existing descriptor ABI, but its profile-descriptor bytes
now name `ntvdmcfg.yaml`; the root descriptor is checked against the root
derived from that YAML pathname.  Native CLI also publishes the validated
CONFIG/AUTOEXEC paths to the existing session binding.

Fresh runner graph r3 rebuilt and passed both runner policy scripts.  Fresh
full graph r3 compiled the changed YAML loader, native CLI, DEM startup
composition, and COMMAND runtime-session objects.  Linking the complete
native executable remains blocked before these changes execute by the retained
CPU C++/OpenNT header closure (`error.h`/`config.h` macro types); this is a
pre-existing whole-core graph frontier, not evidence of a successful native
guest run.

## Follow-up

Complete S3's remaining end-to-end startup execution and governance checks,
then transfer any unresolved child/lifecycle behavior to its explicit owner.
