# T484 S3 P195: cached tab-stop observation declarations

## Retention and implementation

This focused regression observes W32WordBreakProc before/after guest-controlled
tab-stop phases and separately verifies the guest records. Keep that coverage.
The existing native thread sampler cannot substitute: it selects an NTVDM RVA
and suspends threads; this observer reads a WOW32 DWORD without suspension.
Do not add a second sampler or change the assertion into a weaker witness.

Move the existing three public P/Invoke declarations byte-identically (LF)
into test/observation/tabstop_reader.cs. The existing native C# build rule now
handles window-inspector and tabstop-reader through one loop, with the same
Framework64 csc, AnyCPU profile, source/compiler dependencies and stable cache.
The caller builds before launching the guest and loads the resulting DLL;
there is no Add-Type source compilation. No new memory-read algorithm, product
component, guest change or release artifact. Existing map/package checks,
owned process/module selection, two snapshots and guest assertions are retained.

## Verification

Owned background27616, build/t484-s3-tabstop-reader/run.ps1, verifies:

- extracted declarations exactly match the preceding committed inline body;
- actual cached DLL load and exact four-byte read from test-owned memory;
- invalid-address failure and handle closure;
- repeated CMD build preserves output hash/mtime;
- unsupported x86 build selector returns2;
- existing window-inspector output hash is unchanged after sharing the rule.

These checks exercise the actual compiled interop, not an original guest run.
The full guest tab-stop scenario is not newly accepted by this delivery;
its retained input probe and runtime/staging preparation stay explicit.
No production or original guest source changes.

## Tracking

Three of the13 supplemental candidates now have dispositions. Ten remain to
review;21 supplemental chains await shared preparation/staging (including
this one). Original355 census remains316 disposed and39 unresolved. Native
producer completion is not whole-chain acceptance or a Full-suite pass.

The combined run completed12:39:29 UTC with exit0 (75seconds), including
cached full build and all11 raw/22 published image identity checks. Product
bytes match the current dual publication exactly; no redundant overwrite.
Final governance and script parsing precede the containing P commit.
