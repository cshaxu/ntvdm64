# Self-Authored Copyright Text Experiment

## Purpose

This isolated experiment tests whether the checked OpenNT
`dos/v86/inc/copyrigh.inc` input is a data-only text insertion for the
NTDOS candidate build. It does **not** test NTDOS runtime behavior, remove
other historical text or code from the artifact, establish retail equivalence,
or make the resulting image redistributable.

## Correction to Earlier Research Record

The current OpenNT checkout does track
`base/mvdm/dos/v86/inc/copyrigh.inc` as Git blob
`d854d5da283a2891c403ef29866f2018ac25baa1` (SHA-256
`5F5BA93635D6200394F545D20D91532DA8FF30D1EACDA0D786D79D61CD488AE7`).
The earlier statement that this input was absent was incorrect and has been
corrected in the artifact matrix and official MS-DOS source audit.

## Isolated Input

`ntdos-stage-v20-self-authored-copyright` is a clone of the verified
`ntdos-stage-v19` staging directory. Only its staged copy of
`base/mvdm/dos/v86/inc/copyrigh.inc` differs. It contains the user-provided,
self-authored MASM data declarations:

```asm
DB "NT DOS Research 5.00 Copyleft 2006 Neko"
DB "Unlicensed Material"
DB "All vibes reserved "
```

The OpenNT input checkout and v19 stage were not modified.

## Procedure

1. Reassemble only `doskrnl/dos/mstable.asm` with the established JWasm
   compatibility invocation (`-omf -Zm -I..\\..\\inc`).
2. Reuse the original 38-object NTDOS link order via
   `link-opennt-ntdos.ps1`.
3. Apply the existing source-built `opennt-reloc` helper at load segment
   `0000`, then the existing source-built `opennt-stripz` helper.
4. Compare staged v19 and v20 output lengths, text offsets, and the strip
   result.

## Result

| Check | Result |
| --- | --- |
| `mstable.asm` assembly | 0 errors; one pre-existing legacy macro warning |
| Link | completed; WLink emitted its existing no-stack/no-start-address metadata warnings |
| Relocated image | 36,738 bytes |
| Headerless `NTDOS.SYS` candidate | 27,954 bytes |
| Headerless SHA-256 | `942AE8F4FF9097F5161938EBF6105503E8FFC52FA709627F38AD9C9B350B7FB7` |
| Difference from v19 headerless size | -48 bytes |
| Self-authored first-text offset | 571 |
| Original first-text offset in v19 | 571 |
| Original first-text found in v20 | no |
| Strip validation | `ntdos.bin[0x2250..]` equals staged `NTDOS.SYS` |

The successful assembly/link confirms that this include contributes ordinary
data at this site. It is not a source-closure blocker for the current OpenNT
checkout. The experiment is retained only as a staging-level structural proof;
it is not promoted to a runtime artifact or release payload.
