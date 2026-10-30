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

The pinned original `v86/monitor/i386/monitor.c::EventVdmIo` dispatches
reflected byte and word I/O through SoftPC `inb/outb/inw/outw`. The selected
CCPU's `OUT8` invokes the same installed device tables directly; its source
explicitly avoids `outb` because its virtualization path could re-enter the
CPU. This is a concrete reuse candidate, but equivalence for optional
virtualization, unregistered ports, widths, and reentrancy is not established.

An isolated experimental candidate routed unprefixed nonpaged `OUT DX,AL`
to the existing `OUT8` after its diagnostic CLI/STI handler. The observer
timed out after 15 seconds, with no graphics handshake and a blank captured
console; its test script correctly rejected this as a non-pass. The trace
did not contain a `dpmi-fault vector=0d` line, but this is not evidence that
the interactive application started, that a graphics frame was produced, or
that all failures were covered. Pending-interrupt behavior remains unresolved.
Source hash `A4626512E31B26AA023BCD6E67942F7FD5CEC424A70A8E7A16A76A4E585AE5B2`,
trace hash `31D47B8D7E2F70FC665B9D305CCC7B5C3065C645C759028E7CEA04B53F1CD1B7`,
and observer receipt hash
`C54236BE6B1641B2165FC94F7DAEF225F35905DC97CE8E9A2697EBB0FCAD1920`.
This unapproved-semantics diagnostic stays in the isolated build tree only.

`graphics-handshake=none` specifically means the observer's optional
`MVDM_OBSERVER_GRAPHICS_RETURN` handshake was not enabled. It is not a
negative observation of graphics output. The blank console snapshot is also
insufficient to determine whether an interactive graphics application is
alive and rendering. The 15-second observer timeout confirms only that its
outer process had not completed within that interval.

The observer's single timeout snapshot identifies the staged worker at
module base `00ea0000` and its main sampled thread at `00eab3fd`. Resolving
that address against the diagnostic build's own linker map gives `phy_w16+0xd`;
other captured worker threads resolve to timer, input, and publication paths.
This is a momentary host CPU-memory-write observation, not proof of a guest
loop, successful display, a deadlock, or the missing interrupt's cause. The
observer record does not capture the guest CS:EIP at this point; further
attribution must correlate guest execution or a frame with this exact
candidate, rather than interpreting the timeout as a definitive failure mode.

## Frontend frame observation

An isolated x64 frontend was linked with the existing test-only
`tests/app/console_video_observed.c` wrapper and staged with the experimental
worker. The receiver logged its PID, console geometry, and a successful
console write containing the DOS/4GW banner. During the 15-second observation
it logged no `FRAME`, `TEXT_FRAME`, or `FRAME_ERROR` records; the outer process
remained alive until the observer timeout. This verifies that the observer
was active and no frame was received by that instrumented frontend in the
observation interval. It does not prove the guest could never produce one,
nor distinguish guest execution, video generation, or transmission failures.
The existing optional graphics-return handshake was not enabled. The observer
is test-only and was not published; guest and released images were unchanged.

Observed frontend hash
`DE3449A682E034C36B347A2964EAAFF758B5EC7DCCC85CA629D9792D7E636CD1`,
frame log hash `24ABEF8A40F7182F2CE6433F34C1CD5B7CA0AFEE117994B4DEDC95DD14FBE84D`,
and observer receipt hash
`5004950B3D93AC8075CDC638B78B14DBFFDB8C3E4B696163903D9D99C4316077`.

A subsequent run of the same staged frontend/worker enabled the observer's
existing read-only first-MiB guest-memory capture at timeout. The BIOS data
area mode byte at `0040:0049` was `13h`, with 40 columns and 25 rows recorded;
the captured value is consistent with a mode-13h transition. The frontend
frame log again contained its receiver and console records but no `FRAME`,
`TEXT_FRAME`, or `FRAME_ERROR` entries. This narrows the investigation to
the relationship between the guest's BIOS mode state and video/frame path;
the BDA byte is not proof that the VGA device entered that mode or that a
frame was generated. The zero-filled apparent A000 region in the raw memory
dump is not treated as a framebuffer observation because video memory can have
separate device backing. The guest was read only.

Guest-memory capture SHA-256
`0E7234EE0C7A0A9281D894EF8A31F77A9C1E5CB57FD2E1DF8A9A9EF844300FC2`,
frame log hash `DFAEA8D123B8C4BD6270E6E323033518E6A3C3C00995B2DB20E0368A05D3B9D4`,
and observer receipt hash
`27BD89B5CE1BE4B4D7345028015EE4F2BDF8C7E0BE6A658009C11504E0B41062`.

An additional timeout-only observer read fixed symbols from the exact
experimental worker map, without writing process memory. The snapshot reports
`Currently_emulated_video_mode=13h`, `Video_mode=13h`, `sc.ScreenState=1`
(`FULLSCREEN`), `sc.ModeType=1` (`GRAPHICS`), `ConsoleInitialised=1`, and
`ConsoleNoUpdates=0`. A separate x86 compile of the selected source struct
confirmed the `ScreenState` and `ModeType` offsets as 48 and 52 bytes. The
same run's observed frontend recorded no completed frame. These are samples
at timeout, not proof of the full transition sequence: neither painter
completion, bitmap/palette readiness, publication signal nor transport outcome
has yet been established. The value `CntrlHandlerState=00010000` does not
include the `CNTRL_VDMBLOCKED` bit `00020000`.

Observer-source hash `66B2C3F1551C30D99790D4B9AB75430EFE339B2CDE1CF16F3909D9DF460D7716`,
timeout-live receipt hash `53B35A8F1937430980C82D5FE9FD74B535134EC1A02B468127FA550C393FB302`,
and frontend report hash `E45C55D756FA79E806583844793D8548FBC884CD126963412A0E9BB7A10519F5`.

A second timeout snapshot of the same staged candidate also read the
map-selected `nt_graph.c` values: `flush_count=0`, `update_vlt=0`, and
`mode_change_now=0`. These fields are sampled once, so zero does not establish
that the timer callback failed to run, that a mode transition finished, or
that a painter completed. The source's `nt_graphics_tick` also gates work on
`get_mode_change_required()` and the selected update algorithm; a causal
decision needs a witness at those boundaries, not more conclusions from a
single counter value. The timeout-live receipt hash is
`1A4B6AE0B8418B389924357261BAF0AB442EFBD833153FABE5F509DD89199481`.

A later isolated worker linked a test-only copy of `nt_graph.c` with three
monotonic entry counters; no production or guest source changed. At the
timeout snapshot, the test counted 159 `nt_graphics_tick` entries, 77
`software_update_completed` branches, and zero `nt_flush_screen` entries;
its same snapshot reported FULLSCREEN/GRAPHICS and mode 13h. The source
counter increments after the tick's update path, not inside the painter, so
it does not prove that `calc_update` produced dirty pixels or that a graphics
buffer was ready. The initial state of the counters is zero but they span the
whole run, including startup; no per-mode timing is implied. This evidence
rules out the simple claim that no graphics ticks occurred. It does not
establish whether lack of `nt_flush_screen` calls explains this workload.

The test-only `nt_graph.c` hash is
`67D2838990F70F908854F26743C85BD90A989F97F6D702A33CEF11131408305B`
and timeout-live receipt hash is
`A8DF779F1CA56C39F8702B0FE9430D656482DF2A416B1E63322CBF5840FDDCD1`.

A distinct isolated diagnostic counted only calls made while `sc.ModeType`
was GRAPHICS. At timeout it recorded 151 graphics ticks, 76 graphics-mode
completed update branches, and 76 entries each to `nt_start_update` and
`nt_end_update`; no `nt_flush_screen` entry was observed. The instrumented
frontend still received no completed video frame. The paired painter callbacks
prove that a graphics update path ran, but do not prove it rendered the
intended pixels or that its bitmap/palette was ready for capture. They also
do not establish why the `host_flush_screen` boundary was absent. The
test-only source hash is
`B3F1E33C4FECCEF68A9C5AF3AC9D979F856A25ED254B6E97D036836E7EBA1D6B`
and the timeout-live receipt hash is
`3B79C712128887FC8B7399C25DF54876B1750183D6C16BBC30B40E6B7679005C`.

An additional isolated selected-object witness instrumented the project-owned
graphics capture, signal, invalidation and update-end paths. At timeout it
recorded 71 successful bitmap captures, 74 publication signals, zero signal
errors, one palette-not-ready capture and two no-work captures. The final
update-end sample had a bitmap identity and palette, with dirty state set;
71 graphics invalidations were recorded. The independently instrumented
frontend logged no completed video frames in this observation. This narrows
the remaining question to publication admission/send, transport, and receiver
boundaries after capture; signal success alone is not delivery success.
The witness contains the earlier experimental privileged-instruction
interception, is not production code and does not prove the input pixels are
correct. The test-only source hash is
`8E7F1E3D1BF43771CF2A8D3E2BA8E00B4B68A79C38DA7530232D5CE6B203CBB9`;
timeout-live receipt hash is
`0C8F7F14C843BECB1D30D58B1A4DA877FF9039316BB35C76D17D52F2AD88F1FF`.

