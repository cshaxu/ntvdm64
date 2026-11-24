# T484 S4 P38 Win31 matrix monitor preparation

The matrix delegates monitor copy and package identity binding to the
existing CMD preparation and physical staging owners. The reader and writer
remain the existing package identity tool. The attachment uses the identity
recorded in the freshly generated manifest and is written only into the
disposable stage. Binding requires a fresh package and an executable input.

The management client verifies its sibling components in
src/common/base_srv/scope.c. A first isolated run placed it under tests and
the client could not find those files (Windows error 2). A second run
preserved adjacency and hit a same-name test executable already in the
source diagnostic package. The final runner uses prepare-test-inputs.cmd to
create a separate matrix-monitor.exe, then stages it next to the package
images. The staging collision check remains in force.

Evidence root: build/t484-s4-bound-monitor. The full cached build and
test/build/Test-PackageIdentityTool.ps1 passed on PID16704 with the final
CMD template. The test checks the attachment identity, adjacent placement,
source integrity, cleanup, and rejection of invalid options. The two
published locations retain the same 22 host hashes. The matrix program,
driver, and package inputs are isolated from the installed guest.

PID14948 ran 386-dosonly0 using the established private-desktop observer and
normal Userenv. The monitor took a nonempty live task snapshot at the
W31DOS.BIN marker. The guest reached Program Manager and exited normally.
The later W31READY.BIN marker raced with ordinary guest exit on PID24400.
That run obtained an empty post-exit snapshot with exit code zero, and did
not satisfy the live snapshot assertion. No test pacing was changed.

The matrix preparation chain is closed. P37 covers ordinary DOSONLY=0/1
startup, exit, profile selection and exact restoration. P38 covers the
previously pending management attachment preparation and live snapshot path.
The independent dual-editor controller and other parent callers remain
pending. This does not claim a managed shutdown or a full regression pass.
