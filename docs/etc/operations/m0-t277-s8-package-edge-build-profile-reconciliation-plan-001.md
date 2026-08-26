# M0 T277 S8 — package-edge and build-profile reconciliation plan

S8 derives package-level tracker artifacts from the already closed source-form
and file disposition ledgers. It does not inspect a new provider body or run a
build.

- Aggregate every `reviewed-future-host` effective row by caller package,
  source family and named owner boundary; preserve exact counts.
- Record package-level cycles honestly. The original `softpc.new/host/src`
  startup calls `dos/command/CMDInit`, while DOS uses old SoftPC interface
  forms: this is a source co-link group inside `opennt-mvdm-host`, not a
  license to introduce a cyclic static-library link or a second executor.
- Give all 23 selected package roots a formal x86 and x64 profile state.
  States describe candidate, declaration-only, guest/firmware/tool exclusion
  or not-host-runtime; none claims compilation, linking or enablement.

The result supplies the dependency-topological import plan required before a
physical re-root or source package recovery task is admitted.