## Correction: frontend witness coverage

An additional selected-object sender witness observed 75 sends started,
75 returned and zero returned errors; the final blob size was `0003ec18`.
This does not prove on-screen display. Source review then found that the
existing `console_video_observed.c` wrapper records `frontend_video_data`,
but the normal standalone path in `console_channel.c::video_data` calls
`frontend_video_stage_data` and commits via `frontend_session_video` instead.
Therefore the earlier absence of `FRAME` records **does not establish that the
frontend received no frame**. Earlier claims in this record of "no frame
received" are superseded: only no frame was recorded by that particular
wrapper. A witness covering the staged path and actual render is still needed.

The isolated sender source hash is
`72FB2130616DD05D8581FCA8D0FDAD0F3E8DC26E0D239BD23E2914764CF90B32`
and timeout-live receipt hash is
`7F17B949E20973C4C895496748A079D7F0E963F135FA3C1552A5C270400AA062`.

The following isolated frontend witness wrapped the actual
`frontend_video_stage_data` and `frontend_video_commit_pending` calls. It
recorded 78 successful stage and 78 successful commit entries, including the
last two 640x400 8-bit graphics frames, each with 256000 payload bytes. This
confirms logical frame reception and commit for that experimental package and
observation period; it does not establish rendered pixels, application input,
or correct privileged instruction/interrupt semantics. The process remained
alive until observation timeout; that is not itself failure for an
interactive application. The prior missing-record observation was a test
coverage limitation, not evidence of a broken publication route.

Test-only frontend wrapper hash
`0BEEBC4067D0F186EC24D623E605F3D473DDC0AEBDFA4658B97161D8AFFEC9B4`,
frame log hash `9AB08B0E9B25EB73036A5CA28BA9A77D9733158B6749FC826793502049426086`,
and timeout-live receipt hash
`2BE24045F58C561E61A5229EF59DF46667F4F8AC4D830E255C3F1C8FB15AB9CB`.

An additional test-only wrapper copied the most recently committed DIB and
its palette to a build-owned file. Converting its top-down 640x400 8-bit
payload yielded a frame showing the NESticle menu and mouse pointer; 16
distinct palette indices occurred. Thus the experimental worker reached and
published an actual application graphical interface, not only its DOS/4GW
banner. This is the frontend's committed pixel buffer, not a screenshot of a
rendered desktop window; neither input responsiveness, an actual game, nor
correct exception and interrupt behavior is established. The original guest
was unchanged and this experimental worker remains unsuitable for release.

The committed description and payload hash is
`0649F68D080DD4DDD2B9284D0B53D0C8BE7BA194A8CFD0B89DADAC4317617066`,
the converted image hash is
`B3F34581E5A9C87DC56F2BF8D2FF8D03552EED39F873A156883855BBDC257949`,
and the test wrapper hash is
`64FAF0CED32F82B9B1C8BD6E63662C2DA9A5E094434B315FEAEA115E851391C0`.
All three artifacts remain under `build/t473-s11/committed-pixel-witness`.

The source-level continuation comparison exposes a remaining timing concern:
the selected CCPU `c_cpu_continue()` jumps to the exception continuation in
`ccpu()`, which ordinarily reaches `NEXT_INST`; the existing handled-hook
path does not itself explicitly acknowledge a pending PIC interrupt. The NT4
`OpcodeSTI` handler, in contrast, invokes `VdmDispatchIntAck` immediately
when its fixed VDM pending bits are set. The original CCPU's legal STI path
also explicitly goes to `NEXT_INST` with one-instruction suppression. These
facts do not by themselves establish which ordering a particular DOS/WOW
guest observes after the experimental trap: event-map state and build-time
`SYNCH_TIMERS` influence the selected path. They do rule out assuming that
setting IF alone proves equivalence. A production host bridge must validate
the pending hardware and timer routes and preserve legitimate guest fault
reflection; this has not yet been implemented.

For the selected software-CPU build, the original PIC master raises the
hardware event through `host_set_hw_int()` -> `cpu_interrupt(CPU_HW_INT, 0)`;
the CCPU event map and PIC acknowledgement own its delivery. The original
timer also has a CPU timer-tick route. `MIPS_BIT_MASK` selects the software
CPU fallback in guest `FCLI/FSTI`; the fixed VDM state word is not, by this
fact alone, a complete replacement for the CCPU event map or the PIC's state.
Moreover original `BiosSti` / `BiosCli` and `c_IOVirtualised` deliberately
re-enter guest ROM instructions when protected, so a host bridge must avoid
recursive virtualization while preserving the guest's ability to handle a
real fault. These source observations identify state owners and call paths;
they do not yet prove pending-event ordering or authorize duplicating those
states in a new adapter.

The existing selected CCPU/PIC `ccpu-halt-reset-test.exe` was re-executed
and exited zero, including its stale-PIC notification control
`T430_CCPU_STALE_PIC_SENTINEL_OK`. Its executable SHA-256 was
`4E379968630FBECFF9DC09949A28A7559319CB6F1CF7C03005BBE6F9DAFE22CF`;
fixture source SHA-256 was
`724A621F5D63BCA48F4E704467013F8C2AAD8A4FAD9A94B1C6E511D9D8F2E461`.
That particular control executes legal STI in real mode with a stale event
and no PIC line; it does not cover a protected-mode STI fault, pending real
IRQ, or NT4's immediate pending interrupt semantics. It is a useful negative
control, not acceptance of the experimental bridge.

## Controlled protected-mode pending IRQ ordering

A separate test-owned extension of `ccpu_halt_reset_test.c` used the selected
CCPU, real PIC initialization/acknowledgement, registered fault and hardware
callbacks, a ring-3 16-bit code segment, and an IRQ0 queued with IF clear.
Its synthetic instruction sequence was `STI; mov byte [0300h],1; ...`.
The **test callback**, not product code, handled the STI fault by setting IF
and advancing IP exactly like the earlier simplified experiment. In three
runs, the IRQ callback observed `IP=1006h` and marker 1: the guest write after
STI had already executed before the pending IRQ was accepted. Each run saw one
GP and one IRQ and reached its expected stop sentinel. This demonstrates the
experimental callback's actual ordering and makes it unsuitable as a claimed
implementation of NT4 `OpcodeSTI`'s pending-ack path. It is not a test of
unmodified product STI handling or of all timer/IRQ cases, and the fixture's
synthetic CPU memory is test-owned, not a changed guest image.

The first fixture attempt had an incorrectly encoded synthetic code descriptor
and was rejected (exit 13); the corrected descriptor produced the three
observations above. Test-only source hash
`31932655E69C8C1FEFB68A1135A09123FFE439F4D48CBD9D8077687C1CB3788D`,
executable hash `067A723D24E230CD8DB12FACA8A185654F70AA37AB496D0E46099A83B9418907`,
and recorded output hash
`8F2D5C95B530D014E141141C17BE2034246520D487CA296C82ED87CFBCF65158`.

A paired test-only callback advanced IP and enabled IF, then called the
existing `ica_intack` and registered `host_hwint_hook` directly for the
controlled pending IRQ. All three runs observed the handler at `IP=1001h`,
before the following write instruction, with one fault, one acknowledged
IRQ, and the final stop sentinel reached. This demonstrates a possible
ordering using existing selected objects, without changing CCPU source; it
is not an implementation ready for release. In particular the fixture's
registered handler always handles vector 8, so it does not cover a handler
returning FALSE, normal `do_intrupt` fallback, empty/stale PIC state, IRQ
arrival races, IF/virtual-state concurrency, timer route, or nested faults.
It manually acknowledges unconditionally for a controlled test and must not
be copied as a general bridge.

Test-only source hash
`05945384C51130D3FAB4E4BBD8B60B42D95D53B54E205A6CFB517D6CB184C6F2`,
executable hash `63FB1C2856776DFAA37D506109CBAABDAC9ADC3D47A570B79B3829BFDD9377E8`,
and three-run output hash
`20862136AEBC3865F3651F5D781EC026D20975A5E64C8012D904B385658F938F`.

A further test-only variant used a real 16-bit protected-mode IDT interrupt
gate and original CCPU `do_intrupt` fallback when the registered host IRQ
callback returned FALSE. The synthetic guest handler increments a marker,
executes `OUT 20h,AL` for EOI and returns with `IRET`; a narrowly targeted
test fault callback dispatches that OUT through the actual CCPU `OUT8` device
table. The callback does not issue EOI on the guest's behalf. Three runs
returned zero with one STI fault, one IRQ acknowledged at IP `1001h`, one
guest EOI OUT, cleared master PIC ISR bit 0, handler marker 1 and the final
`BEEF` stop sentinel. This demonstrates one controlled original fallback and
guest EOI/return path; it is not a general opcode monitor, test of production
bridge, proof for absent hooks, nested faults, timer races, or a replacement
for full DOS/WOW regression. The synthetic instructions exist only inside the
test-owned host fixture; no guest image was changed.

