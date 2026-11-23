# T484 S3 P65 COMMAND and ThunkNLS verifier producers

B0073 reuses the native command-redirection target already linked against the
actual cmdredir.obj and its current session/guest-memory providers. It no
longer scrapes a historical global cflags variable or emits compiler commands.
The supplied BuildRoot selects the explicit existing native worker graph.
Original standard-handle, pipe input/output bytes and thread-exit assertions
remain unchanged. The actual fixture passed with its production object hash
recorded in result.json.

B0074 moves its existing six-case ThunkNLS wrapper into a source template and
extracts the same current WOW initialization statements during native configure.
The source is a configure dependency; a missing/ambiguous start or missing end
fails configuration. No original source or algorithm is changed. Native
compilation retains /MT /W4; registry value/type/error and locale controls are
unchanged. The old CompilerDriver argument remains optional for caller
compatibility and cannot redirect production.

The first source-comparison check failed because configure_file adds a final
LF absent from the old generated text. Inspection showed equality through all
1996 original characters, followed only by that LF; all six runtime controls
already passed. The follow-up check requires precisely that one-byte difference,
not arbitrary whitespace normalization. The failed receipt is retained.

Raw runs live in build/t484-s3-command-redirection; concise evidence is in
artifacts/build-migration/t484/s3-command-nls. Final delivery requires cached
host build, unchanged dual-package hashes, native cache and documentation gates.
Neither verifier needs guest runtime staging. These checks establish their
bounded contracts, not a new full guest integration pass.

Cached full build and final delivery checks passed. Both native executables
retain hashes/timestamps on no-op builds. All11 host input hashes and22
published hashes remain unchanged, so no product recopy is necessary.
Documentation governance passes. Fixed inventory:355 entries,137 completed
dispositions and218 remaining (54 pending chains,164 awaiting reconciliation).
