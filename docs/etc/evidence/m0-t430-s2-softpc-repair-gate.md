# T430 S2 SoftPC repair gate

## Question and authority

Owner requires original guest defects retained; original MVDM/OpenNT host
defects may use existing SoftPC fixes only, otherwise TODO; attributable
project-code defects require designed corrections. S1 remains closed.
S2 admits the conditional stack profile, not all subsequent stages at once.

## Inputs and procedure

Baseline S1 ee57f6d64 and production5b9931b8e/T429 unchanged. Read-only commands:
`git -C O:/repos.hobby/softpc show --stat --oneline
ce5f53515d3e6ce0a64e66a5465aa7a8fbca00f7`, then `git show` of that revision
limited to call.c, ret.c, iret.c and keyba.c. No sibling writes/build, binary
import, product launch, process termination or publication.

## Observations and conclusion

The commit replaces operand-sized SP/ESP selection after loading new SS with
existing set_current_SP(new_sp) in CALLF, outer RETF and outer IRET. It also
assigns code_to_send=input_port_val for 8042 C0 and removes JOKER restriction
around output-buffer-read IRQ1 deassertion. These are actual existing patch
bodies, not conclusions drawn only from a commit title. Other changes in that
commit are not admitted for copying.

S2 may adopt the same stack correction with minimal registered diff and
actual-instruction tests. S3 has a matching candidate but must verify this
product's PIC ordering. Neither observation proves product runtime behavior.

## Mixed-origin obligations

- K02: CX-1 underflow predates our adaptation. Without a matching SoftPC fix
  it stays deferred; the added Unicode/OEM conversion and bounded copy need
  their own caller-capacity contract tests. Do not fix originals incidentally.
- K03: original ReadFile writes guest data before completion result words;
  our staging defers data copy until after them. This project-added boundary
  requires failure/callback design and tests, not an original-defect exemption
  or an assumed new atomic-transaction contract.
- K04: retain original valid-buffer/no-error-return ABI; verify added copy/
  conversion, do not invent CF/AX.
- K01: missing project service binding is an integration gap; recover the
  original service if required, not a rewritten fallback algorithm.

Known original VGA/guest limitations remain in TODO. Unknown WRITE cause
stays with the WOW diagnostic owner, not classified as an original defect.

Current sibling src filename search found no vrnetapi.c/vrnmpipe.c/demsrch.c
counterparts. This establishes no available matching fix in the inspected
tree, not proof that none ever existed. The inherited CX0 issue is therefore
recorded as deferred TODO. Current sibling VGA do_new_cursor still bases
visibility on scan-line geometry; the previously recorded bit5 limitation is
not newly fixed here. No unrelated VGA-history patch is adopted.

## Delivery boundary

### Source recovery ledger

All paths below are ccpu386 files. Rung 1 (original OpenNT) has the faulty
operand-size selection after load_SS_cache. Pinned SHA256: call.c
CE5F56EA27239D0C85243CEE23E01DE34B1F82D3BA92ECED687E2B3BD90A2A94;
ret.c 9531F24AC58029B3A7EB768A49B8E99242B6E9D9FBC5825B6982AE53687688F5;
iret.c 091378AB6583C06E7593B5B34F336531D874E695BE633596D652EA42F5446EB3.
Rung 2 (existing historical repair) is SoftPC
ce5f53515d3e6ce0a64e66a5465aa7a8fbca00f7, replacing precisely those three
sequences with set_current_SP(new_sp). Rung 3 (existing compatible local
provider) is unchanged c_stack.c::set_current_SP; it selects GET_SS_AR_X.
Rung 4 (project binding) requires no new adapter: direct use of that original
helper, registered as MVDM-HOST-DIV-327. No validation or guest policy changes.

Pre-repair main blobs at 699a9cb86 are call.c
d6d551c039cbc6dab066e72452243eef3913a537, ret.c
0ef33ab5f2241e13c8c70ec925c9375184e1bd5d, iret.c
7a1d257390fd92ad35cda36d4dbd78d7f38a2d48. Existing fast-BOP/report hooks
are not imported, rewritten or removed. The source patch contains three
helper substitutions, the same semantic patch as SoftPC.

### Focused results

