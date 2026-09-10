# M0 T391 S8 — three immutable ROM executable resources

The CPU40 x86 profile selects exactly three immutable startup ROMs:

| SoftPC filename | PE resource name | bytes |
| --- | --- | ---: |
| `bios1.rom` | `SOFTPC_BIOS1` | 28,420 |
| `bios4.rom` | `SOFTPC_BIOS4` | 8,191 |
| `v7vga.rom` | `SOFTPC_V7VGA` | 18,832 |

The formal build generates the existing SoftPC resource input with those three
`RCDATA` records only. `host_read_resource` maps only the original ROM
filenames for `ROMS_REZ_ID` to those records. All non-ROM resource requests
retain their historical file path, so `profile.spc` and mutable `cmos.ram`
are not EXE data. `bios2.rom` is neither selected by CPU40 nor embedded.

The resource probe passed against the linked x86 candidate:

```text
SOFTPC_BIOS1=ok
SOFTPC_BIOS4=ok
SOFTPC_V7VGA=ok
bios2.rom=absent
profile.spc=absent
cmos.ram=absent
s8-embedded-rom-resources=pass
```

Published product:

```text
O:\\repos.hobby\\ntvdm64\\build\\output\\ntvdm32.exe
O:\\ntvdm64\\ntvdm32.exe
SHA-256 dddc8b0c097b3c20e345189de06aae4ecd215c283ca872df751a871c7930a054
```

All four package ROM files (`bios1.rom`, `bios2.rom`, `bios4.rom`,
`v7vga.rom`) were removed from `O:\\ntvdm64\\softpc` before the final real
Console test. The existing clean-Console matrix still returned zero for tight
`command/c`, ordinary and nested `COMMAND.COM /c ver`, and interactive
`COMMAND.COM` `exit`.
