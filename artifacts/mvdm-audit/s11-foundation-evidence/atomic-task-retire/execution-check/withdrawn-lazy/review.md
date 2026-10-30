# Empty withdrawal-set allocation

The project-owned page-domain adapter previously allocated and zeroed a
65,536-byte withdrawal bitmap for every link replacement, including when
`retired_count` was zero. ADAPTER-WOW-106 skips this allocation only for the
empty set, and guards the sole lookup that can occur outside the existing
nonempty-set loops. Nonempty retirement still allocates the same bitmap and
retains full identity, incoming-edge, rollback and commit checks. No new
retained state, original mirror or guest change was introduced. Failure to
allocate an unnecessary empty bitmap can no longer fail that path.

The current x86 worker graph compiled and linked with the change. The
real-CCPU graph-snapshot fixture passed with atomic link replacement,
retirement and reuse, dead-root rejection, withdrawal and retained-reader
checks; its output and exit code are retained here. The first fixture attempt
failed to create its isolated desktop with error 5 in the restricted session;
the authorized retry supplied the passing result. A later source edit added
only the adapter registration comment and the worker was rebuilt again.

This saves a 64 KiB requested temporary allocation and its zeroing on each
empty-retirement invocation; no end-to-end latency or working-set reduction
has been measured. The updated worker still needs an identically sealed
package and full application/product regression before release.

An isolated ten-image package combining this worker with the current
Alt+Enter candidate passed the immutable guest manifest (59 files, excluding
configuration) and three SOL Options open/close/parent-recovery rounds; its
launcher exited zero. The exact package manifest SHA-256 is
`09C9E25C097434A88102D9ADED045196D2341410733414EE21DBE6DAF376DD1E`;
the manifest, probe and observer reports are retained in `sol/`. Opens took
2.109–3.891 s and closes 2.203–2.641 s; these observations do not establish
an improvement or rule out a regression, and S11 performance remains open.

The existing guest-pending crosswalk checker was also rerun against the
current audit tables and passed for 195 selected host-scope decisions; its
JSON is retained as `pending-crosswalk.json`. The mixed ledger still marks all
195 entries pending, and this check does not prove other CPU profiles or
runtime capability. It does not replace the remaining S11–S19 work.
