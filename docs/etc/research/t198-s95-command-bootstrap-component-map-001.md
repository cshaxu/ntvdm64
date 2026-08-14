# T198 S95 COMMAND bootstrap component map 001

## Question

Does the first source-built guest boundary `BOP 54:02` have an existing,
whole-command component that also owns `54:0F`, and can a later task attach
that component to the current `bx-vdm` composition without turning either
Bochs or the adapter into a general historical dispatcher?

## Inputs

- OpenNT COMMAND service constants and dispatcher:
  `src/opennt/base/mvdm/inc/cmdsvc.h` and
  `src/opennt/base/mvdm/dos/command/cmddisp.c`.
- Original guest callers:
  `src/opennt/base/mvdm/dos/v86/cmd/command/init.asm` and `rdata.asm`.
- Original host owners:
  `src/opennt/base/mvdm/dos/command/cmdmisc.c` and `cmdenv.c`.
- Current `bx-vdm` plane, retained bootstrap service, old runtime, current
  composition and composition manifest.
- S94's one exact-input observation, which stopped at `54:02` because the
  current composition declined it.  It is evidence of reachability only, not
  a guest-continuity result.

## Original OpenNT component

`cmdsvc.h` defines `SVC_CMDCOMSPEC` as service 2 and
`SVC_GETINITENVIRONMENT` as service 15.  `cmddisp.c` maps them respectively
to `cmdComSpec` and `cmdGetInitEnvironment`; they are not unrelated adapter
invented endpoints.

| BOP | Guest caller and input | Original host owner | Result and lifecycle |
| --- | --- | --- | --- |
| `54:02` | Permanent `COMMAND.COM` in `init.asm` places `RESGROUP:ComSpec` in `DS:DX`, calls the service, and stores returned `AL` in `SCS_PAUSE`. | `cmdComSpec` in `cmdmisc.c` only acts while `IsFirstCall`; it reads the NUL-terminated guest path, makes `COMSPEC=<path>`, records `cbComSpec`, and sets `AL` to `!fConOutput || VDMForWOW`. | It establishes the COMSPEC prefix used by the later environment handoff.  On later calls, original code returns without changing the registers. |
| `54:0F` | `rdata.asm` supplies a receiving buffer at `ES:0` and its capacity in paragraphs in `BX`.  A too-small result causes the guest to allocate a larger segment and call again. | `cmdGetInitEnvironment` in `cmdenv.c` only acts while `IsFirstCall`; it builds/caches a converted first environment, prefixes the `cmdComSpec` value, copies when capacity suffices, and returns the required paragraph count in `BX`; after a successful cached environment copy it frees the cached environment. | `BX=0` means no first environment is available.  Subsequent non-first calls explicitly return `BX=0`. |

The pair is therefore a single COMMAND bootstrap component with a shared
first-call lifetime.  Service 2 is classified in the current global COMMAND
plane as `LAUNCH`, while service 15 is classified as `ENVIRONMENT`; that
catalogue grouping does not erase their shared original bootstrap state.

## Historical implementation versus current runtime

An existing project-authored service,
`bx_ntvdm_cmd_comspec_bootstrap_service.{h,c}`, already represents a bounded
source-derived first-profile subset:

1. `54:02` asks the selector-blind mechanical layer to copy exactly 64 bytes
   from physical `DS:DX`; it rejects an unterminated input rather than doing
   an unbounded guest read.
2. Completion preserves `AH`, returns `AL=1`, and stores
   `COMSPEC=<guest-string>\0` in private component state.
3. `54:0F` returns the exact required paragraphs in `BX` if the guest buffer
   is too small; otherwise it writes that private buffer at `ES:0` through a
   checked multi-write transaction and returns `BX=0`.

The retained old `bx_ntvdm_adapter_runtime.c` initializes that state, queues
the read through its historical session, completes it, and dispatches the
environment write.  It is explicitly `availableButDetached` in
`tools/t198-s50-bx-vdm-composition-manifest.json`, and is not an allowed
runtime root.  The current `bx_ntvdm_boot_namespace_composition_v1` neither
contains the component state nor compiles
`bx_ntvdm_cmd_comspec_bootstrap_service.c`; after global ingress/classification
it only routes the separate command-launch provider.  Thus S94's `54:02`
decline is explained by source membership and composition, not by a missing
Bochs mechanism.

## Fidelity disposition

The retained service is **not** an original OpenNT provider compiled into the
new process.  Its full original owner depends on historical environment,
ANSI/OEM conversion, host console/WOW state, memory allocation and fatal NT
host behavior.  The contained first CLI profile must not expose its ambient
host environment merely to imitate that composition.

The only currently justified replacement is consequently a clearly labelled
source-derived, contained bootstrap provider: its generated environment is
only `COMSPEC=<guest string>\0`, with no inherited variables, `WINDIR`, host
console, WOW state, conversion policy, allocation or process-termination
behavior.  This is an explicit profile restriction, not a claim that it is
the full historical `cmdGetInitEnvironment` implementation.

Two existing helper details need correction before it can be admitted:

- once its `ready` flag is set, repeated `54:02` currently declines, whereas
  original `cmdComSpec` resumes with no register delta after `IsFirstCall`;
- after a successful `54:0F`, repeated service 15 currently republishes the
  retained buffer, whereas original post-first-call behavior is `BX=0`.

These are component-state fidelity defects, not reasons to add a new BOP
special case.  They belong in the one shared bootstrap state machine.

## Bounded implementation seam for S96

S96 may admit one `bx-vdm` COMMAND-bootstrap composition component only if it
does all of the following together:

1. retain the global BOP ingress/provider registry and require the existing
   COMMAND-plane classification for service 2 or 15 before dispatch;
2. own one private first-profile state object in the current composition,
   with explicit pre-COMSPEC, COMSPEC-ready and environment-consumed states;
3. perform the existing fixed 64-byte read and the existing checked
   multi-write through the selector-blind mantle mechanical-action ABI;
4. reproduce the original repeat outcomes within the restricted profile
   (no-op resume for later 2, `BX=0` resume for later 15);
5. document and test the contained COMSPEC-only environment disposition;
6. leave every other COMMAND endpoint deferred/unavailable exactly as the
   global plane records it.

This is a `bx-vdm` composition repair only.  It does not require a Bochs
intrusion, a mantle change, a DOS filesystem implementation, host environment
exposure, or a native guest run in S96.

## Interpretation and follow-up

Confidence is high: original declaration, dispatcher, guest callers and host
owners agree on the pair; the current manifest and source membership directly
explain the detached route.  S95 is closed as a source map.  The next task is
S96, a single component-level repair and focused source-built composition
test; only a subsequent separately admitted task may rebuild/run the guest.
