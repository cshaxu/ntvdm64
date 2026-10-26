# T461 S2 — shell/Registry binding acceptance

## Implemented ownership boundary

- The selected original `wshell.c`, `wkernel.c`, `mapembed.c`, `wdde.c` and
  `wow32fax.c` bodies remain unedited. Their Registry calls are force-bound
  only at the formal WOW32 build edge; the source sweep found no selected
  write path left directly bound to the system Registry.
- `ntvdm.exe` owns the one layered `NTVDM.REG` view and all raw key objects.
  S2 adds its in-memory mutable top layer; it does not write host Registry or
  persist an overlay.  S3 owns atomic persistence, tombstones across restart,
  and cross-worker serialization.
- `wow32.dll` owns only nonzero, non-reused worker-local DWORD tokens.  It
  resolves the current parent process's fixed private façade exports and
  releases every retained parent key on DLL teardown.  No raw pointer or host
  HKEY enters guest storage.

## Executed evidence

- Regenerated the formal x86 worker graph with repository `build/.ccache`,
  rebuilt `ntvdm.exe`, regenerated the complete 77-body WOW32 provider and
  linked `wow32.dll`. The final parent export table exposes all eleven fixed
  façade ABI entries.
- `t461_registry_overlay_fixture.c`, compiled in a fresh build-owned leaf,
  passed `T461_S2_REGISTRY_OVERLAY_OK`: default-value create/set/query/enum,
  leaf deletion, non-leaf rejection, enumeration, close and stale-handle
  rejection.  Its only file is created with `CREATE_NEW` under its supplied
  disposable directory and was absent after the test.
- `dumpbin /exports ntvdm.exe` confirms all eleven façade exports, including
  the original `RegSetValueEx` shape required by `wow32fax.c`.
- A complete coherent ten-image candidate (`product-r4`) passed the retained
  WOW WINMINE/SOL/WRITE frontiers, Console-17 and Window-17 groups in
  163,212 ms. Its marked short physical stage was removed after the identity
  checks. The candidate hashes published below are therefore not a link-only
  claim.

## Publication and S3 handoff

The final candidate publishes `ntvdm.exe`
`45C4B79871CD6636270B73B77E0037ED235874667376DD8C4A34BA2C34FF94A7` and
`WOW32.DLL` `0E73912D4F9DB609A8DA825956090DD3931FF0B076AC81ADDC832B0D7D0D5E84`
to both `assets/release` and `O:\winnt\system32`, with the same ten-image
manifest verified at both locations. Guest media is unchanged.

The existing host Shell callback probe still reaches an unrelated
Windows.Storage provider access-violation control branch before this Registry
route loads; it is retained only as a non-pass host observation. S3 owns the
remaining durable overlay commit, restart tombstones and cross-worker
serialization; it does not reopen the S2 raw-key/token ownership boundary.
