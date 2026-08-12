# T183 S2 CLI shim/bundle recipe 001

Date: 2026-08-12  
Packet: M0 T183 S2  
Disposition: build and assembly recipe defined; no compiler or engine has run.

## Build recipe

Create a new MinGW build root using the same GNU/MinGW Makefiles configuration
as T181. Build exactly these current CMake targets:

- `ntdos64-run`;
- `ntdos64-bochs-engine`;
- `runner-engine-probe`; and
- `bochs-engine-probe`.

The first two are the production CLI process boundary. The probes are needed
only for the existing fixed-bundle policy test. This invokes no Bochs source,
adapter source, engine link recipe, or guest process.

## Bundle recipe

`tools/New-T183S2FixedBochsBundle.ps1` accepts four explicitly named source
files and an empty output root. It copies the linked engine as the one fixed
`ntdos64-bochs.exe` name and copies the exact three ROM files under their fixed
names. Every copy is SHA-256 verified and the script writes exactly the
five-line `ntdos64-bochs-engine-bundle-v1.txt` manifest consumed by the
existing shim. It provides no option mechanism and cannot accept a bochsrc,
profile or BYOB path.

S3 may use this recipe once to compile the CLI target set, policy-test the
shim with the fake parser, and assemble the real fixed bundle. It must not
launch `ntdos64-bochs.exe`; the finite-sequence observation remains separate.
