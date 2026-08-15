# T214 S3: OpenNT COMMAND Environment Recovery

## Question

Why does the current CLI composition reject before CPU entry at
`COMMAND_CONTEXT_CAPTURE`, and what implementation order preserves the
OpenNT COMMAND environment contract?

## Inputs

- OpenNT source: `src/opennt/base/mvdm/dos/command/cmdenv.c`, especially
  `cmdGetInitEnvironment` and `cmdCreateVDMEnvironment`.
- Current adapter capture:
  `src/bx-vdm/bx_ntvdm_composition_runtime_v1.c`.
- Current copied context and transaction contracts:
  `bx_ntvdm_command_host_context_v1.h` and `bx_ntvdm_multi_write_abi.h`.
- Full current-source composition fixture: T214 S3 owner fixture, run against
  the declared `profile-v7.json` and normal-return root.

## Source And Composition Finding

The original `cmdGetInitEnvironment` reads the NTVDM process environment,
removes drive-current-directory entries headed by `=`, replaces `COMSPEC`,
removes `WINDIR` for DOS (except the separate-WOW case), supplies
`PROMPT=$P$G` when absent, converts ANSI to OEM, uppercases variable names,
and dynamically grows its temporary block.  It returns the required DOS
paragraph count in `BX` when the guest-supplied buffer is too small.

`cmdCreateVDMEnvironment` applies the same host-variable filters while
merging the separately parsed 16-bit AUTOEXEC environment; it grows the
buffer in 512-byte increments.  Neither source path imposes a 4 KiB policy
limit.

Direct compilation is not currently composable: the original translation unit
depends on COMMAND/SoftPC execution state (`GetVDMAddr`, register access and
historical service headers), and its result must enter the modern process only
through the checked bx-vdm guest-RAM transaction.  This is therefore a
step-(b) source-derived rehost under the governing recovery ladder, not new
COMMAND policy.

## Prior Failure And Correction

The former adapter context reserved 4,023 bytes because the old checked-write
payload was 4,096 bytes after a worst-case COMSPEC line.  A current normal
Windows environment measured 6,100 OEM bytes after the existing filters, so
capture returned false and the full current-source fixture observed install
detail `45` (`COMMAND_CONTEXT_CAPTURE`).  That is an adapter restriction, not
a COMMAND.COM or unavailable-Win32 limitation.

The repaired seam keeps the original filters and OEM conversion, removes the
per-entry 1 KiB conversion limit, and admits up to 65,462 bytes of filtered
environment.  The 73-byte reservation makes `COMSPEC` plus environment fit in
the existing 65,535-byte fixed-width mechanical guest-RAM transaction.  This
is the actual 16-bit transfer boundary; larger input fails explicitly rather
than truncating or inventing a whitelist.

## Verification

`tools/Invoke-T210S3HostContextFamilyProbe.ps1` source-builds with MSVC x64
`/MT` and exercises a 5,001-byte double-NUL OEM environment through the
existing `54:02` COMSPEC and `54:0F` environment request path.  It verifies
that the transaction carries the complete environment and no longer rejects
it at the former 4 KiB threshold.

The full-current T214 fixture is retained as the admission integration check.
After the repair it reaches CPU entry and returns the engine's controlled
guest-terminal disposition with detail `1`; this demonstrates that the former
context-capture rejection is gone, not that any COMMAND/BOP package or guest
workload is complete.

## Disposition

Retain this source-derived adapter seam.  Do not modify OpenNT `cmdenv.c`,
place COMMAND semantics in Bochs, add a host-variable policy whitelist, or
add a Bochs intrusion exception.  A later COMMAND package can add the
original AUTOEXEC merge path only after its whole-package admission.
