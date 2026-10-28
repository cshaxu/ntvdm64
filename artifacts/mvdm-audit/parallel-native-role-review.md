# S10 parallel review: selected DOS and C-VID owners

At base `69fa3fd4d`, the coordinator replays the shard 2 identity and image
observer and shard 3 SINIT012 observer before merging five pending paths as
partial. Both original editions, the frozen inventory and current source
identity agree. No guest or product image is modified by this checkpoint.

| Path | Evidence and remaining boundary |
| --- | --- |
| `dos/command/cmddisp.c` | Original `CmdDispatch` binds to the released image. Guest BOP reachability, invalid indexes, return and lifecycle remain open. |
| `dos/dem/demdasd.c` | Original absolute read/write bodies bind to the released image. Media identity, geometry, failures and cleanup remain open. |
| `dos/dem/demioctl.c` | Original `demIOCTL` binds to the released image. Original DOS prevalidates ordinary IOCTL subfunctions; direct BOP reachability and bounds still need proof. |
| `dos/dem/demlock.c` | Original `demLockOper` binds to the released image. Cross-process locking, failures and handle retirement remain open. |
| `softpc.new/base/cvidc/sinit012.c` | 33 original generated getter/setter bodies bind to the released image. Actual mode-specific calls, backing state, concurrency and teardown remain open. |

The bindings mask relocation words and do not prove actual call targets,
execution or complete behavior. Each source retains 100% original text; an
overall restored-behavior percentage cannot be inferred. The potential
debug-only dispatch checks are source findings, not reproduced crashes.
Additional shard proposals remain separate pending coordinator review.

The [follow-up shard review](shard-2/parallel-review-next.md) independently
resolves all 17 command and 19 IOCTL table entries in the released image. The
coordinator replayed its observer and matched the recorded receipt. This
establishes initial table targets, not guest execution or error handling.