Test-only source hash
`466EA5C56E07E6F17AB18DAE5E4EA1E0446A221B8E43A00DA3CEC43695B4A5B5`,
executable hash `306992176619C855C1896D6BDFD7FB2BFEAFA0E22203243BE6E0006160055B28`,
and three-run output hash
`4378140AA33E3406F0BE66D30887AD14A6BC79D4CE84EF5EDFABB95C2472C920`.

A four-case extension of that controlled fixture checked the immediate
test callback with (0) an unmasked pending IRQ, (1) no IRQ, (2) a masked
pending IRQ, and (3) an empty PIC with a stale CCPU hardware notification.
All four exited zero. Only case 0 invoked the IRQ callback; case 2 retained
IRQ0 in the PIC IRR, and every case ended with IRQ0 absent from the PIC ISR.
The unmasked case additionally observed the guest's EOI through the test
OUT path and its original `IRET`. This is evidence for these four selected
states only: the test callback still acknowledges synchronously and is not a
production bridge, and concurrent transitions, other vectors, and protected
instruction decoding remain untested.

Test source hash `B3A6F8429EEF244595DC24FF457FED43451B1932D67B9606F8E049F27B939A06`,
executable hash `1B8A1FCA9B4A2A061A8BF13929512B64F7B8B408AE9EB5B5DCBF1F5B66547DD9`,
and output hash `96A4E29055235416AEC3C4ED501B27E7AC4556C572D968D7E037F30A8042A9E2`.

Original `ica_intack` acquires the ICA lock, accepts master/slave state,
handles the spurious `-1` result and releases the lock before returning.
Its `ica_accept` owns automatic EOI. The historical x86 kernel
`VdmDispatchInterrupts` also takes its ICA lock, but retains it while
selecting the vector and pushing the guest interrupt frame; it additionally
has explicit delayed-IRQ restart, AEOI monitor return and IRET-hook handling.
The selected non-MONITOR CCPU `ica_intack` returns hook address zero, while
its actual interrupt dispatch passes through `host_hwint_hook` and
`do_intrupt`. Therefore the earlier controlled immediate callback proves only
the measured case, not equivalence of lock coverage, delayed IRQs, auto EOI,
or IRET hooks. Restoring these requires following the selected software-CPU
owners and their existing behavior; merely nesting another lock around an
arbitrary host callback would need separate reentrancy and deadlock evidence.

## Protected-instruction decoder source boundary

The pinned NT4 `OpcodeIndex` routes segment, operand/address-size, lock and
repeat prefixes through `OpcodeGenericPrefix`; the latter advances within
the CS limit, counts instruction bytes and has a source-defined limit of 128.
Its opcode table includes CLI/STI, byte and word scalar IN/OUT, immediate and
DX ports, string I/O, and several other operations. Unknown or out-of-limit
instructions return unhandled. This table is wider than the experimental
handler's bare FA/FB/EE cases. Each family must be compared and connected or
explicitly left to original guest fault handling; inventing a successful
fallback for missing operations would mask failures.

One material source detail is that the pinned NT4 protected-mode handlers
for scalar `INW`/`OUTW`, including immediate and DX forms, pass **size 2**
to `Ki386VdmDispatchIo`; the table comments explicitly warn about 32-bit I/O.
The selected CCPU provides separate `IN32`/`OUT32` software-device functions.
Their existence alone does not prove that substituting size 4 in the NT4
protected fault route preserves the original behavior. Operand-size and
prefix cases, including 32-bit defaults, need a behavior witness and explicit
disposition before an integrated decoder can claim original compatibility.

A read-only source matrix checked all eight scalar immediate/DX IN/OUT
handlers and found byte handlers pass size 1 and word handlers pass size 2,
with read/write direction matching the opcode. It also checked presence of
all 11 prefix table mappings. The checked source hash remains
`6356B416E0942EB3814E4C5A14FF5ED09DB13DC1443842BA51AB9F512522D6E4`;
the output is `build/t473-s11/opcode-source-matrix.txt`. This verifies source
statements, not runtime prefix, operand-size, I/O or exception behavior.

## Combined host-chain candidate, still isolated

A build-only `host_exint_hook` candidate combines the prior nonpaged CLI/STI
observation with PIC acknowledgement, the existing hardware hook and
`do_intrupt` fallback; it also routes unprefixed scalar IN/OUT immediate and
DX forms through the selected CCPU device I/O functions. An isolated sealed
package with an observed frontend reached the NESticle menu; the frontend
recorded 80 successful staged/committed frames and its last captured frame
hash matched the earlier menu witness. The outer process stayed alive until
the 15-second observation timeout, appropriate evidence of an interactive
process but not an application-exit pass or proof of keyboard/game behavior.

The same candidate object was linked directly into the existing CCPU/PIC
fixture. Four controlled cases passed: an unmasked IRQ was delivered at
IP `1001h` through a guest handler and guest EOI/IRET, no-pending and stale
notification cases delivered none, and a masked IRQ remained in the PIC IRR.
The separate fixture fault handler recorded zero calls, confirming these
observed STI and EOI cases were handled by the combined candidate. These are
selected-object tests, not full real-guest or concurrency proof.

The candidate is **not publishable**: it intentionally does not decode
prefixes or string I/O, refuses paging, performs direct nonpaged byte fetch,
uses a non-atomic fixed-state read/modify/write, and unconditionally calls
PIC acknowledgement for each trapped STI. The relation of the latter to
concurrent event admission, original monitor delayed IRQs, AEOI and IRET
hooks still requires source-derived design and tests. Its word I/O follows
the observed NT4 size-2 dispatch but other operand-size behavior remains
unresolved. Do not modify guest or CCPU to paper over those boundaries.

Candidate source hash
`DAD853F7D812E26210C9203836404AD7DF897386CAF01FE108B753A2774FDF05`,
sealed package manifest hash
`AD8C48705294282907CFA7CDD2AB46DE611290075E0C652E2C1FC5B06D12A061`,
fixture source hash `C6DA254642024E3FF9527D0D886B9227012324C1A144D9EA671BBC9335E0A5F6`,
and four-case output hash
`8B86571FC5A41C2FC96FBA6BB2BA6A47DB38B3C5C173D6415FC9EEB08736DA3B`.

## Paging fetch: selected CCPU source audit

The selected CCPU's `xtrn2phy` is an existing external linear-to-physical
translation interface. Its source says it neither raises a page fault nor
changes TLB contents. Its access argument independently selects read/write,
user/supervisor, and whether to update page accessed/dirty bits: on a TLB
miss, setting bit 2 can write the PDE and PTE, while clearing it avoids
those writes. This matters because merely inspecting an instruction in the
host exception hook must not accidentally change guest paging state.

This interface is not itself a complete instruction-fetch solution. The
caller must check CS limits and arithmetic, request the correct privilege,
translate every page crossed by the decoded instruction, and handle failed
translation by returning to the existing fault path. The resulting physical
address must also be checked before querying `c_sas_memory_type`: that
function indexes `memory_type[addr >> 12]` without an internal bounds check.
Furthermore, `phy_r8` routes through the generic physical read dispatch;
it cannot be assumed to be a side-effect-free RAM-only read for arbitrary
memory types. These are source observations, not yet paging runtime proof.
The monitor candidate remains limited to its already documented nonpaged
experiment until these cases have executable evidence.

A selected-CCPU fixture now exercises `xtrn2phy` with an absent directory,
a mapped last byte of one page followed by an absent next page, and then a
different physical mapping for that next page. It also checks that access
request 2 leaves the page accessed bits clear while request 6 sets them.
All assertions passed, along with the fixture's existing protected-mode IRQ
case. This tests translation only: the candidate still does not use this
interface, and privilege, device-memory fetch and actual paged execution
remain unproven. The fixture and its output are temporary under
`build/t473-s11/paging-fetch-fixture`; it links one existing x86 release
decoder object to satisfy the selected fixture's current link dependency.

An additional selected-object observation exposes an important limit on
using this translation helper as an authorization check: with user-accessible
PDE `00005027` and supervisor-only PTE `00006003`, `xtrn2phy(3000h, 2)`
returned success and physical `6000h`. The selected source's external
translator combines the U/S bits with OR; its normal faulting translation
instead selects protection from the lower combined PDE/PTE permissions for
this case. Thus the adapter must not assume that successful `xtrn2phy`
alone proves the selected CPU would permit a user instruction fetch. The
probe measures the external helper; it does not itself execute an instruction
on this mapping or establish a product regression, and it does not authorize
a CPU change. An execution comparison and a host-side safe-fetch contract are
needed before using this helper in the monitor.

The execution comparison has now been run with the real selected CCPU:
a synthetic ring-3 code segment fetched from a supervisor-only PTE below a
user PDE produced #PF error code 5 (present, user, read). The test's fault
handler then granted user access, flushed the TLB, and the same instruction
sequence reached its `BEEF` sentinel. Together with the preceding helper
observation, this confirms that trusting `xtrn2phy` alone would bypass a
fault that the CPU actually raises in this case. It is an adapter interface
constraint, not a request to alter the CPU. The selected-object fixture
passed; actual monitor paging implementation and full regression remain open.

