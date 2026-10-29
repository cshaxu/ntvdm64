"""Build the 46-path S10 header review; does not edit product or shared ledger."""
import csv
import pathlib
import re
import subprocess

ROOT = pathlib.Path(__file__).resolve().parents[2]
OUT = pathlib.Path(__file__).resolve().parent
skip = set('chkmallc gfisflop mouse16b herc mda rommap cdrom cdrom_fn d_inst d_oper dasm decode smeg smeg_hea mark_ima'.split())
roles = {
 'acc_efi': ('CPU access lookup initializer', 'S11: generated-table inputs, selected producer and memory-access semantics'),
 'disk_geo': ('disk geometry validation and constants', 'S13: geometry/host disk provider, boundary and media behavior'),
 'dpmi': ('DPMI/LDT descriptors and constants', 'S12/S17: exact path resolution and DPMI owner; same-name headers differ'),
 'egatrace': ('EGA read/write trace event constants', 'S17: debug trace provider and scope; S13 retains VGA'),
 'getptis': ('CPU copier action-name initializer', 'S11: generated producer, table indexing and copier ownership'),
 'gisp_sas': ('GISP ROM offsets and BIOS state', 'S13/S17: GISP profile/ROM owner and applicable display behavior'),
 'gispsvga': ('GISP ROM initialization declarations', 'S13/S17: GISP profile and replacement display/ROM behavior'),
 'gore': ('generated graphics offset arithmetic constants', 'S13: video transfer/graphics path and state'),
 'haw': ('SoftWindows host audio format interface', 'S13/S17: audio provider and profile applicability'),
 'hfx': ('historical HFX redirector structures and operations', 'S12/S17: redirector mapping, lifecycle and alternate HFX profile'),
 'hg_cpu': ('host CPU register/access representation', 'S11: CPU representation and selected owner'),
 'hg_sas': ('host guest-address memory interface', 'S11: memory mapping/protection and alternate provider'),
 'hg_stat': ('host CPU statistics interface', 'S11/S17: selected execution and optional statistics profile'),
 'host_msw': ('SoftWindows display host constants', 'S13/S17: distinct current display owner and original profile'),
 'host_pth': ('host pathname conversion declarations', 'S12/S17: host path/file owner and error semantics'),
 'host_sig': ('host signal-handler declarations', 'S17: host signal/exception provider and applicability'),
 'host_str': ('host string helper declarations', 'S12/S17: selected string/path provider and failure semantics'),
 'host_uis': ('legacy host user-interface declarations', 'S17: profile and UI provider'),
 'hosthunt': ('HUNTER host configuration and file modes', 'S17: HUNTER provider and diagnostics'),
 'hwvga': ('hardware VGA host interface declarations', 'S13: selected VGA provider, geometry and access'),
 'ipx': ('IPX packet/operation declarations', 'S12/S17: original network provider, ownership and applicability'),
 'j_c_lang': ('generated C calling/register conventions', 'S11/S13: exact generated-header resolution and C-VID ABI'),
 'jcgensyn': ('jcode generator/parser token definitions', 'S11: original generator inputs and reproducibility'),
 'jformate': ('jcode loader binary format', 'S11/S17: generator/loader and profile applicability'),
 'lm_code': ('original licensing vendor codes', 'S17: licensing applicability under source policy'),
 'loader': ('jcode loader and executable memory declarations', 'S11/S17: generator/loader path and memory lifecycle'),
 'low_intp': ('printer emulator text rendering attributes', 'S13/S17: printer provider and text/character behavior'),
 'lq2500': ('LQ-2500 printer interface/constants', 'S13/S17: device provider and printer applicability'),
 'm_errorp': ('legacy host UI error-form interface', 'S17: UI error reporting owner and profile'),
 'm_fs': ('legacy host UI file-selection interface', 'S17: file selection and UI provider'),
 'm_lpcmp': ('legacy host UI component definitions', 'S17: host UI provider and applicability'),
 'make_dis': ('disk image creation/deletion interface', 'S13: disk creation owner, errors and media lifecycle'),
 'ms_windo': ('SoftWindows display/host interface constants', 'S13/S17: selected display owner and original profile'),
 'msw_dbg': ('SoftWindows diagnostics macros', 'S17: debug provider and original profile'),
 'msw_keyb': ('SoftWindows keyboard interface', 'S13/S17: keyboard input semantics and original profile'),
 'msw_snd': ('SoftWindows waveform/audio interface', 'S13/S17: audio provider and original profile'),
 'novell': ('Novell network frame/control definitions', 'S12/S17: packet/provider and applicability'),
 'profile': ('profiling event/timestamp structures and API', 'S17: optional profiler lifecycle and profile scope'),
 'spc_icon': ('SoftPC host icon bitmap constants', 'S17: legacy UI/resource ownership'),
 'standard': ('legacy C types and utility macros', 'S17: precise compiler/include ownership'),
 'tcpip': ('TCP/IP operation declarations', 'S12/S17: original network provider and supported scope'),
 'tredltr': ('translator generator/parser token declarations', 'S11: generator input and generated-source ownership'),
 'vgastate': ('VGA register/state snapshot representation', 'S13: real VGA save/restore owner and interoperability'),
 'winfiles': ('SoftWindows HFX file-operation declarations', 'S12/S17: file redirection semantics and SWIN_HFX scope'),
 'winhelp': ('legacy Windows help command declarations', 'S17: original help/UI integration'),
 'worm': ('WORM optical device state declarations', 'S13/S17: device provider and media/error behavior'),
}
source = ROOT / 'artifacts/mvdm-audit/shard-3/pending-host-header-crosswalk.csv'
rows = [r for r in csv.DictReader(source.open(encoding='utf-8-sig'))
        if r['path'].startswith('softpc.new/base/inc/')
        and r['path'].split('/')[-1][:-2] not in skip]
