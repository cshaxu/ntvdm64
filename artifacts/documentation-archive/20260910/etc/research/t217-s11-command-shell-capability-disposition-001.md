# T217 S11 — COMMAND Configuration Shell Capability Disposition

## Finding

S10's 32-byte, three-component C: WFP is not a SYSINIT relocation defect.
The current `FILES=20` source input is transformed by the existing
source-derived configuration provider. Its `append_config_shell` reproduces
OpenNT `cmdconf.c::ExpandConfigFiles`: it appends
`shell=%SystemRoot%\System32\command.com /p %SystemRoot%\system32`.
Original BIOS `sysconf.asm` consumes that `SHELL=` command and overwrites the
otherwise literal `commnd`; original SYSINIT then opens the resulting command
path. For the current copied `C:\Windows` system root, that exact source path
has 32 NUL-inclusive bytes and three components, matching S10 without
disclosing a pathname from the native run.

## Host Capability Check

On 2026-08-15, the ordinary user-mode file capability check found neither:

```text
C:\Windows\System32\command.com
C:\Windows\SysWOW64\command.com
```

No Win32 API is missing: `GetWindowsDirectoryW`, normal file attribute/open
APIs and the ordinary host C: namespace are usable. The absent capability is
the historical OpenNT `System32\command.com` product file and its associated
host composition, not an API that this project should restore into the host
OS.

## Required Disposition

The existing configuration transform is source-derived and correct when that
historical host command capability is available. It must not blindly inject an
unavailable shell into every CLI profile. The smallest compatible seam is a
copied, installation-time shell capability disposition:

| Capability | Configuration result | Ownership |
| --- | --- | --- |
| Original host `System32\command.com` is present and admitted | Retain OpenNT's generated `SHELL=` line and ordinary host namespace resolution. | OpenNT COMMAND source contract through bx-vdm capability seam. |
| Historical host command is absent | Do not add a substitute `SHELL=` line. Original SYSINIT retains its own `\\COMMAND.COM` default, resolved through the already declared immutable guest command image. | CLI profile adaptation at the COMMAND configuration boundary. |

The second row does not invent a BOP result, change BIOS/DOS, or overwrite
the host. It withholds the historical host-only override only because its
named original dependency is unavailable, then uses the pre-existing original
SYSINIT default and declared guest input. It is therefore rung 2 of the
mandatory source-first ladder, not a new DOS implementation.

## S12 Admission

S12 may add this one copied capability field to the complete existing
COMMAND configuration provider. It must test both capability dispositions,
preserve original CONFIG filtering/country/AUTOEXEC/merge behavior, and prove
that the absent-capability path contains no generated `SHELL=` line while the
present-capability path retains the exact OpenNT shape. It may then run one
native normal-startup integration. No BOP leaf handler, host file creation,
Bochs change, raw pathname observation, or host-system modification is
admitted.
