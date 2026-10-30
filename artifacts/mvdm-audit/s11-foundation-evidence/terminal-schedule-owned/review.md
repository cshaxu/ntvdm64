# Scheduled retirement of a published window

This expands the earlier property-only terminal scheduling fixture. After
original task initialization, a real native window is bound to that controlled
task and published into the actual CCPU page-domain handle table. The main
thread holds the shared USER runtime lock and successfully projects the graph.
A second bound resource thread then enters terminal retirement. Test-only
event hooks force the second thread to reach the acquisition point while the
first still owns the lock. After release, both retirement mutation points
observe the lock held; the same guest handle disappears and the graph projects
again. The test reports `first_phase=1 held=2 unheld=0 retired=1` and exits
zero. The source and executable hashes and fixture output are retained here;
the temporary inputs, link map and event-hook source are in
`build/t473-s11/terminal-schedule-owned2/`.

An initial attempt omitted the session binding on the second resource thread.
Its retirement returned false because the actual page-domain retirement
requires the owning session on the calling thread; that run is a fixture
non-pass, retained in `build/t473-s11/terminal-schedule-owned/`, not evidence
of a product failure. The corrected fixture binds and unbinds that same
session on the resource thread.

This is still a controlled resource thread without a second registered Win16
task or guest callbacks. It does not reproduce the earlier SOL 1400 or prove
all task scheduling, finalizer and failure paths. No product or guest was
changed; the temporary provider object contains test-only event hooks.
