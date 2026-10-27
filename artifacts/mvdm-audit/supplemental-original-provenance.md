# Original supplemental identity reconciliation

The frozen inventory's `current-only` label means absent from its two
primary comparison sets. It is not proof that a file was invented by this
project. The source policy separately authorizes the OpenNT-src-2
mirror-completeness supplement. This review reconciles all 1,013 such paths
against that exception and the historical T301 import record.

The [per-path table](supplemental-original-provenance.csv) and
[summary](supplemental-original-provenance-summary.json) record actual
source/current SHA-256, the historical manifest disposition/copy flag and
original Git blob correspondence. The read-only source checkout is pinned
at `5e4619ab61c2aa76151e03973cce340be2933e61`; its actual HEAD was checked.
Git-tree blobs were checked against working bytes, allowing only explicitly
recorded CRLF checkout normalization. The historical manifest SHA-256 is
`84D314A2BBBE9B78304EDBFFBB92BDB7ABA6FD209010CC1FF0341AE761910EDF`.

Results:

- 1,011 paths match the actual supplemental original, historical source and
  target hashes, recorded size and Git identity.
- `README.md` is the project-authored divergence register, not a file in the
  supplemental manifest. Its content audit remains separate.
- `softpc.new/obj.vdm/obj/i386/ntvdm.def` matches the original source in the
  historical manifest but the current copy adds one export. Its source and
  selected-build consequences are in the [export review](export-source-recovery-review.md).

The `ledger_status` field is an observation at this pre-disposition
checkpoint, not a second classification authority; use the current combined
index for live disposition. No frozen source identity is overwritten.

Four ANSI message files illustrate the distinction. T301 records them as
`already-tracked-identical`, `copy_applied=no`; they were not newly copied by
this work. Their current/original bytes match. Original macro invocations
prove CTL/CL1/CL2/CLA are guest assembly inputs. That establishes file roles,
not source derivation of the currently installed ANSI.SYS or its behavior.

This is identity reconciliation only. It neither extends the T301 exception
into host build/runtime admission, selects a third edition over a conflicting
primary original, adds a BFS source root, nor enables a provider. The
currently selected use of a supplemental generated export carrier requires
the separate original-template recovery described in the export review.
No supplemental source or binary was copied, patched, rebuilt or executed
by this pass.
