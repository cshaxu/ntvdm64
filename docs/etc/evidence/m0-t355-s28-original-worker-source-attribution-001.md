# M0 T355 S28 — original-worker source attribution

## Question

Which original worker owns the sole live `worker=1` session binding observed
when the fixed CPU40/x86 product reaches app disposal?

## Inputs

- S26/S27 disposal reports and the selected CPU40/x86 product graph.
- Original `softpc.new/host/src/nt_timer.c` and COMMAND/Redirector worker
  call sites.
- Immutable `O:\ntvdm` stage and its existing fixed observation container.

## Procedure

1. Reviewed the formal process-link line.  It links COMMAND but not
   `vdmredir/vrnmpipe.obj`; therefore the preliminary named-pipe-worker
   hypothesis cannot explain the observed startup binding.
2. Extended only the existing CDECL-to-WINAPI thread facade to retain a
   bounded source spelling in the session-local diagnostic snapshot.  The
   spelling is not guest state, a native pointer, or a behavior input.
3. Regenerated the CPU40/x86 Ninja graph and linked
   `original-softpc-process.exe` successfully (46 incremental actions; normal
   historical compiler warnings remained visible).
4. Made exactly one fixed-container observation with
   `MVDM_SESSION_DISPOSE_REPORT_PATH=O:\ntvdm\m0-t355-s28-dispose.txt`.

## Observation

The one report at `O:\ntvdm\m0-t355-s28-worker-attribution.txt` recorded:

```text
exit=0x00000048
MVDM-SESSION-DISPOSE reason=binding-count code=2 total=1 entry=0 worker=1 unspecified=0 worker-source=HeartBeatThread
```

The observer's JSON records product SHA-256
`d738132c99baf53e5204b0b29192d2ea0000690122d3280d39028da82e517a78`, fixed
stage manifest SHA-256
`cd007a94bd72b0abcc145dd6d27930b3d3c38d1d7d3a144a8a17545e3067758f`, and
fixed-media manifest SHA-256
`15b6a215e9bf3cede6650c0990602b0f421181f18ea2ff431031fd237cd82858`.

## Interpretation

The live binding belongs to original SoftPC
`nt_timer.c::HeartBeatThread`, not a Redirector worker.  Original
`TerminateHeartBeat` uses the source-shaped alert-and-wait sequence
(`NtAlertThread`, then `WaitForSingleObjectEx`), while the worker exits from
its alertable wait.  S28 changes no lifetime behavior and makes no claim that
the timer shutdown is already recovered.

## Successor

The next source-owned recovery must be the complete original heartbeat
termination cohort: public modern alert/cancellation binding, retained
`TerminateHeartBeat` order, worker completion and handle lifetime before
session disposal.  It must not revive the disproven Redirector route or
force-unbind a live heartbeat worker.
