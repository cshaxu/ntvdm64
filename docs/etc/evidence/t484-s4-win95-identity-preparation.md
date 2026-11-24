# T484 S4 P42 Win95 identity and guest-only preparation

The identity and guest-only runners use reapply-win95-test-launch.cmd on
marked installation copies. It invokes the existing launcher removal
and apply operations, manages the known PATCH files, and reuses the CMD
profile selector. The helper requires a matching owner marker and keeps
the empty TEMP directory requirement. Both runners preserve their source
installation hash assertions.

The shared prepare-editor-inputs.cmd entry is now named
prepare-guest-witness.cmd because identity and guest-only tests use it too.
It applies load and run keys through guest-ini. The existing input owner
stages optional reuse, controller and DOS test images. The PowerShell
scripts retain test selection, native tool arguments, runtime assertions
and copied evidence. The approved toolkit operates only on the owned
copies. No original installation or guest image is rebuilt.

The evidence directory is build/t484-s4-win95-identity-preparation.
PID16312 ran the real Win95 InstallViaLaunch and NativeCommand caller.
Launcher apply reported PATCHED=1. Two concurrent DOS VMs reported distinct
identities and generations. Negative API behavior, 320 callback assertions
and four native command returns passed. The source inventory remained
unchanged. The launcher exit code was 1, which does not establish normal
Win95 shutdown.

PID18292 ran the guest-only DOSONLY=1 profile with multitask disabled after
the identity run retired. The real DOS witness wrote PASS and no native
CUI worker was present while it waited. The test fixture retired the
session. This does not prove normal Windows shutdown. The input sources
and the two owned guest-copy cleanup operations passed.

The Win95 identity and guest-only preparation routes are migrated. The
census is 353/355 disposed, 2 pending, plus 4 overlapping supplemental
chains. This delivery does not include the pending Win95 editor runtime
issue or the full regression gate.

PID30840 passed the cached full build, the Win31 shared witness and
configuration regression, and all 22 hashes at the two host publication
locations. The coherent released package was reused because the changed
inputs are test preparation scripts.
