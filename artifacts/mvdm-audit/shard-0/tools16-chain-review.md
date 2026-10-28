# Historical tools16 chain and retained copies

Nine pending original mirror paths are absent under `src/mvdm/tools16`, but
the [observer](tools16-chain-observer.py) and [snapshot](tools16-chain-observations.json)
verify each has a byte-identical OpenNT-4.5 copy in `tools/historical/opennt-4.5`.
None appeared at the inspected system32 path or by basename in the ZIP.
These tools are not thereby proven unnecessary, executed or shipped in a
runtime package.

Original DOS message generation invokes `buildidx`; DOS BIOS generation
invokes `reloc`; Win16 OLE and library recipes invoke `mkpublic` and `implib`.
The retained compiler driver's binary contains names of `c1xx3216.exe`,
`c23216.exe` and `c23pcd.exe`; the retained resource compiler contains
`rcpp.exe`. This supports a historical producer/toolchain relationship, not
proof of dynamic invocation or exact old search-path resolution. The
`link_60.exe` role and selection remain to be traced independently.

The [other partition](../shard-3/tools16-build-input-role-review.md) records
additional retained tool identities and build references. Current admission
does not permit guest recompilation; none of these files or guest images was
executed, rebuilt or modified by this review.
