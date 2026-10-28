from pathlib import Path
import csv, hashlib, re

root = Path(__file__).resolve().parents[3]
audit = root / 'artifacts/mvdm-audit'
with (audit / 'host-ch-scope/inventory.csv').open(encoding='utf-8-sig', newline='') as f:
    inventory = list(csv.DictReader(f))
with (audit / 'shard-3/header-inclusions.csv').open(encoding='utf-8-sig', newline='') as f:
    deps = {r['relative_path']: r for r in csv.DictReader(f)}
reports = {}
for path in audit.rglob('*.md'):
    if path.name == 'pending-host-header-crosswalk.md':
        continue
    try:
        text = path.read_text(encoding='utf-8', errors='replace')
    except OSError:
        continue
    for filename in set(re.findall(r'(?i)(?<![\w.])[a-z0-9_]+\.h(?![\w.])', text)):
        reports.setdefault(filename.lower(), []).append(path.relative_to(audit).as_posix())

def group(name):
    if re.search(r'cpu|inst|oper|decode|dasm|gisps|gore|j_c_lang|jcgen|jform|low_intp|make_dis|smeg|tredltr|acc_efi|getptis|hg_', name): return 'execution/CPU', 'S11'
    if re.search(r'cdrom|disk|gfi|flop|key|kybd|mouse|vga|herc|mda|bios|rommap|hwvga|hostgrph|nt_paint|nt_conf', name): return 'devices/BIOS/input/video', 'S13'
    if re.search(r'ipx|novell|tcpip|lm_|hfx|host_hfx|winfiles|ms_windo|msw_|dpmi|xmsexp', name): return 'DOS/host services', 'S12/S17'
    return 'profile/host support', 'S17 (scope first)'

def role(s):
    m = re.search(r'(?im)^\s*\*?\s*(?:purpose|description|title)\s*:\s*([^\r\n]{4,150})', s)
    return re.sub(r'\s+', ' ', m.group(1)).strip() if m else 'Header declarations/constants; inspect cited report/source'

rows = []
for rec in inventory:
    rel = rec['path'].replace('\\', '/')
    if rec['scope'] != 'host-candidate' or not re.search(r'^(softpc\.new/(?:base|host)/inc/).+\.h$', rel, re.I) or rec['ledger_status'] not in ('pending','待核验'):
        continue
    p = root / 'src/mvdm' / rel
    ref = Path('O:/repos.external/OpenNT/base/mvdm') / rel
    if not p.is_file():
        desc, same = 'Missing local file; identify original role', 'local missing'
    else:
        raw = p.read_bytes(); desc = role(raw.decode('latin1'))
        same = ('identical' if hashlib.sha256(raw).digest() == hashlib.sha256(ref.read_bytes()).digest() else 'different') if ref.is_file() else 'reference missing'
    d = deps.get(rel, {})
    hits = d.get('selected_product_object_count','not indexed')
    examples = d.get('selected_object_examples','')
    refs = reports.get(p.name.lower(), [])
    cat, stage = group(p.name.lower())
    rows.append([rel,cat,desc,same,hits,examples,'; '.join(refs[:6]),stage,'pending: inspect cited source and active branches; dependency alone is not behavior'])

out = audit / 'shard-3/pending-host-header-crosswalk.csv'
with out.open('w', encoding='utf-8', newline='') as f:
    w=csv.writer(f); w.writerow(['path','owner_group','original_role_excerpt','OpenNT_byte_comparison','cached_exact_dep_count','cached_selected_examples','existing_reports','provisional_stage','safe_current_disposition']); w.writerows(rows)
print(f'rows={len(rows)} output={out}')