A further test-only fetch helper walks RAM/ROM-backed PDE and PTE words,
checks the selected CPU's documented protection selection for a user read,
and bounds-checks the physical byte before reading it. In the selected
fixture it rejects the supervisor PTE above without modifying the output,
while reading an accessible mapped RAM byte correctly. This is only an
experiment: TLB-cached mappings, external aliases, address wrapping, segment
limits and concurrent mapping changes have not been proven equivalent, so
this helper is not used by the product or the monitor candidate.

A subsequent selected-CPU probe established why a fresh page-table walk is
not sufficient either: after CPU execution populated the TLB, the test
changed the PTE to supervisor-only without invalidating the TLB. The existing
`xtrn2phy` returned the cached user mapping, while the test-only fresh-walk
helper rejected it. Therefore the helper above cannot preserve CPU semantics
for stale TLB entries. This is a synthetic observation, not a production
change; resolving it requires using the CPU's authoritative cached mapping
when present and its actual faulting translation rules on a cache miss.

An independent fixture linked the actual product exception hook rather than
the combined experiment. A ring-3 prefixed CLI straddled two pages; at its
real #GP callback, the test temporarily revoked both PTE user bits without
invalidating the TLB. `xtrn2phy` still returned both original user mappings,
and after restoring the test PTEs the guest reached its `BEEF` sentinel.
Output: `MONITOR_DECODE_CACHE fault=1 first=1 second=1 ax=beef`. This shows
the two bytes of this *decoded* instruction were cached before this fault;
it does not establish the same for all opcodes, instruction lengths, TLB
eviction, or reads of bytes not already decoded. No product change follows
from this witness alone.

The selected combined candidate was also exercised with master PIC automatic
EOI enabled. A pending IRQ0 was delivered at IP `1001h`; the real PIC's ISR
bit was already clear when the installed hardware hook observed delivery,
the guest handler completed, and the fixture passed. This is one AEOI
observation only: it does not establish slave cascade, concurrent resets,
or all historical monitor AEOI/IRET behavior.

The fixed guest state word cannot be treated as private scratch space for the
experimental monitor. Original `nt_eoi.c::BlockWOWIdle` modifies its
`VDM_WOWBLOCKED` bit under the ICA lock, while the selected CPU host's fast
BOP synchronization updates the real-mode bit through SAS; original guest
WOW assembly also contains locked updates of the virtual interrupt bit.
The monitor-context scalar `mvdm_monitor_ntvdm_state` is separate storage
and is not evidence that the guest word can be replaced with it. The current
candidate's ordinary whole-word read/modify/write can overwrite other bits.
An atomic update of only the virtual-IF bit is necessary to avoid a lost
update against other atomic writers, but would not by itself establish safety
against existing unlocked whole-word writes or SAS memory mapping changes.
The final integration must reconcile actual writers and their synchronization;
no product concurrency guarantee is claimed from source inspection alone.

The pinned NT4 `SetVirtualBits` confirms the monitor updates the fixed word's
virtual-IF bit with locked operations; the selected software CPU must also
retain its own execution IF state. A new build-only candidate therefore uses
an interlocked bit update under the existing ICA lock and leaves PIC
acknowledgement outside that lock. Its actual object passed the six selected
CCPU/PIC cases (pending, empty, masked, stale notification, auto EOI and
slave cascade). This is not a concurrency stress result and does not prove
unlocked SAS writers safe. The complete candidate executable did not link in
the formal fixture tree due to a missing redirector object; only its object
was linked into the test fixture, so there is no new integrated guest result.

The modified candidate was subsequently linked in the actual x86 worker
build directory and staged only in an isolated package. Its first runtime
attempt did not reach DOS4GW: the observer reported `result=exited` with
exit code `0x0000051a`, no graphics handshake and no committed frame. The
test script incorrectly printed an observation success because it checked
only the `result=exited` line, not the exit code. The temporary script has
been corrected to reject a nonzero exit; this run is a **failure**, not
evidence that the atomic candidate works in a real guest. The precise source
of the exit remains to be traced; no diagnosis from the numeric code alone is
claimed, and the candidate remains unpublished.

The `0x51a` staging failure was then traced to package identity: the custom
link output did not contain the `NTVDM_PACKAGE_ID` resource, whereas the
isolated package's other components did. The RPC package check in
`rpc_security.c` rejects a missing or different resource with
`ERROR_REVISION_MISMATCH`. Applying the existing package-identity tool to
the isolated candidate and updating its manifest allowed the same probe to
proceed. It subsequently recorded 82 staged/committed 640x400 frames, with
the observer reporting a 15-second timeout while the interactive program
remained active. That is frame-progress evidence, not a zero-exit test or a
proof of input/concurrency behavior. The corrected probe properly rejects
the timeout as a completion pass. No installed component was changed.

The isolated candidate now contains a limited nonpaged prefix experiment.
A real CCPU/PIC test of operand-size-prefixed STI delivered a pending IRQ at
IP `1002h` (after both instruction bytes), and the six previously selected
IRQ cases passed again. The change is not integrated into the staged package
used in the preceding 82-frame run. Paging, multiple prefixes, instruction
length limits, lock/repeat prefixes, operand-size I/O and full original
opcode dispatch remain unvalidated; this experiment is not release-ready.

A second synthetic STI used fourteen operand-size prefixes (15-byte total):
the real CCPU and experimental hook delivered the pending interrupt at
IP `100fh`, and the fixture passed. Source inspection also exposed an
off-by-one in the experimental immediate-I/O length check; it was corrected
in the temporary source. The candidate and test fixture were rebuilt and
all eight existing selected cases passed again; none exercises the changed
immediate-I/O boundary, so that particular fix is still unverified. The
observation does not settle the NT4 monitor's different prefix limit or prove
behavior for an overlong instruction.

The rebuilt selected CCPU/IOS fixture now runs four synthetic ring-3
scalar I/O pairs against a test-owned IOS byte device: immediate and DX
ports, each with byte and word width. All four completed with zero reflected
faults and exactly one or two real IOS read/write callbacks as appropriate.
This exercises all eight corresponding unprefixed IN/OUT opcode forms in the
experimental candidate. It does not verify port-specific devices, paging,
string I/O, 32-bit widths or prefixed immediate length boundaries.

A negative controlled case uses `66 e6 80` which this experimental
candidate explicitly declines. The preinstalled test fault callback observed
one #GP, the IOS device observed zero reads and writes, and the test callback
then advanced to a sentinel. This proves the selected fallback reaches the
registered callback without a device side effect; the callback's deliberate
skip is test machinery and is not proof of guest fault reflection or of the
correct final semantics for this prefix.

Original-source tracing of protected string I/O found an additional concrete
boundary. NT4 `OpcodeINSB/INSW/OUTSB/OUTSW` calls
`Ki386VdmDispatchStringIo`; the kernel tries its registered I/O handler and,
if that does not handle the operation, reflects `VdmStringIO` to the user-mode
monitor. Original `EventVdmStringIo` in `v86/monitor/i386/monitor.c` calls
`insb/insw/outsb/outsw`, then adjusts the 16-bit index according to DF and
clears the low CX for REP. The original kernel path explicitly warns that
32-bit index/count are not supported. By contrast the selected CCPU already
has its own string loops, width/address processing and page-fault behavior.
Calling those CCPU loops after temporarily changing IOPL would require a safe
single-instruction restore boundary; directly calling IOS requires preserving
segment translation, partial failures, DF and register progress. Neither is
proven by the scalar-I/O fixture, so the current candidate correctly does not
claim string-I/O support. This traces the original dependency and does not
authorize a 32-bit semantic substitution or a guest/CPU change.

The existing selected-CCPU port-privilege fixture was rerun for REP string
I/O at byte, word and dword widths. Its denied-port cases produced #GP with
zero device reads and writes. Its selected inaccessible source/destination
cases produced the expected #PF with zero device callbacks; when both port
permission and destination access were denied, the selected case produced
#GP first, again with no callbacks. All twelve runs passed their assertions.
This establishes the observed CPU ordering only; it does not establish the
new monitor candidate's string-I/O behavior. Fixture executable SHA-256:
`1BCD3784018005D018EA478CADAB4E1C0A4A20E650F34FD09267C74DB02CC6AB`.

An additional synthetic baseline ran the actual CCPU with permission to
execute `REP OUTSB` and `REP INSB` in descending direction. Two writes and
two reads reached the IOS test device; SI and DI each decreased by two, CX
became zero, and the two destination bytes matched the input device. This
shows selected CCPU direction/count behavior, not the unimplemented monitor
path: the fixture deliberately used IOPL 3 so no protected-I/O exception
was raised.

