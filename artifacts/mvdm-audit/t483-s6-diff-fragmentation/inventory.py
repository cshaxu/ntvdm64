from pathlib import Path
import re,json,csv,hashlib,difflib,subprocess,importlib.util,sys,argparse
sys.dont_write_bytecode=True
parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,required=True);args=parser.parse_args()
ROOT=Path.cwd(); A=ROOT/'artifacts/mvdm-audit'
output=args.output.resolve()
if not output.is_relative_to((ROOT/'build').resolve()) or output.exists():raise SystemExit('Use a fresh output file under build/')
output.parent.mkdir(parents=True,exist_ok=True)
spec=importlib.util.spec_from_file_location('conditions',ROOT/'tools/audit/inventory-mirror-conditions.py');mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod)
# Extend the existing maintained lexical parser with branch-end physical rows.
src=__import__('inspect').getsource(mod.arm_inventory)
src=src.replace('arm = len(arms)\n                    arms.append', 'arms[stack[-1]["active"]]["end_line"] = line_number\n                    arm = len(arms)\n                    arms.append')
src=src.replace('if stack:\n                    stack.pop()', 'if stack:\n                    arms[stack[-1]["active"]]["end_line"] = line_number\n                    stack.pop()')
ns={'re':re};exec(src,ns);parse=ns['arm_inventory']
def clean(raw):
 return re.sub(r'/\*[\s\S]*?\*/|//[^\r\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',lambda m: re.sub(r'[^\r\n]',' ',m.group()) if m.group().startswith('/') else m.group(),raw)
def metric(a,b):
 ops=[q for q in difflib.SequenceMatcher(None,a,b,autojunk=False).get_opcodes() if q[0]!='equal'];return dict(blocks=len(ops),added=sum(l-k for _,i,j,k,l in ops),deleted=sum(j-i for _,i,j,k,l in ops))
