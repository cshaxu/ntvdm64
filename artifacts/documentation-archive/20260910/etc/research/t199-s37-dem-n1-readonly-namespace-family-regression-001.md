# T199 S37 DEM N1 Readonly Namespace Family Regression 001

## Question

Does the current readonly namespace provider preserve its declared OpenNT
handle/path contracts as one component, without a host filesystem, host handle
or raw BOP pass-through?

## Inputs and procedure

- OpenNT `demhndl.c`, `demfile.c`, `demdir.c`, and `demdisp.c`.
- `src/bx-vdm/` readonly namespace, file-service, path-check and terminal
  providers.
- Fresh MSVC x64 `/MT` builds of both fixture scopes:
  `artifacts/build/t199-s37-dem-package-r85-n1-baseline/` and
  `artifacts/build/t199-s37-readonly-file-r85-n1-baseline/`.

The complete DEM fixture checks all 73 canonical identities for a valid typed,
non-pass-through result and includes immutable mutations, `demCommit`,
`demCheckPath` and pipe-unavailable dispositions.  The focused readonly-file
fixture checks open, seek, read, FastRead, close/stale handle, immutable
`demFileTimes` get and set branches.

## Result

Both source-built fixtures compiled, linked and ran with exit code zero.
N1 keeps all guest bytes and metadata inside the declared immutable profile;
the only guest-memory path is a typed checked action.  No host path, host file
handle, device creation, raw pipe, or Bochs selector knowledge is admitted.

This closes the current N1 family-regression baseline only.  It is not a
claim that DEM/S37 is complete, nor a native Bochs trace.
