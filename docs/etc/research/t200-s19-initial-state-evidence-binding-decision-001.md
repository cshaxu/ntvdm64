# T200 S19 — initial-state evidence/profile binding decision

## Decision

The retained r18 snapshot is **not bound** to the current minimal mantle as a
same-machine snapshot.  It is explicitly incompatible for that claim.

The r18 evidence was captured after a CPU5 stock-ROM/floppy POST profile.  In
contrast, `bx_ntvdm_minimal_machine_c` constructs SIM, RAM, default port
space and CPU reset only; it neither loads ROMs nor executes POST.  The
CPU5 configuration projection identifies a decode/build profile, not a
firmware-state identity.  Neither the current v7 BYOB profile nor direct
composition carries a machine-initial-state declaration.

Using r18's raw bytes as an implicit minimal-machine default would therefore
be a false equivalence and violates the source/evidence boundary.

## Consequences

| Candidate action | Disposition |
| --- | --- |
| Treat r18 snapshot as minimal-mantle reset output | rejected |
| Inject r18 bytes in bx-mantle by address | rejected |
| Reuse finite-run preentry fixture | rejected |
| Keep S18 `ABSENT` declaration for current v7 composition | accepted |
| Add a later profile-selected external initial-state record with evidence identity | admitted design direction |

The last option is not a firmware claim: it is explicit profile composition
data.  It must carry immutable raw ranges and the r18 evidence identity, be
selected only by a new profile revision, and remain opaque to mantle.  A later
generic mantle publisher may apply it only after complete preflight; its code
must not distinguish any range value or guest name.

## Verification

This conclusion follows from source inspection of
`bx_ntvdm_minimal_machine_c::initialize`, the CPU5 projection manifest, the
r18 snapshot record, and the current v7 profile parser/output.  The S18 ABI
probe remains the executable proof that the current absence is valid but not
publishable.  No machine, RAM write, CPU loop, or guest execution occurred.

## Follow-up

T200/S20 should implement a new profile revision's explicit external
initial-state record and bind it into direct composition.  It must include a
focused parser/composition test for exact evidence identity and must preserve
the current v7 absent behavior.  It still may not publish bytes to RAM.
