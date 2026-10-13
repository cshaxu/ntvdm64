# T445 S5 NTW16S and W16Style component delivery

## Scope and baseline

Owner accepts S4 r261 and replaces the proposed static-core/DLL-wrapper split:
the entire core belongs to src/ntw16s-dll; src/w16style-exe owns the thin demo.
Both DLL widths and x64 W16Style.exe are release components. S6/S7/S8 retain
propagation/integration/final acceptance; no guest, mirror or existing product
consumer changes are authorized here. Baseline source is 65a530b32, accepted
demo SHA256 EB97CDF25C9D637E4C94C97C05A33BF488CBE0041CFE3C16374F707B96A5BE7E.

## Ownership and ABI

All frame/menu/control/font resources and algorithms migrate to NTW16S.
ntw16s.h is the public C interface; api.h declares its version and DLL import/
export boundary. HWNDs and font leases stay on their owning GUI thread.
Opaque control scopes are created/freed by the DLL, never by the EXE's CRT.
DllMain performs no USER/GDI work. Callers must retain the module until every
attachment, scope and font lease is removed and callbacks/calls have returned.
There is no unload from inside a subclass callback, forced teardown of another
GUI thread, global hook, worker thread or second style implementation.

W16Style creates the existing 27 sample forms, native content/data and scroll
responses; automatic tests, capture and fault injection stay below tests/.
It links only the DLL import library and host APIs. Its per-owned-window theme
selection retains S4's explicit caller contract, not arbitrary-app inference.
Internal menu-bar geometry is separated from its painter so oracle fixtures
can read layout without linking a second frame renderer. Private fault-injection
fixtures may compile the same source statically; no production consumer does.

## Attempts

- Admission governance PID7568, 2026-10-13 08:16:29 local, deadline45s,
  exit1: moving the owner invalidates one historical resource link. Mechanical
  target correction retains its historical prose. PID1644 at08:19:43 exit0.
- r001 PID3556 at08:20:03, deadline600s, exit1: thin-demo extraction leaves
  two unused test variables. Removed, without changing runtime logic.
- r002 PID25152 at08:21:17, deadline600s, exit1: the test's internal bar
  reader also links an unused painter's private dependency. Split that painter
  into its own internal translation unit; do not export private frame helpers.
- r003 PID25964 at08:22:52, deadline600s, exit1: x86 build passes, first
  private-desktop creation fails ERROR_ACCESS_DENIED before any GUI assertion.
  Subsequent runs use the permitted owned-desktop execution boundary.
- r004 PID43372 at08:26:38, deadline600s, exit1: new fixture helper name
  conflicts with Winsock send. Rename the test-only helper to send_owned.
- r005 PID35912 at08:27:32, deadline600s, exit1: retained both-width tests
  pass (27,261 each, GDI54 to54). New real-demo viewport assertion uses
  DPI-virtualized coordinates. Fix the observer DPI awareness, not the product;
  add an owned kill-on-close job to prevent failed tests leaving their demo.
- r006 PID2776 at08:32:09, deadline240s, exit0: both-width boundary tests
  pass against unchanged r005 images with the corrected observer.
- r007 PID12296 at08:35:22, deadline600s, exit0: complete final source build
  and both-width retained plus new DLL/actual-demo tests pass.
- r008 PID31908 at08:41:17, deadline600s, exit0: repeat after whitespace-only
  cleanup of menu_bar.c; published source hashes match the final tree exactly.
- r009 PID10600 at08:45:53, deadline600s, exit0: final staged-diff review
  exposes trailing whitespace in the newly added gallery; formatting-only
  cleanup followed by full rebuild/regression passes. This is the final source.

Product regression uses assets/winnt.zip as staging base and the unchanged ten
published images, verified by hash. Obsolete T444 build directories no longer
exist, so its initial attempt fails before execution; the retry rebuilds the
observers under build/M0-T445/S5 and uses retained side-palette WOW frontiers.
product2.out and product-exit.txt record exit0: Console17/Window17 and all three
retained WOW frontiers pass. WOW frontier checks are not gameplay acceptance.

PE audit finds x86/x64 DLLs and the matching imported DLL in each executable;
the demo imports public style routines, not a linked renderer. There are 23
exports including ABI version. New fixture verifies duplicate installation,
foreign-thread rejection, independent GUI-thread fonts, restoration, two
load/cleanup/unload cycles and opposite-width loader rejection. Actual demo
tests open/close all27 pages and verify content movement, then normal exit0.

## Publication contract

New images join release-manifest.json (not addon-manifest.json). All original
ten asset hashes match that manifest and configured O:/winnt/system32 at
the initial read-only audit. Preserve those images and their source boundary.
Deployment must back up replaced files, verify all thirteen identities and
test the actual deployed DLL/demo before claiming delivery. Original source
provenance remains attached; no new Microsoft executable code is imported.

## Published result and sequential review

The first publication invocation (PID43848,08:43:04) stops before any write:
r008 was still running and its success marker did not yet exist. No bypass.
After confirmed r008 exit0, PID37940 at08:43:52 (deadline240s) publishes and
tests the deployed images, exit0. Evidence root is build/M0-T445/S5/publication:
backups.json, verified-manifest.json and deployed-test/{x86,x64}/test.txt.
Both deployed boundary tests pass, including the actual x64 production demo.
Final r009 publication PID25084 at08:48:21, deadline240s, exit0, repeats the same
backup/hash/deployed-test gates under build/M0-T445/S5/publication-final.
The table below identifies this final delivery, replacing r008 images.

| Image | SHA256 |
| --- | --- |
| ntw16s32.dll | 91A73CF672E41B5154E1AB6F34467FAEA63AEFE7C9C14BFD6A519702505A56DB |
| ntw16s64.dll | 42CC4FED7EFFDF6719AE3B54D4BEF54C3E091CAF3CF798D37A30DEF4EB5E9D78 |
| W16Style.exe | A64423A84B2E651C0EED9B1DD4AA362C2F14AE3597AD81748C53A30CDE818239 |

All thirteen manifest entries match assets/release and O:/winnt/system32.
Only the new three images and manifest are written; original ten are unchanged.
Publish-Ntw16s.ps1 checks candidate input hashes, machine types and existing
package identities, backs up exact targets, and restores them on test failure.

Sequential review confirms one production renderer owner, no guest/mirror
diff, no hard-coded runtime root, no core object linked into W16Style, and no
test/capture entry points in the app. DllMain is inert. Safe unload requires
caller cleanup and no active callbacks; arbitrary unloading while attached
is not supported. No claim of transparent application-wide integration is
made: S6/S7 must handle propagation, API interception and WOW ownership.

Owner visual acceptance remains pending. The private-desktop probes validate
27 windows and geometry/interaction; they do not replace final physical UI
acceptance. Unrelated WOW proposals and QUEUE changes remain uncommitted and
are not absorbed into this S5 delivery.
