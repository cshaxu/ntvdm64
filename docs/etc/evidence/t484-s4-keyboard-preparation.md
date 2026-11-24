# T484 S4 keyboard test preparation

P10 moves the keyboard runner's remaining artifact/configuration preparation
into tools/prepare-keyboard-test.cmd. The existing CMD fixture build remains
the only compiler owner. The new bounded preparation entry accepts an existing
marked direct O:/tmp stage plus a repository-build output directory; it copies
the reader to its isolated system32 location, invokes the unchanged native PIF
builder and copies the checked-in shadow registry fixture into the owned stage.
It rejects installed/unmarked targets, outside-build output and repeated setup.
It has no deletion command: the caller retains its existing scope/finally cleanup.
The runner now only delegates preparation, launches the observer and reader,
asserts results and writes evidence. No original algorithm or guest changes.

## Verification

build/t484-s4-keyboard-preparation records PID26732,2026-11-24T16:52:05Z to
16:53:34Z,exit0. Cached full host build passes. The existing real private-desktop
PIF route produces MEM output and the original-reader fixture reports configured,
malformed, absent and layout-unavailable/no-install success. A host Console
layout query may legitimately choose the original no-install path; actual guest
startup is not claimed to force a particular Registry lookup. The deterministic
reader supplies and asserts those separate inputs as before.

Delivered reader bytes match the canonical worker fixture. Copied registry bytes
match the source fixture. Invalid stage/output and repeated preparation reject
without a product fallback; outside-build rejection leaves KTEST absent. Owned
stages are removed through existing cleanup. Eleven raw host inputs and22 dual
published hashes remain exact matches; no changed host image required copying.
The test uses the complete current-token Userenv block. No new Full runtime pass,
original guest rebuild or winnt.zip modification is claimed.

## Accounting

The keyboard entry becomes migrated-delivered. Census335/355 disposed,20 pending:
19 producer entries and1 reconciliation entry. The24 overlapping supplemental
preparation chains remain separately pending. S4 stays active; S5 is not complete.
