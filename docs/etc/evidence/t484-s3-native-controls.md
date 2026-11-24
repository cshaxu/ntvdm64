# T484 S3 P174 native modal and carrier producers

B0035 and B0037 still issued cl.exe directly through caller-supplied wrappers.
Both have current observable contracts, so preserve their tests rather than
retiring useful coverage. Add explicit targets to the shared fixture graph:
x86 native-modal-control/native-modal-peer with MT/O2/W4/WX and their existing
USER/SHELL libraries; AMD64 ntvwm-carrier-game with its existing flags and USER
library. The CMD selector rejects wrong architecture rows. The source bodies,
private desktop, process ownership, time budgets and success assertions stay
unchanged. Deprecated wrapper arguments are optional compatibility parameters;
they no longer choose a compiler or create a second build path.

Actual verification runs under the established normal-user environment helper
and modern hidden pwsh, never Computer Use. Supervisor PID4200 launches PID20320;
evidence: build/t484-s3-native-controls. The native modal reference reports
MessageBox=1, ShellAbout=1 and normal process exit. It loads no WOW/style/guest.

The carrier case uses the user-supplied O:/winnt/mysmb64.exe, SHA256
BAD201751B9CA7F2C2A029C7A4B945720A6DDD1A93758EDFCB712ECB2D43C8B5.
The test owns a fresh runtime stage and unswitched private desktop, launches
through actual Run16, sends the existing Console Esc input and observes
exit=0 hidden=1. This is real application/carrier behavior, not physical input
coverage. Do not commit or redistribute the external game sample. Stage cleanup
remains at the existing shared owner; original package/game inputs are unchanged.

The same run then executes cached full host build and11 raw/22 published hash
checks. No production source or original guest is modified. VT fidelity and
WOW DLL-entry tests are not closed by this batch: their historical object/source
assumptions still require review before migration. They remain distinct pending
rows. This batch closes B0035/B0037 only:313/355 disposed,42 remain.

PID20320 completes all checks in90.3 seconds.11 raw inputs and22 published
hashes remain unchanged; no redundant publication. The disposable source
package copy is removed after confirming no process uses its exact path.
Stable caches and concise runtime logs/manifests remain; governance result
is closure-result.json in the same batch.
