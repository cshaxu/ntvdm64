from pathlib import Path
import csv,hashlib,json,os
base=Path('artifacts/mvdm-audit/shard-1'); source=base/'file-audit.csv'; manifest_path=Path('artifacts/documentation-archive/20260910/etc/operations/ledgers/m0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv')
with source.open(encoding='utf-8',newline='') as f: rows=list(csv.DictReader(f)); fields=list(rows[0])
with manifest_path.open(encoding='utf-8-sig',newline='') as f: manifest={r['source_relative_path'].lower():r for r in csv.DictReader(f,delimiter='\t')}
repo=Path('src/mvdm'); primary=Path('O:/repos.external/OpenNT/base/mvdm'); alt=Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'); supplemental=Path('O:/repos.external/opennt-src-2/nt/private/mvdm'); installed=Path('O:/winnt/system32')
installed_index={x.name.lower():x for x in installed.iterdir() if x.is_file()}; h=lambda p:hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else ''
output=[]; failure=[]
for r in rows:
 if r['audit_disposition']!='immutable-guest-or-guest-build-input':continue
 path=r['relative_path']; lower=path.lower(); item=repo/path; got=h(item)
 if got!=r['current_sha256']:failure.append((path,'current identity drift'));continue
 primary_hash=h(primary/path); alt_hash=h(alt/path)
 if primary_hash!=r['opennt_sha256'] or alt_hash!=r['opennt45_sha256']: failure.append((path,'upstream identity drift'));continue
 sup_hash=''; provenance=''; sup_state=''
 if r['current_identity']=='current-only-needs-provenance':
  m=manifest.get(lower); assert m and m['target_sha256'].upper()==got and m['source_sha256'].upper()==got,path
  sup_hash=h(supplemental/path); assert sup_hash==got,path
  provenance='OpenNT-src-2 supplementary source exact; T301 import manifest; T414 original-relative re-root';sup_state=m['disposition']
 elif r['current_identity']=='formatting-only-opennt-selected-or-identical': provenance='Primary OpenNT formatting-only difference; remains partial by owner byte-diff rule'
 elif r['current_identity']=='exact-opennt-on-conflict': provenance='exact OpenNT on two-edition conflict; verify package-level selection rationale'
 elif r['current_identity']=='exact-opennt45-selected-or-identical': provenance='exact OpenNT-4.5; record selected-edition role'
 elif r['current_identity']=='exact-opennt-selected-or-identical': provenance='exact primary OpenNT (may be dual-identical)'
 else: provenance='identity/provenance unresolved'
 ext=item.suffix.lower();
 if ext in ('.obj','.res','.lib','.map','.sym','.cl1','.cl2','.cla'):role='retained guest build object/resource/symbol/intermediate; not host link input'; need='Original guest build provenance recorded; not evidence that this specific intermediate generated deployed guest binary. Guest rebuild prohibited.'
 elif ext in ('.exe','.com','.sys','.dll','.drv','.rom','.bin'):role='historical guest executable/device/firmware candidate; not host link object';need='Identify role and exact selected guest package; deployed byte identity alone is not execution or behavior proof.'
 elif ext in ('.asm','.c','.h','.inc','.def','.skl','.rc','.rcv','.msg'):role='historical guest source/declaration/resource/build input; not host link object';need='Trace original guest build/package lineage and relevant real guest behavior; do not rebuild or modify guest.'
 else:role='guest-family configuration, build or historical artifact; investigate exact role';need='Resolve role and original package relationship; not direct host link evidence.'
 family='other guest-family'
 evidence='docs/etc/operations/policy/source-policy.md; docs/etc/evidence/m0-t473-s1-whole-closure-baseline.md'
 if lower.startswith('dpmi/'): family='DOSX 486 guest package' if lower.startswith('dpmi/486/') else 'DPMI/DOSX guest package'; evidence+='; docs/etc/evidence/m0-t420-s19-dpmi-guest-package-recovery.md'
 elif lower.startswith('dos/v86/cmd/command/'): family='COMMAND guest source/build'; evidence+='; docs/etc/evidence/m0-t473-s2-dos-command-xms-reconciliation.md'
 elif lower.startswith('wow16/kernel31/'):family='Win16 kernel guest source/build'
 elif lower.startswith('wow16/mmsystem/'):family='Win16 multimedia guest source/build';evidence+='; docs/etc/evidence/m0-t466-s1-multimedia-owner-matrix.md'
 elif lower.startswith('wow16/drivers/'):family='Win16 guest display/input driver source/build'
 elif lower.startswith('wow16/ole/'):family='Win16 guest OLE source/build'
 elif lower.startswith('wow16/ddeml/'):family='Win16 guest DDE source/build'
 elif lower.startswith('wow16/'):family='Win16 guest source/tool/build'
 elif lower.startswith('dos/v86/'):family='DOS guest source/tool/build'
 deployed=''; deployed_hash=''; same='not-compared'; candidate=installed_index.get(item.name.lower()) if ext in ('.exe','.com','.sys','.dll','.drv','.rom','.bin') else None
 if candidate: deployed=str(candidate).replace('\\','/');deployed_hash=h(candidate);same='yes' if got==deployed_hash else 'no'
 extra=''
 if lower=='dos/v86/cmd/setver/setver.exe':extra='Installed hash differs by one contiguous 13-byte version-table record at 0x28d; config.nt loads SETVER and autoexec.nt explicitly requests WIN100.BIN 3.30. Configuration explains record shape, not proven exact write time.'
 if same=='yes':extra+=' Deployed same-name bytes match; this is not a guest execution witness.'
 if r['reporting_class']=='partial-mirror-diff': extra+=' Retain partial classification even for formatting-only difference.'
 marker='per-file guest role/hash and installed observations: artifacts/mvdm-audit/shard-1/guest-source-review.csv'
 if marker not in r['evidence']: r['evidence']+='; '+marker
 r['next_required_evidence']=need
 output.append({'relative_path':path,'frozen_current_sha256':got,'frozen_primary_sha256':primary_hash,'frozen_opennt45_sha256':alt_hash,'supplement_sha256':sup_hash,'supplement_manifest_status':sup_state,'source_provenance':provenance,'guest_family':family,'input_role':role,'host_link_selection':r['product_link_selection'],'installed_same_name':deployed,'installed_same_name_sha256':deployed_hash,'installed_matches_mirror':same,'role_evidence':evidence,'remaining':need,'bounded_observation':extra,'audit_status':r['audit_status']})
assert len(output)==625 and not failure, failure
out=base/'guest-source-review.csv'
with out.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=output[0]);w.writeheader();w.writerows(output)
tmp=base/'file-audit-guest.tmp'
with tmp.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=fields);w.writeheader();w.writerows(rows)
os.replace(tmp,source)
print('guest rows',len(output),'supplement exact',sum(bool(r['supplement_sha256']) for r in output),'installed matching',sum(r['installed_matches_mirror']=='yes' for r in output),'installed different',sum(r['installed_matches_mirror']=='no' for r in output),'drift',len(failure))