build/M0-T430/S2/r001/before.txt records 24 actual-entrypoint cases with six
expected failures: all three instructions in the two width-mismatch cases.
The original matching-width positives and illegal-SS negatives pass.
after.txt records the same 24 cases with zero failures. The expanded final
profile-final2.txt records 36 cases, zero failures, adding target-stack/code
limit negatives and checking complete parameter frames and return adjustment.
Segment-selector stack slots assert their defined low16 bits: original
spush16 reserves a dword for operand32 but leaves its upper word unchanged.
That source behavior is preserved, not rewritten to satisfy a test.

The link map identifies CALLF/RETF/IRET in original-ccpu386 call/ret/iret.obj,
not fixture substitutes. Fault interception checks original CPL, SS, CS,
ESP and restart EIP before the real exception delivery boundary. It does
not assert complete IDT delivery, which is covered separately by existing
halt/reset tests. high-linear.txt, halt-reset.txt and thread-lifecycle.txt
retain successful original CCPU/SAS, debug/fault/reset and thread proof.

The first ancillary build found obsolete CPU-only fixture imports for ten
new broker client entrypoints. The test-only ccpu_host_fixture_seams now
declares their real types and exits ERROR_CALL_NOT_IMPLEMENTED if reached;
it does not contact NTSRV or pretend broker success. The rebuilt original
halt/reset and lifecycle fixtures pass. No production broker code changed.

### Admitted finite profile and implementation work

Owner directs implementation after the planning gate. The predicate is a
protected-mode privilege-changing CALL gate / outer RETF / outer IRET whose
operand width and destination SS address width are independent. Stack and
control groups cover both widths, parameter frames, return adjustment,
high ESP preservation/replacement and real validation faults before commit.
Scalar arithmetic and RMW/string instruction groups are inapplicable: this
recovery changes no arithmetic, data access, opcode dispatch or string loop;
only the post-validation stack-pointer installation is replaced with the
existing source-owned set_current_SP helper. VM-return IRET already uses it.

The new ccpu-stack-transition-test links production CCPU, SAS and descriptor
providers, invoking actual CALLF/RETF/IRET entrypoints. Its test-only host
exception hook observes faults before delivery, without replacing validators.
This is entrypoint proof, not a claim of complete guest decoder integration.
Build intermediates remain in the accepted formal cache; S2 run evidence is
stored below build/M0-T430/S2. No runtime package is published until the full
profile and required production gates pass.

The preceding policy/planning P changed no production source. This admitted
implementation adds the three existing SoftPC substitutions and the focused
tests above. Production runtime/publication gates are recorded separately
below; no closure follows from compilation or the fixture alone. Unrelated
side-session proposal remains excluded from this work.

### Product delivery

The current formal cache build/M0-T427/S2/r001 regenerates its source manifest
and relinks the affected x86 /MT CCPU40 targets (production-build.txt).
r002-runtime starts from the accepted T429 eight-file runtime and replaces
the six EXEs with these linked targets; unchanged WOW32/VDMREDIR and guest
media retain the accepted identities. APP/RPC/I/O versions are unchanged.

Invoke-ProductVerification.ps1 runs serially with that exact runtime, cache,
Observer build/M0-T427/S4/r049/console-startup-observer.exe, WindowObserver
the sibling worker-window-snapshot.exe, G7 fixture M0-T425/S9/r008/G7.COM,
and WOW baselines T429/S8/r009/product plus T427/S3/r003. r003-product passes
Console17 and Window17, including COMMAND/MEM/EDIT/direct/nested exits and
guest witnesses. Independent WINMINE/SOL/WRITE frontiers are unchanged.
226592ms total: WOW68602, Console74317, Window76733. This is compatibility
proof, not a performance improvement or SOL/WRITE usability claim.

r004-handoff uses verify-broker-io-handoff.ps1 for native, nested-console
and nested-window. All three pass actual input, parent resume, exit23 and
Window CAF where applicable, with unchanged package hashes and Z: removed.
r005-publication retains the previous eight-file package in recovery and
records equal candidate/published SHA256 for all eight system32 files.
NTVDM SHA256 is E175734CA3EAEAE6D41E3492D29034C6125FC50EF9E2FAA0FF021CB15AB7527F.
r006-published-smoke passes MEM and native-zero in both Console and Window
using the actual O:/winnt package. Z: is removed. S2 is boundedly delivered;
T430 remains open, with the later stages unimplemented.

The test contract remains bounded: actual entrypoints and production guest
regressions, not an exhaustive opcode/protected-mode/WOW recovery claim.
No original guest defects or unrelated sibling CPU/8042 changes are fixed.
