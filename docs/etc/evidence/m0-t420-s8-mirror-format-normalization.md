# M0 T420 S8 — mirror-format normalization P

The owner admitted an immediate S1--S8 mirror-format sweep and made the same
check mandatory for every later S closure.

## Method and result

Every mirrored C/C++/assembly/include/resource file in the completed
`ccpu386`, `cvidc`, `video`, `system`, `keymouse`, `bios`, `support` packages
and the active `disks` package was compared to its pinned OpenNT counterpart.
The comparison first normalizes CRLF/LF and then removes trailing horizontal
whitespace; a file is overwritten only when that comparison is equal.

Nineteen real format-only drifts were found and restored to the upstream bytes:
ten CCPU386 and four system files had only line-ending drift; BIOS had two,
disks had one, and C-VID generated `sevid019.c` and `sevid020.c` each had one
upstream trailing-space line. The remaining enumerated files were already
byte-identical or had a non-format difference and were not touched. A repeat
sweep reports zero remaining newline-only or trailing-whitespace-only drift.

`src/mvdm/**` is marked `-text -whitespace`, matching the existing
`opennt-host` rule, so Git preserves upstream line endings and historical
whitespace rather than reformatting the mirror. The coding rule requires the
same bytewise-plus-normalized sweep at every future S closure.

The S8 disks package remains active; this P does not claim its package closure.
