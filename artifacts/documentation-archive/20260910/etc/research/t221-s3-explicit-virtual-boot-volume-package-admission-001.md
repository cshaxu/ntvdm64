# T221 S3 explicit virtual boot-volume package admission

## Decision

Virtual mode remains an explicit, default-off boot-volume profile. It is not
an Overlay alias and never substitutes an admitted host C: drive. It may be
admitted only when CLI policy explicitly requests a virtual boot letter (the
approved excluded-host-C fallback is one example).

## Current reusable boundary

`mutation_profile_v1`, `dem_file_view_v1`, session host context, and startup
configuration policy already distinguish `VIRTUAL` from Direct, Readonly and
Overlay. They carry only profile/session state; no virtual file namespace,
opaque token backend, search entries, drive facts, or boot image materializer
exists yet.

## Required one-package implementation

One selector-blind virtual namespace must own:

1. configured virtual drive identity and root/current-directory state;
2. session-private files/directories/metadata, opaque file tokens and FCB
   lifecycle;
3. namespace mutation, wildcard and search entries; and
4. declared guest startup inputs without creating `IO.SYS`, `MSDOS.SYS`,
   `COMMAND.COM`, `CONFIG.SYS`, or `AUTOEXEC.BAT` on the real host volume.

The four DEM view modes then select one owner once. No individual `50:xx`
service may invent a virtual fallback. Machine/BIOS drive facts and CLI option
admission remain separate owner packages.

## Admission consequence

T221/S3 cannot claim its all-four-view exit criterion until this whole
namespace package is implemented and family-regressed. Until then Virtual
continues to return its explicit unavailable result, which is safer and more
truthful than a synthetic host fallback.
