# Part 8: WFWNET guest network and provider boundaries

The seven owned paths below are individually identified against both original source editions in `guest-family-inventory-review.md`. Original `wow16/wfwnet/makefile:100-145` builds a Win16 `WFWNET.DRV` from wfwnet/to32/print/misc/lfn/ints and libentry objects and resource, with the export definition. This is a historical recipe, not proof of source-to-binary reconstruction. `wfwnet.def` declares a WOW replacement and preserves network/print/LFN ordinal identities.

`to32.c` explicitly resolves Win32 endpoints and calls them via `CallProc32W`. For example WNetAddConnection validates parameters, lazily resolves MPR, allocates aligned copies, maps the Win32 error and exits through cleanup. The guest owns its thunk arguments and local state; host MPR owns the actual connection. Print calls have a distinct print-provider path; do not manufacture success for unavailable devices or services. `ints.asm` first registers the IPX/SPX VDD and only then installs guest interrupt vectors when registration succeeds. This is source-level behavior, not proof that a modern IPX provider exists or vectors are restored on every failure.

| Owned path | File role and classification boundary |
| --- | --- |
| `wow16/wfwnet/ints.asm` | Original VDD registration and interrupt hook implementation, linked by original recipe. Preserve failure branch and original ownership; registration, coexistence and teardown require runtime evidence. |
| `wow16/wfwnet/lfn.c` | Original LFN functions explicitly return `WN_NOT_SUPPORTED`; this is an original functional limit, not a promise of long-name support or grounds to remove all WFWNET. |
| `wow16/wfwnet/misc.c` | Recipe-selected guest network capability/error helpers. Per-function success/failure still requires real caller proof. |
| `wow16/wfwnet/print.c` | Recipe-selected guest print forwarding; preserve print job close/error ownership, do not infer a functional printer. |
| `wow16/wfwnet/to32.c` | Recipe-selected network/host thunk and aligned temporary buffers. Host owns native resources; guest owns each temporary and error mapping. |
| `wow16/wfwnet/wfwnet.def` | Original guest exported ordinal contract; preserve, not a new host implementation. |
| `wow16/wfwnet/wfwnet.rc` | Historical resource input selected by recipe; packaged resource identity and localized variant not established. |

Historical `m0-t422-s2-user-client-lifecycle-ledger.md:625-679,840-860` records an identity-pinned WFWNET profile as a branch in a WinMine class/window witness. The relevant pass is a startup/profile witness, not proof of network connection, printing, IPX delivery or teardown. `m0-t468-s1-printing-spool-owner-matrix.md` explicitly separates WNET job entries and WFWNET exports. No runtime tests were executed here.

S14 should use an unchanged guest caller for host MPR success/failure and aligned-buffer cleanup, then verify interrupt registration failure, vector restoration and task exit under a controlled provider. S13 owns hardware or device-dependent network/print behavior, and S19 binds guest image plus profile to release evidence. LFN unsupported is an original boundary: do not silently convert it to success. Preserve external environment and user data; this review neither modifies guest nor installs a network provider.

Replay: `rg -n "CallProc32W|Get32BitEntryPoints|GetAlignedMemory|FreeAlignedMemory" src/mvdm/wow16/wfwnet/to32.c`, read the makefile/definition, `ints.asm` and `lfn.c`; rerun part-8 inventory observer. No shared ledger or product changes.
