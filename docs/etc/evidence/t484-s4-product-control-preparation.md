# T484 S4 P44 Product control test preparation

The product verification runner now invokes the existing CMD build target
for version test programs and the shared CMD input and package staging
owners for all nine control attachments. The physical package preparation
copies and verifies the attachments, assigns the existing package identity
and retains the original runtime image hashes. Replacement is allowed only
for test attachments in the disposable package. A product executable
cannot be supplied through that option.

The Node runner now reads prepared test executables and performs runtime
assertions. It neither builds nor modifies a package. The Python tools in
the gate remain standalone readers of guest media and export contracts.

The evidence root is build/t484-s4-product-control. PID17600 completed
the cached full build, existing package identity tests plus preserved
identity attachment coverage, and the version runner environment tests.
The first Control invocation could not find Node in the normal Userenv
PATH, so the real test run uses explicit Node and Python executable paths.

The complete Control gate then ran under PID 9688 and exited 0. All ten
version cases passed, including the launcher and worker variants. The
gate also passed the console, RPC, lifecycle, nested handoff and real
DOS4GW/NESticle menu checks. See control-result.json and control.log.
All eleven host images matched the verified build input in both
publication locations. No runtime images needed republication.

This closes B0207 in the frozen inventory. Four supplemental preparation
chains remain in S4.
