# Real DOS DPMI task cleanup on current package

The current sealed ten-component package was copied into an isolated runtime.
Previously built, SHA-256-checked `D36T.COM`, `D36N.COM` and `D36L.COM` were
copied as test inputs; no guest was built or changed. The parent warms the
original DOSX path, measures XMS largest/available capacity, runs four children
that exit with a live DPMI allocation, and checks that capacity returns to the
same values after each. The guest console reports four live-allocation exits
and `S36_DPMI_TASK_EXIT_CAPACITY_RESTORED_OK`; Run16 exited zero. The package,
guest inputs, output and identities are retained in this directory. The
temporary runner is `build/t473-s11/dpmi-prebuilt-task-cleanup.ps1`.

An earlier run using the inherited long PATH timed out before any guest
milestone; it is a non-pass recorded in `initial-timeout.txt`. A separate run
using only the isolated system32 and host Windows system32 in PATH passed.
This is consistent with the separately documented COMMAND environment
boundary, but the two observations alone do not prove its cause. The temporary
Z: mapping was absent after both runs.

The passing witness covers normal child task exit and observed aggregate pool
capacity; it does not prove injected provider failure, descriptor cleanup,
data preservation, all allocation sizes, abnormal termination, or every DPMI
consumer. No installed image or product source was modified.
