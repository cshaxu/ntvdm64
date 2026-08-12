# T95 S6 guest artifact placement contract 001

## Question

What is the smallest source-shaped contract that can make the already
identity-verified `COMMAND.COM` payload reachable by the OpenNT DOS guest,
without making the BYOB root a DOS filesystem or adding a COMMAND-specific
BOP loader?

## Inputs and procedure

This is a read-only correlation of:

- `base/mvdm/dos/v86/doskrnl/bios/sysinit2.asm:1547-1548`, which defines the
  kernel's initial `commnd` buffer as `\\COMMAND.COM`;
- `base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm:1468-1603`, which opens that
  filename, sizes it, then invokes the ordinary DOS `EXEC` path;
- `base/mvdm/dos/v86/cmd/command/init.asm:1121-1128` and
  `base/mvdm/dos/command/cmdmisc.c:635-650`, which establish that `54:02`
  reports an already-resident guest COMSPEC rather than locating the image;
- the current BYOB profile/session implementation in
  `src/cli/byob_profile.[ch]` and
  `src/bx-ntvdm-adapter/bx_ntvdm_startup_session.[ch]`; and
- the current runner/adapter resource ABI record
  `adapter-session-resource-abi-v1.md`.

No source, profile, payload, guest memory, Bochs configuration, or adapter
dispatch was changed or executed.

## Observations

1. The DOS kernel declares its initial interpreter pathname as the
   drive-relative absolute guest name `\\COMMAND.COM`; it is not an NT path,
   a PIF name, or a BOP argument.
2. After CONFIG processing, `sysinit1.asm` performs the normal DOS open,
   seek, close, and `EXEC` sequence on that name.  An open failure takes the
   `comerr` path and then issues `SVC_DEMEXITVDM`.  This is an ordinary guest
   file/loader dependency, not a recoverable COMMAND service call.
3. `SHELL=` may replace the default name, but the historical materializer's
   replacement is itself a guest file path that must be visible to that same
   DOS filesystem.  It cannot be satisfied by a host pathname copied into
   guest RAM.
4. Current BYOB v1 has only canonical host file names, byte counts and
   digests for NTIO, NTDOS and COMMAND.  Its selection and startup session
   load the three payloads into adapter-owned buffers, but have no guest-path
   field, guest drive, placement map, guest file record, or DOS open service.
   The v1 session-resource ABI deliberately contains no path or payload.
5. The existing `--byob-root` is an immutable local identity authority.
   Treating it as a DOS root would permit guest path interpretation and
   arbitrary later host reopening, contradicting its declared boundary.

## Minimal contract

The first profile needs a new, versioned **guest artifact placement**
declaration before any CONFIG, AUTOEXEC, or read-handle implementation.  It
is an adapter-owned guest-composition input, not a Bochs setting and not a
host-drive capability.  Its minimum record is:

| Field | Constraint |
| --- | --- |
| resource role | Must name an already CLI-identity-verified payload; initially `command` only. |
| guest path | Canonical DOS absolute path, initially exactly `\\COMMAND.COM`; no host path, `..`, wildcard, device, UNC, or relative form. |
| guest drive binding | Must bind the path to the declared initial DOS drive; it is not inferred from the host current drive or BYOB root. |
| byte source | The immutable adapter-owned bytes loaded from the verified descriptor; no path reopen after session admission. |
| lifetime | Read-only for one adapter session; disposed with that session. |
| absence/failure | Missing, malformed, conflicting, or unavailable placement rejects the selected profile before guest execution. |

The eventual DOS Open/Seek/Read/Close projection may expose this one
placement as a read-only file record.  It must use the same canonical guest
namespace and opaque-token rules as all later projected files; it may not add
a parallel `COMMAND` lookup fast path.  The initial placement is therefore a
closed input to that future capability, not an authorization to implement a
file BOP today.

## Ownership and exclusions

| Owner | Responsibility | Explicit exclusion |
| --- | --- | --- |
| CLI/profile admission | Select and validate the immutable placement declaration together with the resource identity. | Guest-memory writes, DOS path resolution, payload loading into Bochs. |
| Adapter guest composition | Retain the verified bytes and the canonical guest placement as session state; later provide it through the admitted read-only projection. | Ambient host file lookup, PIF/SystemRoot policy, CPU/device behavior. |
| OpenNT guest | Performs its original DOS open/seek/read/EXEC flow and owns the `SHELL=` interpretation. | Knowledge of BYOB roots or host handles. |
| Bochs | Supplies only ordinary RAM/CPU mechanics for a later generic transaction. | Paths, BOP selectors, payload identity, DOS files, or COMMAND semantics. |

Neither `54:02`, `54:0C`, `54:0D`, nor a new Bochs exception case belongs in
this contract.  `54:02` remains listener-only until a runnable COMMAND path
is reached; it is downstream of placement.  The historic host `CONFIG.NT`,
`AUTOEXEC.NT`, PIF, SystemRoot, locale and temp-file behavior remain excluded
until S7 specifies an explicit CLI-selected configuration profile.

## Interpretation and confidence

The source evidence proves the required *guest name* and the ordinary DOS
consumer path.  It does not prove that current ntdos64 has a guest filesystem
implementation capable of honoring it.  Thus this record closes the
placement design input but not its implementation or runtime reachability.

Choosing the initial path `\\COMMAND.COM` preserves the kernel default.  A
future profile may admit another canonical path only alongside a complete
source-derived `SHELL=` and namespace contract; it must not silently change
the default profile.

## Follow-up

Admit S7 only after this declaration is represented in a new profile/schema
revision with negative validation tests.  Then close the one coherent
read-only Open/Seek/Read/Close capability for both the COMMAND placement and
explicit CONFIG/AUTOEXEC materializations.  Its first runtime trace must
prove the guest's normal file operation; it must not special-case COMMAND or
resume an unimplemented BOP.
