# S10 eight-way audit work partition

The owner requested eight-agent-style parallel audit. The current execution
host admits at most four concurrent agents including the coordinating owner;
up to three independent reviewers therefore run at once. The coordinator
checks and integrates their evidence, then assigns subsequent partitions.
All reviewers preserve guest and product files and write separate reports;
only the coordinator changes the shared status ledger.

The disjoint path predicates below apply in order to the 1,453 pending
physical rows at `f472b0926`. Already classified rows still require the
separate evidence and recovery work recorded in the S10 ledger. Counts are
work partition sizes, not evidence of completeness or functional restoration.

| Partition | Pending path predicate | Rows |
| --- | --- | ---: |
| 1 | `softpc.new/base/cvidc/*.h` | 97 |
| 2 | `dos/v86/cmd/**` | 53 |
| 3 | `wow16/drivers/**` | 33 |
| 4 | Other `dos/**` | 139 |
| 5 | Other `wow16/**` | 390 |
| 6 | Other `softpc.new/**` | 227 |
| 7 | All remaining physical MVDM paths | 514 |
| 8 | The separate 42,963 external catalog relevance rows | Not a physical-row count |

Each classification must identify the original source, actual selected
consumer and residual behavior and lifecycle obligations. No reviewer may
promote a source solely because it compiled, or exclude a guest capability
solely because it is absent from the host build. Mirror differences remain
partial. Source-text retention and behavior-restoration estimates are distinct.
