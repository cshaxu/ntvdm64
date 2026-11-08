# T481 S4 closure — Win95 VM qualification

S4 is complete within the admitted entry path; S5 performs combined product
qualification. No production source or original guest bytes changed in S4.

| Area | Evidence and limits |
| --- | --- |
| Identity, lifetime, wake | `artifacts/t481-s4/reuse001.json`: two actual VMs, 320 callbacks and actual handle reuse with new generation; stale direct and queued wakes rejected. Ordered wait succeeds. The Win95 VMM 0x400 status bit remains unclassified; it is not used by the driver's wake decisions. |
| Initialization failure | `init002.json`: test-only host injects failure after reservation; actual guest driver rolls back and the same host IRQ and EOI hook can be reacquired. This does not prove every failure point. |
| Production host commands | `native002.json`: two VMs execute four real native commands through BOP54:08 on the candidate host with a fresh isolated identity; guest checks return codes, registers, flags and stacks. This does not prove physical input or BOP54:0A. |
| Input and presentation | `editor003.json` and `../operations/m0-t481-multitask-completion-plan.md`: actual two prompt VMs, keyboard, mouse menus, save and command completion with diagnostic host. Exact production-host interactive input is not asserted. |
| Batch entry | `batch-entry.json`: historical and candidate guest direct native batch behavior both leave empty output; the observed trace never reaches cmdCheckBinary, cmdExec or VM admission. This differs from the admitted interactive COMMAND entry. The emitting image and precise failure cause remain unknown; no fix is claimed. |
| Exit | Windows process exited after the guest controller requested ExitWindows. Observer code 1 matches historical and current no-addon controls; its original meaning is not proved, so normal Win95 shutdown is not claimed. |
| Delivery | `publication-check.json` rehashes all eleven images at both destinations and both W31 drivers. S4 changes only tests and records; candidate579 remains the complete published package. |

Source, original guest installation, and published product bytes remain
unchanged. The Win95 batch difference and the exit/input coverage limits are
carried into S5's final audit without being represented as passing results.
