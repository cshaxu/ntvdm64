# T484 S6 Clean CMD build and release acceptance

The clean source snapshot is f86e70841a3164d15bb0f9c3574ab3c0ab5b483b.
The archive hash and selected native tool directories are in source.json.
The snapshot started without a build graph or compiler cache. CMD entered
the supported build commands from outside the source root. The production
build and addon/fixture sequences had a native-only PATH. Searches for
Python, PowerShell and Node failed. The expected missing Visual Studio
script run returned failure. The first addon configure also correctly
rejected missing WLINK. Its retry used the project's existing native Watcom
bin directory and completed from the otherwise untouched addon graph.

The clean host build produced eleven AMD64/x86 components, and the clean
addon build produced five drivers and ten utilities. The second passes
reported no Ninja work. Clean x86 and x64 native payload, DOS observation
and memory-profile fixture targets also built through CMD. Fourteen selected
Ninja graphs expose 2,199 generated command rows. Their clean logs contain
executed edge receipts. Neither these graph commands nor the checked CMD
and CMake entry sources invoke a prohibited interpreter. The standalone
Full gate retains its separate observation and assertion interpreters.

The clean host package stage wrote eleven manifest entries and verified
each input hash, output hash, machine and UUID. A missing VCDEX input and
an existing stage root were rejected. Addon publication in the snapshot
verified all fifteen output hashes and source metadata. The normal addon
publisher's rejected input, rejected destination and locked output checks
are also covered by the S5 P5 release. An isolated INI source edit rebuilt
its own addon. An isolated NTMON source edit changed its one host image
while the other ten were byte-identical. An intentional compiler error
returned failure and preserved the previous successful NTMON image.
Both source files were restored byte-for-byte and rebuilt. These edits
were confined to the clean snapshot.

Nine clean staged host binaries differed only in PE timestamps. The x86
NTVDM and WOW32 images had 38,301 and 49,215 differing bytes respectively
against the formal package, counting timestamps. The clean path lengthens
embedded source path strings and changes the layout of read-only data.
Linker maps retain the same code section lengths. An uncached control
rebuild at the original source path, with a separate output directory,
reduced each image to exactly six differing bytes, all in PE timestamp
fields. Its raw comparison inputs matched the formal manifest. The five
driver binaries were byte-identical. All ten AMD64 utilities differed
only in verified PE timestamp fields.

The official release retained its existing package. Its eleven runtime
images matched the manifest in the tested package, repository assets and
O:/winnt/system32. The Full run passed all twenty gates on this exact
package, including actual NESticle menu pixels and Escape hide/restore.
No original guest source or winnt.zip changed against the T484 baseline.

The [recorded acceptance summary](../../../artifacts/build-migration/t484/s6/acceptance-core.json) preserves the hash, graph and Full timing evidence. Raw receipts and runtime logs are under build/t484-s6. Remaining live Win31 and Win95 launcher matrices are
recorded after their runners complete.
