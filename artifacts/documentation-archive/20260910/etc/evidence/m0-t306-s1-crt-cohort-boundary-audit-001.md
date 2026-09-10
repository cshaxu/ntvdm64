# M0 T306 S1 P7 — CRT cohort boundary audit

## Question

Does the remaining signature-proven historical CRT cohort require an OpenNT
CRT import?

## Inputs

- Fifteen `memcpy` rows in
  [the signature-seed package ledger](../operations/m0-t306-s1-signature-seed-package-ledger.tsv).
- Original definition `base/crts/crtw32/string/memcpy.c`.
- The mandated x86 and x64 MSVC `/MT` host build policy.

## Observation and decision

All fifteen physical rows name the standard C `memcpy` contract. The selected
OpenNT body is a historical CRT implementation, not MVDM-specific behavior or
a required host product algorithm. The supported MSVC CRT is the terminal
public leaf for both supported target widths. No historical CRT file is
selected for `opennt-host`, and no adapter is required solely for this form.

The companion [CRT cohort ledger](../operations/m0-t306-s1-crt-cohort-ledger.tsv)
retains the physical count and final disposition.
