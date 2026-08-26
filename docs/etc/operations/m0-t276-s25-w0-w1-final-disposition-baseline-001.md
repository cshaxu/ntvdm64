# M0 T276 S25 W0/W1 final expected-integration dispositions

## Question

What is the final product treatment of every selected `dirs`, `makefil0`,
`inc`, `oemuni`, `suballoc` and `vdmutils` path, rather than its current
enablement state?

## Inputs and procedure

This backfill applies the final-disposition taxonomy to the 140 paths already
audited in T276/S11.  It uses the S11 file-disposition and runtime-support
binding ledgers, original `sources` files and the original package roles.  It
does not compile a body, introduce an adapter, alter a mirror or infer a final
state from a current build selection.

## Final conclusions

- All 28 `vdmutils/*` paths are `tool-only`: they are independent historical
  programs and support material, not a source of hidden host providers.
- `oemuni/toemuni.c`, `suballoc/debug.c` and `suballoc/tsa.c` are likewise
  `tool-only` historical test/diagnostic products.
- The two original `oemuni` library bodies are `adapter-backed` through
  `adapter-win32`.  Their OEM conversion and error algorithms remain in the
  imported bodies; the later adapter must preserve the historical API shape,
  temporary-string ownership and public/NT RTL error behavior.
- `suballoc/suballoc.c` is `binding-only`.  It retains its original allocator
  and callback algorithm.  The machine-facing callbacks are bound only at
  their original callers through `adapter-softpc`; the allocator never turns
  a `ULONG` into a native pointer itself.
- All `inc/*` declaration/build carriers and the library-specific headers and
  build inputs are `binding-only`.  They remain byte-exact inputs to their
  selected consumers and do not establish a generic compatibility library.
- `dirs` and `makefil0` are `not-host-runtime` original build-control evidence.

The global final tracker now has a concrete end-state for every W0/W1 row.
`final-disposition-audited; not implemented` means the conclusion is planned
and evidence-backed, not that a body has been linked or executed.

## x86/x64 rule

No W0/W1 conclusion allows an identity cast of a native pointer or `HANDLE`
into a historical 32-bit value.  Reached native identity is owned by the
named adapter and session mapping-manager instance.  `suballoc` preserves
numeric `ULONG` callback arguments; the caller, not the library, obtains any
bounded guest-memory lease.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Set-T276S25W0W1FinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S25W0W1FinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

The scripts verify final-planning coverage only.  They do not compile, link
or execute a MVDM package.
