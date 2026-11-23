# T484 S3 P46: complete discovery producer pair

B0044 no longer invokes NASM or a caller-supplied CL driver. The existing Win16
helper invokes a native discovery target that owns both the authored NE witness
and its native desktop peer. Source, /O2 /MT /W4 /WX, CRT-warning definition and
user32 linkage are preserved. The standalone helper publishes the same local
DISCOVER.EXE, probe.lst and peer.exe names to the runtime runner.

The guest executable and listing match the former NASM command exactly. The
118784-byte native peer differs from the old CL command only in two PE timestamp
bytes; comparison inspects actual PE timestamp locations. The peer still rejects
missing arguments with2. Both edited scripts parse. Runtime assertions, private
desktop ownership and original guest inputs are unchanged. No new guest runtime
pass is claimed; equivalent bytes reuse prior coverage under owner authorization.

[Producer verification](../../../artifacts/build-migration/t484/s3-discovery/verification.json)
and adjacent PE comparison retain actual evidence. Cached full host build,
11 raw image hashes,22 published hashes and documentation governance are recorded
separately. Product images require no recopy when unchanged.

B0044 remains pending for shared runtime package production. Its two local
compiler invocations are removed, but that alone is not complete chain closure.
The message probe's dynamic NASM/native-peer combinations were inspected and
remain a separate outstanding producer; they are not covered by this delivery.
