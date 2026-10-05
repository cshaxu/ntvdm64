# M0 T432 S4 cross-width installation progress

S4 reaches its bounded engineering conclusion; production P/publication is
deferred to the integrated S5 gates, not claimed delivered. Original T431
accepted nine-image package remains unchanged. Build root S4/r001; formal
cache M0-T427/S2/r001. Other-session proposal/TODO edits remain preserved.

## Source and implementation

One shared installer now selects actual target machine and the matching DLL.
Same-width uses the retained import update; opposite-width uses the official
matching rundll32 helper in the registered W-only seam. Context-only x86
launcher still receives no interceptor. Creation handles and caller suspension
remain caller-owned; the helper never executes/replaces the target.

[Adaptation register](../operations/t432-detours-helper-adaptation.md) records
original pinned hash and why the unmodified internal wait cannot be adapted
through its callback. Current creatwth.cpp SHA256:
391E7697B03BF47CA263E5FD4664284D677EC465D56F8ACBE96E73E129433507.
Only DetourProcessViaHelperDllsW changes: actual Windows path, explicit selected
DLL paths (no directory-name rewrite), single resume, finite10-second target/
helper wait, exact-owned helper cleanup and concrete failure. Import update,
restoration, AllocExeHelper and other source bodies remain unchanged.
No original OpenNT/MVDM mirror body changes or new installed executable.
Hook entry skips interception in the official helper; original helper callback
is exported at ordinal1. MIT notices remain unchanged.

## Runtime evidence so far

- build32.log/build64.log and formal-product-build.log: both native families
  and the affected formal product closure link. Formal source graph/export
  changed, so its integration package must be regenerated for the final path
  correction before final product/P evidence.
- attributes32.log/attributes64.log:201/203 assertions pass. Actual32-to64,
  64-to32 and alternating child/grandchild propagation, matching DLL before
  immediate descendants, A/W, explicit HANDLE_LIST, Unicode environment/CWD/
  streams, CUI/GUI authority stripping and actual nonzero Windows exit37.
  Includes retained same-width/context/malformed/missing/invalid-capability.
- Helper callback-only negatives pass in both widths: create failure5,
  real10-second timeout1460, target death1067 and helper nonzero completion1114.
  Failed helper is signalled before return; local handle count unchanged.
  Target remains caller-owned/suspended except the explicitly injected death;
  creator terminates/joins only its unpublished target. Poisoned fixture WINDIR
  does not alter the selected real loader. No production fault switch.
- r004-real-cross32: all ten original CMD legacy chains plus two actual32 CMD
  -> native64 CMD -> bare MEM/explicit Run16 COMMAND -> parent return pass,
  along with visible WINMINE startup. Actual output, exit0 and output-before-
  parent-marker remain mandatory. Uses accepted short-history geometry.
- r003-real-cross32 is a retained failing test-input run: newly nested quoted
  /c command yielded error9009 and no MEM, while exit0 alone did not pass.
  Observer CRT re-quoting supplied backslash-quotes to CMD's different parser.
  Cross cases now use the actual short Windows/Z paths, like existing cases,
  without embedded quotes. Both output/order assertions remain; no retry-to-
  success loop or product workaround. Windows paths with spaces are not proved
  by this observer form. Host CMD control comparison was successful separately.

Reproduce fixture with architecture run-ninja.cmd nthook-install-test.exe and
the executable's --opposite-fixture actual opposite executable. x64 also needs
--launcher-fixture actual x86 fixture. Both folders contain pinned current x86
Run16 for exact launcher file identity. Full original assertions are retained.
The path32/path64 runs pass212/214 assertions, including deliberate
helper32.path/helper64.path directories and successful-helper local-handle
checks. After the actual-process path correction below,
process-hook32/process-hook64 retain212/214 passes; rollback32/rollback64
pass228/230, additionally proving second-capability duplication failure and
wrong opposite-width DLL failure. Recipient handle counts do not grow; the
actual target remains alive and suspended until its creator rolls it back.

## Actual-process image identity correction

The existing Hook reopens QueryFullProcessImageNameW's DOS path for subsystem
and pinned-launcher identity. An x86 caller querying an actual AMD64 System32
CMD then reopened the I386 SysWOW64 image through WOW64 redirection.
process-path-before.log proves actual8664 versus DOS-open014c repeatedly.
The fixture must create using the original Sysnative spelling, not a normalized
DOS filename that would itself redirect; the first diagnostic run exposed
that fixture-input error before the retained reproducer.

Common's existing native image-section classifier now also accepts a borrowed
actual process through common_process_image_path/common_classify_native_process.
It obtains PROCESS_NAME_NATIVE and reopens through the documented GLOBALROOT
namespace. Hook classification and pinned-launcher file comparison use this
same path mechanism. No caller redirection is disabled, no second PE parser,
search, execution registry or protocol is added. Original DOS/WOW classification
and OpenNT/MVDM bodies remain unchanged. NTSRV's queried DOS image is only
display text, not a reopened image classifier; launcher pre-create resolution
still intentionally uses the caller's namespace.

Primary API contract: [QueryFullProcessImageNameW](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-queryfullprocessimagenamew)
and [NT namespaces](https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file#nt-namespaces).
process-path32/process-path64 each pass169 assertions on real suspended
SysWOW64/System32 CMD targets with zero steady-state handle growth. The x86
diagnostic DOS reopen remains intentionally wrong; the production native-path
query must identify the actual target correctly at both widths. These are
host metadata fixtures, not guest/Console handoff acceptance.

r005-runtime/r006-real-cross64 passed all twelve actual native64 CMD routes
plus visible WINMINE, before this additional process-path correction. They
remain an immutable intermediate result, not the final candidate identity.

## Remaining gates

process-final-generate/process-final-product logs record the affected formal
rebuild. r007-runtime is the fresh eleven-image final-source integration set.
r008-real-cross32 and r009-real-cross64 each pass the ten retained routes,
two actual reciprocal native child/DOS parent-return routes and visible
WINMINE startup. They use the admitted short-history fixture, not evidence
that the default private-desktop geometry limitation is fixed. Z: is removed
by each serial run's exact-owned cleanup. The first metadata diagnostic's
suspended child PID25460 was separately pinned by parent17048, creation time
2026-10-05 12:37:52 and exact command line, then terminated/joined; unrelated
host CMDs were retained. The metadata fixture now rolls back its own current
child on assertion failure. machine-final32/machine-final64 each pass169 checks.

Source review covers the W-only helper seam, matching-DLL propagation,
context-only launcher, resource rollback and actual-process namespace binding.
Detours after hash above is unchanged by the project-owned path correction.
OpenNT/MVDM git diff is empty. No helper becomes a resident execution owner.

Retained product Console/Window/WOW,
broker/worker fault/reuse/isolation, coherent eleven-image publication and P
remain required. Positive helper cleanup is not normal worker retirement proof.
