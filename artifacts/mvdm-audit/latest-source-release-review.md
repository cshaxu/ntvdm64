# S10 latest committed-source publication correction and Setup tools

The owner rejected the previous package-c restoration and explicitly required
all ten components built from latest committed code. The owner also requested
audit and inclusion of the pending `tools/win31-setup` changes. This delivery
implements those instructions; it does not close S10 or T473.

## Exact product and deployment

Build source commit: **55c7373344cfc7e9b150b4fc39fff23bf8e2f25f**. Core
production sources and CMake inputs match this commit. The separately reviewed
add-on source moves/removal below are included in this delivery.
The companion Setup tools/tests and governance records do not alter the ten
component production source closures. There is no restoration of older code.

The actual stable Ninja/ccache trees build all selected outputs:

| Architecture | Freshly verified producers |
| --- | --- |
| x86 | NTVDM, WOW32, VDMREDIR, Hook32 |
| x64 | Run16, NTSRV, NTCON, NTVWM, NTMON, Hook64 |

The x86 machine/provider build reuses valid intermediates. Native targets
rebuild the stale dependencies selected by their actual graphs. The staged
package uses explicit fresh producer paths for all ten images; historical
package-c supplies guest/template files only, never an executable/DLL producer.

Package UUID: **e3f8f484-7580-48b4-9a01-8c4b5e18b530**. Its
[ten-image manifest](latest-source-release/ten-component-manifest.json) binds
each published hash to its raw producer hash and architecture. Every image
was synchronized to assets/release and O:/winnt/system32, including unchanged
components. All **20 destination hashes** match this exact staged package.
The actual source/output paths, commands, owned PIDs and terminal results are
retained under [delivery evidence](latest-source-release/manifest.json).

Repository-local CCACHE_DIR, CCACHE_TEMPDIR and 5-GiB maximum are present in
the generated compile commands. No cache is cleared or global cache moved.
The first incremental harness failed opening a redirected log. A second run
showed three x86 and six x64 no-work markers, but correctly rejected two
producer hashes changed by a prior regeneration/relink. Both failures are
retained. A forward rebuild, complete restaging and repeated Full verification
produce the final r3 package; no old-package restoration is performed.

## Actual product and tool checks

The current **Full selected product suite passes** on this exact ten-image
package, using the proven medium-token private-runtime runner. It includes
Console17/Window17, WOW/frontier and existing RPC/native/Console handoff,
close, nested return, relaunch and isolation controls. The retained stdout,
terminal exit 0, runtime manifest and both 17-case summaries bind that claim.
It is not every historical test or all T-wide unrecovered capabilities.

Five Setup-tool groups pass:

- Actual CMD APPLY, repeat, adjacent backup identity, exact UNAPPLY and
  preservation of unknown/modified payloads on a build-owned media copy.
- Profile-only finalization, repeat, both PIF modes/legacy flags, conflict
  preservation and unchanged installed-input fixture bytes. Original Setup
  is a test shim here; no actual installation is claimed.
- Incomplete-installation rejection and local temporary-directory handling.
- PATH Run16 launch arguments and return code propagation.
- PowerShell payload preparation idempotence and unknown-file preservation,
  including delivery of the matching PIF.EXE; the obsolete installed-guest
  patcher and redundant original/adapted binary payload are removed.

The CMD roundtrip verifies the real original/derived compressed bytes; all
five supplied original media file hashes remain unchanged. Tests modify no
original media or existing installation. The released mouse substitution is
limited to the user-approved derived-copy tool workflow and its exact identity
checks; it is not a universal-driver or enhanced-mode success claim.

## Review fixes included with the supplied tools

The supplied changes place compressed originals' backups beside their media
files, include the released mouse driver, and generate installed profiles
without a second installed-guest patch pass. Review additionally:

- Preserves a changed PATCH/PIF.EXE before any media replacement.
- Generates six profile files in a fresh owned directory, compares every
  existing destination before copying, supports an identical repeat, and
  preserves custom profiles. Cleanup removes only the six known files.
- Retains close-on-exit, maximum XMS and legacy 386/NT launch flags rather
  than relying on PIF.EXE's different default values.
- Supplies PIF.EXE through the older PowerShell packaging path too, and
  updates corresponding tests and backup-location documentation. Non-Setup
  installed-backup assertions remain unchanged; Setup's exact original-media
  backup guarantee is checked by the CMD roundtrip.

Historical failed publication receipts and the rejected restoration remain
indexed evidence. EXECUTION now explicitly prevents treating an old verifier's
hashes as rollback authority. The current source-role gate checks the selected
fresh publication manifest instead of demanding old installed bytes.

The older intermittent SOL Options non-passes remain unresolved; the Full
pass does not establish their cause or repair. Source reconciliation still has
1,540 pending roles, 448 partial, 53 full and 2,643 excluded physical roles.
No new MVDM mirror difference or original guest build occurs in this delivery.

## Owner-directed utility cleanup

PATCHSET's source, resource build, packaged EXE and obsolete dedicated test
are removed. PowerShell packaging no longer builds it, embeds PIF templates,
or copies original/adapted guest binaries as unused PATCH payload. Profile
creation remains covered through the actual PIF tool and SETUP.CMD tests.
SZDD is now `src/addon/szdd/szdd.c`; W31PATCH is
`src/addon/w31patch/w31patch.c`. Each uses its own BUILD.CMD with the same
single-output convention as PIF/GRP/LNK. The shared add-on release staging
script owns publication and manifest entries for both AMD64 tools. APPLY.CMD
uses their release copies. No old helper EXE is retained under tools.

SZDD's additional roundtrip checks use Windows expand.exe for empty, one-byte,
7/8/9-byte and 1025-byte binary inputs, preserve an existing output and reject
missing input. The first wrapper incorrectly interpreted the last expected
failure's native exit as a suite failure; the fixture now clears its terminal
native exit after checking that negative result. Both receipts are retained.
The final source-name build and exact release/system32 hashes are recorded.
This is an add-on move and dead-path removal, not a new MVDM mirror change.

The final documentation/link/syntax/delivery gates pass. The first delivery gate correctly rejected an intermediate duplicate utility record after the owner renamed the component; staging now replaces matching file identities as well as IDs, and the repeated gate proves one W31PATCH entry. Its failed and successful receipts are retained.
