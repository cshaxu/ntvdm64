# M0 T358 S7 — fixed `/C EXIT` lifecycle observation

## Question

After the S6 single-session Base VDM DOS-record recovery is formally linked,
does the fixed original `--command EXIT` workload reach an original COMMAND
completion marker, or does it expose an earlier source-owned predecessor?

## Fixed inputs

- Formal CPU40/x86 product:
  `build/M0-T359/S2/formal-x86/original-softpc-process.exe`
- Product SHA-256:
  `de71a87fb1fc1e582dc68e115864520fdaddb4c0010c95f6719d81fed9e394b3`
- Fixed SystemRoot package: `O:\\ntvdm64` (10 visible characters).
- Stage manifest SHA-256:
  `d58a396332401eedc0e482331f5bb1225290d7e398ba5dcf1370ad84b14abdaa`.
- Fixed-media manifest SHA-256:
  `43f09b928f459fbca4c26a6cd6a24f08faf23e74c5ea3a7c291c955828de1cfb`.

The observer verified all 16 immutable media hashes before replacing only the
declared mutable product slot.  The pre-existing `mvdm` subtree was not an
input.

## Procedure

One console-owning, non-debug observation was run with the fixed product,
root, `--command EXIT` declaration and eight-second timeout:

```text
node tools/observation/ObserveSoftpcStartup.mjs \
  --launcher build/tools/console-startup-observer.exe \
  --product build/M0-T359/S2/formal-x86/original-softpc-process.exe \
  --stage O:\\ntvdm64 \
  --report O:\\ntvdm64\\observation-t358-s7-fixed-exit.txt
```

The first attempted invocation used Windows-style paths through Git Bash and
failed its pre-launch existence check.  It did not start the observer or
product and is not an observation.  The command above is the sole launched
observation.

## Observation

The root layout was accepted by original consumers.  The reached DEM opens
include:

```text
O:\\NTVDM64\\SYSTEM32\\HIMEM.SYS
O:\\NTVDM64\\SYSTEM32\\COUNTRY.SYS
O:\\NTVDM64\\SYSTEM32\\COMMAND.COM
```

The source-neutral BOP record contains two successful original `54:0F`
returns:

```text
MVDM-BOP-RETURN 54:0F cs=03F4 ip=03AA ax=049F cf=0
MVDM-BOP-RETURN 54:0F cs=03F4 ip=03CF ax=049F cf=0
```

No `54:01` request occurs before the fixed timeout.  The stopped guest frame
is `CS:03F4 IP:1F8F`; the observer records fetched bytes
`63 61 6c 2f 54` there.

## Source attribution

`mvdm-host/dos/command/cmddisp.c` maps `54:0F` to the original
`cmdenv.c::cmdGetInitEnvironment`.  Original guest
`mvdm-guest/dos/v86/cmd/command/rdata.asm` invokes that service once after
allocating the replacement environment and a second time after reallocating
when the first returned paragraph count is larger.  A failed allocation or a
second returned size different from `EnvSiz` follows the original
`init_env_err -> nomem_err -> Alloc_error` path.

The selected original `command.map` places `Alloc_error` at `0000:1F8D`.
The stopped IP is two bytes into that source-defined non-returning error
entry after its resident relocation to `03F4`.  Its observed bytes are not a
completion marker and do not establish whether the predecessor is the
paragraph count, the NTDOS allocation carry/error state, or a later resident
memory overwrite.

## Interpretation and follow-up

S6's Base VDM return record is not reached by this run: the first `54:01`
cannot occur until the original guest environment setup completes.  The
SystemRoot layout is also not the blocker.  The exact next owner cohort is
the original COMMAND initial-environment delivery and NTDOS environment
allocation path: `cmdenv.c::cmdGetInitEnvironment`, its `ES:0`/`BX`
contract, and guest `rdata.asm`'s two-call allocation continuation.

T358 is therefore paused after S7.  A later bounded S must first source-map
that entire cohort and add only state-neutral evidence for the returned `BX`,
`ES`, allocation result and resident-code integrity before proposing any
recovery.  It must not alter guest media, fabricate `54:01`, or treat this as
a BaseSrv, path-layout, or BOP-leaf repair.
