# T95 S7 COMMAND Bootstrap And User-Frame Source Audit 001

## Question

Does the source-built runtime trace stop because the CLI/COMMAND composition
is missing, because the reached read-only BOP service has the wrong result, or
because the DOS user-frame restoration is already invalid?

## Inputs

- Immutable runtime observation:
  `artifacts/analysis/t95-s7-runtime-trace-023-20260811-001/bochs.log`
  (source-built NTDOS SHA-256
  `CB8C595AA7C0F1AA1EC68827E37B6B53AFD249BE1E12D38B4E6D1BE5E9AB5C7E`).
- Source-built map:
  `artifacts/toolchain-runs/ntdos-tools16-historical-v1/base/mvdm/dos/v86/doskrnl/dos/NTDOS.MAP`.
- OpenNT sources: `base/mvdm/dos/command/cmddisp.c`, `cmdmisc.c`,
  `base/mvdm/dos/v86/cmd/command/{tcode,tmisc1,init,rdata}.asm`,
  `base/mvdm/dos/dem/demhndl.c`, and
  `base/mvdm/dos/v86/doskrnl/dos/{handle,msdisp,mscode}.asm`.

## Procedure

1. Correlate every reached selector `54` observation with the COMMAND service
   table and the corresponding guest call site.
2. Correlate the final `50:12`, `50:42`, and `50:02` sequence with its NTDOS
   symbols and the original DEM result contracts.
3. Follow the NTDOS return path from `SYS_RET_OK` to `LeaveDos`, identifying
   the instruction which actually restores the caller's stack.
4. Keep unreached COMMAND services and optional Bochs diagnostics separate
   from the cause of this stop.

## Observations

### The trace has entered COMMAND.COM initialization, but not command intake

The trace reaches selector `54` services `05`, `0C`, `02`, `0D`, and `0F`.
The first three configuration calls correspond to COMMAND's original
set-information, CONFIG.SYS path, COMSPEC, AUTOEXEC.BAT path, and initial
environment requests.  In particular, the trace reaches `54:02` from
COMMAND.COM and then `54:0D` and `54:0F`.

It never reaches `54:01` (`SVC_CMDGETNEXTCMD`).  `cmddisp.c` maps that service
to `cmdGetNextCmd`; `tcode.asm` invokes it only from COMMAND's command loop.
The original `cmdGetNextCmd` calls NT host/UI facilities such as
`GetNextVDMCommand`, manages current directories and lifecycle blocking, and
on its first call consumes the already-created COMMAND environment.  That is
the later CLI/composition replacement boundary: a CLI-selected initial
command must eventually supply the source-shaped result for this one service.
It cannot explain a stop which happens before `54:01` is observed.

### The final zero-byte read is a source-defined success outcome

The map identifies the final sequence as `$OPEN`/`$READ`/`$CLOSE`; the reached
instruction sites are the existing NTDOS BOP call sites.  At the final
`50:42`, the trace requests `CX=0020` and commits zero payload bytes.  Original
`demRead` defines success as `CF=0` and `AX=bytes read`, including zero at
EOF.  `handle.asm` then tests that count and continues its normal read path.
The zero-byte payload and a successful `50:02` close are therefore not
themselves errors.  However, r24's `50:42` result incorrectly wrote a broad
general-register delta, including BP; the source-defined read result is AX
and CF only.  That adapter ABI correction is recorded in
`t95-s7-demread-register-contract-001.md` and must be re-observed before the
remaining stack-fault provenance is classified.

The final close at `$CLOSE` resumes at `61A7` without a general-register
delta.  This agrees with `demClose`: success changes only CF.  The earlier
close-register correction therefore remains valid.

### The recorded CPU dump is already in exception escalation

The subsequent fault is a real-mode stack prefetch failure with
`SS=00A7`, `ESP=0003`, followed by Bochs's unchanged triple-fault handling.
The final dump shows `CS:EIP=0000:019D` (`push es`), but that address is not
evidence of the normal DOS continuation: it is recorded while Bochs is
escalating an exception.  In real mode, `real_mode_int` pushes FLAGS, CS, and
IP before loading the IVT target.  With a 16-bit SP of `0003`, the first word
can use offset `0001`, while the following word wraps to `FFFF`; the original
`stackPrefetch` range check rejects that two-byte access.  Attempting to
deliver `#SS`, then `#DF`, repeats the same stack-frame failure and produces
the observed third-exception shutdown.  Thus `0000:019D` can be an exception
vector target rather than the instruction which made SP invalid.

`msdisp.asm` captures the caller's `User_SP` and `User_SS` at the normal DOS
system-call entry, switches to `DskStack` for the reached non-1--12 calls, and
later `LeaveDos` executes `mov SS,User_SS` followed by `mov SP,User_SP` before
popping the caller frame.  `SYS_RET_OK` does not load SS:SP; it only accesses
the saved user frame to update its carry and AX fields.  `LeaveDos` is
therefore a source-supported candidate for restoring a bad value, but r24 does
not prove that it was the last writer.  The concrete bad value must predate
the first exception or external-interrupt delivery which tried to build a
frame, not merely the last BOP response; it is still not evidence for an
additional BOP implementation.

## Interpretation

Confidence is high for the negative classification:

| Candidate explanation | Classification | Evidence |
| --- | --- | --- |
| Missing CLI first-command service | Deferred, unreached | `54:01` is absent from the trace and is called only by COMMAND's later loop. |
| Incorrect final DEM read GPR mask | Corrected; runtime re-observation required | r24's `delta=2f` writes BP despite `demRead` defining AX and CF only. |
| Incorrect final DEM close ABI | Rejected for this stop | Original contract permits success-only CF; r24 close has `delta=00`. |
| Missing DOS filesystem semantic | Not established | The trace uses the bounded read-only projection successfully through the final close. |
| Invalid caller stack before exception/interrupt delivery | Active cause category | `ESP=0003` makes the native real-mode frame push wrap to `FFFF`; the original trigger and writer remain unobserved. |
| `LeaveDos` restoration | Plausible but unproven writer | It explicitly loads `User_SS:User_SP`; the retained trace has no instruction-level proof that it was last. |
| x87/IRQ13 or `0xA1800` mapping | Separate hypothesis | This retained r24 tail does not establish either as the first delivery cause or the stack writer. |

The current evidence does **not** authorize a stack synthesis, a new BOP
handler, a COMMAND `54:01` implementation, enabling a device, or moving DOS
semantics into Bochs or the adapter.

## Next Evidence Gate

The next investigation must identify the first exception or external interrupt
that tries to build a frame with `SP=0003`, then identify the last guest or
native mechanism which set that SP.  The already registered, default-off x87
compatibility and IRQ13-transfer diagnostics are eligible only if an admitted
same-configuration build can enable them; they observe their original Bochs
owners and introduce no adapter semantics.  Otherwise the observation must
reuse an original same-configuration facility or obtain a separately admitted
object closure; the rejected r27/r28 CPU-loop instrumentation may not be
revived.  Only after that provenance is known may a Bochs-native prerequisite
or a composition change be proposed.

The BOP listener stays enabled only as its existing default-off generic
observation seam.  It will continue to catalog and log every reached BOP; an
unimplemented selector/service continues to fall through unchanged until a
separately admitted source contract exists.
