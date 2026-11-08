# T481 combined obligation audit

This is an evidence index, not a claim that each test proves every case.

| Original group | Reached evidence | Boundary |
| --- | --- | --- |
| Input return | T481 S2 `artifacts/t481-s2/receipts.json`: ordered input, repeats, F1/F11, two VM isolation and CP437. | Approved INT16 return does not replay raw IRQ1/TSR callbacks. |
| Input delivery | `artifacts/t481-s3/editors535.json` and `artifacts/t481-s4/editor003.json`: two real guest prompts, keyboard, mouse menus, saved contents and completions. | Diagnostic host delivers test stimuli; exact production host interactive physical input is not established by those tests. |
| Text operations | S3 working plan reached caller/provider matrix and real editor/Win3.1 evidence. | Unreached protocol operations are not claimed implemented. |
| Geometry, fonts, codepage | `artifacts/t481-s3/font539.json`, `directory498.json`, S3 closure and codepage evidence in working plan. | One pinned NTDOS layout; 50 visible rows unavailable on the current desktop; no universal DBCS or runtime-codepage claim. |
| Graphics and metadata | S3 closure and working-plan real guest application acknowledgements. | Windowed-DOS garbled text is an already declared separate issue. |
| Lifecycle and failure | S2 receipt matrix, `artifacts/t481-s4/reuse001.json`, `init002.json`. | Init failure uses a diagnostic link; no proof of all fault points. Historical fill002/119 remain non-passing history and are investigated only if reproducible per owner direction. |
| Nested chains | S2 ordered native-chain and S3 `editors535.json`; Win95 `native002.json`. | Native002 checks BOP54:08, not interactive BOP54:0A or physical input. |
| Win95 | S4 closure and `native002.json`, `reuse001.json`, `init002.json`, `editor003.json`. | Exit code 1 has baseline controls but unproved original meaning; Win95 VMM status bit and native direct batch failure remain as stated in S4 closure. |
| Delivery | `artifacts/t481-s4/publication-check.json`; S5 cached builds, Win3.1 four-case matrix and Full 16/16 passed; `qualification001.json` and `publication001.json` record the complete delivery. | Only a passed Full and hash-verified complete publication can qualify a new package. |

The working plan records the source-level caller/provider distinctions and
individual test receipts. A passing diagnostic seam is not a production-binary
claim. Original guest binaries must remain unchanged.

The optional DOS4GW/NESticle case was skipped in Full because no fixture
parameter was supplied. Separate tests using the original hash-verified
`O:/winnt` fixtures reproduce the existing inherited-environment illegal
instruction while the minimal-environment control reaches its graphics window.
See `dos4gw-controls.json` and the existing TODO; this is not a Full pass.