The existing CCPU fixture also exercised `REP` with count zero at byte,
word and dword widths. With port permission denied, it raised #GP before
any device callback; with permission granted, it completed without a fault
or device callback. All six selected cases passed. The original NT4 string
handler constructs its monitor event using count zero, and its user-mode
monitor calls the registered IOS string routine; that source fact alone does
not prove all devices suppress side effects for a zero count. No change to
original IOS or to the unimplemented experimental string monitor is implied.

A separate selected-object cascade case initialized both real PICs, queued
slave IRQ0 through master IRQ2, and dispatched its vector through a synthetic
guest handler issuing EOI to the slave and then the master. The candidate
delivered once at IP `1001h`, and both PIC ISR values were zero afterwards;
the fixture exited successfully. This does not cover concurrent resets,
masked cascade changes, auto-EOI combinations or arbitrary nested IRQs.

The same real-hook probe was extended to immediate-port `OUT` crossing the
page boundary. Both instruction pages were present in the TLB at #GP. With
the immediate-byte page initially absent, the CPU first delivered one #PF;
the test handler mapped that page, after which the CPU delivered one #GP,
both pages were cached, and the test resumed to `BEEF`. This establishes the
fault order for these selected synthetic cases, not a general decoder or
permission guarantee for all instructions.

## Provisional product hook integration

Following the owner instruction to connect the chain and iterate, the experimental nonpaged protected CLI/STI and scalar I/O dispatch has been inserted in the real CPU40 `host_exint_hook`, and registered as MVDM-HOST-DIV-473. The actual changed source was compiled and linked into an isolated NTVDM executable. A fixture linking that actual object passed 15 selected cases, including IRQ, AEOI, cascade, scalar I/O, controlled declined-opcode fallback, and the test-only concurrent WOW-bit witness. This is in-progress product source, not a completed original monitor or release candidate: paging, full prefix and string I/O semantics, failure/concurrency closure, and product-wide build/regression remain outstanding. No guest changed and no package was deployed.

The isolated package using the newly compiled product-source hook reached 74 committed 640x400 frames from the immutable DOS4GW program. The observer ended at its 15-second interactive timeout; this is visible frame progress, not a successful program exit or full acceptance. The package was isolated and not deployed.

The provisional product hook now attempts checked paged instruction fetch through the selected CPU's `xtrn2phy`; it checks page-table entries and RAM/ROM bounds before use. A real CCPU/PIC synthetic case with paging enabled reached the correct pending IRQ boundary and completed; the preceding 15 selected cases also passed. The current user-page precheck is conservative and can decline a stale-but-valid TLB mapping, so semantic parity remains incomplete. Do not release this implementation as complete; segment/prefix/string, memory aliases, failure and broader product tests are still required.

The actual provisional hook was additionally tested with paging enabled and a prefixed STI split across two guest pages. It delivered the pending IRQ at IP `2001h`, after the full instruction, and the guest handler completed. The test does not cover the known stale-TLB conservative fallback. The selected CCPU `c_tlb.h` exposes `xtrn2phy` but no public hit-versus-miss result; its TLB entries are internal. Therefore avoiding a separate authority without modifying CPU source still requires further design.

The earlier conservative stale-TLB rejection has now been removed in the provisional source. A minimal registered CCPU boundary accessor returns the existing cached read decision as hit, denied or miss without modifying TLB state; the host uses that decision before walking page tables. On a miss it applies the selected CPU's user permission conditions. A real CCPU test populated the instruction-page TLB, changed its PTE to supervisor-only without invalidation, executed STI and delivered the pending IRQ at IP `1007h`; the test passed and confirmed the PTE remained supervisor-only. This proves that selected stale-mapping case, not all TLB, paging and failure behavior. The CCPU change is an added observation boundary, not a change to its instruction or TLB update algorithm. Broader regression and release remain outstanding.

The provisional hook now also dispatches a restricted unpaged 16-bit string-I/O path with REP and the original user monitor's count/index update shape. A selected actual CCPU/IOS fixture with denied hardware I/O and DF=1 completed two writes and two reads, with SI/DI decremented and CX cleared. The original monitor passes the starting pointer to its string routine even when DF is set; the selected generic routine advances that pointer, while its register update decrements the index. The fixture intentionally checks this source-shaped behavior, which differs from the direct CCPU baseline. Paged string buffers, overrides, fault and partial-operation behavior remain incomplete; this is not release-ready. A later source edit to resolve each byte separately across physical pages has not yet been rebuilt.

The in-progress string event now uses the actual cached CPU access decision and page translation for read/write buffers; a paging-enabled REP INSB/OUTSB selected fixture completed the same two-device-operation case as the unpaged monitor path. The CPU and host objects rebuilt, and paged STI, cross-page STI, stale-TLB STI and both string cases passed. This is not a complete paging contract: accessed/dirty bit updates on a cache miss, concurrent remapping after preflight, page-fault/partial-operation semantics and segment overrides remain unresolved. The code remains uncommitted and must not be released until these are addressed and the full product gate passes.

The in-progress string monitor now invokes the selected original IOS string-function tables once per operation instead of substituting individual IN/OUT calls. It snapshots validated guest byte mappings and uses a temporary contiguous buffer for the IOS routine. A controlled fixture with independently registered specialized string read/write handlers confirmed one call to each handler, two bytes each, and the expected original monitor register update; generic-handler, paged and unpaged cases also passed before this last focused test. The temporary buffer and preflight change fault/partial-progress timing relative to a direct original pointer, and mapping lifetime and accessed/dirty behavior are not yet proven; this remains an unpublished candidate rather than a full semantic claim.

The in-progress string path now performs a checked accessed/dirty update for each translated page before calling the IOS string handler. A paged test using separate source and destination pages passed and verified source accessed and destination accessed/dirty bits along with the expected string operations and register results. This is not proof of failure-order equivalence: preflight may mark an earlier page before a later page fails, and mapping invalidation/lifetime during a callback remains open. Nothing has been published.

The protected NT4 string handlers use ES:DI for input and DS:SI for output even when the prefix decoder recorded a segment override; they also use the 16-bit index/count and byte/word size in the inspected source. The provisional dispatch now follows that source boundary instead of declining these prefixes. A selected test gave ES and DS different bases, used an ES prefix on REP OUTSB, and verified the output came from DS while the subsequent INSB wrote through ES. It passed. Source also distinguishes F2 `PREFIX_REPNE` from F3 `PREFIX_REP`; the provisional repeat decision was corrected accordingly. The earlier 66-prefixed scalar negative observation is superseded: this case is now handled and its selected test writes once. This is not proof of other prefix combinations or full exception semantics.

Protected STI in the provisional hook previously acknowledged the PIC without consuming CPU40's corresponding pending-event bit. It now takes that existing atomic event before acknowledgment, through the minimal registered DIV-314 boundary, so the normal CPU dispatcher does not later see the same stale notification. Both selected CPU libraries and the actual host candidate rebuilt; 24 fixture invocations passed, including pending, masked, stale, AEOI, slave cascade, paging, stale-TLB and string cases. This does not yet prove all concurrent interrupt arrivals or parity of the normal CPU dispatch's external-interrupt bookkeeping.

The handled-STI IRQ delivery has been moved into a registered CPU40 boundary function, so it applies the CPU path's external-interrupt marker, synchronized tick, hardware hook and saved EIP. It clears the handled contributory fault before dispatch, preventing a subsequent interrupt-dispatch fault from being treated as nested inside the already handled #GP. Both CPU libraries and the host candidate rebuilt and the 24 selected fixture invocations passed. This is a source-derived alignment, not yet proof of every nested-fault, AEOI, or concurrent delivery path.

Further review found that normal CPU dispatch saves the post-instruction EIP before constructing an interrupt frame, while the provisional handled-STI path only saved it afterwards. The registered CPU boundary now saves EIP before interrupt delivery as well, retaining the later save after delivery. Both CPU libraries and the host object rebuilt; the 24 selected cases passed. An actual fault during interrupt-frame construction has not yet been exercised, so the failure-path conclusion remains source-derived rather than a proven nested-fault regression.

An actual CCPU test made the pending IRQ's IDT gate not present. The handled STI delivered the IRQ, interrupt-frame construction raised #NP (vector 11), and the registered test callback observed the completed STI's IP `1001h`, rather than a double fault or the STI's original IP. The fixture callback then redirected execution to a sentinel. The PIC ISR remained set because the synthetic fault path deliberately did not execute an EOI; this is not a guest recovery/EOI test. The case passed and supports the specific saved-EIP/fault-nesting fix only.

The newest integrated candidate did not reach the graphical frame in a 15-second isolated DOS4GW observation; the prior candidate, run with the same package and observer, reached 80 committed graphics frames. The new process remained live, and one sampled CPU thread address resolved to `POPA` in the selected worker map; that sample alone does not establish the cause. This is a candidate regression against the controlled comparison. The current source must not be published until traced and corrected. Both observations reached the timeout, which is not a normal-exit pass.

A same-package A/B control confirmed the regression: the earlier candidate reached 80 committed graphics frames while the current one produced only a text frame during the same 15-second observation. A diagnostic build logging only instructions successfully fetched by the new exception path produced no such instruction log in that interval. This does not prove that the hook was never entered: a failed precondition or failed fetch would also omit that log. A broader entry trace is being prepared. The source remains uncommitted and must not be released.

