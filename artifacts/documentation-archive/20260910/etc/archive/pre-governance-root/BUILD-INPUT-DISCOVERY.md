# Historical Build Input Discovery

## Read-Only Local Scan

On 2026-08-06, a filename scan under `D:\home\repos` found only the checked-in
NTVDMx64 patch documentation, scripts, and patch fragments. It found no local
MinNT source archive, no `old-src` source archive, and no completed historical
NTVDM source tree beyond the OpenNT evidence snapshot.

This is a discovery result, not a request to download anything. The source
archives remain excluded from this research workspace until their lawful owner
provides them and authorizes an isolated build experiment.

## Consequence

There are two distinct workstreams:

1. **Historical reproduction**: requires owner-provided MinNT/old-source/SDK
   inputs, the documented patch order, and a disposable isolated VM. Its
   outputs cannot become a dependency of the non-invasive dispatcher.
2. **Owned BYOB engine**: continues independently with original code, local
   runtime identity manifests, guest-machine implementation, and documented
   user-mode host capabilities.

The lack of the historical inputs prevents verification of the requested
"compiled from Microsoft source" artifacts in this workspace. It does not
prevent continued work on the owned engine or its BYOB adapter boundary.
