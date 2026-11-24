# T484 S3 P137 retire historical marker compiler chain

Ordered B0209--B0212 review also resolves B0226, the direct companion of B0210.
No production/test caller remains outside this historical chain.

- plan-mirror-marker-build.py freezes10 targets from task-specific T476 cache
  roots and accepts only old direct cl command spelling. It cannot describe the
  current native CMake object/profile graph by simply replacing directory names.
- run-mirror-marker-build.py rewrites those frozen commands into a T476 marker
  shadow, initializing MSVC through Python and compiling independent objects.
- verify-mirror-marker-preprocessing.py imports that runner's environment setup
  and runs original/shadow compiler commands. Remove it with the runner rather
  than leaving a broken import. Historical hits/equivalence results remain.
- scan-mirror-compiler-lines.py freezes six sources/old T476 graphs and the old
  s13-r1314 census, writes per-case compiler scripts, and reports unique-line
  leads. Its scope is historical investigation, not current line correctness.
- Test-T420S36OriginalXmsCompile.ps1 extracts old xmsblock flags to trial-compile
  xmsmemr.c. Current source-groups.cmake explicitly adds xmsmemr.c to the selected
  XMS owner and the formal product build compiles it. The trial no longer has a
  separate contract. Actual XMS allocation/move tests remain under their native
  P133 targets; do not replace them with compilation-only evidence.

Delete447 lines across these five tools. Retain historical reports/receipts;
no claim that current branches have been re-audited or marker evidence renewed.
No original guest, mirror, product or publication change. Cached full host build
and11 raw/22 published-copy identity checks follow removal. This is not a new
Full runtime pass. S3/S4/S5 remain open; B0207 is still pending.

[Receipt](../../../artifacts/build-migration/t484/s3-retire-markers/result.json).
Raw evidence: build/t484-s3-retire-markers.
