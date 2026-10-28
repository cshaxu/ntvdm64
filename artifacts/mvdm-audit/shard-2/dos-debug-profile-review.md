# Shard 2: DOS diagnostic/profile source roles

Five pending files match both original trees. The observer/JSON record the captured `PROD` build, selected object sections and worker map. All four C files are actually compiled and passed to `original-softpc-base-trace.lib`; “not active” must never be reported as “not built”.

| Entry | Captured evidence and bounded disposition |
| --- | --- |
| `dasm.c`, `ega_dump.c` | Their primary source bodies are guarded by `#ifndef PROD`; the captured objects have no code section and no map owner. Candidate exclusion **only of active functional bodies in this release profile**, while retaining the debug-profile decision. |
| `trace.c` | Captured object has code and data sections; worker map attributes `io_verbose` and `sub_io_verbose` data to it. Candidate **partial**; map data does not demonstrate all trace functions execute or prove debug behavior. |
| `profile.c` | Captured object has executable sections but no map owner lines. Keep **pending**: archive input and code existence do not show whether functions are linked, discarded or reachable under other profiles. |
| `markfrom` | Header describes a 64×64 one-bit icon table. Exact selected consumer/profile not established; keep **pending**, rather than exclude based solely on filename or appearance. |

The captured behavior is limited to one build snapshot. Source guards and object sections do not establish all `PROD`, `HUNTER` or debug configurations, map absence does not prove functionality is unused in every build, and no debugger execution, state lifetime or failure behavior was checked. Before feature-level disposition, trace actual selected debugger/profile entry points and requirements for the other profiles.

Replay `python artifacts/mvdm-audit/shard-2/dos-debug-profile-observer.py` with `PYTHONDONTWRITEBYTECODE=1` and compare its JSON. No guest, product, installation, shared ledger or build tree was modified.
