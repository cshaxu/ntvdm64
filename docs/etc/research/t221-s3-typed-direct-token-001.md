# T221 S3 P2: typed Direct token precondition

## Question

Can the existing opaque DEM token table explicitly distinguish a Direct Win32
file token before later provider views introduce non-handle token backends?

## Inputs and procedure

- T221 S2 token requirement: every handle operation validates token generation
  and backend kind before a backend action.
- Current `dem_file_session` and all current Direct `HANDLE` callers.

Added `DIRECT_WIN32_HANDLE` and `NONE` slot kinds.  Adoption records the
Direct kind; `lookup` rejects a live token of any other kind before exposing a
`HANDLE`; release and owner-release clear it.  A token-kind query permits
future provider dispatch to select a non-handle backend without exposing
native values.  No new guest encoding was introduced: AX:BP remains the
opaque generation/slot token.

## Observation

Fresh MSVC x64 C11 `/W4 /WX /MT` fixtures in `build/M0-T221-S3/002` compiled
and ran successfully:

- token fixture: adoption, Direct-kind query, stale query rejection, owned
  release and PDB-owner behavior;
- local-file fixture: Direct/Readonly behavior, Overlay/Virtual no-fallback,
  malformed path and session/view-profile mismatch rejection.

## Interpretation and limits

Only the Direct kind exists today.  This is a hard ABI/state precondition, not
an assertion that readonly namespace tokens, Overlay files, or Virtual files
are already represented by this table.  The historical readonly startup
namespace continues to use its own bounded fixture token until it is migrated
through the complete typed provider contract.

## Follow-up

Define the provider-owned readonly/overlay/virtual token operations and their
state lifetimes as one package, then migrate handle, FCB and search partitions
together.  Do not add a BOP-local token kind or use a non-Direct token as a
Win32 handle.
