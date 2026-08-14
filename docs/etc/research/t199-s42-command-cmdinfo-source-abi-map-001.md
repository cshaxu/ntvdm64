# T199 S42: COMMAND CMDINFO source and ABI map

## Question

What complete `54:01` CMDINFO contract is constructed by the OpenNT 16-bit
COMMAND caller, consumed by the original host provider, and available for a
contained CLI provider without importing the historical NT host shell?

## Historical sources and method

This map reads the pinned OpenNT tree only:

| Source | Role |
| --- | --- |
| `base/mvdm/inc/cmdsvc.h` | packed `CMDINFO` wire declaration and `54` service identities |
| `base/mvdm/dos/v86/cmd/command/tspc.asm` | resident/transient 16-bit record storage, in declaration order |
| `base/mvdm/dos/v86/cmd/command/tcode.asm` | real-mode caller construction and carry retry loop |
| `base/mvdm/dos/v86/cmd/command/comequ.asm` | `COMBUFLEN=128` and `EXECPATHLEN=273` |
| `base/mvdm/dos/command/cmdmisc.c` | `cmdGetNextCmd` and `cmdSetInfo` host semantics |
| `base/mvdm/dos/command/cmdenv.c` | prior `54:0F` initial-environment producer |
| `base/mvdm/dos/command/cmdexec.c` | `54:0B` return/next-command lifecycle |

No binary was imported and no historical object is linked by this map.

## Caller construction and fixed layout

`tcode.asm` executes the following sequence before `CMDSVC
SVC_CMDGETNEXTCMD`:

1. obtains the environment segment and byte count, storing `ENV_PTR_SEG` and
   `ENV_SIZE`;
2. obtains the current DOS drive and stores `SCS_CUR_DRIVE`;
3. sets `CMD_SIZE` to `COMBUFLEN` (`128`), points `CMD_PTR_SEG:CMD_PTR_OFF`
   at `UCOMBUF`, and sets `EXECPATH_SEG:EXECPATH_OFF:EXECPATH_SIZE` to the
   transient `EXECPATH` buffer and `273`;
4. copies the resident redirector completion pointer into `SCS_RDRINFO`; and
5. sets `DX` to `ENV_PTR_SEG`, retaining transient `DS`, then invokes `54:01`.

`tspc.asm` declares the record in exact `cmdsvc.h` order.  It is 36 bytes,
packed on two-byte alignment:

| Offset | Field | Caller role | Original `cmdGetNextCmd` role | CLI provider disposition |
| ---: | --- | --- | --- | --- |
| 0 | `EnvSeg` | input | initial environment base; later output target | checked input and contained environment output |
| 2 | `EnvSize` | input capacity | initial environment size; retry/output size | checked input; CF/AX retry when contained response exceeds capacity |
| 4 | `CurDrive` | input | return current directory / output drive | declared CLI drive result |
| 6 | `NumDrives` | input storage | output drive count | admitted-drive count |
| 8 | `CmdLineSeg` | input | target command-tail buffer | checked output target |
| 10 | `CmdLineOff` | input | target command-tail buffer | checked output target |
| 12 | `CmdLineSize` | input capacity (`128`) | command-tail capacity | caller-defined capacity and bounded CLI response |
| 14 | `ReturnCode` | input | terminated-program status | recorded terminal input; no ambient host execution policy |
| 16 | `bStdHandles` | output storage | redirection result | explicit no-redirection result |
| 18 | `pRdrInfo` | input/output storage | host redirection completion pointer | explicit null/no-redirection result; never a host pointer |
| 22 | `CodePage` | output storage | mapped code page | declared CLI code-page result |
| 24 | `fTSRExit` | input | console exit notification | explicit unsupported/deferred CLI console disposition |
| 26 | `fBatStatus` | output storage | batch status | declared non-batch result |
| 28 | `ExecPathSeg` | input | target executable-path buffer | checked output target |
| 30 | `ExecPathOff` | input | target executable-path buffer | checked output target |
| 32 | `ExecPathSize` | input capacity (`273`) | executable-path capacity/output size | caller-defined capacity and bounded CLI response |
| 34 | `ExecExtType` | output storage | executable extension classification | source constants `COM=8`, `EXE=4`, `BAT=2`, otherwise `9` |

