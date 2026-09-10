# T225 S8 Primary/Reference Provenance Comparison 001

## Question

Did the independently staged primary original-toolchain sequence and the
approved secondary OpenNT-tree reference sequence produce the same
four-image installation result, without collapsing their distinct provenance?

## Inputs

The primary sequence was staged by
`tools/build/Stage-T225S8PrimarySourceBuiltInputs.ps1` in
`build/M0-T225-S8/primary-inputs-r2`. Its manifest names only these fresh,
manifest-bearing original-toolchain stages:

| Role | Primary stage | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| NTIO | `ntio-primary-source-build-r8` | 33,792 | `cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937` |
| NTDOS | `ntdos-primary-source-build-r1` | 27,858 | `957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84` |
| COMMAND | `command-primary-source-build-r6` | 50,384 | `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43` |
| SHARE target | `share-primary-source-build-r1` | 882 | `69dabbdb754b358ac4fe4b22de04c0e4c93076816f14bb0730caa9fd223996fc` |

The secondary sequence is the owner-approved
`approved-reference-binary-research-input` staged from the pinned OpenNT tree,
as recorded by
[`t225-s6-reference-guest-input-admission-001.md`](t225-s6-reference-guest-input-admission-001.md).
It has the same four role identities, but it is not a source build.

## Procedure

1. The S8 primary stager checked every primary manifest classification and its
   declared artifact size/SHA-256 before copying any input.
2. A fresh Ninja-linked MSVC x64 `/MT` four-image installation fixture ran
   against the primary staged root; its result is recorded in
   [`t225-s8-primary-direct-readonly-installation-result-001.md`](t225-s8-primary-direct-readonly-installation-result-001.md).
3. The recorded primary outcome was compared role-for-role and mode-for-mode
   with the separately executed S6 reference witness in
   [`t225-s6-reference-direct-readonly-installation-result-001.md`](../research/t225-s6-reference-direct-readonly-installation-result-001.md).

## Observations

| Mode | Primary original-toolchain sequence | Secondary reference sequence |
| --- | --- | --- |
| Direct | accepted | accepted |
| Readonly | accepted | accepted |
| Overlay | rejected at installation (`stage=-1`), deferred | rejected at installation (`-1`), deferred |
| Virtual | rejected at installation (`stage=-1`), retired | rejected at installation (`-1`), retired |

Both sequences therefore pass the same bounded installation contract with
identical four-image byte identities. The primary input manifest remains
classified `primary-original-toolchain-source-built`; the reference manifest
remains classified `approved-reference-binary-research-input`.

## Interpretation and confidence

This is a successful cross-validation of image identity and installation
admission across two physically distinct input sequences. Hash equality and
matching fixture outcomes establish content and bounded installation
correspondence only. They do **not** reclassify the reference images as
source-built, permit either sequence as default runtime discovery or release
material, or prove guest instruction execution, DOS `EXEC`, COMMAND lifecycle,
child return, BOP behavior, or Bochs continuity.

Confidence is high for the explicitly checked four identities and four-mode
installation outcome; no conclusion outside that bounded fixture is made.

## Follow-up

T225 may now select a separately admitted multi-program execution package.
That package must define its guest fixture, source/ABI ownership map, BOP
package boundaries, and runtime evidence independently; it may not use this
installation witness as a guest-trace substitute.