# Prebuilt DPMI interrupt witness: code segment distinction

The historical S38 record names `D38K.COM` as a 32-bit-code-segment interrupt
and fault-return witness. A current filename scan under repository `build/`
found no `D38K.COM`. A separate content scan of `.COM` files under `build/`
for `S38_INT32_RETURN_OK` found the three basenames in `basenames.txt`; this
does not search the entire machine or prove no differently labelled witness
exists. The only additional prebuilt basename is `S39I32.COM`.

The four Git revisions that changed `assets/winnt.zip` were also inspected
read-only in memory. None contains a `D38K.COM` entry or a `.COM` entry with
the expected `S38_INT32_RETURN_OK` marker. The reproducible script and result
are `search-old-zips.py` and `old-zips.txt`. This covers these archives, not
other historical packages or media.

The retained first 40 instructions of the exact `S39I32.COM` and
`S39H32.COM` hashes show a 32-bit DPMI client request (`AX=1`), followed
directly by 16-bit code after the far call. Neither has the descriptor clone,
default-size-bit update and far jump which the current `CODE32` test source
uses before its 32-bit instruction stream. The existing `S6PROBE.COM` is the
same hash as `S39H32.COM`. These facts cannot prove every possible executable
path, but they do not support treating either prebuilt as the dedicated
32-bit-code interrupt/fault-return witness.

No guest was built or modified. The dedicated current-release CODE32
interrupt/fault-return boundary remains unverified; the separate existing
32-bit code-segment INT 21 witness is not the same test. User's guest
immutability restriction remains in force. This is an inventory result, not
a product regression or a reason to modify original CCPU behavior.

## Existing CODE32 guest: actual DPMI dispatch observation

The unchanged prebuilt `D38R32.COM` was rerun against the current staged
package with the existing `MVDM_S42_DPMI_BOP_REPORT_PATH` trace enabled.
The guest emitted both required CODE32 INT 21 and failure-cleanup markers;
Run16 exited zero, and the temporary Z: mapping was removed. The trace file
contains 914 `dpmi-bop` lines covering indices 00, 01, 02, 03, 04, 05, 06,
07, 08, 0a, 0c, 0d, 0f, 10, 11 and 13; none reports index 15 or 17, the
selected 32-bit interrupt and fault-return entries. The relevant reporter
is called by `DpmiDispatch`; it does not record guest CS descriptor mode,
and failures to open/write its optional log are silent. Thus this run does
not promote the CODE32 INT 21 probe to an interrupt/fault-return witness or
prove those entries unreachable. The guest and product binaries were not
modified or rebuilt.

The complete trace SHA-256 is
`6807AA02F0C5CB379A09DFEE9ABCFABD2D6F31BA156A5CAFD96154B7295FB350`;
the guest console SHA-256 is
`E5B87AB5CAE9E3CC7C50672017D94475002D54BD88940E1F9DF084254D8DE5EC`;
the isolated runtime image manifest SHA-256 is
`204D67973C30B17AC581A303890D8F053A8343721411E1AEF09AFB9B32E20C85`.
Results and the exact eight recorded image hashes are under
`build/t473-s11/dpmi-code32-entry-trace-run/`; the full trace is
`build/t473-s11/dpmi-code32-entry-trace.txt`. This is a bounded real guest
observation, not exhaustive exception or interrupt validation.

## DOS4GW and NESticle: non-passing fault-loop observation

The existing `O:/assets/nesticle-x.xx/DOS4GW.EXE` (SHA-256
`B8265123AC8A189637448618409EF3ECD2E9F3E1A47062C685A02240F688DEC1`)
was copied without modification into a test-owned package and invoked without
a program argument. The existing observer timed out after 25 seconds. The DPMI
trace contains one index 15 and 212277 index 17 events. This is an observed
non-pass, not proof that standalone no-argument operation is supported or that
an actual 32-bit code-segment return succeeded.

A separate test copied the user's unmodified `NESTICLE.EXE` (SHA-256
`224E348E1FD5C11B16EF096FF5607C7C57C49D65FA7E4DBBC9B2A5C9C53E832B`)
and the same DOS4GW to the isolated package, exposing the test directory on
PATH. The application copy was named `S6PROBE.EXE`, so name/working-directory
dependencies have not been excluded. During the 15-second observation the
existing product fault trace recorded 36729 vector-0d dispatches with
`frame32=1`: first from `00c7:00004978`, then repeatedly from
`01c7:00006b51`, with handler `01c7:00006abb`. The BOP trace separately
recorded one index 15 and 36728 index 17 events. The observer timed out and
reported no graphics handshake. This shows repeated fault-dispatch/return
entry under instrumentation, **not** successful returns, a proven DPMI source
fault, or a confirmed code-segment default-size bit. The test app is
interactive, so timeout alone cannot establish that its normal exit failed.
A separate no-trace run also timed out with no graphics handshake, but did
not independently establish the same exception sequence. No guest binaries
were changed or rebuilt.

