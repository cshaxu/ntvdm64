# SAS tail breakpoint: Win16 task observation

The isolated fresh-identity package ran original WINMINE under the v9-derived
debug observer from process creation. The observer recorded NTVDM worker
PID 2840, image base `0x00f00000`, selected SAS breakpoint target
`0x010782e0`, and the owned `Minesweeper` window. It posted WM_CLOSE to that
exact worker-owned window. Run16 --wait returned zero and no SAS target
breakpoint occurred during the observation. The guest SHA-256 is
`633C1328922B4092351E0019B1BA1C6D56C0F0FEA83824CC8B7E2E731B15E80B`.
The existing package stage reseals the host for a fresh identity; manifest and
stage image hashes are retained. The observer continued explicitly marked
original BOP 5B debug events, not arbitrary noninitial exceptions; its source
and compact result are retained. The full event log remains in build and its
hash is in `inputs.csv`.

The test covers creation through this task's exit, not the resident WOW
worker's complete lifetime: the worker did not itself exit before Run16 did,
and the isolated scope subsequently cleaned up its own processes. It does not
prove gameplay, other guests, external providers, exception paths or universal
unreachability. The selected address comes from the pre-seal input map, not an
independent mapped-byte check. No guest or product binary was modified or
rebuilt. An initial compile invocation incorrectly treated `user32.lib` as a
C source; the corrected link invocation succeeded before this run.
