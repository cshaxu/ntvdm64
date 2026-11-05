# T477 S2 P4 integrated delivery

The owner closed T476 with its [recorded evidence limitations](../../history/m0-t476-closure.md), integrated the nine existing T477 commits via merge, and requested one coherent cached build, full verification, publication and push. The merge retained T477 history; asset conflicts kept the prior main baseline until replacement by the verified combined package. No guest source or guest image was modified or rebuilt.

The x86 and x64 cached product builds passed. A supplemental x86 build linked VDMREDIR and nthook32; clean x64 component builds and all required observers passed. An initial supplemental command exceeded the Windows command-line limit after repeated environment initialization; the separately initialized x64 run succeeded. An outdated observer Ninja graph lacked the source include root; regenerating it from the current repository generator and rebuilding passed.

The first verification attempt passed both 42-case service matrices, frontend and RPC units and six frontend integration fixtures, but packaging failed because the independent T477 worktree and its temporary baseline disappeared during execution. The retry used the previously successful T473 baseline, the G7 fixture with SHA256 86EBA64D5173C29B4749FE0F2472FC88AEF5CBF3C62FF9067DFC7CEF8EDD56C0, and the recorded T473 Full WOW frontiers. The full retry exited zero: 16/16 selected gates passed in 443276 ms, including Console-17 and Window-17, actual RPC, GUI and lifecycle paths. WOW frontiers prove startup observations, not gameplay.

Full explicitly skipped DOS4GW/NESticle without an external fixture root. A separate run using hash-verified existing fixtures and the approved minimal environment exited zero and observed the application remaining active with a visible graphics window; gameplay and rendered pixels were not asserted.

The coherent ten-image package and hook license were published to assets/release and O:/winnt/system32 and hash-verified at both locations. Frontend ownership audit and documentation governance including self-tests passed. Build, retry, timings and publication details reside under build/t477-integration-p4; the retained publication manifest records the delivered hashes.

T476 cross-context and diagnostic-line evidence limits remain as documented in its closure; successful integration tests are not a proof of every mirror branch. T477 S3 and S4 remain subsequent work.
