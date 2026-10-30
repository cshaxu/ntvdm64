# S11 retirement collector code review

`wow_window_words_collect_retired` takes a reference to each pending binding
under the shared publication lock, drops the lock before terminal cleanup,
and releases that reference afterward. `wow_window_words_terminal` can itself
release table and property references; the collector's retained reference
keeps the binding alive for that call. The candidate always evaluates the
terminal call on each collected binding: the `&& complete` operand only
controls recording the first failure. It saves `GetLastError()` before the
collector drops its retained reference and restores it after the scan.
Successful collection does not set a new error value by contract.

The controlled fixture uses two pending entries with distinct failure codes
in both index orders and verifies the first code, followed by restoration,
reentry and final table reclamation. The graph and native-nested tests cover
other lifetime paths. This review does not prove arbitrary concurrent table
mutation, every terminal exception path, or the intermittent SOL failure.
No new source change was required by this bounded review.

The fixture now additionally verifies the forward-progress case directly:
an earlier synthetic failure leaves its binding pending while the later
binding is freed, preserves the earlier error, then frees the restored earlier
binding on the next scan. The selected-object graph run passed; see
`continue-after-error/` for logs and hashes.
