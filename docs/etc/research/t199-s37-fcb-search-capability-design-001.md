# T199 S37 FCB Search Capability Design 001

## Question

What is the smallest complete adapter-owned capability required to restore
OpenNT DEM FCB search without moving DOS search semantics into Bochs or
selecting individual services from native traces?

## Source and existing inputs

- `src/opennt/base/mvdm/dos/dem/demsrch.c`: `demFindFirstFCB` and
  `demFindNextFCB` own the FCB/SRCHBUF contract.
- `src/opennt/base/mvdm/dos/dem/dosdef.h`: packed `SRCHBUF`/`DIRENT` layout.
- `src/bx-vdm/bx_ntvdm_search_{request,result,plan,session,transaction}_v1.*`:
  contained profile-entry search, per-PDB continuation ownership, opaque
  tokens, and both pathname and FCB output forms.
- `src/bx-vdm/bx_ntvdm_dem_path_search_service_v1.*`: established copied-read
  and checked-write sequencing for pathname DTA search.

## Required package

The FCB package is the pair `50:0A`/`50:0C`, plus the shared FCB search-state
layout used by the already classified `50:07`, `20`, `2C-31` family. It must
perform the following bounded sequence:

```text
guest SRCHBUF + FCB pathname + current PDB
  -> checked adapter copied read
  -> profile search snapshot + FCB query decode
  -> search-plan FCB form / opaque per-PDB continuation
  -> checked guest write of DIRENT and continuation fields
  -> typed OpenNT-shaped success or failure result
```

No host pathname, host handle, Bochs pointer, or DOS filesystem algorithm
crosses this sequence. The profile snapshot, as for pathname search, is the
only directory universe in the first CLI profile.

## Current state

The generic search-plan serializes the FCB form at the exact `SRCHBUF`
offsets. `fcb_first_entries` and `fcb_next` operate over the immutable
profile-entry array, using the same opaque token and PDB ownership as pathname
search. The source-derived exhausted/unknown continuation form clears
`SRCHBUF` token bytes 32..39 and returns AX=12 with CF set.

The checked BOP-facing copied-read/copy-write attachment is now present in
the DEM namespace plane. Source-built r27 proves a concrete `50:0A` first,
`50:0C` continuation, then exhausted `50:0C` sequence against the two `.COM`
entries in the contained profile. It also proves a legal no-match `50:0A`
search returns AX=12/CF=1 while clearing the continuation words, rather than
the package fallback. r29 additionally proves the opaque continuation cannot
be consumed under a foreign PDB; it clears the continuation and returns
AX=12/CF=1. This is a positive attachment milestone, not complete FCB package
closure: malformed tokens, single-entry inputs, and the remaining FCB family
are still subject to the acceptance requirements below.

The provider also rejects an `ES:DI` pathname range that cannot contain the
required 128 copied bytes before it creates a mechanical action. The package
then returns its explicit FCB unavailable form (`AX=5`, `CF=1`); it does not
query a host namespace, consume a continuation, or leave a pending read.
This is an input-aperture boundary, not a trace-derived service rule.

The r69 all-DEM x64 `/MT` fixture extends that package evidence with the two
remaining continuation shapes: a one-entry profile search returns its first
entry and then the same `AX=12`/`CF=1` exhausted result, while a nonzero but
altered opaque token is rejected as no-more-files.  The latter is important:
the adapter never treats a guest token as a host handle or as an index into an
arbitrary host session.  These are source-built provider-family checks, not a
native trace or a claim that the writable FCB family is complete.

## Implementation acceptance

One package implementation must add both FCB first and next wiring, positive
single-entry and multi-entry continuation tests, malformed/foreign-token
failure tests, PDB ownership tests, and `SRCHBUF` offset assertions. It must
also state the source-derived failure register form for no-match and invalid
input before it replaces the current failure provider. A native trace is only
permitted afterwards as package integration evidence.

`artifacts/build/t199-s37-dem-package-r69-fcb-boundaries/` is the MSVC x64
`/MT` all-DEM regression containing the FCB first/next continuation boundary
cases, including the retained aperture-crossing FCB-first case.
