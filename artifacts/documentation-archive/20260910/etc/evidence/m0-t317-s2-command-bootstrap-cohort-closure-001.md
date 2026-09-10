# M0 T317 S2 — COMMAND Base VDM bootstrap cohort closure

## Question

Do the selected original COMMAND bootstrap units compose under the sole
Win32/x86 `CPU_40_STYLE`/CCPU40 product profile while preserving the original
Base VDM request, size, retry and re-entry contract?

## Inputs

- The original COMMAND manifest and the T317 S1
  [boundary ledger](m0-t317-s1-dem-command-softpc-boundary-ledger-001.md).
- Original `cmd.c`, `cmddata.c`, `cmddisp.c`, `cmdmisc.c`, `cmdenv.c`,
  `cmdconf.c`, and `cmdkeyb.c` under `src/mvdm-host/dos/command/`.
- The same-shaped local Base VDM client/server boundary under
  `src/adapter-mvdm-host-out/basesrv/` and the session-owned SoftPC guest
  location lease.

## Procedure

1. Re-read each `GetNextVDMCommand` call in `cmdmisc.c`: the original body
   first requests environment capacity, retries after the returned size, then
   requests the command with its original retry/re-entry bits.  No local
   COMMAND dispatcher or alternate command source was introduced.
2. Re-read the cohort's `GetVDMAddr` use.  It continues through the existing
   SoftPC binding, which resolves a checked session mapping lease; no
   COMMAND-private guest mapper or native address identity was added.
3. Generate the external formal graph with:

   ```text
   tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86 \
     -BuildRoot build/M0-T317/S2/command-bootstrap-r1 \
     -NodeExecutable O:\.nvm\versions\node\v22.22.1\bin\node.exe
   ```

   Then build `original-mvdm-command.lib` and `basesrv-bindings.lib`.
4. Generate and run the focused x86 Base VDM fixture through
   `New-T307BaseVdmBrokerNinja.ps1`.

## Observations

- The formal graph completed all 15 requested edges.  `original-mvdm-command.lib`
  contains the selected original COMMAND manifest; the seven S2 units compile
  with `CPU_40_STYLE`, `CCPU`, `C_VID`, and no `CPU_30_STYLE`, `MONITOR`, or
  Bochs input.  `basesrv-bindings.lib` compiles the only local protocol
  provider.
- The focused fixture printed `PASS: local Base VDM broker contract`.  It
  covers first-VDM query/clear, environment-only request, insufficient-buffer
  size return, copied command record, pending wait/wake/retry, re-entry
  increment/decrement, explicit WOW decline, current-directory copy, and
  session-local `ExitVDM` completion.
- `cmdkeyb.c` retains its original registry/console fallback: failure takes
  `NoInstallkb16`, returns `DX=0`, and calls the original `cmdInitConsole`
  path.  S2 does not claim a keyboard-layout installation or a console product
  shell.

## Interpretation and disposition

The original bootstrap cohort is source and bounded-contract closed for the
selected profile.  Existing adapter code is a narrow BaseClient/BaseSrv
transport substitute and SoftPC lease boundary; it does not replace COMMAND
algorithm or dispatcher-table entries.  The compile warnings are retained
original-source diagnostics and were neither suppressed nor treated as a
successful runtime claim.

`cmdexec.c`/`cmdexit.c` and `cmdredir.c`/`cmdpif.c` remain outside this
closure.  They transfer unchanged to T317 S3, where child lifecycle and local
stream endpoints must be recovered as complete owner cohorts.

## Confidence

High for source selection, direct call shape, formal compilation, and the
local Base VDM protocol.  This is not proof of a guest command loop, DOS
`EXEC`/PSP return, a BaseSrv/CSRSS broker, or COMMAND child execution.
