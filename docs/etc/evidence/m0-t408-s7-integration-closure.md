# M0 T408 S7 — integration closure record

## Restored cohorts

D11 environment, D12 status conversion, D13--D15 x86 RTL helpers, D16 USER
character conversion and D26 NetLib copy ownership have their original-source
dispositions and focused x86 fixture/provider proof in S2--S6 evidence.

## Runtime boundary observation

The deployed S4 worker hash `37d57df9c1048ffa27c29e95c6fb1e3a10aa69e1fd26a4abdfbf0f6027a83077`
previously returned from MEM (PID 29868: `app.return` after `ems.init.return`).
S7's new PID-owned watchdog ran that same hash with redirected standard streams;
PID 27108 reached `ems.init.return` but not `app.return` within ten seconds and
was killed by its watchdog.  Its stdout/stderr were empty.

This is a Console/standard-handle integration difference, not evidence that
the restored RTL/USER/NetLib owners regressed.  It is transferred to the
existing Console/COMMAND integration work; no source-owner rollback is
authorized.  The watchdog is retained under `tests/observation` for future
bounded runs.

