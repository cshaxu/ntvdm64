# T481 S14: Windows 95 launch mode selection

`tools/win95-launch/APPLY.CMD` offers DOSONLY 0 or 1 before making changes.
Mode 0 invokes the existing shared multitask installer; mode 1 sets DOSONLY
and does not install the pair. A launch-owned marker ensures UNAPPLY only
reverses the multitask configuration installed by this invocation. Existing
multitask files or entries are refused before preparation.

The tool reads released materials from `assets/release`, including the new
unchanged SETVER.EXE asset. Its SHA-256 is
`7D8B0765C302358117186D93283AB70D2D0DC72852757AFB4CFEECA69FD74653`,
identical to the previously tracked `tools/win101-setup/SETVER.EXE` and the
installed NT runtime copy. Its addition is a copy, not a guest rebuild.
The selected installed Windows directory remains the source of the existing
identity-bound Win95 patch inputs.

`test/integration/multitask/win95-launch-modes.ps1` passed both mode choices,
released-driver identity, original SYSTEM.INI restoration and protection of
an existing independent multitask payload. The existing Win95 command safety
and failed-prepare rollback cases also passed on isolated fixtures.

The actual launch tool prepared an isolated Win95 copy for mode 0, then
`verify-win95-identity.ps1 -InstallViaLaunch -NativeCommand` observed two
concurrent guest DOS VMs and four actual host native commands, including
guest return and register/stack checks. The original installation hashes were
unchanged. `verify-win95-guest-only.ps1` used the actual launch tool in mode 1,
started Win95 and observed a guest DOS program write its result. Its fixture
then retired the session; it is not evidence of normal Win95 shutdown. Both
checks cleaned their isolated O:/tmp/q95 copy. Both were repeated against
S14 candidate `eee4aa05-2940-47ee-91d1-a34d94f4cd28` and passed; the
same candidate also passed the four Win3.1 mode launches. The first combined
test process completed mode 0 but could not find Python for the next probe
because the first probe cleared its process environment. Mode 1 and Win3.1
were rerun in separate processes and passed.

Cached x86/x64 builds and actual nested link targets passed. The Full
selection passed 16 gates with zero failures; DOS4GW/Nesticle was skipped
because the external fixture was not supplied. The eleven tested candidate
images were published to `assets/release` and `O:/winnt/system32` and
hash-verified. The W31 addon pair matched the released files.
