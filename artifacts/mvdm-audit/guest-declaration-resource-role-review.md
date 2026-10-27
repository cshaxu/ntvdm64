# DOS/WOW16 declaration and resource ownership checkpoint

This S10 checkpoint compares against `01723c610`. The four disjoint roles
reviewed exactly **434** then-pending DOS/WOW16 paths with declaration,
resource, template or module-definition suffixes. Neither directory nor
suffix decides a file's role. The per-file tables are [shard 0](shard-0/guest-declaration-resource-owner-review.csv),
[shard 1](shard-1/guest-declaration-resource-owner-review.csv),
[shard 2](shard-2/guest-declaration-resource-owner-review.csv) and
[shard 3](shard-3/guest-declaration-resource-owner-review.csv).

## Positive source-role decisions

The main review accepts [129 dispositions](guest-declaration-resource-role-dispositions.csv):
69 from shard 0, 23 from shard 1, 21 from shard 2 and 16 from shard 3.
These are historical guest/tool build-input file roles, **not capability
exclusions**. Each carries original/current identity and an actual original
include, macro-qualified dependency, resource copy/compile or linker edge.
No classification is based only on an absent current graph input.

The DOS chains include DEBUG, EDLIN, EXE2BIN, Graphics and KEYB assembler
includes, message skeletons and NLS/HIMEM inputs. For shared build include
roots, a concrete CL16/MASM consumer and search-root declaration are required;
standard-header basename matches alone remain insufficient. Default COUNTRY
and message paths remain conditional. One original H2INC module definition
belongs to a historical OS/2-bound build utility, not a current host service.

The WOW16 chains include COMMDLG bitmap/dialog resources, USER declarations,
driver resource/version inputs, OLE and multimedia modules, ToolHelp import
declarations and WRITE assets. `RES_DIR`, LANG/DBCS, generated local copies,
USEBUILD and public-import generation are retained in their original roles.
A copied-resource target is not proof of current installed guest identity.
Even an explicit original make prerequisite is only a build-input role,
not proof that the compiler selected a particular guarded body.

The fresh worker/WOW32 graph and dependency observations, bound by hashes in
[the observation manifest](guest-declaration-resource-role-observations.json),
contain none of these 129 exact input paths and contain no stale dependency
blocks. This supports the positive original-role evidence; it cannot establish
every possible future host selection or any guest behavior. Original source
bytes are unchanged, so no new mirror diff or exception is introduced.

## Remaining 305 paths are still S10 obligations

No unresolved row is silently classified or handed away to manufacture closure:

* `dos/command/cmdkeyb.h` and `dos/dem/demmsg.h` have actual selected worker
  dependency consumers; `dosdef.h` is included by the DEM host header.
  Their host ABI/behavior remains to audit. `cmdpif.h` still needs an exact
  selected consumer; a DOS pathname does not make it guest-only.
* `wow16/gdi/gdi16.h` explicitly supports both WOW16 and native GDI. Its
  dual-purpose header must retain that distinction rather than inherit the
  classification of nearby guest files.
* CTL3D resources have real references but also WIN32/V2/profile branches;
  their final original target closure is not established by this pass.
* Standard-header and shared macro includes require actual search-path
  resolution. In particular the local and shared `keybshar.inc` files have
  different contents. An ASM basename and a make prerequisite naming the
  shared copy do not prove which one the assembler used.
* Unreferenced bitmap variants, locale data and declarations without a
  demonstrated consumer remain in the per-file pending tables. A literal
  scan miss is a lead, not a proof that they were unused or unnecessary.

This checkpoint introduces no new runtime recovery, guest rebuild, product
publication or functional restoration percentage. Existing 236 partial and
52 full statuses stay unchanged. Counts become **2,010 pending, 2,386
excluded, 236 partial and 52 full**. S10 remains active; both audit catalogs
and the T-wide recovery objective retain their original scope.

## Verification and reproduction

The existing source-role checker is reused with one additional cohort name;
no separate verifier framework is introduced:

```text
python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py build/t473-s10/guest-declaration-resource-owner/checkpoint.json --base 01723c610 --prefix guest-declaration-resource-role- --expected-count 129
```

Run it in the required owned background context. The
[result](guest-declaration-resource-role-verification.json) binds the exact
129-row delta, original/frozen identities, four source tables, unchanged
unrelated shard rows and full-source entries, current graph observations
and the separately recorded live source drifts.

Ninja read-only observation PIDs 28768, 25916, 8756 and 30296 exited 0.
The first WOW32 inputs command used uppercase `WOW32.DLL` and was rejected
as an unknown case-sensitive Ninja target (PID 18200, exit 1); the actual
graph declares lowercase `wow32.dll`, which the successful command uses.
No build, runtime change or product failure is inferred from that invocation
error. Other sessions' setup/video candidates remain preserved and outside
this audit-only delivery.
