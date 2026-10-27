"""Reproduce the frozen pending guest-library container observations.

Run from the repository root. This inspects retained files only; it does not
link them, validate dictionary/trailer semantics, or execute guest code.
"""
import collections,csv,hashlib,json,struct
from pathlib import Path
A=Path('artifacts/mvdm-audit');M=Path('src/mvdm')
def rows(p):return list(csv.DictReader(p.open(encoding='utf-8-sig',newline='')))
selected=[r for r in rows(A/'combined-initial-status.csv') if r['status']=='pending' and r['relative_path'].lower().endswith('.lib')]
assert len(selected)==32
frozen={r['relative_path']:r for r in rows(A/'frozen-file-reconciliation.csv')}
out=[];detail=[]
for row in selected:
 rel=row['relative_path'];b=(M/rel).read_bytes();h=hashlib.sha256(b).hexdigest().upper()
 assert h==frozen[rel]['current_sha256'],(rel,'frozen identity changed')
 if rel=='dos/v86/cmd/append/sysmac.lib':
  assert b[:1]==b';' and b'DISP   MACRO' in b and b'include sysmac.lib' in (M/'dos/v86/cmd/append/append.asm').read_bytes().lower()
  out.append(dict(relative_path=rel,sha256=h,format='MASM source text, not a library archive',modules=0,records=0,library_end=False,dictionary_bytes=0,trailer_bytes=0,checksum_nonzero_mismatches=0,scope='Source include role; macro functionality not executed'))
  continue
 assert len(b)>=16 and b[0]==0xf0,rel
 page=struct.unpack_from('<H',b,1)[0]+3;dictionary=struct.unpack_from('<I',b,3)[0];blocks=struct.unpack_from('<H',b,7)[0]
 assert page>=16 and page&(page-1)==0 and page<=dictionary<=len(b) and dictionary+blocks*512<=len(b),rel
 pos=page;module=False;modules=records=0;libend=False;mismatches=[];padding=[];segments=collections.Counter()
 while pos<dictionary:
  kind=b[pos]
  if kind==0xf1:
   assert not module and pos+3<=dictionary,rel
   length=struct.unpack_from('<H',b,pos+1)[0]
   assert pos+3+length<=dictionary,rel
   libend=True;pos=pos+3+length
   padding.append(dict(offset=pos,bytes=dictionary-pos,nonzero=sum(x!=0 for x in b[pos:dictionary])))
   pos=dictionary;break
  assert pos+3<=dictionary,(rel,pos)
  length=struct.unpack_from('<H',b,pos+1)[0];end=pos+3+length
  assert length>=1 and end<=dictionary,(rel,pos,kind,length)
  if not module:
   assert kind in (0x80,0x82),(rel,pos,kind,'missing THEADR/LHEADR')
   assert b[pos+3]<=length-2,(rel,pos,'module name exceeds header')
   module=True;modules+=1
  else:assert kind not in (0x80,0x82),(rel,pos,'nested header')
  if kind in (0x98,0x99):segments['use32' if b[pos+3]&1 else 'use16']+=1
  if b[end-1] and sum(b[pos:end])&255:mismatches.append(dict(offset=pos,type=kind,stored=b[end-1]))
  records+=1;pos=end
  if kind in (0x8a,0x8b):
   module=False;aligned=(pos+page-1)//page*page
   assert aligned<=dictionary,(rel,pos,'alignment beyond dictionary')
   if any(b[pos:aligned]):padding.append(dict(offset=pos,bytes=aligned-pos,nonzero=sum(x!=0 for x in b[pos:aligned])))
   pos=aligned
 assert not module and libend,(rel,pos,'module or library end missing')
 assert not mismatches,(rel,'nonzero stored record checksum mismatch',mismatches)
 assert not any(p['nonzero'] for p in padding),(rel,'nonzero alignment padding',padding)
 out.append(dict(relative_path=rel,sha256=h,format='OMF LIBHDR/module framing/LIBEND/dictionary bounds',modules=modules,records=records,library_end=libend,dictionary_bytes=blocks*512,trailer_bytes=len(b)-dictionary-blocks*512,checksum_nonzero_mismatches=len(mismatches),scope='Structure only; dictionary and trailer semantics, linking and functionality not validated'))
 detail.append(dict(relative_path=rel,page_bytes=page,dictionary_offset=dictionary,dictionary_blocks=blocks,segment_attributes=dict(segments),checksum_observations=mismatches,padding_observations=padding,trailer_prefix=b[dictionary+blocks*512:][:16].hex()))
p=A/'guest-library-format-observations.csv'
with p.open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=list(out[0]));w.writeheader();w.writerows(out)
summary=dict(result='pass',paths=len(out),omf_libraries=31,macro_sources=1,modules=sum(r['modules'] for r in out),records=sum(r['records'] for r in out),nonzero_checksum_observations=sum(r['checksum_nonzero_mismatches'] for r in out),details=detail,
 script_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest().upper(),
 scope='Read-only framed container observations, not full OMF/dictionary semantics or guest behavior')
(A/'guest-library-format-observations.json').write_text(json.dumps(summary,indent=2)+'\n')
print(json.dumps({k:v for k,v in summary.items() if k!='details'}))
