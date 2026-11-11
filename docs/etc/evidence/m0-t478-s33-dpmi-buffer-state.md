# T478 S33: DPMI buffer and saved-state disposition

Parent 48884962e; finding 12. This is a current-evidence reconciliation,
not a product repair. The owner requires original semantics without additional
tolerance. No product, guest, release, O:/winnt or remote changed.

## Original ownership and remaining candidates

`dpmi32/buffer.c` is byte-exact in both pinned OpenNT editions and this tree:
SHA256 6123C2ACEFA990BE5BF87600A232A32FEB1B7435EA0C2F346AA174B56CA37ABC.
`savestat.c` matches after comments, diagnostic line directives and the original
literal `#if 0` body are excluded. The existing excluded branch stays excluded;
there is no lost selected implementation to recover.

| Earlier candidate | Current evidence and disposition |
| --- | --- |
| Failed or short allocation | This owner is a fixed 128-byte small buffer plus an 8192-byte arena, not heap allocation. ReadWriteFile requests at most 4096 bytes and releases each chunk on success, error, zero or short result. Exact arena exhaustion deliberately asserts and returns 0xf00df00d in the original; retain this internal-error limit, not a new nullable-allocation recovery. |
| Low/high memory copy and release | Original map chooses passthrough strictly below the V86 boundary; exact boundary uses copying. Controlled checks exercise passthrough, copyback, sequential large allocations, matching release and FreeAllBuffers. Real high-buffer probe covers chunked data, EOF and file cleanup. No missing provider found. |
| FCB/string bounds | Original FCB sizes remain 0x25/0x2c. Ordinary terminated string mapping retains the original extra-byte rule; controlled checks verify both. Arbitrary invalid selectors/offsets and unterminated strings remain original caller-precondition risks, not a claim of safe input rejection or a reason to add generic validation. |
| Save ownership/nesting | Caller-local VSAVEDSTATE records are linked synchronously, not allocated. Save/restore preserves SS/full ESP/DS/ES/FS/GS. Sixteen controlled nested pushes/pops restore every register and leave StateStack empty. Empty pop is original invalid internal use; no new null-stack recovery. |
| INT21 error/exit cleanup | Selected ReadWriteFile has a shared segment/stack restoration tail after error/zero/short/full paths; DpmiXlatInt21Call resets buffer bookkeeping after the translator returns. Actual guest file sequence checks missing-file CF/AX and deletes its test file on direct and nested process chains. No general asynchronous exception unwind or concurrent-global-StateStack guarantee is inferred. |
| Guest-memory lease lifetime | T430's project-owned context ownership correction already exists. Current 45-assertion boundary and session fixtures pass; do not repeat its repair or confuse its independent lease pool with original DPMI scratch storage. |

No concrete project regression or missing original wiring was established.
The old broad candidate list is resolved within these explicit bounds; it was
not proof that every malformed pointer, arbitrary exception or forced thread
termination needed a new product capability.

## Current verification

All runs were owned background processes in this worktree, with captured logs.
The initial restricted governance launch left no output and its PID was absent
under authoritative process inspection. Subsequent admission checks identified
missing active-packet syntax and proposal link; these were corrected before
test implementation. Admission then passed (PID 30668).

- `python test/observation/verify-dpmi-buffer-state.py --build-root build/t478-s33/buffer-state`
  (PID 27384): both original comparisons and 133 controlled x86 body checks pass.
  It compiles the original buffer/state bodies, excluding SegmentToSelector's
  guest executor. Register/selector dependencies are controlled, not real CPU
  execution. Release-style assertions are disabled; exhaustion checks the
  sentinel without dereferencing it, and does not claim safe exhaustion.
- `test/observation/verify-guest-memory-lease.ps1 -BuildRoot build/t478-s33/lease-current`
  (child PID 20200, 180-second owned deadline): 45 boundary checks and session
  fixture exit zero. Existing test source and production source are unchanged.
- `test/observation/verify-dpmi-int21-state.ps1 -PackageRoot build/t478-s32/package-final -BuildRoot <worktree>/build/t478-s33/int21-live -Observer build/t478-s32/observer.exe`
  (PID 22868): four actual guest cases pass, low/high buffers on direct/twice-
  nested COMMAND routes, 2494/1383/1346/1500 ms observer elapsed respectively.
  Each has a fresh identity-bound physical package and private desktop, uses
  existing independent `dpmi_int21_state.asm`, and verifies data/state/error
  markers plus absence of D38IO.TMP before owned cleanup. Nested COMMAND is
  process-chain evidence, not recursive INT21 or arbitrary exception coverage.
  This modern runner preserves the historical checked-in case assertions while
  replacing its obsolete flat-package runtime setup with the existing isolated
  package helper. No original guest is rebuilt or patched.
- `python artifacts/mvdm-audit/shard-0/dpmi-s33-image-observer.py`
  (PID 30432): eleven entire relocation-masked selected bodies, including both
  state functions, buffer owners and INT21 callers, bind uniquely to the current
  released NTVDM. Static binding supplements, rather than replaces, guest tests.

Exact source identities, object/image bindings, guest probe and transcript
hashes, process IDs and ten-image package identities are retained in
`artifacts/mvdm-audit/shard-0/dpmi-s33-results.json`. Runtime stages are copies
resealed with fresh identities; their PE hashes are not asserted identical to
the original stamped package.

## Delivery boundary

Only tests and evidence changed. All ten assets/release hashes and manifest
still equal S32 package 3f06f0c6-d34a-4a6d-a862-4c89ed5373b2. Its coherent
build and 16/16 Full gate remain applicable to those identical product bytes;
no redundant product build, Full run or republication is claimed for S33.
After governance/diff review, this S receives one local P; S34/finding 21 follows.
