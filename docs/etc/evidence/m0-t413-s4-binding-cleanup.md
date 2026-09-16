# T413 S4 — Direct original accessors and IP bookkeeping

## Request and inputs

Owner reopens T413 to remove autonomous accessor composition, then explicitly
adds the independent IP-wrap repair. Admission is 65aa091f6; implementation
baseline is fbb121d72. Source input is the selected pinned OpenNT MVDM mirror,
compared read-only with O:\repos.external\OpenNT\base\mvdm. No external source,
guest media, license or component ownership changes.

## Original recovery decision

Original base/inc/egacpu.h already declares all 38 getter/setter pairs with
the exact cpu/src/evid/vglob.c types. That unchanged 451-line original unit
explicitly says it replaces EDL context changes for external field access.
The selected full base/cvidc/evidgen.h subsequently macro-substituted those
calls back into generated Video slots. Binding original functions to those
older slot types caused the unnecessary 20 wrappers.

Recovery ladder and actual composition:

1. Original vglob.c and egacpu.h compile and link unchanged, with the C-VID
   GDP profile. The original short five-slot evidgen.h would preserve direct
   field calls but cannot describe C_Video's full 81-slot layout: the latch
   setter offsets differ. Existing full-vector consumers cannot simply switch
   headers without violating that original layout.
2. Preserve the original full table and its latch/selector routes. A seven-line
   registered CPU_40_STYLE preprocessor seam in the full header disables only
   the obsolete field-dispatch macros. Calls now use the original typed
   declarations and original function bodies directly. No runtime conversion,
   new public ABI, copied field algorithm or replacement table is introduced.
3. The earlier overlay rebinding, generated declarations, assignments and all
   20 typed conversion functions are deleted, not relocated. Existing CPU/SAS
   composition is unchanged and is not claimed removed by this accessor task.
4. No newly authored accessor behavior remains. Zero mirror changes is not
   claimed: the finite header selection is an explicitly measured exception.

The final map resolves original accessor bodies from original-softpc-cvidc:
vglob.obj and contains no video_get_/video_set_ wrappers. The updated static
gate rejects reintroduction of the deleted binding generation.

## IP recovery

Retire DIV-125's x64-origin instruction-delta helper and restore the complete
original UPDATE_INTEL_IP, UPDATE_INTEL_IP_USE_OP_SIZE and page-advance
expressions. The USE16 result is (EIP + delta) & 0xffff, not EIP + (delta &
0xffff). Original scalar/RMW/string, branch, CALL and exception algorithms
are not rewritten. BuildCcpuIpProfile.mjs checks exact original macro identity
and page-advance spelling before generating tests.

The extracted original macros pass 168 host cases across both operand/CS sizes,
positive/negative spans, page boundaries and 16/32-bit overflow. The previous
expression fails the explicit fffe+4 negative control. Test-created MZ images
stay under build and execute through the actual published run16/worker pair;
they are not replacements for guest media.

## Build and verification procedure

Fresh root: build/M0-T413/S4/formal; x86 MSVC /MT, original CCPU40,
APP_VERSION 0.0.413 and protocol 3. Configure with
New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -BuildRoot <root>, then
run-ninja-parallel.cmd run16.exe basesrv.exe ntvdm.exe
cvidc-vector-binding-fixture.exe. A subsequent configure refresh reused the
same unchanged toolchain/objects and relinked manifest-dependent products.

The real owner-library fixture passes all 38 direct pairs at four values,
64-KiB memory guards, 32 scratch sentinels, original 81-slot layout, latches,
selectors, repeated setup and 100,000 concurrent refreshes. The old generated
setter still fails its deliberate scratch-clobber negative control.

BuildConsoleRegression.cmd, BuildCcpuIpProfile.cmd and BuildConsoleResize.cmd
place all observers/fixtures under build/M0-T413/S4/integration. Runtime
captures are only O:\winnt\logs. Prior products are recoverable under that
integration root's prior-products directory. The owner approved ending the
occupied package processes before final publication and tests.

Observed results:

- All 17 command cases pass: m0-t413-s4-command-summary.json.
- Four ConPTY sizes, mouse and live resize pass, with output mode 7, one EXIT
  echo and exactly one blank row after each final MEM report. Captures:
  m0-t413-s4-video-final-{80x25,45x34,60x50,120x30,mouse,resize}.raw.
