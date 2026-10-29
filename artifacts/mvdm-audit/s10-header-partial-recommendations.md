# S10: remaining 46 base/inc headers — explicit partial recommendations

These physical files are retained, byte-identical original interfaces or generated inputs; each is assigned to a named behavior group. Recommend **partial** as retained source/role with incomplete selected-provider and behavior proof. This is not a statement that any unselected interface is live, equivalent, or that runtime validation is complete. The 15 bounded physical exclusions are separate in [the profile review](s10-header-selected-profile-deep-review.md). No code or shared ledger changed.

Direct include hits are textual basename matches in current SoftPC source, not resolved includes; worker entries are exact path hits in VALID-or-invalid Ninja dependency records as reported by `ninja -t deps`, not proof a macro expanded or a linked symbol ran. `none observed` covers this one worker graph only. `j_c_lang.h` appears only in the binding test fixture; actual C-VID object examples resolve to a different `base/cvidc/j_c_lang.h` ([exact path review](host-ch-scope/build-id-generated-allocation-review.md)). See the CSV for all hits and prior evidence references. No tests or guest changes were performed.

| Header | Original role | Exact worker deps; direct includes (bounded) | Explicit remaining owner/contract |
|---|---|---|---|
| `acc_efi.h` | CPU access lookup initializer | none observed; none observed | S11: generated-table inputs, selected producer and memory-access semantics |
| `disk_geo.h` | disk geometry validation and constants | none observed; none observed | S13: geometry/host disk provider, boundary and media behavior |
| `dpmi.h` | DPMI/LDT descriptors and constants | none observed; none observed | S12/S17: exact path resolution and DPMI owner; same-name headers differ |
| `egatrace.h` | EGA read/write trace event constants | none observed; base/video/ega_trcr.c | S17: debug trace provider and scope; S13 retains VGA |
| `getptis.h` | CPU copier action-name initializer | none observed; none observed | S11: generated producer, table indexing and copier ownership |
| `gisp_sas.h` | GISP ROM offsets and BIOS state | obj/bios/emm_fncs.obj; base/bios/emm_fncs.c; base/bios/reset.c | S13/S17: GISP profile/ROM owner and applicable display behavior |
| `gispsvga.h` | GISP ROM initialization declarations | obj/bios/bios.obj; obj/system/rom.obj; base/bios/bios.c; base/system/rom.c | S13/S17: GISP profile and replacement display/ROM behavior |
| `gore.h` | generated graphics offset arithmetic constants | none observed; base/video/vga_mode.c; base/video/ega_prts.c (+2 more) | S13: video transfer/graphics path and state |
| `haw.h` | SoftWindows host audio format interface | none observed; none observed | S13/S17: audio provider and profile applicability |
| `hfx.h` | historical HFX redirector structures and operations | obj/bios/bios.obj; base/bios/bios.c; base/debug/yoda.c (+1 more) | S12/S17: redirector mapping, lifecycle and alternate HFX profile |
| `hg_cpu.h` | host CPU register/access representation | none observed; base/comms/com.c | S11: CPU representation and selected owner |
| `hg_sas.h` | host guest-address memory interface | none observed; none observed | S11: memory mapping/protection and alternate provider |
| `hg_stat.h` | host CPU statistics interface | none observed; none observed | S11/S17: selected execution and optional statistics profile |
| `host_msw.h` | SoftWindows display host constants | none observed; none observed | S13/S17: distinct current display owner and original profile |
| `host_pth.h` | host pathname conversion declarations | none observed; none observed | S12/S17: host path/file owner and error semantics |
| `host_sig.h` | host signal-handler declarations | none observed; none observed | S17: host signal/exception provider and applicability |
| `host_str.h` | host string helper declarations | none observed; none observed | S12/S17: selected string/path provider and failure semantics |
| `host_uis.h` | legacy host user-interface declarations | none observed; none observed | S17: profile and UI provider |
| `hosthunt.h` | HUNTER host configuration and file modes | none observed; none observed | S17: HUNTER provider and diagnostics |
| `hwvga.h` | hardware VGA host interface declarations | none observed; base/support/terminat.c; base/bios/reset.c (+3 more) | S13: selected VGA provider, geometry and access |
| `ipx.h` | IPX packet/operation declarations | none observed; none observed | S12/S17: original network provider, ownership and applicability |
| `j_c_lang.h` | generated C calling/register conventions | obj/tests/cvidc_vector_binding_fixture.obj; base/cpu/src/evid/ev_glue.c; base/cvidc/ev_glue.c (+34 more) | S11/S13: exact generated-header resolution and C-VID ABI |
| `jcgensyn.h` | jcode generator/parser token definitions | none observed; none observed | S11: original generator inputs and reproducibility |
| `jformate.h` | jcode loader binary format | none observed; none observed | S11/S17: generator/loader and profile applicability |
| `lm_code.h` | original licensing vendor codes | none observed; none observed | S17: licensing applicability under source policy |
| `loader.h` | jcode loader and executable memory declarations | none observed; none observed | S11/S17: generator/loader path and memory lifecycle |
| `low_intp.h` | printer emulator text rendering attributes | none observed; none observed | S13/S17: printer provider and text/character behavior |
| `lq2500.h` | LQ-2500 printer interface/constants | none observed; none observed | S13/S17: device provider and printer applicability |
| `m_errorp.h` | legacy host UI error-form interface | none observed; none observed | S17: UI error reporting owner and profile |
| `m_fs.h` | legacy host UI file-selection interface | none observed; none observed | S17: file selection and UI provider |
| `m_lpcmp.h` | legacy host UI component definitions | none observed; none observed | S17: host UI provider and applicability |
| `make_dis.h` | disk image creation/deletion interface | none observed; none observed | S13: disk creation owner, errors and media lifecycle |
| `ms_windo.h` | SoftWindows display/host interface constants | none observed; none observed | S13/S17: selected display owner and original profile |
| `msw_dbg.h` | SoftWindows diagnostics macros | none observed; none observed | S17: debug provider and original profile |
| `msw_keyb.h` | SoftWindows keyboard interface | none observed; none observed | S13/S17: keyboard input semantics and original profile |
| `msw_snd.h` | SoftWindows waveform/audio interface | none observed; none observed | S13/S17: audio provider and original profile |
| `novell.h` | Novell network frame/control definitions | none observed; base/bios/reset.c; base/support/terminat.c | S12/S17: packet/provider and applicability |
| `profile.h` | profiling event/timestamp structures and API | obj/base-debug/profile.obj; base/debug/profile.c | S17: optional profiler lifecycle and profile scope |
| `spc_icon.h` | SoftPC host icon bitmap constants | none observed; none observed | S17: legacy UI/resource ownership |
| `standard.h` | legacy C types and utility macros | none observed; none observed | S17: precise compiler/include ownership |
| `tcpip.h` | TCP/IP operation declarations | none observed; none observed | S12/S17: original network provider and supported scope |
| `tredltr.h` | translator generator/parser token declarations | none observed; none observed | S11: generator input and generated-source ownership |
| `vgastate.h` | VGA register/state snapshot representation | none observed; none observed | S13: real VGA save/restore owner and interoperability |
| `winfiles.h` | SoftWindows HFX file-operation declarations | none observed; base/bios/bios.c | S12/S17: file redirection semantics and SWIN_HFX scope |
| `winhelp.h` | legacy Windows help command declarations | none observed; none observed | S17: original help/UI integration |
| `worm.h` | WORM optical device state declarations | none observed; none observed | S13/S17: device provider and media/error behavior |

CSV: [46 exact-path recommendations](s10-header-partial-recommendations.csv). Regenerate with `python artifacts/mvdm-audit/s10-header-partial-recommendations.py`; the script only reads source and the Ninja dependency log, and writes these two report files. A classification is not a coverage metric.
