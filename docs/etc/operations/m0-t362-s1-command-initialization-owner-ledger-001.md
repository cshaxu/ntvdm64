# M0 T362 S1 — Original COMMAND initialization owner ledger

## Question

What complete original owner path lies between the valid CPU40/x86 observation
of `SVC_CMDCOMSPEC` (`54:05`) and its first expected
`SVC_CMDGETNEXTCMD` (`54:01`), and which immediate source-shaped cohort must
be recovered before another workload observation?

## Inputs

- Selected guest `mvdm-guest/dos/v86/cmd/command/{init.asm,rdata.asm,tcode.asm}`.
- Selected MVDM host `mvdm-host/dos/command/{cmddisp.c,cmdmisc.c,cmdenv.c,cmddata.c}`.
- Selected `mvdm-host/inc/{cmdsvc.h,softpc.h}` and the current
  `adapter-mvdm-host-out/{softpc,basesrv}` bindings.
- T358 S13's valid fixed-root observation and T359/T360 closure evidence.

## Original control-flow and ABI ledger

1. `init.asm` enters the permanent COMMAND-only branch with `DS=RESGROUP`,
   places the original 64-byte `RESGROUP:ComSpec` address in `DX`, emits
   `CMDSVC SVC_CMDCOMSPEC`, then stores returned `AL` in `SCS_PAUSE`.
   `rdata.asm` owns both the 64-byte `ComSpec` field and `SCS_PAUSE`.
2. `cmddisp.c` maps selector 2 to original `cmdComSpec` in `cmdmisc.c`.
   The function is active only while original `IsFirstCall` is true. It
   prefixes `COMSPEC=`, copies the guest NUL-terminated path, sets
   `cbComSpec`, and returns the existing-console disposition in `AL`.
3. Guest initialization continues as original resident/transient setup. At
   `rdata.asm` it emits `SVC_GETINITENVIRONMENT` (selector 15) while `ES`
   identifies the newly allocated environment. Original `cmdenv.c` combines
   the initial host environment with the `lpszComSpec`/`cbComSpec` state,
   reports an insufficient paragraph count in `BX`, or copies the completed
   environment to guest memory.
4. The transient `tcode.asm` finally constructs `CMDINFO` and emits
   `SVC_CMDGETNEXTCMD` (selector 1). Original `cmdGetNextCmd` obtains the
   prepared environment and makes the first `GetNextVDMCommand` request with
   `ASKING_FOR_FIRST_COMMAND`; the selected same-shaped Base VDM client/local
   broker owns its copied request/result record.
5. Clear carry from selector 1 reaches guest `run_cmd`. For the declared
   `/C EXIT` workload, the complete subsequent terminal remains original
   guest `$EXIT` -> selector 0 -> `cmdExitVDM` -> source-shaped controlled
   session stop. That terminal is not reached by T358 S13 and is not claimed
   here.

## Current binding and gap classification

The selected original function bodies are already formal-link inputs. The
Base VDM record and command-delivery owner are connected; no BaseSrv/CSR
server import is the immediate gap.

However, the original x86 `GetVDMAddr` macro in `softpc.h` converts `seg:off`
to a low linear numeric address and the three first-call functions cast it to
a native pointer:

- `cmdComSpec` reads `DS:DX` with `strcpy`;
- `cmdGetInitEnvironment` writes `ES:0` with `strncpy`/`memcpy`;
- first-call `cmdGetNextCmd` reads the `CMDINFO`, environment and transient
  buffers through the same form.

That is an original NT4 process-address alias assumption. It is not a safe
project cross-boundary contract even in the current x86 profile: the mapping
manager is the required owner of checked guest ranges. T358 S13 does not
prove that this assumption caused its timeout; it only proves the first
`54:01` was not observed. The static gap is nevertheless real and immediate.

## Recovery ladder and S2 decision

1. **Direct original source:** selected `cmdmisc.c`/`cmdenv.c` bodies are
   already present and selected; their direct low-address aliases cannot be
   retained under the project mapping rule.
2. **Smallest same-shaped binding:** reuse the existing
   `mvdm_guest_location` plus session mapping-manager lease. This retains the
   original function names, order, fields, `AL`/`BX`/carry outcomes and the
   original `lpszComSpec`/`cbComSpec` state; it changes only synchronous guest
   read/write materialization.
3. **External intrusion:** not applicable; no SoftPC/guest/controller change
   is needed.
4. **New behavior:** rejected; no new command/environment policy is needed.

S2 is therefore the whole **COMMAND first-call guest-memory access cohort**:
`cmdComSpec`, `cmdGetInitEnvironment`, and the first-call `cmdGetNextCmd`
accesses, with one shared mapping-manager lease discipline and focused
positive/negative behavior tests. It is not a `54:05` leaf repair. It must
not touch later EXEC, BaseSrv/CSRSS, DOS child or device semantics.

## Confidence and follow-up

High confidence for source ownership and the static low-address assumption:
both selected external baselines contain the same `GetVDMAddr` definition,
and all three selected functions use it in the reached first-call path.
Runtime causality remains unproven. S2 must preserve original failure
direction for an unavailable/malformed guest range and formally link before a
single frozen observation is admitted.
