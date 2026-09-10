# T221 S3 P30: Virtual namespace source and ABI map

## Question

What is the smallest whole-provider design for the owner-approved, default-off
Virtual boot-volume profile, without turning Overlay into an implicit host
fallback or adding BOP-specific virtual behavior?

## Inputs

- The Virtual admission decision in
  [P28](t221-s3-explicit-virtual-boot-volume-package-admission-001.md).
- Current DEM file-view/profile contract:
  `bx_ntvdm_dem_file_view_v1.[ch]` and `mutation_profile_v1.[ch]`.
- The current selector-blind private storage and token machinery:
  `bx_ntvdm_dem_overlay_store_v1.[ch]` and
  `bx_ntvdm_dem_overlay_file_v1.[ch]`.
- Existing Overlay namespace view, resolver and mutation backends, plus the
  DEM namespace, FCB, wildcard and search partitions.

## Source-recovery decision

OpenNT has no historical CLI virtual boot-volume provider: its normal DEM
paths model a host-backed VDM composition. Therefore this is an
owner-approved product feature and reaches the authored-behavior rung only
after reuse of the existing source-derived provider contracts.

The following existing contracts are reused unchanged:

| Reused boundary | Reason |
| --- | --- |
| Mutation profile and DEM file view | They already select `VIRTUAL` at package scope without a selector/service decision. |
| Overlay store and private file tokens | They carry bounded DOS-relative names, private bytes, DOS metadata, token kind and teardown without guest pointers or host handles. |
| DEM route partitions and checked gather/write continuations | They preserve the existing source-shaped guest ABI, result layouts and execution boundary. |

The Overlay host-base resolver, host namespace view and COW materializer are
not reusable for Virtual. They dereference an admitted real host root and may
copy a host file; using them would silently turn Virtual into Overlay/Direct.

## Required implementation boundary

One new selector-blind `virtual_namespace_view` will own a private store and
one configured drive index. Its root exists only in that store. It must:

1. answer node, directory-empty and enumeration requests solely from private
   entries, with no `bx_ntvdm_host_namespace_v1` call;
2. open/create private files through opaque private tokens, with no host-file
   resolver or `HANDLE` path;
3. perform directory/file create, delete, rename and metadata updates against
   the private view; and
4. feed existing FCB wildcard and pathname/FCB search encoders through a
   private entry source.

The whole provider selects this view only when its shared file view is
`VIRTUAL`; Direct, Readonly and Overlay retain their existing paths. The
configured virtual drive index is a provider input, not a BOP argument and
not a machine/BIOS assertion.

Startup inputs are a second sub-boundary. The private namespace can host
declared guest startup bytes, but it must not create `IO.SYS`, `MSDOS.SYS`,
`COMMAND.COM`, `CONFIG.SYS` or `AUTOEXEC.BAT` on a real host volume. CLI and
engine option admission remain separate until this namespace package has a
positive all-family regression.

## ABI and failure rules

- Guest-visible values remain existing DOS results, FCB/DTA/SRCHBUF layouts
  and opaque adapter tokens; no virtual host pointer or handle crosses bx-vdm.
- A request for another drive returns the existing profile/path failure; it
  cannot consult a real host root through Virtual.
- An absent private path returns the existing source-derived not-found path.
- Pipes remain Redirector-owned unavailable. `50:47/48` are not promoted by a
  virtual file store.
- Virtual remains unavailable until the root, token, namespace, wildcard and
  search family regressions are all present. Parsing an engine option alone
  is not admission.

## Implementation order and proof

1. Create private-root query/enumeration and no-host-call fixture.
2. Bind private open/create/handle state and prove real-host non-observation.
3. Bind namespace mutation/metadata, FCB and search through the one view.
4. Add one four-mode family regression: Direct and Overlay retain their
   semantics, Readonly refuses mutation, and Virtual has private root-only
   visibility. Only then may a separately admitted CLI/engine descriptor
   select the virtual drive.
