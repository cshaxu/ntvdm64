# Win16 support-source checkpoint

This audit-only S10 checkpoint compares against `27fb93577` and accepts
[108 exact-path historical guest input roles](wow16-support-source-role-dispositions.csv):
20 from the [main session](shard-0/guest-support-owner-review.md),
27 from [shard 1](shard-1/guest-support-owner-review.md),
38 from [shard 2](shard-2/guest-support-owner-review.md), and
23 from [shard 3](shard-3/guest-support-owner-review.md).
The DDEML.LIB proposal in the earlier shard record is already covered by the
previous 28-library checkpoint and is not counted a second time.

Every accepted source/resource matches its frozen and selected-primary
original identity. Original source/object/link, literal include or actual
resource-consumer edges are per-file; selected-host input/dependency absence
is separately recorded in the [observations](wow16-support-source-role-observations.json).
No file is excluded on a filename or graph absence alone. The main review
checked the actual DDEML object groups/16-bit tools/link response,
REGEDT16 and REGLOAD object/resource branches, SHERLOCK's response and
country/DBCS resources, and WIN87EM's source/flag/include/target paths.

## Boundaries established by source

* WIN87EM's source serves Windows DLL/import-library, DOS-library, MTHREAD
  and other historical variants. `emthread` belongs to the MTHREAD EMMT
  library route; remainder/square-root/transcendental includes depend on
  `!SMALL_EMULATOR`. `win87em2.def` belongs to the separate old Win2 `.ex2`
  target. These are input roles, not activation of another runtime profile
  or proof the default Windows DLL contains every retained body. None is
  the modern NTVDM host CCPU floating-point implementation.
* DDEML's DDESTRS `sources` explicitly sets `C_DEFINES=-DWIN32` and
  `UMAPPL=ddestrs`. Those native test files remain separate from the
  CL16/MASM DDEML guest DLL and are not excluded as guest sources here.
* REGEDIT's default `all` selects REGEDT16, while REGLOAD has an explicit
  additional target with distinct definition/resource handling. SHERLOCK
  builds DRWATSON.EXE; it is not the native host debugger engine.
* CTL3DV2, untraced resource variants, test ownership and prebuilt images
  still need their own component/consumer proof. No native capability is
  discarded just because a retained guest source or build artifact is
  excluded from current host implementation inputs.

Excluded rises from 2,148 to 2,256; pending falls from 2,256 to 2,148.
Full stays 52 and partial stays 228. This does not change the functional
status of a guest library or interface, establish its installed-image
derivation, or replace immutable-guest execution checks. No mirror-different
file receives an exclusion. No guest, runtime source or release image changes.

## Native audit handoff

The accompanying pending SoftPC owner reviews cover
[164 shard-1 paths](shard-1/native-pending-owner-review.md),
[137 shard-2 paths](shard-2/native-pending-owner-review.md) and
[153 shard-3 paths](shard-3/native-pending-owner-review.md).
These 454 rows are proposals for deeper review, **not** accepted status
changes, complete whole-file behavior audits or a complete native inventory;
the main-session native shard remains outstanding. Map addresses belong to
their named cached/historical executable and cannot be applied to package-c
without a matching image identity. Graph/map/dependency consumers and prior
family-level tests do not establish every conditional definition or function.

The main next-step review must resolve actual header consumers and profile
branches, device callback/state/retirement owners, CCPU exception/return
ordering and per-vector video semantics. Existing finite findings stay in
the approved S11-S17 worklist; shared headers cannot be assigned wholesale
to S17 to avoid their actual consumers. The native proposals do not convert
missing tests into either proven defects or unsupported-function exclusions.

## Verification

Run in an owned background process from the repository root:

```text
python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py build/t473-s10/wow16-core-roles/support-checkpoint.json --base 27fb93577 --prefix wow16-support-source-role- --expected-count 108
```

It verifies exact transitions, source/original/frozen identity, disjoint
partition, unchanged unrelated shard rows, full-leaf identity, recorded graph
snapshots and current drift. It does not establish guest behavior or close
S10/T. Concurrent Win3.1 setup/video product candidates remain preserved
and outside this audit-only delivery.

The [108-role verification](wow16-support-source-role-verification.json)
passed on 2026-10-27 (PID 21544, exit 0). The native inventory has a
separate [reproducer](native-pending-owner-checkpoint.py) and
[454-row identity result](native-pending-owner-verification.json): it checks
the exact pending cohort, disjoint shard, frozen/current and available
original hashes and the recorded hash fields, without promoting any row
(PID 5472, exit 0).
Reproduce it with `python -B -X utf8 artifacts/mvdm-audit/native-pending-owner-checkpoint.py build/t473-s10/wow16-core-roles/native-pending-checkpoint.json`
in the same required owned background context. Neither verification is a
substitute for the explicitly outstanding source/call/behavior review.

The main native trace also resolves an apparent
[C-VID stub concern](cvidc-inert-scaffold-review.md) as unused test scaffolding,
not a production substitution. Its metadata checks do not claim actual
video execution; the finite cleanup belongs to S18.

Final documentation governance (PID 25464) and relative links (PID 29224) passed
with exit 0; `git diff --check` passed. CURRENT is 15,079 bytes. This
audit-only checkpoint does not require product rebuild or publication.