The controlled regression was traced to repeated CPU40 #GP callbacks with error code 1 at the same guest CS:IP. The provisional hook had been incorrectly restricted to error code zero; CPU40 includes its external-event marker in bit 0 even when the selector/index bits are zero. Restoring the candidate's original `(error_code & ~1) == 0` condition allowed the same immutable DOS4GW workload in an otherwise identical isolated package to reach 80 committed graphics frames again. The observer stopped at its interactive timeout, not a completed guest exit. The test-only negative selector-error case now uses code 4, and 25 selected fixture invocations passed after rebuilding. This validates the observed regression and its correction, not the remainder of monitor integration or release gates.

The selected CCPU monitor boundary fixture is now repository source at `tests/mvdm-host/ccpu_monitor_boundary_fixture.c`, and the x86 worker Ninja generator has an explicit target linking the actual host exception-hook object before the fixture seams. `tests/observation/verify-ccpu-monitor-boundary.ps1` runs the 25 selected cases. The generated formal graph built the target and the script passed all 25 cases. This makes the existing focused evidence repeatable; it does not replace the required full package and guest regression.

The CPU's physical-memory reads apply `SasWrapMask` before resolving RAM. The new monitor's direct checked reads and string-buffer mapping initially omitted that mask. The provisional source now applies the same mask, including when comparing a checked page translation. A tracked selected-CCPU fixture with A20 wrapping enabled executed a protected STI from linear address 102000h backed by physical 2000h, delivered the IRQ at IP 2001h and completed. The formal Ninja fixture and all 26 scripted cases passed. This tests one wrapping path, not the entire SAS mapping lifecycle.

A source audit found the in-progress string handler did not check the CPU's cached segment access rights before touching an IOS device. It now declines invalid/unreadable sources and non-writable or code-segment input destinations before device calls, and recognizes the selected CPU's expand-down limit shape. The real CCPU fixture's read-only ES case reached the registered fault callback with zero device accesses; the callback then intentionally skipped to its sentinel. The formal Ninja target and 27 scripted cases passed. This is a safety/fallback observation, not proof that original NT4 emits the same guest-visible segment exception or handles every expand-down/wrap case.

The original NT4 decoder distinguishes F2 (REPNE) from F3 (REP) in its string monitor dispatch. A tracked real CCPU/IOS case now verifies the selected F2 OUTSB path performs one output while the subsequent F3 INSB repeats twice. The formal target rebuilt and all 28 scripted cases passed. This checks the inspected NT4 handler boundary and controlled callback results, not all x86 or device behavior.

The selected NTVDM CPU's normal accepted-IRQ path and handled-STI monitor path now share the same existing external marker, synchronization tick, hardware hook, guest interrupt dispatch and saved-EIP tail. A tracked control executes STI at IOPL 3 so it takes the normal CPU route rather than the new #GP route; it delivered the IRQ once at IP `1006h`, retaining CPU40's instruction-delay behavior. The formal 29-case suite passed. This refactoring keeps the normal non-NTVDM branch unchanged; full product and concurrency verification remain outstanding.

A closer audit corrects the earlier prefix claim. In the pinned protected-mode `instemul.asm`, dispatcher EBX starts at 1 and `OpcodeGenericPrefix` increments it; the prefix macro ORs the flag into `RiPrefixFlags`, while the string handlers test the REP flag in EBX. The inspected path does not visibly copy `RiPrefixFlags` into EBX before those handlers. Likewise the CLI/STI handlers increment the trap EIP once, despite prefix decoding having advanced EDI. Consequently the current candidate's F3 repetition and full-prefixed-length advancement are not yet proven byte-for-byte equivalent to this source, and the previous F2/F3 test proves only the candidate's behavior. The original runtime behavior and whether another control path repairs those state values must be established before an original-fidelity claim; do not hide this as a passed test.

The same source routes 0F faults to `base/ntos/vdm/x86/vdmop0f.asm` for LMSW, CLTS and selected special-register operations. The provisional host hook currently declines these. This is an identified original-monitor coverage gap, not a proven actual DOS4GW use or authorization to change the CPU's independent 486 implementation.

The provisional host dispatch now connects the inspected NT4 `VdmOpcodeClts` behavior: the protected CLTS operation advances without changing CR0. A tracked selected-CCPU case entered with CR0.TS set, executed CLTS at ring 3 and verified the TS bit remained set with no device operation; the 30-case formal fixture suite passed. This covers only CLTS. LMSW, special-register operations, prefixed 0F details and actual original-runtime comparison remain open; no full 0F parity or release is claimed.

The provisional 0F dispatch now also handles register-form `MOV CR0,r32` and register-form LMSW by passing only MP/EM to the selected CPU40 virtual CR0 and preserving PE, paging and TS. Original `KiVdmSetUserCR0` stores the VDM's NPX control state in the VDM context and native thread FPU state, whereas CPU40 exposes its own guest CR0; this is a source-shaped adaptation, not proof of equivalent multi-task or NPX lifecycle. Two tracked ring-3 cases passed, alongside all 32 selected fixture cases. Memory-form LMSW, reading control registers, other special-register opcodes, guest NPX interaction and actual cross-task behavior remain open.

The provisional 0F dispatch now handles register-form `MOV r32,CRx`: selected CR0 reads use CPU40's guest CR0 with TS hidden, and other selected CR numbers return zero, following the inspected monitor's branch structure. Two tracked CCPU fixtures verified the CR0 and CR3 branches, and the full selected 34-case fixture passed. Original NT4 derives its non-NPX CR0 bits from the physical host processor and NPX bits from per-thread native FPU state; CPU40 exposes virtual guest state instead. The equivalence of those non-NPX bits and cross-task NPX state has not been established, so this remains an unapproved-for-release implementation boundary, not a claim of exact 0F parity.

The provisional monitor now connects 16-bit memory-form LMSW address calculation, segment selection and guest word reading to the selected CPU's register/segment state and existing checked paging access. The existing effective-address adapter translates a selector/offset but does not decode the instruction's ModR/M byte, so this remains a new host-side decoder, not a direct call to the original kernel `VdmDecodeOperand`. A tracked direct-displacement case passed and the 35-case suite passed. Other addressing forms, paged/denied operands, 32-bit address override and precise fault ordering remain unverified; source and fixture are not release-ready.

Two additional tracked LMSW memory cases passed through the actual CCPU and host hook: BX plus signed displacement, and BP+SI with DS and SS based at different locations to verify the default SS selection. The 37-case selected suite passed. This does not yet prove all ModR/M and segment-override combinations, paged failure semantics or 32-bit address decoding.

The in-progress memory LMSW decoder now includes 32-bit address-size forms, including SIB scaling, displacement and default SS selection for ESP/EBP bases. Tracked actual-CCPU cases for absolute disp32 and EBP+ECX*4 with DS/SS at different bases passed; the 39-case focused suite passed. These tests establish selected address calculations only. Original-monitor prefix ambiguity, all ModR/M cases, memory fault/partial progress, per-task NPX state and product-wide validation remain open.

A tracked paged LMSW test leaves the operand page absent. The provisional monitor declines the operation, the test's registered #GP callback observes it, no device I/O occurs and MP/EM remain unchanged; its callback deliberately skips the instruction afterward. The selected 40-case fixture passed. This proves no fabricated LMSW success in this fixture, not that the resulting guest-visible #GP versus page-fault behavior matches original NT4.

The provisional 0F dispatcher now handles register-form debug-register reads and writes using CPU40's selected debug register storage and MOV_DR provider. It applies the inspected NT4 masks: DR0-3 limited to 24 bits, DR4/5 reads zero and writes ignored, DR6 legal bits, and DR7 global-to-local translation plus legal-bit mask. Three tracked selected-CPU cases cover DR0 truncation, DR4 read and DR7 global/local conversion; the full 43-case focused suite passed. Cross-thread debug-context ownership, hardware breakpoint behavior and all other 0F cases remain unproven; this is not release qualification.

A real CCPU execution-breakpoint test exposed a bug in the provisional DR-write path: DR0/DR7 and the original breakpoint table were updated, but a breakpoint on the immediate next instruction did not fire. Merely clearing quick mode, and then also setting `took_absolute_toc`, both failed the witness; those speculative changes did not solve it. Source tracing showed handled exceptions resume at NEXT_INST after the ordinary inter-instruction check. The CPU40 monitor boundary now advances/saves the completed instruction and invokes the existing breakpoint check before continuing, after ending the handled fault if it must deliver #DB. Tests for a breakpoint on the immediate next instruction and on the following instruction both passed, as did the 45-case focused suite. This is not yet evidence for all data breakpoints, RF/single-step, nested faults or multi-task debug ownership.