assert len(rows) == len(roles) == 46
deps_text = subprocess.run(['ninja', '-C', 'build/.ninja/msvc-x86-release/worker', '-t', 'deps'],
                           cwd=ROOT, check=True, stdout=subprocess.PIPE).stdout.decode('utf-8', 'replace')
deps, current = {}, ''
for line in deps_text.splitlines():
    if line and not line[0].isspace(): current = line.split(': #deps')[0]
    norm = line.strip().replace('\\', '/').lower()
    if '/softpc.new/base/inc/' in norm:
        deps.setdefault(norm.rsplit('/', 1)[-1], []).append(current)
output = []
for r in rows:
    name = r['path'].rsplit('/', 1)[-1]
    stem = name[:-2]
    pattern = r'#\s*include\s*[<\"]' + re.escape(name) + r'[>\"]'
    hit = subprocess.run(['rg', '-l', '-i', pattern, 'src/mvdm/softpc.new'], cwd=ROOT,
                         text=True, capture_output=True)
    assert hit.returncode in (0, 1)
    includes = [x.replace('\\', '/').removeprefix('src/mvdm/softpc.new/') for x in hit.stdout.splitlines()]
    output.append({'path': r['path'], 'original_role': roles[stem][0],
                   'worker_exact_deps': '; '.join(deps.get(name.lower(), [])) or 'none observed',
                   'direct_named_includes': '; '.join(includes) or 'none observed',
                   'recommendation': 'partial: original interface/table retained; selected behavior not established',
                   'handoff': roles[stem][1], 'prior_evidence': r['existing_reports']})
with (OUT / 's10-header-partial-recommendations.csv').open('w', newline='', encoding='utf-8') as f:
    w = csv.DictWriter(f, fieldnames=output[0].keys()); w.writeheader(); w.writerows(output)
md = ['# S10: remaining 46 base/inc headers — explicit partial recommendations', '',
      'These physical files are retained, byte-identical original interfaces or generated inputs; each is assigned to a named behavior group. Recommend **partial** as retained source/role with incomplete selected-provider and behavior proof. This is not a statement that any unselected interface is live, equivalent, or that runtime validation is complete. The 15 bounded physical exclusions are separate in [the profile review](s10-header-selected-profile-deep-review.md). No code or shared ledger changed.', '',
      'Direct include hits are textual basename matches in current SoftPC source, not resolved includes; worker entries are exact path hits in VALID-or-invalid Ninja dependency records as reported by `ninja -t deps`, not proof a macro expanded or a linked symbol ran. `none observed` covers this one worker graph only. `j_c_lang.h` appears only in the binding test fixture; actual C-VID object examples resolve to a different `base/cvidc/j_c_lang.h` ([exact path review](host-ch-scope/build-id-generated-allocation-review.md)). See the CSV for all hits and prior evidence references. No tests or guest changes were performed.', '',
      '| Header | Original role | Exact worker deps; direct includes (bounded) | Explicit remaining owner/contract |', '|---|---|---|---|']
for r in output:
    direct = r['direct_named_includes'].split('; ')
    summary = '; '.join(direct[:2]) + (f' (+{len(direct)-2} more)' if len(direct)>2 else '')
    md.append(f"| `{r['path'].rsplit('/',1)[-1]}` | {r['original_role']} | {r['worker_exact_deps']}; {summary} | {r['handoff']} |")
md += ['', 'CSV: [46 exact-path recommendations](s10-header-partial-recommendations.csv). Regenerate with `python artifacts/mvdm-audit/s10-header-partial-recommendations.py`; the script only reads source and the Ninja dependency log, and writes these two report files. A classification is not a coverage metric.']
(OUT / 's10-header-partial-recommendations.md').write_text('\n'.join(md)+'\n', encoding='utf-8')
