# T484 S4 P39 Win31 continuation observer

The Win3.1 editor and identity witnesses need the existing test-only
continuation interposition. The current worker graph now builds it through
`tools/build-fixtures.cmd x86 vm-continuation-observer`. It links the
authored witness and two separately compiled objects against the normal
worker inputs. Each replacement uses the selected product owner's compiler
options. No mirrored source or guest media is changed.

The existing system32 staging owner accepts a build-owned diagnostic NTVDM
with its link map for an isolated test package. It checks the machine,
copies the inputs, stamps the package identity and records raw and stamped
image hashes in the same manifest used by the runtime stage. The map and
unmodified raw diagnostic output are preserved.

Run records are under build/t484-s4-continuation-observer.
PID22576 built the diagnostic worker successfully. The map assigns
the public bridge and test dispatch to witness.obj and the delegated
bridge to bridge.obj. PID16424 staged an eleven-image diagnostic package
and verified the raw NTVDM hash against the manifest. Earlier invocations
did not reach the guest. One exposed a MAP path normalization bug in the
new option. The next supplied the wrong hook64 input path and failed the
existing PE check. The successful stage used the released x64 hook.

The real two-editor attempt in PID16424 failed. The observer recorded
exit zero, but the controller did not produce even its boot record.
No editor interaction or normal guest shutdown is accepted on that
record. The source installation was copied and the owned copy cleaned.

PID30872 ran the same diagnostic image and interrupt observer with the
ordinary Win3.1 matrix witness. Enhanced mode, DOSONLY=0, Program Manager
and normal guest exit passed. PID16548 completed the full cached build
and system32 staging test, including rejection of an outside-build
diagnostic image and of a missing link map. Both release locations
matched all 22 host hashes. The diagnostic images are test artifacts.

PID1884 repeated DOSONLY=0 with the same diagnostic package, copied
installation, interrupt observer, driver images, MouseMenu and SaveWitness.
It omitted ExplicitVmd. Both distinct VM identities completed. Menu clicks,
mouse-driven exit, saves and normal Windows exit passed. The comparison
shows the extra device line is associated with early exit in this setup.
A single run per profile does not prove the general VMD failure mechanism.

The editor build, configuration and staging chain is migrated. The optional
ExplicitVmd run remains a runtime exception with the failure recorded above.
Other editor permutations and the complete regression gate are outside this
receipt. The audit count is 350/355 disposed, 5 pending, with 5 overlapping
supplemental chains.
