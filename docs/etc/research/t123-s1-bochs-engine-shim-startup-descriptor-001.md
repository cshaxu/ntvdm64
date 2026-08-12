# T123 S1 Bochs-engine shim startup descriptor 001

## Result

The real engine boundary is a thin process shim, not a Bochs command-line
extension. `src/bochs/main.cc` accepts only its native options and panics on
unknown dash options; in particular it cannot receive runner `--byob-*`
arguments. BYOB admission remains child-environment-only.

## Frozen first-profile descriptor

| Field | Owner and validation | Shim disposition |
| --- | --- | --- |
| Engine binary | CLI-selected exact file identity; T121 SHA-256 is recorded in its manifest | Required, copied only as process image path. |
| BYOB profile/root and drive policy | CLI validates; adapter independently installs from its child-only environment | Preserve environment, never encode in argv/bochsrc. |
| ROM and option ROM | Descriptor names immutable files from the verified engine fixture | Copy/read only into a private run directory and hash before launch. |
| Machine configuration | Fixed T121 deferred fixture directives only: nogui, 4 MiB, PCI off, VGA extension none, selected plugins off, disk boot | Emit an exact versioned bochsrc; reject any caller directive or ambient config discovery. |
| Native argv | Bochs parser ownership | Exactly `-q -f <private-bochsrc>`; later benchmark/log fields require their own declared CLI option. |
| Standard handles and exit | CLI owns child stdin/stdio and exit propagation | Keep existing EOF stdin policy; return engine exit unchanged. |

## Exclusions

The shim may not parse target bytes or DOS arguments, construct guest state,
select BOP/DEM behavior, alter adapter session data, enable a device, modify
Bochs source, accept arbitrary bochsrc text, or use ambient host paths/config.
Failure to validate any descriptor input rejects before engine creation.

## Implementation gate

T124 may implement only this descriptor's private-directory materialization,
exact argv and process forwarding. Its tests must use a fake Bochs parser that
rejects non-native argv, then a separately admitted real T121 engine run.
