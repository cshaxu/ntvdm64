# DPMI normal task cleanup on graph-owner release

The reusable `verify-dpmi-prebuilt-cleanup.ps1` uses three previously built,
hash-checked DOS witnesses without rebuilding or changing guest images. A fresh
short isolated stage used the graph-owner release inputs; all ten stage input
hashes match the published candidate. The guest reported successful DPMI
allocation, reallocation, data and free, then four child exits with live
allocations and restored aggregate XMS capacity. The launcher exited zero.
The screen, observer report, stage manifest and witness hashes are retained.

This rebinds the earlier normal-path cleanup witness to the current release.
It does not inject a provider failure, establish individual allocation or
descriptor ownership after every exit, or test abnormal termination. Those
S11/S12 boundaries remain in the existing XMEM owner review. The test uses a
short isolated physical directory and does not create a drive mapping.
