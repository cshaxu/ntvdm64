import csv,hashlib,json,collections
from pathlib import Path
base=Path('src/mvdm'); old=Path('O:/repos.external/OpenNT/base/mvdm'); folder=Path('artifacts/mvdm-audit/host-ch-scope')
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def rows(p):
 with p.open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
inv=rows(folder/'inventory.csv'); a=[r for r in inv if r['scope']=='role-pending' and r['path'].startswith('wow16/')]
b={r['path']:r for r in rows(folder/'guest-header-round4-inventory.csv')}
e=json.loads((folder/'guest-header-round4-evidence.json').read_text(encoding='utf8')); witnesses={r['path']:r for r in e['results']}
dos=Path('O:/repos.hobby/ntvdm64-dos16-audit/management/source-file-catalog.csv'); wow=Path('O:/repos.hobby/ntvdm64-wow32-audit/management/source-file-catalog.csv')
d={r['path'].casefold():r for r in rows(dos)};w=collections.defaultdict(list)
for r in rows(wow):w[r['path'].casefold()].append(r)
results=[]
for row in a:
 p=row['path']; src=base/p; ref=old/p; h=sha(src); refh=sha(ref) if ref.exists() else None; key=('base/mvdm/'+p).casefold();
 dr=d.get(key); wr=w.get(key,[]); oldproof=witnesses.get(p); candidate=bool(oldproof and oldproof['candidate'] and h.upper()==oldproof['source_sha256'] and refh==h)
 if candidate:
  for x in oldproof['guest_witnesses']:
   s=base/x['source']; candidate &= sha(s).upper()==x['source_sha256'] and ('#include' in s.read_text(encoding='latin1').splitlines()[x['line']-1].replace(' ',''))
 notes=[]
 if oldproof and not oldproof['candidate']:
  if oldproof['host_same_basename_includes']: notes.append('host basename collision: '+', '.join(oldproof['host_same_basename_includes']))
  if oldproof['host_exact_recorded_dep']:notes.append('recorded exact host dependency')
 if oldproof and oldproof['candidate'] and not candidate:notes.append('stale witness or hash; recheck')
 results.append({'path':p,'result':'guest-build-candidate' if candidate else 'unresolved','sha256':h,'same_primary_original':h==refh,'dos_catalog':dr['sha256'] if dr else '', 'wow_catalog': ';'.join(x['edition']+':'+x['sha256'] for x in wr), 'witnesses':'; '.join(x['source']+':'+str(x['line'])+' -> '+x['target'] for x in oldproof['guest_witnesses']) if oldproof else '', 'notes':'; '.join(notes)})
out=Path('artifacts/mvdm-audit/s10-followup-wow16-ownership.csv')
with out.open('w',encoding='utf8',newline='') as f:
 writer=csv.DictWriter(f,fieldnames=results[0].keys());writer.writeheader();writer.writerows(results)
print('results',collections.Counter(x['result'] for x in results),'matching original',sum(x['same_primary_original'] for x in results),'dos',sum(bool(x['dos_catalog']) for x in results),'wow',sum(bool(x['wow_catalog']) for x in results));print('catalog hashes',sha(dos),sha(wow));print('with collisions',sum(bool(x['notes']) for x in results))