The focused fixture now also exercises a write data breakpoint, a CPL3/IOPL3 privileged CLTS, and an execution-breakpoint callback that redirects EIP. The latter exposed an unconditional EIP write after the callback; the candidate now preserves the callback's result. The dispatcher also distinguishes privileged 0F instructions from I/O and interrupt-flag operations governed by IOPL. All 47 selected cases pass. These results do not settle original prefix behavior, virtual NPX ownership, all exception ordering, or product-wide compatibility.

The first full component build compiled the x86 and x64 targets but failed its redirector import gate before incremental verification. The generated provider definition names `session_thread_finish` at ordinal 219 and the real VDMREDIR import table contains ordinal 219; the gate incorrectly required a name import. The gate now verifies the provider's generated ordinal against the selected ntvdm import table, also accepting a named import. This fixes the gate, not a product runtime defect. The repeated full build and incremental verification are recorded separately and must pass before release.

The repeated cached component build completed with ten x86/x64 images and 1,536 unchanged intermediate objects on the second pass (`build/t473-s11/cpu-monitor-build-r2/steps/result.json`). A coherent stamped candidate package has been staged from the prior complete immutable guest package and the current build. Full product verification is running against that candidate; no release or Full pass is asserted until its result is recorded.

The first executing Full attempt passed WOW frontiers, Console-17 and Window-17, then its RPC fixture failed before creating a case because the runner was launched under Windows PowerShell 5.1, where `ProcessStartInfo.ArgumentList` is unavailable. The Full suite is being rerun with the previously used PowerShell 7; the failed attempt remains recorded as a test-host setup failure, not a product pass or a runtime defect.

PowerShell 7 Full completed all 16 gates on the pre-normalization package (`cpu-monitor-full-r4/timings.json`). Restoring the original LF line endings in `c_tlb.c` reduced its actual source diff to 29 added lines; the rebuilt NTVDM image hash changed, so a new sealed package was tested instead of transferring that Full result. This later run passed guest-media, exports, WOW frontiers and Console-17 but failed Window-17 `missing`: the observer sent seven letter key-down/up pairs and no Enter, while the screen showed `missING` and the full lowercase echo milestone timed out. The previous package's same case passed. The evidence does not yet distinguish input-state/timing failure from a candidate regression; do not call this run a pass or automatically dismiss the failure. Output: `build/t473-s11/cpu-monitor-full-r5/Window-17-missing.txt*` and `timings.json`.

The provisional monitor exception IDs were corrected to DIV-473 through DIV-476 because 312 through 315 were already assigned to other mirror differences. This is an identification change, not new behavior; final source/build identity and coverage must be refreshed after it.

The pre-normalization, LF-normalized and identifier-corrected NTVDM builds have identical `.text` SHA-256 `674f9c72498f6f4ba1b5602537a35f30a0bf15c9ef97e6255c0c6f311d48c41d` and identical `.data` SHA-256 `9ff6a9b3075db4d7b39a8f1f262808e31c278228ca6d36362ee19f875ecdd5ea`. The first two builds' `.rdata` differs at two bytes within the PE debug-directory timestamp; this is evidence of equal selected code/data sections, not a claim that the failure's cause has been found. The later sealed package is independently running Full; its Window-17 `missing` input-delivery witness has passed, but the earlier lowercase/uppercase mismatch remains recorded without attribution.

Full verification of the identifier-corrected sealed candidate subsequently completed all 16 gates (`build/t473-s11/cpu-monitor-full-r6/timings.json`, exit 0). This verifies those product routes for that exact package; the earlier input mismatch is still not attributed and the separately required Winmine gameplay and semantic-boundary review are not established by Full alone.

The same candidate subsequently passed independent Winmine flag/counter, reset, open-cell and normal-close gameplay, SOL deal/draw/normal-close gameplay, and WRITE close/restart/second-close witnesses (`build/t473-s11/cpu-monitor-{mine,sol,write}`). The witnesses prove those paths, not all task reentry, instruction prefixes, exception timing, or the historical uppercase input mismatch.

SOL Options opened and closed four times with parent recovery and launcher completion against the same candidate (`build/t473-s11/cpu-monitor-options/result.txt`, process exit 0). The final recorded round took about 4.3 seconds to show and 4.8 seconds to recover; this remains the separately queued multi-window performance issue, not an optimization claim.

A subsequent source review found that DIV-474's cache query did not consult CPU40's `FAST_TLB` first. A real CCPU fixture first reads linear page 2000 mapped to physical 8000, changes the page table without flushing, then writes the same page, creating a later dirty mapping to 9000. The CPU's fast lookup returned 9000, but the monitor's old slow-table iteration returned the earlier 8000 entry (`build/t473-s11/tlb-duplicate-baseline`, exit 1). The cache query now follows the CPU's fast lookup before its existing slow-table fallback. The same witness returns 9000 on both paths, and all 48 selected CCPU cases pass (`build/t473-s11/cpu-monitor-fixture-run-r4`, exit 0). This invalidates the previous package's Full results for the new source; rebuild and full regression remain required.

The rebuilt TLB-corrected ten-image package completed the 16 Full gates (`build/t473-s11/cpu-monitor-full-r7/timings.json`, exit 0). Application gameplay and lifecycle witnesses for this package are running separately; the previous package's witnesses are not attributed to this one.

The same TLB-corrected package passed the four independent application witnesses (`build/t473-s11/cpu-monitor-game-r4/runs.json`): Winmine flag/reset/open/close; SOL deal/draw/close; WRITE close/restart/close; and four SOL Options dialog open/close/recovery cycles. All four child processes exited zero. These do not prove all protected-mode monitor semantics or resolve the earlier Window-17 uppercase-input observation.

The existing `mvdm_softpc_guest_memory_acquire` is not a direct replacement for the NT4 monitor's string-I/O pointer: its session lease allocates a bounce buffer and reads/writes SAS physical addresses, whereas this call receives a protected-mode linear address whose page mappings may be noncontiguous. It also snapshots and commits, so merely switching allocators would not establish equivalent callback-time mapping behavior. The actual IOS monitor wrappers select the same string-function tables as the candidate; their generic bodies can call the device once per element. The remaining issue is the data/mapping and fault-lifetime contract, not an unidentified second device dispatch table.

An initial prebuilt NESticle/DOS4GW observation using `io-monitor-experiment/observed-ntcon.exe` with the current worker produced zero frame commits within 15 seconds, while the older `pm-atomic-candidate/package` produced 82. That was not an isolated worker comparison: the original package's NTCON and NTVDM had different code sections from those in the first new stage. In a controlled cross using the earlier successful observed NTCON and otherwise current package, the exact earlier package NTVDM produced 74 frame commits and the current TLB-corrected NTVDM produced 73 (`build/t473-s11/cpu-monitor-cross-observe/{oldboth,oldfrontend}/frames.txt`). The other observed NTCON with the earlier standalone NTVDM also produced zero. These are 15-second interactive observations; both launchers remained running and the scripts reported timeout rather than successful guest exit. They demonstrate progress through this workload with the earlier observation frontend, not full application acceptance, and the zero-frame result with the other observation frontend remains unattributed. No guest was modified.

The original protected-mode `OpcodeGenericPrefix` advances EDI and the length but not the trap EIP; `Opcode0F` then initializes `RiEip` from that original trap EIP, and `VdmOpcode0f` advances it once before decoding. With any leading prefix, that byte is another prefix or `0F`, not a supported second opcode, so the original entry declines the dispatch. The candidate incorrectly accepted prefixed 0F instructions. It now falls back to the original exception handler when a prefix precedes 0F; the earlier claims of original-equivalent 32-bit-address LMSW handling are superseded. Selected tests retain the two prefixed LMSW cases as fallback assertions and add a 66-prefixed CLTS fallback case; all 49 selected cases pass (`build/t473-s11/cpu-monitor-fixture-run-r5`). This source-derived boundary does not resolve the separate REP and prefixed CLI/STI ambiguities, and the changed product source needs a fresh build and product regression.

The 32-bit-address LMSW decoder remains reachable without a prefix when the code segment itself defaults to 32-bit address size. A selected host-hook case sets that code-segment attribute and verifies an unprefixed absolute 32-bit LMSW address, its CR0 result and next EIP. The complete focused suite now passes 50 cases (`build/t473-s11/cpu-monitor-fixture-run-r6`). This is one addressing form and does not prove every 32-bit effective-address or operand-fault case.

The preceding conclusion about default 32-bit addressing was incorrect and is superseded. Source recheck shows `VdmDecodeOperand` selects 32-bit addressing solely through `RiPrefixFlags & PREFIX_ADDR32`; the selected protected `Opcode0F` assigns that field from EBX, which carries the small instruction length, and does not add the code segment's default-size bit. Thus the selected unprefixed dispatch uses the 16-bit ModR/M form even for a 32-bit-default code segment. The candidate now follows that source rule and removes its unreachable 32-bit and segment-override branches. Case 51 now verifies DI-based addressing and next EIP 1003 in a 32-bit-default code segment rather than the earlier candidate's displacement32 and EIP 1007. This identifies the original source rule, not evidence that every operand fault path is equivalent.

