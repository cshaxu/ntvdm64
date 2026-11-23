# T484 S3 P32: Console and DOS observation producers

Three old PowerShell producers are retired. Console channel lifetime already
has actual x86/x64 product-graph targets and active transport test consumers;
its standalone builder compiled only grid plus the fixture and omitted the
current frontend closure. No replacement implementation or duplicate target
is introduced for it.

CMD dos-observation-probes builds six independently authored COM witnesses
and a native PIF writer. All six COM images are [byte-identical](../../../artifacts/build-migration/t484/s3-console-observation/probe-comparison.json)
to a fresh run of the retired producer. [PIF checks](../../../artifacts/build-migration/t484/s3-console-observation/pif-result.json)
cover successful DOSONLY=0 creation and refusal to overwrite. They do not
claim DOS observation or guest execution acceptance.

CMD frame-codepage builds x86/x64 fixtures using the current frame-failure
link inputs plus the real Console frontend, replacing only the test object.
The old fixture lacked the current explicit palette parameter; it now reads
and supplies the source palette. Its reacquisition test constrained the old
surface but not the cloned buffer, whose initial viewport comes from the
active Console. The fixture now applies its small viewport to the new buffer
at creation, retaining the no-enlargement assertion and all cell/cursor checks.
Production code is unchanged.

[Builds](../../../artifacts/build-migration/t484/s3-console-observation/build-result.json)
and [private-desktop runtime checks](../../../artifacts/build-migration/t484/s3-console-observation/runtime-result.json)
pass:45046 assertions on each architecture. The reusable runtime-only entry
is test/observation/verify-frame-codepage.ps1; its caller must build the CMD
frame-codepage targets and existing observation-tools beforehand.

[Cached full host build](../../../artifacts/build-migration/t484/s3-console-observation/product-build-result.json)
and [11 raw/22 published identities](../../../artifacts/build-migration/t484/s3-console-observation/publication.json)
pass unchanged; no publication recopy. No mirror or original guest changes.
S3 remains active; this is not final clean/full acceptance.
