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
