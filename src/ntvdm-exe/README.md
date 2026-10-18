# NTVDM worker owner

ADAPTER-WOW-080 bounds copied-view reads to the existing page-domain owner.
The original CCPU40 c_GetLinAdd service is reused for immediate copies of
known allocated WND/CLS RAM that fits one translated page. This removes
per-field/per-character SAS translation for the same immutable-at-this-edge
storage. It is not a new memory mapper or persistent observation cache.
The exact current domain, session and outstanding backing/class lifetime
remain prerequisites; aliases never escape the synchronous copy. Non-fitting
spans retain the existing scalar SAS path. Native observation frequency,
publication, original scheduling and guest bytes/layout remain unchanged.
The copied observation and WW-view read closure is one verification group:
fresh fields, stale-native/committed-view distinction, retirement/reuse,
wrong-owner/directory refusal, failure and actual ordinary-desktop input
latency must pass before publication. Source and quantified baseline are in
the T447 S4 message-source evidence. This entry registers staged source, not
a passing performance result or release.

ADAPTER-WOW-081 extends that owned-view closure to field publication and
reference scans. Reuse original CCPU xtrn2phy/c_GetPhyAdd with the exact
bios_write_double access flags, only for a same-page range in the already
validated allocated WND/CLS RAM. Write only the previously assigned fields;
never overwrite other original private fields. Trace-enabled, cross-page or
failed translations retain original scalar stores. No writable alias escapes
the synchronous operation, no mapping or dirty-state cache is introduced,
and no guest, original memory backend or mirror is changed. Reference scans
use the same immediate read copy; original retention/free checks stay intact.
Required checks include fresh geometry/style/links, untouched private fields,
trace fallback, wrong-directory refusal, rollback/retirement/reuse and the
same actual-input comparison. This registers a candidate, not delivery.

ADAPTER-WOW-083 short-circuits only an empty EMS-alias list in the existing
checked physical-translation service. The list itself remains the authority;
there is no cached enabled flag or new memory manager. Default output is still
the input address, and all nonempty-list owner/session/range validation,
map/remap/unmap/teardown, physical-page lookup and original CCPU accesses stay
unchanged. Original CCPU's existing097 seam and normal-RAM fallback are reused;
no additional mirror or guest change. Verify empty/unbound/invalid/null cases,
nonempty multi-owner identity/range/remap/teardown, retained physical mapping,
original EMS and every-P DOS/WOW floors, plus a same-input timing comparison.
The inactive-alias predicate applies identically to scalar, RMW/string, stack
and fetch/control accesses through unchanged c_GetPhyAdd; there is no new
instruction profile, device path or per-family compensation. Registration
alone is not a passing test or measured speedup.

The Console input adapter bounds each original five-record read by available
relative-mouse FIFO capacity under the original ICA lock. Only the event
thread produces these samples; the CPU consumer frees slots. At capacity it
leaves records with the frontend, yields briefly and returns to the original
alertable suspend/input wait. It does not discard motion/button edges, merge
direction changes, enlarge the queue or change original IRQ timing. See
tests/observation/verify-window-mouse-pressure.ps1 and the S11 evidence ledger.

Original mouse IRQ cancellation flushes pending samples and button/delta
state, but does not disconnect the frontend input source. The bridge retains
the last accepted ENTER/LEAVE ownership so queued upstream movement or source
retirement remains valid after guest reset or original input suspension.
Successful DOS frontend deactivation is a distinct boundary: the frontend
discards its DOS mouse records, and console_client clears the worker route
only after acknowledging that handoff. Failed deactivation preserves it.

This component owns worker-local bootstrap, session, guest-memory and thread
bindings around the original MVDM execution core. It does not own frontend
Console/Window presentation or broker record policy.

`package_layout.c` and its header configure worker-owned media roots and validate
the original COMMAND configuration limits. They were moved from product-package
without changing path or failure behavior; they are not shared product ABI.

The win32 directory keeps private declarations beside their implementation:
environment projection, thread alert, WOW hard-error presentation and the scoped
original-host CRT redirect. Their historical divergence records remain indexed
in opennt-abi/host-compat/README.md. Shared original declarations and genuinely
multi-owner bindings remain separate; this relocation does not introduce a
generic compatibility library or modify original guest media.
