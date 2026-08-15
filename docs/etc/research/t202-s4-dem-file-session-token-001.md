# T202 S4: DEM opaque file-session token

## Question

Can the whole DEM file/handle/FCB/search provider own real Win32 file handles
without serializing a handle or host pointer into guest registers or memory?

## Inputs

- T202 S3's file-token rule and source ABI map.
- `bx_ntvdm_mutation_profile_v1` and DEM's selector-blind profile consumer.
- New adapter-private `bx_ntvdm_dem_file_session_v1` session object and its
  focused test.

## Procedure

1. Defined a fixed 64-slot adapter-private session table.  A guest-visible
   token contains only a bounded slot index and nonzero generation; the table
   alone retains the Win32 `HANDLE`.
2. Made adoption, lookup, release and teardown validate token generation.
   Release closes the handle and increments the generation, so a stale token
   cannot acquire a reused slot.
3. Built the focused test with MSVC x64 `/MT` through
   `Invoke-T202S4DemFileSessionProbe.ps1`.

## Observations

- Initial run `t202-s4-dem-file-session-r1` compiled but returned `3` because
  the test requested `FILE_FLAG_DELETE_ON_CLOSE` and then deliberately tried
  to reopen that deleted temporary file.  This was a test lifecycle defect,
  not a provider outcome.
- Corrected run `t202-s4-dem-file-session-r2` compiled and exited zero.  It
  proves that a live token differs from the native handle, lookup returns the
  internal handle only to adapter code, a released token no longer resolves,
  and a reused slot receives a different generation token.
- Reverification `t202-s4-dem-file-session-r3` passed after the focused test
  corrected its direct-mode registration bit to the shared profile ABI's
  `1 << (mode - 1)` convention.

## Interpretation and confidence

The DEM provider now has its required common ownership substrate for normal
file and FCB operations.  It remains strictly inside `src/bx-vdm`: no BOP
decoder, guest-memory action, CPU state or Bochs interface was changed, and
no token is yet exposed through a live BOP result.  This is therefore not
evidence that `50:00`, `50:02`, `50:12`, `50:16`, `50:1E`, `50:2C..2F`, or
any other DEM service is complete.

## Follow-up

Extend this session through a root-relative source-derived local-file backend,
then replace the old boot/readonly leaf routes as one provider and test all
29 outcomes under the shared direct/readonly/overlay profile matrix.
