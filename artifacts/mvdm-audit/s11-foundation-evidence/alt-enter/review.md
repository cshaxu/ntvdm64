# Alt+Enter interception removal — S11 candidate

The working changes remove the NTCON Window Alt+Enter hotkey registration
and the NTVDM Console input filter; Ctrl+Alt+F remains the Window-to-Console
shortcut. Host Console and operating-system handling still apply. This is an
explicit ADAPTER-WIN32-047 difference from original NT4 Console behavior.
An unused display-toggle event and stale comments were removed; no guest or
original MVDM code was changed for this feature.

The controlled input-batch fixture passes, including Alt+Enter down/up and
subsequent mouse events, non-removing peek, ordering and capacity. Its old
build omitted worker-base and pointer dependencies; the harness now links the
cached worker-base library and uses explicit test-only unavailable boundaries.
The private Window controller fixture passed with real controller code,
test-owned keyboard messages and a private desktop: it asserts both Enter
key events are delivered, the Window remains visible, and Ctrl+Alt+F still
returns to Console. This is not a test of a physical keyboard or all host
terminal implementations.

The controlled queue fixture's test-only stubs now include the actual
worker-base, worker-lifecycle and text-video declarations so their signatures
are compiler-checked. The formal input-batch runner was rebuilt and passed
again; final output and source/binary hashes are retained as
`input-batch-final.txt` and `input-batch-hashes.json`.
The final fixture also checks ordered right-Alt+Enter and Ctrl+right-Alt+Enter
down/up records and verifies that NOREMOVE neither consumes nor rewrites the
first event before NOWAIT returns the complete original batch.

The controller fixture was subsequently strengthened to send actual
test-owned Alt-down, Enter-down, Enter-up and Alt-up window messages, and
to assert all four forwarded events in that order while the Window remains
open. Ctrl+Alt+F still returns to Console. The rebuilt private-desktop test
exited zero; source and binary hashes are in `window-chord-hashes.json`. This
remains a synthetic-message test, not a physical-keyboard or terminal test.

The cached x86 worker and x64 frontend rebuilt successfully. A staged
ten-image candidate passed the guest manifest check for 59 immutable files,
Console-17, Window-17 and all 12 Control gates. The retained manifests show
all ten Control images match the staged candidate; summaries are alongside
this record. These results do not certify the rest of S11 or its Full suite,
and the candidate has not been released. Earlier failed fixture-link attempts
and an unavailable `Logs2` path were test-preparation failures, not passes.

The same staged package manifest (SHA-256
`AD4606A94451C6430EE85EA203FBE02C185035E7AF503A1AC16F4054C2903058`)
also passed three SOL Options interactions and one each of Notepad Find,
Write Find and Program Manager New Group, with each launcher returning zero.
Reports are retained in the corresponding subdirectories. SOL remained slow
(opens 1.406–2.734 s; closes 1.516–1.687 s); these bounded runs do not prove
that the performance defect or intermittent 1400 has been fixed.

A source recheck of the two removed interception sites confirms that NTCON's
window registry retains Ctrl+Alt+F and Ctrl+Alt+M but no Alt+Enter entry,
and NTVDM's `ReadConsoleInputExW` no longer filters Enter records. NTCON's
Console filter only intercepts Ctrl+Alt+F; it leaves other key records intact
while the Console is the active input source. Searches of NTCON and NTVDM
adapter sources found no other explicit Alt+Enter check. The existing batch
and synthetic window-chord tests cover record ordering; this source check does
not establish how every host console, window manager or physical keyboard
handles that chord before it reaches the project.
