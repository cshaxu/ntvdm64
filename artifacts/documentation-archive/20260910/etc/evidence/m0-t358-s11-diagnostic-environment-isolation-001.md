# M0 T358 S11 — invalid continuation observation and diagnostic isolation

## Frozen-run result

The sole S11 container execution completed normally at the observer layer but
did not reach `54:01`: it reached the two original `54:0F` returns and then
timed out at guest `03F4:1FA3`.  No continuation report was written.

That observation is **not admitted as a product continuation result**.  Its
new `MVDM_COMMAND_CONTINUATION_REPORT_PATH` child environment variable was
visible to original `cmdenv.c::cmdGetInitEnvironment`, which enumerates the
whole inherited environment.  Consequently the observer altered the original
guest environment/allocation input it was meant only to observe.

The address maps, after the COMMAND image header, to immutable COMMAND help
text rather than an executable completion marker.  It cannot be used to
attribute a COMMAND, Base VDM, CPU40, or guest defect.

## Selected correction

S12 captures the optional report path in app before original MVDM startup and
removes `MVDM_COMMAND_CONTINUATION_REPORT_PATH` from the process environment.
The adapter retains only a private bounded path and writes the default-off
record through it.  Thus original `cmdenv.c` receives exactly the same
environment it would receive with the observer disabled.  No original mirror,
guest byte, command record, BOP return or media input is changed.

After S12's formal link and format proof, one separately admitted frozen
observation may replace this invalid run.
