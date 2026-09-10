# M0 T369 S1 — Revalidated original DOS EXEC and parent-return ledger

## Scope

This is a source and composition audit.  It does not change guest media, add
a host executable loader, or infer a missing BOP provider from the earlier
fixed observation.

## Original guest-owned route

1. The original transient COMMAND code in
   `src/mvdm-guest/dos/v86/cmd/command/tmisc1.asm` enters `EXTERNAL`, consumes
   the one-shot `EXECPATH_SIZE`/`EXECEXT_TYPE` values written by the original
   host `cmdGetNextCmd`, and jumps through the resident `EXEC_ADDR`.  If the
   record does not supply a complete path it instead uses the original guest
   `path_search`; neither choice is an app-side loader.
2. The guest resident COMMAND path reaches the original INT 21h EXEC
   transaction.  `src/mvdm-guest/dos/v86/doskrnl/dos/msproc.asm:$Exec`
   validates AL, preserves the `SCS_Is_Dos_Binary` classification supplied by
   COMMAND, and calls the original `get_binary_type`.
3. `get_binary_type` uses original `$OPEN`, `$IOCTL`, `$READ`, `$LSEEK` and
   `$CLOSE` state.  An unknown image alone invokes the already imported
   COMMAND provider `cmdCheckBinary` through `SVC_CMDCHECKBINARY`; it is not
   a license to add a host parser.  The body is in
   `src/mvdm-host/dos/command/cmdexec.c` and is selected by the original
   `cmddisp.c` table.
4. The COM branch performs the original largest-arena allocation and
   `Exec_Read_Com -> Exec_Read_Block -> ExecRead` transfer.  Its normal file
   read is the immutable guest `handle.asm:$Read -> SVC_DEMREAD` crossing to
   original `demhndl.c:demRead`, already bounded by the session guest-memory
   lease.  MZ parsing, allocation and relocation stay in the same original
   `msproc.asm` owner; they are not reimplemented by the host.
5. `Exec_Build_Header` creates the original PSP, arena ownership,
   environment/FCB/tail and saved parent return vectors.  `Xfer_To_User`
   emits original `SVC_DEMENTRYDOSAPP`, executes `DOCLI`, installs the child
   stack/register state and uses `retf`.  The corresponding original DEM
   dispatch family, including `demEntryDosApp`, remains the only host owner.
6. Program termination returns through original `$Exit` / `$Abort` in
   `msproc.asm`: saved terminate and Ctrl-C vectors are restored before
   `reset_environment`.  Original COMMAND `command1.asm:Exec_Ret ->
   Exec_Wait -> LodCom` consumes the guest-derived DOS result.  `54:0B`
   belongs only to the separate native-child route and cannot substitute this
   DOS parent-return chain.

## Current boundary disposition

- The original guest bodies above are immutable staged media, not host-linked
  objects.  The formal CPU40/x86 executable already links the original DEM
  and COMMAND archives and the existing session mapping-manager boundary;
  T365--T368 established the source/link/local provider contracts.
- There is no source-defined host loader, synthetic PSP/child frame, or
  standalone parent-return provider to add.  Such a change would violate the
  original ownership shown above.
- The retained T367 observation is not valid evidence for the current
  package-root contract.  Its report says `fixed-system-root=O:\\ntvdm64\\mvdm`,
  while current `src/app/package_layout.c` and
  `tools/build/Stage-OriginalSoftpcRuntime.mjs` both use the executable/stage
  directory itself as SystemRoot and place `system32` and `softpc` beneath it.
  The discrepancy comes from the older observer binary named in the T367
  command, not from the current observer source.  Current
  `tests/observation/console_startup_observer.c` reports `argv[2]` directly.

## S2 selection

The earliest incomplete whole cohort is therefore **current observation
container identity**, not an EXEC leaf service: rebuild the existing
source-shaped console-owning observer from its current source, bind it to the
current staged root and formal product, and prove that it does not alter
guest/COMMAND/DEM semantics.  This is a necessary evidence boundary before
one new fixed observation can say whether the original route reaches `$Exec`.
It does not authorize a guest or host execution implementation change.

## Deferred owners

- Runtime proof of COM load, child entry, `$Exit` and parent continuation is
  reserved for T369 S4 after the current container is formally rebuilt.
- MZ execution remains the same original guest-owner branch but is not
  claimed from a COM observation.
- Redirector, native child, WOW, graphics, multi-session and x64 execution
  remain outside this packet.
