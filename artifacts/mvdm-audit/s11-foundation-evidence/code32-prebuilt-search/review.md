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

## Isolated host-emulation causality experiment

A disposable host-only candidate was linked by inserting a test-owned copy of
`nt_inthk.c` ahead of the original host library. It recognizes only direct
one-byte 16-bit CLI/STI in GP faults, advances the guest IP, updates the
fixed virtual-interrupt bit and CCPU IF, and does not modify guest or CPU
source. It is deliberately incomplete: its instruction fetch, 32-bit/prefix
handling, original interrupt-ack timing, real exceptions and concurrency are
not established. It is **not a publishable repair**.

The first candidate with no per-hit output progressed past the earlier
repeated 0d fault and reached a DOS runtime `R6009 - not enough space for
environment`; its Run16 returned zero, which does not make the application a
pass. A second build with a 32-hit limit recorded the actual initial
`STI` at `00c7:4978`, IOPL 0 and fixed state `00000600`, followed by CLI/STI
hits in other segments. It then repeated a separate GP with error code 1:
its zero-only test predicate missed an otherwise similar handler-entry STI.
The original CCPU contributory exception forms its code as
`(selector & 0xfffc) | EXT`; the low external-event bit therefore matters.
A third diagnostic allowing codes 0 and 1 recorded the original STI and CLI
hits, produced no `dpmi-fault` dispatch line during that run, and again
reached R6009 and observer exit zero. It does not establish why the separate
R6009 occurs, that all faults were logged, or that the application works.
The experiment establishes that this particular host-dispatch change is
causally associated with passing the previously looping STI, not that the
whole product is compatible.

NT4's protected VDM GP path invokes its opcode dispatcher before reflecting
an exception. Original `OpcodeSTI` updates the VDM virtual flag, increments
EIP and invokes `VdmDispatchIntAck` if a pending bit is set; the latter has
different hardware and timer routes. A production change must account for
those routes, CCPU's `c_cpu_continue` and hardware-event scheduling, segment
limits/paging and instruction prefixes, preserve unrelated GP delivery, and
validate actual guest and IRQ behavior. It must not force IOPL globally or
change the original guest/CCPU instruction implementation. The follow-on
R6009 needs a separate attributable control, not a hard-coded workaround.

The third diagnostic source SHA-256 is
`4BAA33EBF84AD20609B552DA29683ACDE79CEF229D57C28C1A8A2F549AE88FFE`;
the sealed package manifest hash is
`46192FDC88743DC0A1EC9EF7B186078CFC8392A095CDD546030CCF8223670032`;
the fault trace hash is
`DA021C577864F6D2F09DC68EBFF28473252932BC8B6E10CD61CBAF81EC8A551F`;
the console receipt hash is
`E81728C375CE613F0F4E2A1F11467C391839421685F4680976FFD1FA63681FCE`.
Diagnostic inputs, map and full logs remain in `build/t473-s11/sti-host-experiment`.
No diagnostic binary was published and the guest remains immutable.

## Reduced-environment follow-on

The same isolated experimental package was rerun with a test-owned observer
environment restricted to `SystemRoot`, `WINDIR`, `COMSPEC`, `PATH`, `TEMP`,
`TMP`, and the existing observer switches. The earlier R6009 message did not
appear in this run. Instead, DOS/4GW 1.97 reported error 2001, a GP fault at
`033f:0028fa18`, with a USE32 code descriptor and error code 1. The outer
observer recorded exit zero; that is **not an application pass**. The console
receipt SHA-256 is
`BEC9FACFBE7D713AEC3E76A87F1EBE9154EE6B7DE80D74155A1E40F61E283A25`;
the observer receipt hash is
`7D24A4C8523CF47CD5F43C7EE26944D8AB1EB0D72D669C5AC1F068DBBC4098D6`.
The disposable runner and receipts are under `build/t473-s11`.

