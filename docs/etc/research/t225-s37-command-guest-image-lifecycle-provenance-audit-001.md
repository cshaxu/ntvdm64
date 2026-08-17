# T225 S37 — COMMAND Guest-Image/Lifecycle Provenance Audit

## Decision boundary

This audit reconciles the source-built `COMMAND.COM` identity, the original
guest load/launch lifecycle, the modern COMMAND host-service package, and the
S35 live BOP locations.  It makes no implementation or observation change.

The resulting classification is deliberately narrower than a source-function
claim: `0C41` is a **live COMMAND-plane caller segment candidate**, while the
exact loaded COMMAND segment and routine at `0C41:2357` are unproven.

## Three distinct ownership layers

| Layer | Original owner | Current evidence | What it does not prove |
| --- | --- | --- | --- |
| Artifact identity | source-built `COMMAND.COM` | S8/T145: 50,384 bytes, SHA-256 `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`. | Its run-specific guest-RAM segment or instruction address. |
| Guest image lifecycle | NTDOS `$Exec`, `dos/v86/doskrnl/dos/msproc.asm:283-850` | The original path allocates a block, reads the resident image at `Exec_DMA:0100`, relocates EXE state where applicable, builds process state and transfers to the result. | That the S35 invocation completed each step with `Exec_DMA=0C41`. |
| COMMAND host-service plane | `dos/command/cmddisp.c`, `cmdmisc.c`, `cmdconf.c`, `cmdenv.c` | S20's full `54:00..10` map and S35 locations `0C41:1B0E` (`54:02`), `0C41:1BFE` (`54:0D`), `0C41:03A6/03CB` (`54:0F`). | A symbolic COMMAND source label or post-BOP continuation state. |

The S35 calls establish that code executing with CS=`0C41` reaches COMMAND
service identities.  This rejects the prior temptation to use `0C41` as a
demonstrated NTDOS load segment.  It does not by itself establish the exact
guest module owning every byte in that segment.

## Original contracts and current composition

The original COMMAND dispatcher has a fixed 17-entry table in
`src/opennt/base/mvdm/dos/command/cmddisp.c:15-33`; index 2 is `cmdComSpec`,
13 is `cmdGetAutoexecBat`, and 15 is `cmdGetInitEnvironment`.  Those three
calls are respectively implemented by `cmdmisc.c:635-650`,
`cmdconf.c:87-123`, and `cmdenv.c`; each is historical host composition, not
COMMAND image loading.

Conversely, `$Exec` owns the image-level transition.  In
`msproc.asm`, it validates the requested function and environment
(`283-521`), allocates the program block (`593-724`), reads resident file
bytes to `Exec_DMA:0100` (`764-818`), and computes relocated execution state
(`820-850`).  The guest DOS loader, not bx-vdm and not a `54h` provider,
therefore owns COMMAND segment selection, PSP, return stack and entry state.

Current `src/bx-vdm/bx_ntvdm_readonly_namespace.c:29-64` and
`bx_ntvdm_cmd_launch_envelope_v1.c:25-51` preserve a declared `X:\COMMAND.COM`
artifact/namespace identity.  `bx_ntvdm_command_package_session_v1.c:79-110`
routes the host-side bootstrap BOP services through the existing package
session.  Neither surface chooses an execution segment or writes a synthetic
COMMAND image: that would violate the original `$Exec` owner boundary.

## Source-map and dynamic-proof limits

T145 remains authoritative: the controlled COMMAND artifact's retained
provenance is not an exact symbol-to-offset proof for this live run.  T151
also shows why raw artifact arithmetic cannot replace lifecycle evidence: for
its separate run, `0BC1:FFFF` lay outside the source-built file interval.
Neither result authorizes carrying an assumed segment from one run to S35.

S35 records BOP CS:IP values and the first fault, but not `$Exec`'s allocation
result, file-read destination, final CS:IP, PSP, or command-image range.
Therefore the following are explicitly rejected:

| Proposed inference | Disposition |
| --- | --- |
| `0C41` equals the earlier conditional NTDOS `EDI` load segment. | Rejected; same-run COMMAND-plane calls make that identity unsupported. |
| `0C41:2357` is a named COMMAND source routine. | Unresolved; no run-equivalent symbol binding exists. |
| A `54:02`, `54:0D`, or `54:0F` provider caused the later DS:FFFF access. | Rejected; S35 proves order only, and each is a host-service route rather than the `$Exec` image owner. |
| bx-core needs segment wrapping or a BOP patch. | Rejected; no source/current-contract evidence supports either repair. |

## Recovery disposition and next owner

The current composed COMMAND host-service package remains valid at its
existing S20 boundary, but COMMAND **guest-image execution** is not closed.
The missing evidence is one coherent guest-loader state correlation, not a
new service implementation.

The next owner must be a whole COMMAND guest-image continuity observation
package.  Before any run, it must source-map `$Exec`'s allocation/read/entry
state and declare one default-off, selector-blind observation of the relevant
guest-loader transfer state.  It must correlate image range, PSP/entry and
later BOP/fault CS without decoding BOP services or changing guest state.
It may not patch `54h`, fabricate a COMMAND segment, or alter Bochs real-mode
limit semantics.  Its result may either identify the range or retain a bounded
ambiguity; only then can a repair owner be selected.
