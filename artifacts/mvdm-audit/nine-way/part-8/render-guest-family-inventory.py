import json,collections
from pathlib import Path
p=Path('artifacts/mvdm-audit/nine-way/part-8');d=json.loads((p/'guest-family-inventory-observations.json').read_text(encoding='utf-8-sig'));groups=collections.defaultdict(list)
for x in d['files']:groups[x['path'].split('/')[1]].append(x)
text='''# Part 8: Win16 guest families — identities, source leads and delivered-image samples\n\nThe 112 owned paths outside the separate eight-file EDIT comparison are grouped below. The companion JSON records each frozen source-edition/current SHA-256, family-local literal filename references with source line/snippet (up to 20; full match count retained), and ZIP name/content observations for EXE/DLL/DRV/MOD files. Basename references include comments, conditionals and collisions: they are leads rather than resolved, selected calls. Exact image equality proves content identity only, not recipe provenance, runtime loading or user-visible functionality. Current-only files are flagged individually and may not be called original.\n\n'''
for group,files in groups.items():
 text+='## '+group+'\n\n| Owned key | Original identity and captured lead | Bounded disposition |\n| --- | --- | --- |\n'
 for x in files:
  h=x['hashes'];origin='both editions' if h[1] and h[2] else ('OpenNT-4.5 only' if h[2] else ('OpenNT only' if h[1] else 'current-only'));refs=x['literal_refs'];example=refs[0]['path']+':'+str(refs[0]['line']) if refs else 'no family-local literal hit';ev=f"{origin}; {x['basename_match_count']} refs, e.g. {example}";path=x['path'];status='Pending exact selected profile, current owner and runtime behavior.'
  if origin=='current-only':status='Current-only artifact; exact provenance and consumer pending.'
  elif x['archive_identical']:status='Exact ZIP image match: '+', '.join(x['archive_identical'])+'; candidate partial image identity only, loading/behavior pending.'
  elif path.lower().endswith(('.txt','.doc','.readme')) or Path(path).name.lower()=='readme':status='Historical documentation role; features described remain separate and pending.'
  elif path.lower().endswith(('.bat','.cmd','.sed')):status='Historical build script/transform candidate; do not execute; profile and produced-file lineage pending.'
  text+=f"| `{path}` | {ev.replace('|','/')} | {status} |\n"
 text+='\n'
text+='''Prior shard-2 family reports contain stronger contextual evidence for SYSTEM, TIMER, TOOLHELP, SHELL, WF WNET, WINOLDAP and guest-loader search; integrate only after matching each owned key and frozen identity. In particular `wow16/user` old guest C sources must not be conflated with the separate currently selected NT USER sources, and test/shell WOWEXEC inputs must not be equated with live runtime behavior without binary selection evidence. All timer, task, clipboard, shell, DDE, OLE, memory, callback, message, destruction and failure-handling contracts remain independently scoped.\n\nReplay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-8/guest-family-inventory-observer.py` and compare JSON. No guest, product, build tree, external source, or shared ledger was modified.\n''';(p/'guest-family-inventory-review.md').write_text(text,encoding='utf-8')
