# T461 S2 — shell/Registry binding implementation checkpoint

## Implemented ownership boundary

- The selected original `wshell.c` and `wkernel.c` bodies remain unedited.
  Their Registry calls are force-bound only at the formal WOW32 build edge.
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
  then rebuilt `ntvdm.exe`; the VdmTib map ownership check passed.
- Regenerated and linked the complete 77-body WOW32 provider; `wow32.dll`
  linked successfully.
- `t461_registry_overlay_fixture.c`, compiled in a fresh build-owned leaf,
  passed `T461_S2_REGISTRY_OVERLAY_OK`: default-value create/set/query/enum,
  leaf deletion, non-leaf rejection, enumeration, close and stale-handle
  rejection.  Its only file is created with `CREATE_NEW` under its supplied
  disposable directory and was absent after the test.
- `dumpbin /exports ntvdm.exe` confirms all ten façade exports.

## Not a closure claim

The existing host Shell callback probe reached a Windows.Storage provider
access-violation control branch twice before the new Registry route is loaded.
It is retained as a non-pass host-environment observation, not converted into
Registry evidence.  S2 remains active until original guest normal/failure
Shell and Registry routes are exercised from a staged package.
