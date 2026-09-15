# T412 S11 Console investigation handoff

## Question and bounded predecessor conclusion

On 2026-09-15 the owner requests cleanup and a new S to repair abnormal blank
rows after COMMAND -> EDIT -> exit EDIT -> MEM. S10's broker process-loss
delivery at 157267256 is retained as a bounded conclusion, not full T412
acceptance. S11 receives this regression and the adverse short-window result.
No production source or uncommitted document existed at cleanup start.
No stash or hidden unaccepted product patch is being carried forward.

## Exact inputs

Published O:\winnt products were rehashed unchanged during admission:

| Product | SHA-256 |
| --- | --- |
| run16.exe | 20709EC0F1DB68D465EB994C3D14E134618A2CB289A64B2B4080FCA6E80F6C49 |
| basesrv.exe | 2152439F487D305E77A1AFD5DCC7D7D4F0079AD48E3BBDF398749CC4AD30D8B2 |
| ntvdm.exe | 3344EA956EF4EA19E53966190C922A78A61263D408B4190EF53A57304A1EAE50 |

Formal source/build lineage: S10/process-loss, MSVC Win32/x86 /MT CCPU40,
APP_VERSION 0.0.412 and protocol 3. Immutable guest media remains in `O:\winnt`.
The owner screenshot shows MEM's last lines separated from the prompt by a
large blank area in Windows Terminal over mobile RDP.

## Procedure and observations

Prior investigation used isolated conhost observation and a ConPTY child with
explicit CONIN$/CONOUT$ standard handles. The sequence was initial MEM,
EDIT, Escape to dismiss its welcome, Alt-F/X to exit, MEM twice, then EXIT.
Raw captures remain under `O:\winnt\logs`:

| Input / observation | Result and limitation |
| --- | --- |
| m0-t412-s10-conpty-bound.raw (39053 bytes), initial 45x34 | Contains 80-column resize requests and padded frame writes; bounded VT replay produced up to 23 blank rows. |
| m0-t412-s10-conpty-80x25.raw (37905 bytes) | Same sequence, fixed 80x25 replay: at most one ordinary blank row. |
| m0-t412-s10-conpty-feedback.raw | Experimental resize feedback still clipped text and produced gaps; NOT a fix or passing test. |
| m0-t412-s10-edit-mem-repeat-40/80 observations | Ordinary conhost repeated MEM did not reproduce excessive gaps. This does not validate Terminal. |
| m0-t412-s10-edit-mem-history-40/80 exception reports | Short-window/prefilled-history tests faulted with 0xc0000005; retain as separate unresolved adverse evidence. |

The replay decoder is a bounded test model, not Windows Terminal. Neither it
nor a successful process exit proves GUI/RDP acceptance. Early ConPTY probes
with non-Console inherited handles were invalid startup controls, not product
failures. Requested dimensions must not replace observed dimensions: one
requested tall-window case actually remained 20x5 after failed resizing.

## Interpretation and source boundary

High-confidence working diagnosis is a guest/host geometry and presentation
lifecycle mismatch after EDIT enters frame output. The exact repair is not
yet established by a passing product. It is not evidence of bad MEM guest
semantics or a CCPU instruction defect.

Source inspection identifies nt_graph.c window/buffer sizing and cursor calls,
nt_fulsc.c stream-to-screen transition, and the win32 adapter's
present_text_invalidation. The latter checks WriteConsoleOutputA success but
not complete returned rectangle coverage. Original guest scrolling and mode
selection must remain source-owned; forcing stream mode after EDIT would
bypass rather than repair that contract.

Read-only comparison with the sibling softpc repository (../softpc) found its
src/compat/graphics_console_compat.c logical geometry facade and
src/lib/console-broker/win32/console.c separate cooked/raw screen buffers,
restoration, shared paint/cursor handling and returned-rectangle validation.
Its resize no-ops are meaningful only with that complete presenter. Do not
copy them independently, import the library wholesale, or claim the adjacent
project was tested under this identical RDP setup.

The short-window fault maps into generated C-VID crules (SEVID000/SEVID019);
one write is a diagnostic/history ring entry (2087), not established video
RAM corruption. Causation is unproved. S11 must triage it before claiming
display acceptance; a required CPU-semantic change hits the explicit stop
condition instead of authorizing a trace-driven instruction patch.

## Cleanup and intentional reuse manifest

Research source remains only in build/M0-T412/S10/console-width for intentional
S11 reproduction review, not a formal source/build dependency or delivered
fix. The generated observer includes the existing observation test; review and
promote only the minimal reproducible fixture into tests before acceptance.
The following SHA-256 identities freeze those inputs:

```text
decode.mjs 874F736ECD93050B784DD7C550D11BBEDC13F01E158598D8F09B384E04DD73D6
generate.mjs 1DB02A880487274C7CFF29C414DD841CE798DB710FF1F76CB4AAA679CDFAA814
observer_sequence.c 0E1DE87C031B11A7E1567FF5EF46CDC1AC5972DFC8EA76FF50F582AC37D5FC8E
pty_probe.c FC93144DCC344B42AF62D8E37B7256C7868AF0013555AA3548F517B794E5D399
run.mjs 99C728B0CF7764CC79389DA2B339EE42FAC0DA149612953DF465946B37DE5A05
width_observer.c BB859536EF3ACC17FD2C9ABC724C63BAE40192C80D2F9FD0F4341B6BDD271DAC
```

Remove only width_observer.exe/.obj and pty_probe.exe/.obj from that exact
research directory. Their source is retained; rebuilding must use a fresh
S11 run root and the recorded x86 /MT toolchain. No raw observation is deleted,
no formal S10 build baseline or O:\winnt product is replaced. A fresh process
inventory at admission found no run16, basesrv, ntvdm or research helper
processes; no process termination was necessary.

## Next verification

The [working plan](../operations/m0-t412-broker-restoration-plan.md)
defines source audit, one coherent boundary repair, and real-program checks.
This record is investigation preservation and admission evidence only;
implementation, fresh build and repaired-product acceptance remain pending.
