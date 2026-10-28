# Part 2: DOS country-message inputs and BUILDIDX lifecycle

This review covers the thirteen owned country `.msg` files and `tools/src/buildidx/buildidx.c` / `buildidx.def`; identities and individual record counts are in `dos-locales-observations.json` and `dos-tools-dispatch-observations.json`. The counts are textual, not functional coverage.

Original `messages/makefile` defaults COUNTRY to usa-ms only when unset, and makes `$(COUNTRY).idx` from its matching `.msg` using BUILDIDX. `dos/v86/make.inc` also changes `msg` for an explicit COUNTRY. `doskrnl/dos/makefile:25-29,78-80,308-309` uses country messages to generate msdos.cl1/.cl2, then includes them in the DOS kernel input chain. This is a guest build path, not a host runtime locale switch. A prebuilt guest's message contents and selected locale cannot be deduced from the presence of a source input or same basename.

`buildidx.c` is an original build utility that scans headers, message counts and offsets, and can update `.MSG`/`.IDX` files; it is not an NTVDM runtime service. Its own fixed-size path buffers and historical file/error handling require a focused safety review before any modern automated use; no script is executed here. `buildidx.def` describes DOS Message Index Builder but says `NAME STRIPZ`, an original inconsistency. The actual selected linker definition is unproved; do not rename it or infer a tool binary's provenance from that field.

| Owned paths | Per-file bounded disposition |
| --- | --- |
| `dos/v86/messages/denmark.msg` | Original Denmark message data; selected guest locale unproved. |
| `dos/v86/messages/finland.msg` | Original Finland message data; selected guest locale unproved. |
| `dos/v86/messages/france.msg` | Original France message data; selected guest locale unproved. |
| `dos/v86/messages/germany.msg` | Original Germany message data; selected guest locale unproved. |
| `dos/v86/messages/holland.msg` | Original Holland message data; do not merge with the separately named Netherlands input. |
| `dos/v86/messages/italy.msg` | Original Italy message data; selected guest locale unproved. |
| `dos/v86/messages/japan.msg` | Original Japan message data; DBCS and runtime behavior unproved. |
| `dos/v86/messages/nether.msg` | Original Netherlands message data; do not merge with Holland solely by language/name. |
| `dos/v86/messages/norway.msg` | Original Norway message data; selected guest locale unproved. |
| `dos/v86/messages/portugal.msg` | Original Portugal message data; selected guest locale unproved. |
| `dos/v86/messages/spain.msg` | Original Spain message data; selected guest locale unproved. |
| `dos/v86/messages/sweden.msg` | Original Sweden message data; selected guest locale unproved. |
| `dos/v86/messages/usa-ibm.msg` | Original US IBM message variant, distinct from observed default usa-ms. |
| `dos/v86/tools/src/buildidx/buildidx.c` | Historical build-time index generator, not a runtime provider; do not execute on protected originals. |
| `dos/v86/tools/src/buildidx/buildidx.def` | Historical definition with mismatch described above; active link consumer pending. |

No freshly executed tests establish translated strings, DBCS behavior or command output. S12 should identify the immutable shipped guest/actual locale, then witness relevant DOS message/codepage and DBCS paths using approved disposable inputs; distinct locale images remain a separately admitted scope. S19 records guest hashes. Preserve all language data and avoid invoking BUILDIDX or rebuilding guest solely to establish selection. No shared ledger, product or guest modifications.

Replay: read `dos/v86/messages/makefile`, `dos/v86/make.inc`, relevant `doskrnl/dos/makefile` targets, and BUILDIDX source/definition; rerun both part-2 observers with bytecode disabled.
