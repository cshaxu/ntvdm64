# WOW shared and thunk header roles

Six byte-identical original C headers are dependencies of selected WOW32
compilation and remain partial. The [shared ABI review](shard-2/wow-shared-abi-header-review.md)
and [generic/SHELL/Winsock review](shard-2/wow32-generic-shell-socket-review.md)
record their individual selection and limits. The guest assembly includes are
not reclassified: host dependencies alone do not prove guest use or absence.

Guest structure layout, selected table relocation, actual calls, callbacks,
provider failures and task/handle lifetime require separate validation. The
generic thunk's project wrapper remains distinct from its original body.
No guest or product was run or modified by this source-role checkpoint.
