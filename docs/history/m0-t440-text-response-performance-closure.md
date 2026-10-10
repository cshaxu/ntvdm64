# M0 T440 — Text responsiveness and DOS/Win16 execution performance closure

T440 closes without claiming that Win3.1 desktop interaction is now smooth.
It established and retained several independently justified improvements:

- source-side VGA capture is deferred until the shared publisher accepts a
  dirty source;
- worker-to-NTCON records carry their actual bounded length (up to 1 MiB),
  removing the fixed 16 KiB graphics fragmentation;
- software-graphics publication recovers after a late palette becomes ready;
- the project-added CCPU HLT poll yields at the original HLT boundary while
  preserving reset delivery; and
- native build generators consistently select MSVC `/O2` and offer an explicit
  opt-in developer ccache route without changing clean verification builds.

The accepted graphical publication repair is retained and republished. The
later global CCPU quick-event pacing experiment is not retained: it changed
guest time pacing, reduced CPU use, and did not produce a meaningful
owner-observed responsiveness improvement. Its source, test seams, and trace
instrumentation were removed rather than being left as speculative policy.

The remaining root cause is not known. Static desktop traces show ordinary
pipe/presentation calls around 80–200 microseconds, while a retained Win3.1
desktop still executes CCPU guest/JIT code. No representative active movement
trace was obtained because the test preflight used a denied CIM query before
starting the workload. That is a test-tool defect, not evidence for a product
repair.

The queue-tail [Win3.1 interactive performance root-cause proposal](../proposals/proposal-win31-interactive-performance-root-cause-001.md)
owns a CIM-free active measurement, attribution, and only then a narrowly
measured repair. It explicitly preserves the accepted T440 work and forbids
reviving global pacing as a default based only on CPU occupancy.

The detailed retained evidence is indexed in `docs/etc/README.md`:
T440 S1–S8. The normal full product matrix remains unavailable to this closure
because its old runner uses prohibited `subst Z:`; no matrix pass is claimed.