origins={r['path']:r for r in csv.DictReader((A/'t480-s1-files.csv').open(encoding='utf-8-sig'))}
tracked=set(subprocess.check_output(['git','ls-files','src/mvdm-host','src/non-mvdm'],text=True).splitlines())
files=[p for p in tracked if Path(p).suffix in ('.c','.h')]
prior=json.loads((A/'t483-remaining-reaudit/p253-global-diff-count.json').read_text(encoding='utf-8'))
files+= [r['path'] for r in prior['files'] if r['path'] in tracked and r['path'] not in files]
rows=[]
for path in sorted(files):
 p=ROOT/path
 op=Path(origins[path]['original']) if path in origins else (Path('O:/repos.external/OpenNT')/path.removeprefix('src/non-mvdm/') if path.startswith('src/non-mvdm/') else Path('O:/repos.external/OpenNT/base/mvdm')/path.removeprefix('src/mvdm-host/'))
 raw=p.read_bytes();oldraw=op.read_bytes();oldhash=hashlib.sha256(oldraw).hexdigest()
 if path in origins:assert oldhash==origins[path]['original_sha256'],path
 old=oldraw.decode('latin1').replace('\r\n','\n').split('\n');new=raw.decode('latin1').replace('\r\n','\n').split('\n');oa,oe=parse(oldraw);na,ne=parse(raw)
 excluded=[];issues=[];oldkeys={}
 for n,x in enumerate(oa):oldkeys.setdefault((x['directive'],x['expression'],x['logical_line']),[]).append(n)
 # Sequence alignment only disambiguates identical directives; logical anchors take precedence.
 tok=lambda x:(x['directive'],x['expression'])
 seq={}
 for i,j,size in difflib.SequenceMatcher(None,list(map(tok,oa)),list(map(tok,na)),autojunk=False).get_matching_blocks():
  for offset in range(size):seq[j+offset]=i+offset
 group_pairs={}
 for ci,ca in enumerate(na):
  if ca['group']!=ci:continue
  possible=oldkeys.get((ca['directive'],ca['expression'],ca['logical_line']),[])
  if len(possible)==1:group_pairs[ci]=oa[possible[0]]['group']
  elif ci in seq:group_pairs[ci]=oa[seq[ci]]['group']
 used=set();om=[];nm=[]
 for i,x in enumerate(na):
  if not x['excluded']:continue
  # These added guards replace runtime host-arrow calls, not original #if arms.
  # Pairing them by sequence would consume the later cursor_display arm.
  if path.endswith('/keymouse/mouse_io.c') and 'DIVERGENCE(MVDM-HOST-DIV-318)' in new[x['line']-1] and x['directive']=='ifndef' and x['expression']=='NTVDM':
   issues.append({'line':x['line'],'reason':'added-host-boundary-guard-no-original-conditional-counterpart'});continue
  if 'end_line' not in x:issues.append({'line':x['line'],'reason':'unclosed'});continue
  interior=new[x['line']:x['end_line']-1]
  reduced=[s for s in clean('\n'.join(interior)).split('\n') if s.strip() and not re.match(r'^\s*#\s*line\b',s)]
  if len(reduced)!=1 or not re.match(r'^\s*#\s*error\b',reduced[0]):issues.append({'line':x['line'],'reason':'not-error-only-body'});continue
  candidates=set(oldkeys.get((x['directive'],x['expression'],x['logical_line']),[]))
  if x['body_line_marker']:candidates.update(oldkeys.get((x['directive'],x['expression'],x['body_line_marker']-1),[]))
  if x['group'] in group_pairs:
   siblings={j for j,z in enumerate(oa) if z['group']==group_pairs[x['group']] and tok(z)==tok(x)}
   if len(siblings)==1 and (not candidates or bool(siblings&candidates)):candidates=siblings
  if not candidates and i in seq:candidates={seq[i]}
  if len(candidates)!=1:issues.append({'line':x['line'],'reason':'ambiguous-original-arm'});continue
  j=candidates.pop();y=oa[j]
  if j in used or 'end_line' not in y:issues.append({'line':x['line'],'reason':'duplicate-or-open-original'});continue
  # An exact directive/line pin is authoritative for masking; fallback also requires exact sibling structure.
  if tok(x)!=tok(y):raise AssertionError(path)
  oldgroup=[tok(z) for z in oa if z['group']==y['group']];newgroup=[tok(z) for z in na if z['group']==x['group']]
  if oldgroup!=newgroup and x['logical_line']!=y['logical_line'] and x['body_line_marker']!=y['logical_line']+1:issues.append({'line':x['line'],'reason':'group-shape-mismatch'});continue
  used.add(j);mark=f'/* EXCLUDED BODY {y["line"]} */'
  om.append((y['line'],y['end_line']-1,mark));nm.append((x['line'],x['end_line']-1,mark));excluded.append({'original':[y['line']+1,y['end_line']-1],'current':[x['line']+1,x['end_line']-1]})
 def mask(lines,changes):
  for start,end,marker in sorted(changes,reverse=True):lines[start:end]=[marker]
  return [s for s in lines if not re.match(r'^\s*#\s*line\b',s)]
 rawmetric=metric(old,[s for s in new if not re.match(r'^\s*#\s*line\b',s)])
 filtered=metric(mask(old[:],om),mask(new[:],nm))
 if oe or ne:issues.append({'reason':'parser-errors','original':oe,'current':ne})
 rows.append(dict(path=path,original=str(op),original_sha256=oldhash,current_sha256=hashlib.sha256(raw).hexdigest(),raw=rawmetric,filtered=filtered,excluded=excluded,issues=issues))
rows.sort(key=lambda r:(-r['filtered']['blocks'],r['path']))
for i,r in enumerate(rows):r['rank']=i+1
out=dict(scope='Live tracked host C/H plus existing audited build/include/def text mirrors;no guest/binary/resources/deleted files',metric='Same normalized zero-context block metric;mask only paired whole error-only branch bodies in both sides before comparison;keep framework,non-error alternatives and all other differences. Ignore#line for block count,validate separately.',files=rows)
output.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8',newline='\n')
print('files',len(rows),'issues',sum(bool(x['issues']) for x in rows),'blocks',sum(x['filtered']['blocks'] for x in rows))
for r in rows[:20]:print(r['filtered']['blocks'],r['path'],len(r['issues']))
