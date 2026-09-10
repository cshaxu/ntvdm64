# M0 T384 S1 — COMMAND environment/resident-overwrite ledger

## Question

Is CPU40 missing support for opcode `0x63`, or did the fixed pure-DOS
observation enter the original CPU40 invalid-opcode path after earlier guest
state corruption?

## Inputs

- Fixed observation: `O:\ntvdm64\observation-t383-s4-exit.txt`, from the
  CPU40/x86 product SHA-256
  `6df5e9767fa7e383275bf63eab3f8398634ff94a344cb19e7a80e12140722271`.
- Selected immutable guest source and media:
  `src/mvdm-guest/dos/v86/cmd/command/rdata.asm` and `COMMAND.COM`, each
  byte-identical to the selected OpenNT source/media.  The `rdata.asm`
  SHA-256 is
  `350cb74da4d43313cf7ce0f9bdb957cefa5759ef32f439f8a83102f0162ed441`;
  `COMMAND.COM` is
  `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43`.
- Original CPU40 and SoftPC sources:
  `softpc.new/base/ccpu386/c_main.c`,
  `softpc.new/base/system/illegalp.c`, and
  `softpc.new/base/bios/bios.c`.
- Current source-shaped environment binding:
  `src/mvdm-host/dos/command/cmdenv.c` and
  `src/adapter-mvdm-host-out/softpc/mvdm_guest_location.c`.

## Static source chain

1. The guest's original `rdata.asm::adjust_env` allocates an environment
   block through `INT 21h/ALLOC`, stores the returned segment in `EnvirSeg`
   and the COMMAND PSP, loads it into `ES`, then issues `CMDSVC
   SVC_GETINITENVIRONMENT`.  If `BX` reports insufficient capacity it frees,
   reallocates and repeats exactly the same sequence.  Thus two `54:0F`
   returns are the expected sizing handshake, not a COMMAND service retry
   defect.
2. `CMDSVC` expands only to `BOP BOP_CMD` followed by the service byte.  It
   does not alter `ES`.  Original `cmdGetInitEnvironment` defines its
   destination as `ES:0`; the current mirror retains that shape and uses only
   the registered synchronous mapping lease instead of the historical native
   `GetVDMAddr` alias.
3. The current fixed observation reached two successful `54:0F` returns.  It
   then faulted at `CS:03f4 IP:2128` with bytes `63 6f 64 65 78` (`codex`).
   The original COMMAND map places `0:2128` inside the relocated resident
   `RESGROUP` data range, adjacent to resident COMMAND startup data.  The
   selected immutable `COMMAND.COM` does not contain `codex` at that file
   location.
4. The inherited host environment demonstrably contains multiple `CODEX_*`
   variables and `codex` path fragments.  Therefore the observed bytes are
   host-environment-derived guest-memory content, not an original COMMAND
   opcode stream.  The only reached original path that copies that host
   environment before the fault is `54:0F` into its caller-selected `ES:0`
   environment block.
5. CPU40's original decoder treats opcode `0x63` (`ARPL`) in real mode as an
   `Int6()` condition.  `bios.c` maps BOP 06 to `illegal_op_int`, which reads
   the original interrupt frame and calls the original `host_error` path.
   CPU40 therefore reports the corrupted resident bytes correctly; it is not
   the producer of those bytes and no ARPL implementation is missing.

## Disposition

The evidence establishes a source-backed **pre-CPU40 guest-memory ownership
failure**: the original COMMAND environment destination must not alias the
resident COMMAND segment, but the fixed observation proves that resident code
was overwritten with inherited host-environment text before the first
`54:01`.

The exact allocator return value (`ES`) is not emitted by the existing fixed
observer.  It is nevertheless bounded by the original caller contract above:
the next owner must audit the original DOS `INT 21h/ALLOC` / arena state and
the SoftPC-backed guest-memory initialization which supplies it.  It must
prove why the environment allocation selected a resident alias (or another
equivalent pre-write corruption), then restore that original DOS/SoftPC memory
precondition.  It must not change `cmdGetInitEnvironment`, suppress host
environment content, skip `54:0F`, or turn `illegal_op_int` into Ignore.

## Confidence and follow-up

High confidence that CPU40's `0x63` result is downstream and correct; high
confidence that the bytes are host-environment-derived corruption; medium
confidence on the precise allocator/state edge because the current report
lacks the BOP-time `ES` snapshot.  T384's CPU40-only S2 boundary is therefore
not valid.  A newly admitted pure-DOS memory/arena startup task must own the
repair, with a default-off `54:0F` register snapshot permitted only as a
post-repair observation aid.