- Native crop/grow/cursor and invalid-input contracts pass in
  m0-t413-s4-resize-native-final.txt.
- Initial nine functional guest instruction cases pass in
  m0-t413-s4-ip-final-summary.json: taken/untaken 16/32-bit branches,
  backward 16-bit jump, page-spanning immediate, wrapped CALL16 return address,
  scalar/REP/loop path and wrapped INT3 saved return IP.
- Supplemental CALL32 and BACK32 both pass in
  m0-t413-s4-ip-extra-summary.json: 11 functional guest cases total.
- Five final-hash prefilled 80x5 runs pass: m0-t413-s4-short-1 through -5.
  Each records buffer 80x300, viewport 0,36..79,40, cursor 0,40 and 40 prefill
  lines; all four post-EDIT per-command snapshots contain completed MEM output.
  All retain the original exit code 1. This proves 20 post-EDIT completions.

## Adverse evidence and explicit exception limit

The real-mode divide-by-zero fixture times out on both the prior S3 worker
and the S4 worker. Logs m0-t413-s4-ip-old-FAULT.txt and
m0-t413-s4-ip-final-FAULT.txt preserve this observation as a functional failure,
not a pass. Original c_xcptn.c::Int0 resets EIP to CCPU_save_EIP and, under
NTVDM, reaches an unconditional c_cpu_continue before real-mode do_intrupt.
That original block is unchanged here. The source and A/B demonstrate this
separate pre-existing profile boundary; neither restoring IP arithmetic nor
changing accessor binding fixes it. INT3 trap-frame IP does pass. Full fault
delivery compatibility is explicitly not claimed; the debt requires its own
source-led exception/host-hook profile, not an opportunistic handler rewrite.

The first command-matrix attempt overlapped the ending guest fixture's Z:
mapping; it stopped before running its matrix. The later serialized matrix
passes. Mapping teardown and unowned-package guards remain enabled.

The first ConPTY script incorrectly treated PowerShell's unavailable process
ExitCode property as failure despite the runner recording wait=0 exit=1.
It now checks that recorded wait/result plus native cell contents. All six
final runs pass; the earlier result is not silently relabelled.

The unchanged prior native resize fixture and its S4 build both initially fail
the cursor assertion in the current Terminal environment. It changed the
viewport after filling its source grid, implicitly assuming the host would
not resize/reflow the buffer. The corrected fixture establishes the viewport
first and asserts the actual 80x25 grid and 70,20 cursor before cropping.
The same crop/grow/data/cursor and invalid-input requirements then pass; no
product Console resize implementation is modified.

## Footprint

Normalized mirror census: CCPU +299/-57; C-VID +40/-21. Total 417 versus
S3's 419 and pre-T413's 506: S4 reduces distance by 2; whole T reduces it by
89. IP restoration removes 9 diff lines, the explicit header seam adds 7.
There are now 22 changed files in the two directories; restored files remain
restored, but evidgen.h is newly registered rather than hidden.

Four-overlay cohort: 136 to 125 lines, back to pre-T413 size. Binding source
returns byte-for-byte to its pre-T413 version. Generator source decreases by
11 net lines. Its runtime include decreases 389 to 213 lines: 176 removed,
including 76 declarations, 76 assignments, 20 wrapper bodies and four guards.
Remaining added generator logic produces test cases only. Tests/build tools
and documentation are separate, not presented as product-source savings.

Actual selected mirror-plus-overlay source net reduction is 5 lines in S4,
79 across the entire T, excluding generated output. This is distinct from
original diff-distance and from the 176 generated lines removed.

## Published identities

| Product | SHA-256 |
| --- | --- |
| run16.exe | 7312BE08BB217C31584129E3FA45BABD3EF6867A8DCB95E980C2824941BE9754 |
| basesrv.exe | 2FB5FFF67D047AD52B1DB610D4A0D2BA1DB5FEBB5AA9DECBE257DC42913396BD |
| ntvdm.exe | 35514F840B61D8FFE546626366764890C1AE13CE4461406A7D4779F579C6E4EB |

These O:\winnt files match formal outputs. User Terminal/RDP visual acceptance
is separate from automated checks and is not asserted as received.
