# T481 S13: Windows 3.1 setup choices

The setup media preparation stages the existing multitask installer and the
released driver pair. After original Setup returns successfully, SETUP.CMD
asks for the installation directory and DOSONLY choice. Choice 0 installs the
released pair and their configuration through that shared installer; choice 1
selects guest-only execution and does not install the pair. Both Standard and
386 Enhanced launch profiles use the selected configuration.

## Setup and runtime evidence

`test/integration/win31_setup_profile_generation_test.ps1` checks both modes,
released driver identity, unchanged installed guest files, repeat execution,
and conflict preservation. `test/integration/win31_setup_cmd_roundtrip_test.ps1`
checks media preparation, repeat preparation and restoration. Both passed.

`test/integration/multitask/verify-win31-setup-modes.ps1` exercised the actual
post-Setup script in isolated copies of O:/w31, and invoked
`win31_runtime_matrix_test.ps1` for each choice. Standard and Enhanced modes
each reached their desktops and exited normally for DOSONLY 0 and 1. The
test used a Setup executable shim, not an unattended original Setup run; it
does not prove the interactive installation phase. Original guest file hashes
were checked, and the test-owned O:/tmp/q31 copy was removed.

The four cases were repeated against candidate package
`c3bc700e-d623-4059-a47a-0f69641fc7dd`. Cached x86 and x64 builds and their
actual nested link targets succeeded. The Full selection passed 16 gates with
zero failures; DOS4GW/Nesticle was skipped because its external fixture was
not supplied. The same eleven-image candidate and manifest were published to
`assets/release` and `O:/winnt/system32` and checked by SHA-256. Installed
W31 addon hashes matched the released pair. The first Full attempt stopped at
the identity check before running any gate; the recorded pass is the rerun
with its comparison cache populated from the stamped candidate.

This evidence does not replace the remaining integrated multitask and Win95
acceptance. The reported console-close dialog remains an unresolved historical
observation, not a claimed fix.
