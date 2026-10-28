# Slave BIOS unexpected-interrupt source finding

Later [BIOS integration evidence](../bios-native-role-review.md) supersedes
the initial pending file disposition with partial integration. The D11
finding below remains source-level and unproved at the real PIC/guest boundary.

This is a source-level finding for S13, not a runtime reproduction, a selected
patch or an explanation of the SOL Options failure. `slave_bs.c` remains
pending in the master ledger until its complete file contract is reconciled.

The selected `bios.c` table routes BOP 72h–74h and 76h–77h to `D11_int`.
The current product map extracts `_D11_int`, `_int_287` and `_re_direct` from
`original-softpc-bios:slave_bs.obj`. This is an actual selected provider,
not merely the similarly named diagnostic string table.

`D11_int` reads both PIC in-service registers. When the master and slave ISR
values are nonzero, it masks the slave's reported line, then its comment says
to send the slave EOI. The actual expression is:

```c
outb( ICA1_PORT_1, 0x20 );
```

The current `ios.h` defines slave port 0 as A0h and port 1 as A1h. In normal
post-initialization operation, `ica.c::SWPIC_outb` explicitly interprets an
odd port as OCW1 and writes `ica_imr`; EOI handling belongs to the command
port. The cited expression therefore appears to replace the just-computed
slave mask with 20h instead of acknowledging its ISR. The subsequent master
EOI does not by itself establish that the slave ISR was cleared.

The [source identities](slave-bios-eoi-source-identities.json) bind the exact
current files and original `slave_bs.c` inputs. This finding must not become
an unverified mirror edit: first exercise the selected original D11 body
with real PIC state, establish ISR/mask/EOI-callback effects, and preserve
the no-ISR and master-only branches. Include non-AEOI and AEOI behavior,
repeat delivery and the current guest interrupt-vector/BOP route. The existing
PIC reset/cancellation regressions do not automatically cover this BIOS
routine. No guest change, fabricated EOI, manually injected success result or
Win95-specific condition is appropriate.

If confirmed, compare the matching original/approved host correction and
register the smallest port-expression repair under the established source
policy, with DOS/protected-mode/WOW/Win3.x regression. The noninteractive
probe/fixture should restore its owned PIC/vector state even on the failing
baseline. Runtime behavior and a fix remain unproven at this checkpoint;
the finite S13 obligation is retained rather than silently treating this
selected source file as recovered.
