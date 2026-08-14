# T200 S1 — Native CLI integrated-execution baseline

## Question

With T199's full BOP routing/disposition closure installed, where does one
source-built CPU5 native CLI execution first stop, and which owner package—not
which individual BOP patch—must diagnose it next?

## Method

One guest run, and only one, was executed:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S97PostBootstrapNativeProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t200-s1-native-cli-baseline-r1
```

The harness rebuilt current composition inputs from
`tools/t198-s50-bx-vdm-composition-manifest.json`, used MSVC x64 `/MT` for its
modern fixture, and used the source-built CPU5/Pentium-MMX guest recipe.  Its
provenance record reports `runs: 1` and guest exit `2`, the fixture's defined
controlled-observation exit rather than a DOS/CLI exit status.

## Observation

The run completed the established DEM traffic and reported:

```text
observed-5011=1 observed-503b-resume=1 observed-dta-resume=1
observed-hard-error-resume=1 observed-drive-resume=1
observed-first-generic-ud=0 observed-stop=1 next=54:01
plane=0/0/1 error-lock=1 gset=1 terminal=1:95ab:03c3
```

It also reached the previously bounded machine handoff observation:

```text
spckbd observed=1 state=8dc8:45a6 eax=0000beef ...
```

## Interpretation and limit

`54:01` belongs to the COMMAND bootstrap/launch/environment/lifecycle package.
It is not a new BOP identity and does not weaken T199's table closure.  The
current observation bridge deliberately records the identity and controlled
frontier but not the stopped BOP's fixed register state or CMDINFO guest bytes.
Therefore it cannot distinguish registration, segment:offset, CMDINFO capacity,
environment, lifecycle, or caller-layout causes.

No implementation conclusion is made from this trace.  The next S must first
recover and compare the full original COMMAND input contract, then—only if
needed—admit one bounded additional observation or a component-package repair.
No Bochs feature, host capability, or selector-specific patch is authorized by
this record.
