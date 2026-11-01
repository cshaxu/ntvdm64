# T473 S12 NTVDM image difference attribution

## Question and inputs

Explain the 44,218 byte NTVDM difference observed during the MVDM path split.
The old input is `assets/release/ntvdm.exe` at `0246d024b`, SHA-256
`23505be60fb646d17864d184ed309a143be287912661b0bc475fce4a2e4048d0`.
The new input is the published image at `ce4ce8f9e`, SHA-256
`4d0e663e11c8882c2d22623ea02b9de81d6547b8cfc752a10961a6afb7943061`.
The immediately preceding frozen build comparison found timestamp-only
differences; `ce4ce8f9e` is the first source-layout delivery producing this
new image. The exact differential originated in the path-split build, before
the final movement of the additional guest/firmware subtrees.

## Observations

The old image was copied to a temporary file and stamped with the new image's
existing package ID using `Set-PackageIdentity.ps1`. Both images are 2,100,736
bytes and differ at exactly 44,218 byte positions. This modifies only the
temporary comparison copy, not either release image. Differences by section:

| Area | Differing bytes |
| --- | ---: |
| PE header | 4 |
| `.text` | 1,716 |
| `.rdata` | 41,497 |
| `.data` | 160 |
| `.reloc` | 841 |
| `.rsrc`, `.detourc`, `.detourd` | 0 |

The old and new images embed the source paths used by `__FILE__`, for example
`src/mvdm/softpc.new/host/src/nt_timer.c` versus
`src/mvdm-host/softpc.new/host/src/nt_timer.c`. Nine embedded source-path
strings change length. Four-byte alignment and later data relocation cause
subsequent contents to move; a byte-at-the-same-offset comparison therefore
counts shifted content as many changes. One additional `.rdata` change is the
debug-directory timestamp, verified from the PE debug-directory RVA.

Both `.text` sections have the same 96,584 HIGHLOW relocation locations and
are byte-identical when those four-byte operands are masked. `objdump -d`
produces 472,185 instruction addresses in each image; boundaries and mnemonics
match, and the 1,563 differing instruction operands all refer to `.rdata`.
After masking nested `.rdata` relocations, all those operand targets match in
their first 32 bytes or are the expected renamed source-path string.

Both `.data` sections have the same 5,115 HIGHLOW relocation locations and are
byte-identical after masking them. Their 136 changed pointer values all point
to matching normalized `.rdata` targets. `.rdata` has 1,049 relocations in
each image; all 1,049 old locations map to new locations after aligning the
normalized section. Of those pointers, 337 retain the same value and 712 point
to matching normalized targets. After masking relocations and aligning 4-byte
words, the remaining differences consist of the nine path strings, padding,
and the debug-directory timestamp. The relocation-table bytes also change
because the `.rdata` relocation locations move.

## Meaning and limits

The observed build-image difference is attributable to embedded build paths,
their alignment and references, and linker timestamps. It is not evidence of
an added CPU or device implementation; the 4,618 moved mirror blobs were
separately checked against the pre-move Git objects. Error diagnostics that
print `__FILE__` can now display the new path, so the images are not identical
in all observable output. The target-content checks sample 32 bytes at each
referenced location; they are strong attribution evidence, not a proof that
every possible runtime behavior is equivalent. Full selected product gates
passed on the published package, with the external DOS4GW/NESticle fixture
explicitly skipped; see [release evidence](s12-mvdm-role-split.md).

No product source, guest image, runtime or released file was changed for this
investigation. The investigation used a Git-extracted old image and temporary
comparison copies under `build/t473-s12-flat`.
