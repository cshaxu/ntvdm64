# T484 S3 P216: retire historical compiler replay

## Decisions

The broad subprocess/compiler-form review found six additional Python build
entries outside the frozen355 inventory. Four are frozen T480 audit replay:
capture-host-mirror-preprocessing.py, capture-t480-s3-environment.py,
capture-t480-s3-objects.py and verify-t480-s2-preprocessing.py, all formerly
under tools/audit. They replay commands from old evidence and restrict outputs
to old task directories. No current test/build caller selects them. Retire them,
retain the old results, and replace two historical Markdown links with source
names so history does not imply a current executable tool.

Retire test/observation/verify-dpmi-swint-diagnostic-source.py. Its GCC-generated
body fixture asserts report/target callbacks that the current DpmiSwIntHandler
no longer invokes. Keep and run verify-dpmi-swint-selected.ps1, which builds the
actual selected dpmiint.c and exercises absent/v86/protected stack failure,
handler/app width, stack limits, invalid aliases and fault dispatch/rollback.
No deleted diagnostic counter is represented as a current required behavior.

Keep the behavior of verify-mouse-callback-reentry.py, but replace its Python/GCC
producer with verify-mouse-callback-reentry.ps1 and the mouse-callback-reentry
CMD/native target. Reuse its register/memory stubs and current-body assertions
in fixtures/mouse_callback_reentry.c. Existing SourceProjection selects current
push/pop/return and entry bodies using unique fixed anchors; source dependencies
invalidate configuration. Preserve nested return, independent registers and
condition mask, stack depth/balance, single return and16-bit stack wrapping.
The optional replay of arbitrary pre-fix source is retired; the T481 historical
negative-control evidence stays unchanged. No product/mirror/guest changes.

After removal, GCC/Clang/TCC/Watcom/compiler-process searches in interpreted
test/tools files have no executable hits. Other subprocess calls inspected in
collect-mirror-header-consumers, freeze-mirror-conditional and library-boundary
read Git/Ninja metadata, not compile objects. This bounded review supplements,
not replaces, the tracked native producer evidence or S4/S5 acceptance.

## Verification

Owned run: build/t484-s3-historical-compiler-retirement/run.ps1; process record,
logs and terminal result.json are retained there: PID33104,2026-11-24T14:46:58.6110519Z through14:48:44.8776394Z, exit0. New mouse target builds and
passes all retained current-body assertions. The original byte-store test stub
emits one MSVC narrowing warning; it stores low16 bits as two bytes exactly as
the previous fixture, and no product warning/change is introduced.

The real DPMI selected-object suite passes. tools/build-addon.cmd all builds
all14 authored addons; every raw output and assets/release copy matches the
current addon-manifest SHA256 exactly. Cached full host build and11raw/22
published host identities are checked in the same owned run. No original guest
rebuild, archive change, fresh Windows runtime/Full claim or redundant product
copy. Governance, tracker, parser and diff checks precede commit and push.

Frozen319/355 disposed,36pending and24 supplemental preparation chains remain.
Five historical tools are retired and one effective regression is migrated;
these supplemental discoveries do not alter the fixed355 denominator. S3 is
not closed by this delivery; configuration/staging obligations remain explicit.