Original protected `OpcodeCLI` and `OpcodeSTI` increment the trap EIP once even when the dispatcher saw prefixes. The candidate previously advanced over all prefix bytes before dispatching a pending interrupt. It now advances once; CPU40 then resumes through any remaining prefix/opcode bytes. Real selected-CCPU tests cover one prefix, fourteen prefixes, and a prefix crossing a page boundary, with the interrupt observed at the original source's EIP; all 50 focused cases passed (`build/t473-s11/cpu-monitor-fixture-run-r9`). This changes an unpublished candidate, not a delivered baseline, and requires fresh product validation.

The protected-mode prefix ambiguity is now resolved at source level: `OpcodeGenericPrefix` only increments EBX's length, while `OpcodeINSB/INSW/OUTSB/OUTSW` test EBX (not `RiPrefixFlags`) for `PREFIX_REP` (`0x00040000`). For a valid short instruction that flag is therefore clear, including F3. The candidate had instead repeated F3 transfers and cleared CX. It now passes a count of one and preserves CX for this monitored path, while retaining the distinct ordinary CPU behavior when IOPL permits native REP execution. The selected CCPU fixture verifies both behaviors, including paged and device-specific string handlers; all 50 cases pass (`build/t473-s11/cpu-monitor-fixture-run-r10`). This records the inspected NT4 source behavior, including its discrepancy from a normal native REP instruction, rather than asserting a hardware-semantic improvement. Earlier candidate F3-repetition conclusions are superseded.

The rebuilt source-aligned candidate passed all 16 Full gates in `build/t473-s11/cpu-monitor-full-r9/timings.json` (process exit 0). Independent application witnesses against this package are running; older package results are not substituted for them.

The same package also passed Winmine flag/reset/open/close, SOL deal/draw/close, WRITE close/restart/close and four SOL Options cycles (`build/t473-s11/cpu-monitor-game-r6/runs.json`, all exit 0). Subsequent code review used the now-established single-operation, byte-or-word bound to replace two per-dispatch heap allocations with two-element mapping storage and an aligned two-byte stack buffer. This removes allocation failure and callback-unwind heap leaks without changing the selected count or IOS call; all 50 focused cases pass (`build/t473-s11/cpu-monitor-fixture-run-r11`). Because the product source changed, Full and the application witnesses must be repeated on the resulting package.

The stack-buffer candidate passed all 16 Full gates (`build/t473-s11/cpu-monitor-full-r10/timings.json`, exit 0). Subsequent review found that an input handler receives the existing guest bytes in the original monitor, but the candidate's temporary input buffer had not been initialized. An actual CCPU fixture with an input handler that leaves its destination untouched reproduced corruption: guest byte 37h became 6Ch (`build/t473-s11/string-preserve-baseline`, exit 1). The candidate now initializes both input and output buffers from the guest before dispatch; that fixture preserves 37h and the full 51-case focused suite passes (`build/t473-s11/cpu-monitor-fixture-run-r12`, exit 0). The source change invalidates the preceding Full for final-package acceptance; rebuild and repeat the product gates.

The rebuilt input-preservation candidate subsequently passed all 16 Full gates (`build/t473-s11/cpu-monitor-full-r11/timings.json`, exit 0). Its independent application witnesses are running separately against that exact staged package.

This selected partial CPU-monitor baseline subsequently passed the independent Winmine, SOL, WRITE and four-cycle SOL Options witnesses (`cpu-monitor-game-r8/runs.json`, all exit 0). A diagnostic-frontend NESticle/DOS4GW run recorded 77 graphical commits in its 15-second observation; the launcher remained live, so this is not a completed-program pass. The owner-authorized interim publication copied the exact ten-image candidate to both `assets/release` and `O:/winnt/system32`, verifying all 20 hashes (`../cpu-monitor-baseline/publication.json`). Full timings, runtime and package manifests, application results, fixture output and frame observations are retained under `../cpu-monitor-baseline/`. The source's earlier provisional comments reflect the still-incomplete semantic review; this publication is a tested partial baseline, not S11 closure or a claim that all mapping/fault/NPX/debug semantics match NT4.

The separately retained S12 diagnostic observer and private-desktop hook compile successfully as test-only x64 artifacts (`build/t473-s11/s12-probe-check`, exit 0). Their compile is not a production input and does not prove the planned S12 handoff behavior. Documentation governance also passed (`build/t473-s11/cpu-monitor-doc-gate`, exit 0).

The same package passed Winmine flag/reset/open/close, SOL deal/draw/close, WRITE close/restart/close and four SOL Options cycles (`build/t473-s11/cpu-monitor-game-r8/runs.json`, all exit 0). An additional unchanged NESticle/DOS4GW observation using the previously successful diagnostic frontend recorded 77 committed 640x400 frames (`build/t473-s11/cpu-monitor-nesticle-r8/frames.txt`), comparable to the 73–74 frame cross-control. The launcher was still live at the 15-second interactive deadline; this is progress evidence, not normal-exit acceptance. The diagnostic frontend is not the release NTCON. The earlier uppercase-input and other-observer zero-frame events remain unattributed, and unhandled mapping/fault boundaries remain partial rather than claimed complete.

Post-publication review of NT4 `VdmDecodeOperand` found a further LMSW mismatch: its 16-bit register values are zero-extended and accumulated with the signed displacement in 32-bit EBX, then the default segment base is added without truncating the sum or checking that default segment's limit. The published adapter truncated to 16 bits and enforced that limit. A selected actual-CCPU/host-hook case with BX=FFFFh and SI=1 expected the source-defined address 10000h and failed against the published code (`build/t473-s11/lmsw-address-baseline`, exit 1). The candidate now uses the 32-bit accumulation and default-base behavior; the same case and a signed negative-displacement case pass in the 53-case suite (`build/t473-s11/cpu-monitor-fixture-run-r14`, exit 0). Invalid/non-readable selector and the linear 4-GiB boundary still fall back; this is not a claim of complete NT4 operand-fault equivalence. This change is not yet published and requires full validation.

`VdmOpcodeLmsw` reads a dword from the decoded address before `KiVdmSetUserCR0` masks the value. The candidate previously checked only the lower word and incorrectly reported success when bytes 3–4 lay on an absent page: the real selected CCPU fixture reported `handled=1`, EIP=1005 and changed CR0 (`build/t473-s11/lmsw-crosspage-baseline-r2`, exit 1). The candidate now validates and reads all four bytes; with the next page absent it declines the handler without advancing EIP or changing CR0, and with that page present it succeeds. All 55 focused cases pass (`build/t473-s11/cpu-monitor-fixture-run-r16`, exit 0). The fallback's eventual #GP versus original page-fault/exception ordering and accessed-bit side effects remain to be proved; this test establishes rejection of false success, not exact fault equivalence.

The intermediate address-only package completed 16 Full gates (`build/t473-s11/cpu-monitor-full-r12/timings.json`, exit 0), but predates the four-byte cross-page correction and is not evidence for the final group. The combined source was rebuilt and staged for a fresh Full run.

The LMSW boundary group was rebuilt and published as a partial baseline. The selected CCPU fixture passed 55 cases, the product Full suite passed 16 gates, and four application witnesses passed. The ten-component package was copied to both release destinations and all 20 hashes matched; supporting manifests, source hashes and results are in `../lmsw-boundary-baseline/`. The cross-page missing-operand case declines the operation without changing CR0; exact original page-fault propagation and mapping/reentry remain open S11 work.

A separate 15-second prebuilt NESticle/DOS4GW observation with the earlier diagnostic frontend and this worker produced 81 frame commits. Its interactive workload did not exit within the observation, so the probe exited 1 and is not an application-exit pass. It does not validate the exact released frontend. Z: was removed; no guest was modified.

Further adapter review found a second, independent authority for page permissions: `monitor_translate` manually chose the numerically smaller PDE/PTE flags before calling the selected CCPU translation. The selected CCPU uses its own combined access rules and its TLB remains authoritative. With a user PDE and supervisor PTE, a real CCPU fixture showed the adapter rejecting a mapping accepted by the CCPU (`build/t473-s11/mixed-permission-baseline`, case 57, exit 1). The adapter now leaves permission decisions to `xtrn2phy`, retaining only safe PDE/PTE memory and presence preflight. Cases 57 and 58 cover both mixed-permission directions; all 57 selected cases pass (`mixed-permission-fixed-r2`). This is alignment with the selected CPU implementation, not a claim that every combination matches physical x86 or NT4. The rebuilt candidate passed all 16 Full gates and four application witnesses; all ten components were published to both destinations with 20 matching hashes. The negative and positive fixture, source hashes and release receipts are in `../mixed-page-permission/`.

NT4 `trap.asm` distinguishes a guest-mode page fault (`VdmDispatchPageFault`) from an unresolvable fault inside kernel VDM support (`Kt0e8`). The latter restores the user trap frame and dispatches a user access violation; it is not evidence that this monitor operand fault should be injected as a guest #PF. The current safe refusal returns through the existing #GP handler, so this failure contract still differs and requires the complete exception-delivery chain before it can be called equivalent.
