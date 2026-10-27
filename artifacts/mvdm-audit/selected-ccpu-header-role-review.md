# Compiler-selected original CCPU declarations

This checkpoint reviews the 96 then-pending CCPU headers at ledger base
`9ca2db194`. The [per-file observation table](ccpu-selected-header-observations.csv)
comes from actual MSVC preprocessing of the selected `c_main.c` translation
unit with the production CPU40/CCPU/SPC386/PROD flags and include order.
It does not infer active declarations merely from an include-cache hit.

## Selection and main-review conclusions

The compiler emits line markers for 74 of these original headers and nonblank
declaration/type tokens for 71. The [71 dispositions](selected-ccpu-header-role-dispositions.csv)
therefore record partial **source/declaration integration**, not restored
runtime behavior. Every row includes its original identity, selected
declaration excerpt, exact translation-unit/profile and remaining S11
caller/provider, state, operand and fault obligations. All sources are
byte-identical to a selected original; no changed mirror file becomes full.

The other 25 headers remain pending. Three have line markers without emitted
declaration tokens: `c_oprnd.h`, `ccpupig.h` and `fault.h`. The 22 without
markers are not declared unused. Macro-only headers, disabled branches,
generated local shadows and separate translation units require independent
review. In particular the selected SPC386 profile does not justify accepting
SPC486 declarations or excluding their capabilities from the whole T.

These are ordinary original declarations such as AAD/ARPL operands,
interrupt entry, code-page reads, segment caches, stack/control instructions,
descriptor operations and CPU state records. The compiler-selected forms
constrain the ABI audit; they do not establish that an adapter's call shape,
structure layout, guest decoder, exception order or lifecycle is correct.
`/P` itself does not perform C type checking or generate an object. The normal
decoder object remains a separately selected/compiled product input, and its
identity is retained rather than treating preprocessing as execution.

## Exact procedure and evidence limits

The [preprocessing input manifest](ccpu-preprocessing/observations.json)
binds the actual `ninja -t commands obj/ccpu/c_main.obj` command. Only its object
output is replaced with `/P /Fi<owned-output>`, preserving flags, compiler,
cache routing, include order and original source. Output goes to a fresh
private directory. Before/after hashes verify that the cached decoder object,
CCPU library, worker EXE and product map did not change. No guest is compiled.

The 2,365,954-byte preprocessor output remains temporary. No full SDK or
other external header body is imported into this evidence. The durable table
contains only the original CCPU source identity, selected-token digest/count
and a bounded declaration excerpt. The [extractor](ccpu-selected-header-extractor.py)
has one marker/token parser reused by the checker. It normalizes MSVC's escaped
path separators; the first temporary parser's empty result was corrected
before any header was classified.

All 71 texts retain 100 percent of their original bytes. This is not a
functional restoration ratio. A declaration has no independent executed-body
denominator; the per-file executable proportion remains explicitly
unmeasurable rather than inventing a percentage from token or line counts.

The common [checker](guest-source-role-checkpoint.py) replays the actual
preprocessor output, verifies all 96 rows and the exact 71-row ledger delta,
retains 25 pending roles, verifies source/configuration/cache identities and
checks all four shards and the unchanged full/source-drift invariants:

```text
python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/selected-ccpu-header-role-verification.json --base 9ca2db194 --prefix selected-ccpu-header-role- --expected-count 71
```

The [result](selected-ccpu-header-role-verification.json) reports 1,697 pending,
390 partial, 52 full and 2,545 excluded physical file roles. No production
source, guest, build graph or runtime image changes in this checkpoint.
