# M0 T310 S33 — support-controller callbacks closure

S33 closes the original SoftPC support-controller source package.  The full
original `ios.c` port-routing tables and width-expansion algorithms are
selected for x86 CCPU40 with no replacement implementation.  Its function
pointers remain private machine state.

`terminat.c` is retained as original source but its historical process-exit
tail is explicitly a later S49 app/session lifecycle proof.  S33 did not hide
that behavior behind a controller shim or falsely claim a runnable teardown.

Evidence: [S33 P1](../etc/evidence/m0-t310-s33-support-controller-callbacks-closure-001.md).
