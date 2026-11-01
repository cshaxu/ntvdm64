# S12 — per-consumer non-CPU40 host header branches

DIV-514 marks six original non-CPU40 alternatives in five selected MVDM
host headers. Current valid product Ninja dependencies were joined to their
actual compile commands: every observed product consumer of these five
headers defines CPU_40_STYLE. Among 434 observed MVDM headers, 129 have mixed
CPU40-on/off consumers and are outside this batch. The selected CCPU and
C-VID SAS interfaces, software V86 and Video7 remain unchanged. The
[ledger](s12-noncpu40-header-subsets.jsonl) records original body hashes,
consumer counts and graph paths. An earlier project-modified SAS alternative
and compound conditions remain held for independent review.

Independent reconstruction checked five files and six replacements against
`71ea01e1e` with zero unrelated byte changes. Cached x86/x64 ten-component
build passed; after aligning package identity, all ten images matched the
published set exactly or only in PE link timestamps. No runtime suite or
republish was repeated for unchanged code. No guest was changed. Other
headers and profiles remain in the audit.