This control changes the next diagnostic: a large inherited environment was
associated with the earlier R6009, but this one run does not establish its
precise cause. The faulting 32-bit instruction itself has not been captured.
The experimental handler uses `getIP()` and a 16:16 pointer, so it cannot
correctly inspect the observed `0028fa18` offset; neither the instruction nor
the cause of this later fault can be inferred from that limitation alone. The
original NT opcode table also handles more than CLI/STI and includes operand,
address and segment prefixes. A production dispatcher needs independently
validated fetch, instruction and pending-interrupt semantics; broad handling
or an application-specific workaround would not be justified by this result.

A second reduced-environment run with the existing CPU trace enabled repeated
the DOS/4GW error at `033f:0028fa18`; the trace records vector 0d, error 0001,
and dispatch to the guest handler `027f:0000044e`. This corroborates delivery
of that fault, but not the faulting opcode or its cause. The trace SHA-256 is
`D01D1C3D885EE564B70F4BAFAE5DD1109E097D00B6FA851A6275D50C04546C3A`;
the second console receipt hash is
`4E29583F9EF71F37807D54AD3DEACFA5E12B329BA0DA10561F6AF455BCFAAD8A`.
The prior automatic approval failure was transient: the same isolated probe
was later approved and completed. No product binary was replaced.

The current original CCPU exposes `xtrn2phy` as an external linear-to-physical
translation that explicitly does not raise a page fault or modify the TLB.
It is a candidate input for a future bounded diagnostic fetch, not by itself
a complete instruction-fetch contract: segment ownership and limit, CPL,
page boundaries, and memory backing still need validation. `c_GetLinAdd`
alone is unsuitable as a failure indicator because its unmapped fallback
returns a pointer to physical address zero. The existing guest-location
adapter is real-mode 16:16 and cannot be used for the observed 32-bit EIP.

## 32-bit faulting byte observation

A separately staged diagnostic candidate retained the earlier experimental
16-bit handler and added a read-only, exact-address witness at the reported
32-bit GP. In that run the guest code descriptor again had base zero and the
captured CR0 was `00000011` (paging disabled). At `033f:0028fa18`, the
observed instruction byte was `fb` (STI). The guest again reported DOS/4GW
error 2001 and exited; this is not a passing application run. The diagnostic
does not intercept or repair that 32-bit instruction and does not establish
pending IRQ or prefix semantics. It does show that the later fault occurred at
the same opcode class as the earlier 16-bit witness, and that the experimental
16:16 handler misses the actual 32-bit offset.

The test-only host source hash is
`DE5E02A0F072924CA04763659F90837B9E9993DFDE3E0700215CDCCB2DFFAFEC`, the
trace hash is `4AC712F9C682BC9EAB5A8D7629C49FB13030B8C3DB02F6513424F0B8F3F206B1`,
and the console receipt hash is
`583D459024FF45CC32714E39227D9D3A18B70314E96BA2F74BDF44CD4C2AF517`.
The candidate and package remain under `build/t473-s11/sti-32-witness` and
have not been published. The source trace and this single-byte capture do not
justify changing guest, CCPU, or the production fault dispatcher yet.

An additional isolated causality candidate handled only the captured
`033f:0028fa18` STI by advancing EIP and setting the experimental virtual
and CPU IF states. The same reduced-environment run progressed to another GP,
`033f:002660b0`, instead of the original 32-bit fault; DOS/4GW still reported
error 2001. This is evidence that execution proceeded beyond the captured
instruction, not that the candidate has correct interrupt timing, handles
other instructions, or runs the application successfully. The second fault's
opcode and cause are not yet established. The temporary source hash is
`05375F6A3F3CCCFCC9EAE21AB13D2B93E1630EB9D22893A70B4AC7897F38F921`,
trace hash `081EDC0CBC18A3539632E61ED83ACAC6E1C99B9D35D8723A6633B95F54854FB2`,
and console hash `54F122C9D61F74658CDD92B41ABCDEAFFB3B2EA10C1DB1BFE14900ABD67564C4`.
This address-specific intervention is a diagnostic counterfactual only; it
must never be used as a production or guest-specific fix.

