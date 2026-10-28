# Pending disk and input headers: selected roles

This groups existing host C/H findings, not a new runtime test. The eight named headers below match the corresponding OpenNT source bytes. Captured x86 dependency and source evidence is profile-specific; no guest or physical device was accessed.

| Header | Evidence and recommended status |
|---|---|
| `gfiempty.h` | No selected inclusion found. `gfi_mpty.c` instead defines its `gfi_empty_*` routines `LOCAL`, and installs them into the selected GFI table; these are not the header's external declarations. The actual selected object/recording fixture and no-media INT13 reset provide bounded partial evidence for **empty GFI behavior**, not evidence that this header is selected or real media works. Classify the header as unselected in this profile; preserve the implementation's partial status. |
| `kybdmtx.h` | Original keyboard matrix declarations; no captured include or selected provider demonstrated. Keep the original matrix capability pending, not equivalent to Windows key translation. |
| `keycaps.h` | Human-readable test labels gated by `UIF_TEST`, with no current include found. Safe to exclude that test-only table from the observed production profile, while retaining the historical file; decide test-profile applicability separately. |
| `m_keybdp.h` | Original X KeySym-to-AT matrix table; no current include found. X-host profile unselected in the captured NT worker; retain pending original capability/scope assessment. |
| `mouse16b.h` | Selected `mouse_io.c` includes it only under `MOUSE_16_BIT`, absent from the captured worker dependencies and flags. That interface is unselected; mouse driver/guest applicability remains pending. Selected generic mouse code is a separate partial owner. |
| `msw_keyb.h` | SoftWindows keyboard driver interface; original `bios.c` BOP path uses `MSWDVR`, absent in the selected profile. That alternate interface remains pending scope/provider analysis; selected NT keyboard/controller path does not prove SoftWindows-driver equivalence. |
| `cdrom.h`, `cdrom_fn.h` | Original SoftPC CD packet and host/SCSI interfaces have no observed selected consumers. Keep capability and provider closure pending; a separate MSCDEX/VDD or Windows drive-type branch does not prove these contracts. |

For existing exact source/fixture/map context and missing tests, see `../host-ch-scope/keyboard-mouse-alternate-header-review.md`, `../host-ch-scope/softwindows-input-audio-profile-review.md`, `../host-ch-scope/cdrom-provider-boundary-review.md`, and `../disk-native-role-review.md`. No unselected header is promoted to a functioning provider; no shared ledger changed.
