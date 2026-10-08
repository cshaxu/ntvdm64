# M0 T436 S4 — pre-copy Windows 3.1 protected-mode adaptation

## Question

Can the two owner-approved `KRNL386`/`WIN386` adaptations be present for
Setup's first post-copy Windows launch without modifying supplied retail media
or altering `SETUP.EXE`/`SETUP.INF`?

## Finding

Yes.  `SETUP.INF` copies `KRNL386.EX_` and `WIN386.EX_` and then enters a
Windows load before the package's former post-Setup `PATCHSET.EXE` call could
execute.  An installed-PATCH-only design is therefore too late for that
boundary.

The package producer now makes a build-owned derived copy.  It changes only
those two compressed source representations, using exact, hash-gated candidate
images.  It does not add same-name uncompressed `.EXE` files and does not edit
`SETUP.EXE` or `SETUP.INF`.

## Exact verification

Inputs were the owner-supplied original media root, checked retail expanded
inputs, `O:\winnt\system32\NTDOS.SYS`, and the existing approved adaptation
scripts.  Run `r060-derived-package` performed all of the following:

1. Expanded retail `KRNL386.EX_` and `WIN386.EX_` with Windows `EXPAND.EXE`.
2. Applied the existing identity-bound candidate transformations.
3. Built `src/addon/win31-setup/szddpack.c`, a literal-only SZDD writer.
4. Replaced only the derived `KRNL386.EX_` and `WIN386.EX_` files.
5. Re-expanded each replacement with Windows `EXPAND.EXE` and compared its
   SHA-256 with the exact candidate.

`tests/component-integration/win31_derived_media_test.ps1` repeated step 5 on
the delivered derived package and passed.  The candidate and retail identities
remain those checked by the existing adaptation scripts:

| Image | Retail SHA-256 | Candidate SHA-256 |
| --- | --- | --- |
| `KRNL386.EXE` | `FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980` | `88E095A7C39C6294E8E3BE71CBD2EDC1E4DCECBB7101D1581021EA7BE7B33181` |
| `WIN386.EXE` | `6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5` | `C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8` |

`PATCHSET.EXE` also now accepts the expected derived-media state: if Setup has
already copied a checked candidate, it creates `PATCH\*.ORIG` only from the
matching, checked retail recovery image carried in PATCH.  It never derives a
recovery file from the candidate or from an arbitrary installed binary.
The short-path integration fixture passed both initial installation and this
prepatched recovery branch.

## Limitation

This proves source representation, Setup input selection and recovery
semantics.  It does **not** prove that a real normal Setup run crosses the
post-copy `Please wait while Setup loads Windows` boundary; that is the next
manual/guest execution gate.  The earlier hardware-scan diagnosis remains
separate and is not claimed repaired here.

## Closure disposition

The owner closed T436 as a limited delivery on 2026-10-08.  The normal Setup
handoff remains unresolved and is transferred to the queue-tail
[post-copy protected-mode transition proposal](../../proposals/proposal-win31-ordinary-setup-protected-mode-transition-001.md).
The derived media is retained as that candidate's verified input baseline, not
as proof of a completed installation. Two late observer attempts stopped
before guest execution (`87` and `68` from invalid observer invocations), so
neither changes this evidence or identifies the product-side wait.
