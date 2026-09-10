# T225 S45 — Source-Built Guest Parent/Child/Return Fixture Asset And ABI Map 001

## Question

Which existing original OpenNT guest artifact can exercise the guest-owned
`EXEC -> child entry -> INT 21h/4Ch -> parent return` path without inventing a
test program, changing guest bytes, invoking a host child process, or requiring
an unadmitted Bochs device?

## Source-first asset decision

The selected child is the existing OpenNT `SHARE.EXE` artifact, profile-named
`TARGET.EXE` only through an identity-preserving target placement:

| Property | Selected value |
| --- | --- |
| Original source | `src/opennt/base/mvdm/dos/v86/cmd/share/share.asm` |
| Original behavior | `org 100h`; `mov ah,4ch`; `xor al,al`; `int 21h`; `ret` |
| Original purpose | OpenNT source comment: an NT stub that does nothing |
| Source-built identity | 882 bytes, SHA-256 `69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc` |
| Reproducible source-build role | `build/M0-T225-S8/share-primary-source-build-r1/base/mvdm/dos/v86/cmd/share/SHARE.EXE`, verified by `share-build-manifest.json` |
| Guest placement | Exact byte alias at declared `\\TARGET.EXE` on the command drive; no byte alteration |

The source has no file, directory, registry, device, console, network or host
process action. Its sole meaningful execution effect is the original guest
DOS termination interrupt with exit code zero. It is therefore a better
fixture child than a TSR or a synthetic marker program: it reaches the
ordinary NTDOS termination sequence without adding another guest-service
family.

`src/opennt/base/mvdm/v86/util/quit.asm` is rejected. It invokes
`BOP_UNSIMULATE`, whose purpose is VDM termination rather than ordinary guest
child return, so it would collapse the required distinction between child
`INT 21h/4Ch`, COMMAND `54:0B`, and top-level exit. No new guest source is
authorized or needed.

## Provenance and artifact handling

T225 S8 records the four original-toolchain-built identities for NTIO, NTDOS,
COMMAND and SHARE. `Stage-T225S10PrimaryBootstrapInputs.ps1` already proves
that `SHARE.EXE` can be a byte-identical `TARGET.EXE` profile component, but
its emitted manifest limits that staging root to the S10 bootstrap observation.
That root must not be silently reused for this new fixture.

The later implementation S must create a fresh disposable fixture input root
from the four primary manifest-bearing stages (or reproducibly rebuild a
missing stage), verify all four identities, and produce a new manifest whose
allowed use is exactly the S46 parent/child/return fixture. It must not use a
reference binary as a fallback, modify an image, or copy any artifact into a
source, release, or `artifacts/` path.

## Fixture crossing contract

| Phase | Required proof fact | Owner | Boundary/disposition |
| --- | --- | --- | --- |
| Bootstrap | NTIO stages source-built NTDOS; original COMMAND obtains the profile-selected target through its normal guest command flow. | Guest NTDOS/COMMAND, existing setup seams | Existing `50:11` and COMMAND package routes; no host loader. |
| EXEC | Original NTDOS opens/classifies the fixed target, creates the guest PSP/arena state, saves parent return state, and reaches `50:36`. | Guest NTDOS | Existing full DEM file package plus no-VDD `50:36` resume. |
| Child entry | `$Exec` performs its original `retf` to the child entry; no observer changes registers or guest bytes. | Guest NTDOS + bx-core/bx-mantle mechanics | A future observation may copy a fixed-width mechanical fact only; it cannot become a BOP handler. |
| Child exit | Original SHARE code invokes `INT 21h`, `AH=4Ch`, `AL=0`. | Guest child + NTDOS | Original `$Exit/$Abort/reset_environment`; existing `50:3c` lifecycle route releases only its defined search state. |
| Parent return | NTDOS restores parent state; COMMAND receives the original `54:0B` result. | Guest NTDOS + COMMAND | Current COMMAND package keeps CF/re-entry/no-next distinction and exports only opaque ordinary completion. |
| CLI result | Engine sees ordinary completion only after normal cleanup. | CLI lifecycle | No BOP identity, guest address, exit code, host handle, or host process crosses this boundary. |

## Mutation-profile and failure contract

The selected child itself performs no host mutation. Therefore Direct and
Readonly must use byte-identical source-built guest inputs and must have the
same guest lifecycle result. Their existing namespace/path admission may
remain profile-specific, but the fixture must reject rather than silently
substitute an unavailable input or host capability.

Overlay remains ABI-reserved and is not a fixture selection. The fixture does
not admit Virtual, host writes, registry changes, process creation, device
enablement, VDD behavior, Redirector behavior, or any Bochs change.

Failure is meaningful evidence, not an implementation cue:

- a missing/identity-mismatched source-built stage fails before launch;
- inability to form the declared command input fails in the selected COMMAND
  package, not in a host fallback;
- a BOP/provider/machine failure is recorded by its existing owner and cannot
  be repaired as an isolated trace hit;
- a bounded run without the lifecycle proof remains an incomplete fixture
  result, not proof of child execution or an instruction to add a device.

## Recovery ladder

1. **Original source reuse:** the child and its `INT 21h/4Ch` contract are
   directly present in `share.asm`; NTDOS and COMMAND own all lifecycle logic.
2. **Smallest adapter/shim:** the existing profile's identity-only target
   placement is sufficient; a fresh S46 staging manifest is a build/provenance
   shim, not a guest-behavior rehost.
3. **External Bochs intrusion:** rejected; no source fact identifies one.
4. **New behavior:** rejected; a new synthetic COM/EXE marker would bypass the
   available OpenNT source artifact.

## S46 admission boundary

A subsequent implementation S may add only the fresh input stager, fixture
contract and default-off observation needed to distinguish the six phases
above. It must build in a new `build/M0-T225-S46/<run>/` root and must run a
native Direct/Readonly pair only after its whole fixture and focused negative
checks pass. Trace output is then an integration result; it cannot generate
another singleton BOP task.
