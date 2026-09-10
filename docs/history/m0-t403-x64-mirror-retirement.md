# M0 T403 historical record

## M0 T403 S1 closure — x64 mirror and mapping retirement

T403 retired the current product's native-x64/cross-width-only residue while
preserving the sole Win32/x86 CCPU40 product. It removed the project-authored
mapping manager and its three session namespaces, the host-identity facade,
the mailslot identity adapter, and their fixtures/build inputs. Reached DEM,
DPMI32, Redirector, VDD, SoftPC, XMS, COMMAND, dialog and presentation paths
now restore their selected original x86 pointer, `HANDLE`, `DWORD` or scalar
carriers where they compose directly.

The closure also removed the OEM pointer-width overlay, XMS A20 lease overlay,
dialog context adapter, UMB numeric codec, COMMAND native-length helpers, and
remaining x64-only sentinel/config/C-VID edits. The evidence inventory records
the small retained seams: bounded guest-memory leases, physical-page mapping,
DEM fast-read fallback, CCPU generated-vector binding, and the x86 PIC EOI
type bridge. Each has a source-specific x86 reason and carries no generic
identity token or native-x64 product claim.

Verification passed on 2026-09-10:

- The r011 formal CCPU40 x86 graph completed 434 actions and linked.
- The PE reports `14C machine (x86)`.
- The focused Redirector x86 handle fixture passed after mailslot-adapter
  removal.
- `build/output/ntvdm32.exe` and `O:\ntvdm64\ntvdm32.exe` were staged from
  r011 with SHA-256
  `479f38247f9640272a24e5518247d2185dea9a93c6975ab5412f12a44370f54c`.
- The owner successfully tested the deployed executable.
- Documentation governance, relative links, build-script syntax,
  removed-component reference search, and `git diff --check` passed.

The detailed per-surface inventory is retained in
[`m0-t403-s1-x86-mirror-retirement-inventory-001.md`](../etc/evidence/m0-t403-s1-x86-mirror-retirement-inventory-001.md).
