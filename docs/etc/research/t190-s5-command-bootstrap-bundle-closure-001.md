# T190 S5 COMMAND bootstrap bundle closure 001

Date: 2026-08-12  
Packet: M0 T190 S5  
Disposition: immutable bundle assembled; no process launched.

`tools/New-T190S5CommandBootstrapBundle.ps1` copied the explicit T190 engine
and retained T189 ROM directory into
`artifacts/build/current/t190-command-bootstrap-bundle-r1`.  It refuses an
existing destination, requires all three ROM inputs, and writes the complete
engine-bundle hash manifest.  It performs no configuration, compilation,
source mutation or process launch.

| Input/output | SHA-256 |
| --- | --- |
| T190 engine / bundled engine | `03C8FFD23FA015B480C02B42250511527450ADE5DCC51F289D4249D617439BCD` |
| BIOS | `D974DE50C94D76445123FD1AC886612351BED7CC5E3C8D07C556814DA72BEBD6` |
| VGA ROM | `CD9FDD6A789DCD22B8A6B3B152788D43238DE49CCE674CFF57BDEB94580246C6` |
| POST option ROM | `E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB` |

The next S must separately admit at most one frozen, no-retry observation.
It must test the actual `54:02` gather/complete path and report only the
bounded result; bundle construction itself proves no guest behavior.