A follow-on read-only witness captured the second faulting byte as `fb` too;
CS remained `033f` with the previously observed base zero, CR0 was
`00000011`, and the reported physical address was `002660b0`. The guest still
received the exception and exited. The test-only source hash is
`229C62B266743E0F7EF0B5C376C200DA720B83296E37BBBCB109191443B6A1A5` and
the trace hash is
`DB51E555CB3B2C39242CCC32AEDC47EB79F6AA37FE2697CA64A2C0FBC1BEEE67`.
This establishes two 32-bit STI sites in this run, not a complete inventory
of privileged opcodes or a correct pending-interrupt implementation.

## Nonpaged generic opcode experiment

A separate isolated host candidate replaced the address-specific experiment
with a diagnostic CLI/STI handler using the live CS base, full EIP and limit,
and bounded nonpaged memory. It requires GP error zero or EXT-only and
CPL greater than IOPL; it does not handle prefixes, paging, instruction-fetch
faults, or original pending-interrupt timing. It progressed beyond both
previous 32-bit STI sites but DOS/4GW still reported GP at `033f:002676b4`;
the trace also observed a later GP at `033f:0026774b`. The bytes and reasons
for these two faults have not been established. Observer exit zero is not an
application pass, and the experiment is not a publishable implementation.

Original NT's STI dispatcher explicitly checks pending interrupt state and
routes hardware and timer acknowledgement separately. The selected CCPU
instead accepts pending hardware through its event map, `ica_intack`, and
`host_hwint_hook` when its own IF permits execution. Equivalence and ordering
between these routes remain unproven; simply writing both IF representations
does not prove correctness.

The test-only source hash is
`04C5358695ADD8BF5CC24AF363D6935C83F54991A733BF6C998E1A6637CD6C7A`,
trace hash `8C724D5553F06E476F3E98A5B5EDEF808DC57BCB57A8DB0F561905147A83149A`,
and console hash `81B825F0339232C5E741F0BDAF6D53770812A41DB884F8F0C9ED17AA3F85A549`.

An additional nonpaged read-only diagnostic recorded byte `ee` at both
`033f:002676b4` and `033f:0026774b`, with CR0 `00000011`; the existing
exception handler still received both faults. `ee` is OUT DX,AL. The pinned
NT opcode dispatch includes `OpcodeOUTB`, which invokes
`Ki386VdmDispatchIo`; consequently the observed path requires evaluating
the I/O monitor boundary as well as CLI/STI. This observation does not prove
the requested ports or values, guest permission, the destination device, or
whether reflecting either exception is incorrect. The diagnostic source hash
is `A8AD0922DB0072AD17202CFCEB0C67CCC1FF69858DD051DEDD8793D042ED9296` and
trace hash `781FF5E756DD8A254D01602EF27115955C6DEDF4987F870F6334CFE3BEB65C8E`.

A repeated isolated run additionally recorded the actual operands: the first
OUT used port `0043`, value `36`; the later OUT used port `0020`, value `20`.
Both reported CPL 3 and IOPL 0. `ios.h` names these as the timer mode and
master PIC command ports respectively. This observation was made after the
experimental CLI/STI interception, not on the unmodified released runtime;
it therefore cannot alone prove the released guest reaches these instructions.
The selected CCPU checks its TSS I/O permission map before dispatching OUT.
Original NT `Ki386VdmDispatchIo` first tries an installed port handler, then
reflects an I/O event to its monitor; it does not simply allow unrestricted
host hardware access. The precise source of the current permission failure
and the faithful route to the virtual devices remain unproved.

The operand witness source hash is
`91B816F53AE8A5E822A50704A4091610E9D09EEFA1A164ABDDA27AFA42578468` and
its trace hash is
`6C93067EDC30F0AE2436841511CCB20FF6507A818CAAF79AFC236D32BF9E8258`.