The initial environment is a package prerequisite, not an optional cosmetic
detail: `cmdGetNextCmd` explicitly states the first call follows
`cmdGetInitEnvironment`.  Original `54:0F` uses `ES:0` and `BX` paragraphs,
returning a required paragraph count in BX when the buffer is too small.  The
contained profile must therefore preserve its own captured environment bytes
and cross-check CMDINFO's `EnvSeg:EnvSize` before first command delivery.

## Original order and failure behavior

The original provider reads CMDINFO from `GetVDMAddr(DS, DX)` before using
`ReturnCode`, executable path, first-call environment, current drive,
redirector info and TSR state.  It returns CF set with AX equal to the needed
environment byte count when the response environment will not fit; `tcode.asm`
then reallocates the DOS environment and re-enters `54:01`.  On successful
delivery it writes command tail, executable-path byte count/extension,
environment when newly constructed, batch/drive/count/code page, standard
handle/redirection result, and SCS-to-sync only if `54:05` has supplied that
separate DOSDATA locator.

`cmdSetInfo` does **not** define CMDINFO.  It separately registers the
SCS-to-sync, DOS-binary and FD-access locations.  A valid `54:01` before a
registration must not manufacture writes to address zero; a valid registration
adds only its two documented post-delivery bytes.

The historical source relies on SAS/CCPU pointer conversion and live Win32
process/console/redirection state. Those are not admitted into the CLI
process.  S42 consequently preserves ordering, field widths, declared retry
and no-redirection outcomes through fixed-width copied ranges; it does not
link `cmdmisc.c` or import a historical host pointer.

## Implementation consequences

The existing adapter already encodes the 36-byte wire record and the two
caller capacities.  It is incomplete because it treats only a narrow launch
response as valid and does not model CMDINFO environment input/output,
first-call state, all field results, or a source-derived contained failure.
The package implementation must:

1. decode every field and validate all segment/offset ranges before any
   mutation;
2. preserve a CLI-owned initial environment from `54:0F` through first
   `54:01`, and implement the original CF/AX size retry shape;
3. deliver command/executable bytes and all admitted CMDINFO outputs as one
   checked multi-write transaction;
4. keep redirection, live console, process environment and host pointers as
   explicit no-redirection/deferred capability dispositions; and
5. cover valid caller-shaped, environment-retry, malformed/aperture,
   registration-present and registration-absent, return and terminal paths in
   one source-built COMMAND regression.

## Confidence and follow-up

Confidence is high for layout, caller order and capacity values because each
is directly stated by matching 16-bit and host source.  The precise live
native values at S41's frontier are not observed; no claim is made that a
particular field caused it.  The S42 implementation and focused regressions
below close this CMDINFO subpackage only; S43 audits the complete COMMAND
service family without a native trace.

## Implemented contained transaction and verification

The adapter now carries the byte count successfully delivered by `54:0F` into
the first `54:01` transaction.  If CMDINFO's `EnvSize` is smaller, it resumes
with CF set and AX equal to the required byte count, before publishing any
guest writes—the same retry shape used by `tcode.asm`.  When capacity is
sufficient it validates the `EnvSeg:0` range as a checked copied range, while
leaving environment ownership with the prior `54:0F` provider.  The command
and executable destinations remain separate from that validation; this avoids
accidentally writing a command response into the environment segment.

Fresh MSVC x64 `/MT` witnesses are:

- `artifacts/build/t199-s42-command-cmdinfo-r3/`: CMDINFO delivery, terminal
  lifecycle, absent-registration handling, CF/AX environment retry, and
  aperture-crossing environment refusal with no published transaction;
- `artifacts/build/t199-s42-command-session-r2/`: bound composition path
  through COMSPEC, `54:0F`, a caller-shaped CMDINFO record and `54:01`;
  and
- `artifacts/build/t199-s42-adapter-runtime-compile-r2/`: the retained legacy
  adapter-runtime call site compiles with the changed fixed-width signature.
  Its strict `/WX` compile still reports three pre-existing unrelated warnings
  (two shadowed `index` locals and an uninitialized `registration`); the
  non-`/WX` object was emitted solely to verify this signature propagation.

No guest execution was run in S42.  This is CMDINFO-package evidence, not a
claim that the whole COMMAND component or the S41 native frontier is closed.