Both owned stages were cleaned up and their Z: mapping removed. The trace
reporters can silently fail to write; the observed lines are positive events,
not completeness proof. The first 4-second attempt never started the observer
and is not a product result. Later investigation must retain the original
file name and startup directory, establish a valid application interaction,
identify the exact faulting instructions and segment descriptors, and compare
with the original host before deciding whether this is an in-scope connection
failure or an excluded independent CPU issue. Do not count this as the missing
successful CODE32 interrupt/fault-return witness.

Evidence SHA-256: standalone BOP trace
`E185AF83DE2DF33AD88AEEAEAAD9A2F1830FEAFB82829CC7DA87FFF1F1763895`;
NESticle fault trace
`501535C8E583F6BE89252C3947C233B3694940C0C4EED4FD6F0F630CBAEA7422`;
NESticle BOP trace
`85EA6925634B76D145F78FD3F6C8DA7D6994EBE90C38CF8F24B836B22FAB97D6`;
traced and untraced observer reports respectively
`511CF091DF4637AAAF28617F1B419D9DFA6F727D02CF0ED21DEFBCF7E6F7D1C3` and
`C5A979F07C1A559DE4C0A8A78CEBF9CFF63116902280F9E3F008A727202D9BD7`.
Full traces and private runners remain under `build/t473-s11`.

## Faulting instruction and original host boundary

A further fresh test used the original `NESTICLE.EXE` name with both it and
DOS4GW in the test working directory. It again produced an initial vector 0d
from `00c7:4978`, then repeated vector 0d from `01c7:6b51`; the 15-second
observer timed out. A read-only, PID- and image-path-bound probe inspected the
same run's live worker memory after those events. The actual selector records
were `905f000811fb0000` for 00c7 and `ffff70e500fb0000` for 01c7: both
were 16-bit code segments with ring-3 access. Exact fetched instruction bytes
at both recorded fault addresses begin with `FB` (`STI`). The trace's 32-bit
fault frame recorded saved EFLAGS `00000006`, i.e. IOPL 0. This is a 32-bit
exception **frame**, not evidence of 32-bit default code segments. The raw
[read-only capture](live-fault-code.json) binds module base, guest memory base,
LDT shadow, descriptors and bytes. The capture occurred after the logged first
fault; it is not an atomic snapshot at that first exception.

The current original CCPU `c_main.c` executes STI only when CPL <= IOPL,
otherwise raising vector 0d. That behavior accounts for the recorded ring-3,
IOPL-0 STI conditions; it is not by itself a CPU defect. The current original
`host_exint_hook` passes installed faults to the DPMI guest handler. In the
pinned original NT host, `ke/x86/trap.asm` instead routes a protected-mode VDM
GP through `Ki386DispatchOpcode` before reflecting an unhandled exception;
`instemul.asm`'s `OpcodeSTI` updates the VDM virtual interrupt state, advances
EIP and dispatches pending interrupt work. `OpcodeCLI` similarly updates the
virtual state and advances EIP. The DOSX `FSTI` macro explicitly contains the
observed STI fallback when interrupt/state bits require it. Thus there is a
specific original-host instruction-emulation connection to investigate; do
not solve this by suppressing all GP faults, forcing IOPL globally, modifying
CCPU or modifying the guest. The same captured STI bytes and dispatch ordering
are concrete evidence, but the full pending-interrupt/exception contract and
an A/B repair witness remain unproved.

The selected immutable DOSX has SHA-256
`C5AF29A29ABF167B243DAABF877459E8278B8C9A339BF6FCEEFF`; its retained
`dpmi/486/dosx.map` places offset 4978 inside `I31_RMCALL`, consistent with
`dxint31.asm`'s `FSTI` following its mode switch. The map/source relation is
supporting evidence, not a claim that map provenance alone proves the binary
build. The source segment byte length and live fault bytes independently match
the observed code. The original host source hashes for `instemul.asm`,
`trap.asm` and `emv86.asm` are respectively
`6356B416E0942EB3814E4C5A14FF5ED09DB13DC1443842BA51AB9F512522D6E4`,
`5D01FEC3B1BA603029A7E22DB32805465BB2B499A884CB27DEB7F278EB736D07`, and
`DCD29FB59584D9D157E75FB5C45A6FE7E9F09CB3F37B0B44E0B6B3AE67B1D702`.
The live capture hash is
`F105F08845F0247D023B44E97184597858598CCB6CEDD0FB7CAA3A529C78CB16`;
source-mode traces and the read-only capture runner remain under
`build/t473-s11`. Initial capture attempts failed to identify the SUBST image
path or locate Python under the intentionally restricted PATH; neither counts
as a guest result. No guest, product source or deployed component changed.
