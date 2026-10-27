# Exact-byte storage of the MVDM audit evidence

S10 read-only comparison against `ecd4a604a` found 726 tracked audit files:
174 already matched their Git blobs, 545 differed only by CRLF/LF conversion
(23,730,475 working bytes), and seven contained the current substantive
checkpoint edits. This is a storage observation, not a source-restoration or
product test result.

The repository has `core.autocrlf=true`. Some audit manifests hash generated
CSV tables and archived scripts/reports as exact bytes. Normalizing those
files when storing or checking them out can invalidate that evidence even
when the text is unchanged. Imported source trees already have their own
byte-preservation attributes; the new rule applies only to this audit directory.

The single `artifacts/mvdm-audit/** -text` rule in
[.gitattributes](../../.gitattributes) stores captured audit bytes unchanged.
Re-adding existing tracked evidence under that rule changes stored newline
bytes where necessary; it does not rewrite the working files, introduce a
hash-normalization layer, alter classifications, or modify product/guest bytes.
Whitespace checks still reject the usual trailing spaces, extra blank EOF
and space-before-tab mistakes, while accepting CR as part of CRLF. The one
captured `shard-3/sas-vector-runtime-fixture.patch` retains its format-required
space prefix on an empty context line under an explicit file-only exception.

The [before observation](evidence-git-byte-baseline.json) records the exact
paths and hashes, including the seven concurrent semantic checkpoint edits.
The [subsequent index comparison](evidence-git-byte-index.json) and its
[owned run](evidence-git-byte-index-owner.json) verified raw equality for
all 744 then-indexed payload files. The frozen inventory retains its established
SHA-256; it is not regenerated. That index proof concerns byte
storage only; it does not upgrade any runtime or behavioral conclusion.
