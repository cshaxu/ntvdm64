from pathlib import Path
import re,bisect,collections,json,hashlib
p=Path('build/t473-s11/native-return-profile');q=Path('build/t473-s11/native-return-profile-sol');t=(q/'probe.txt').read_text();tid=int(re.search(r'game-thread=(\d+)',t)[1]);bounds=[(a,int(b)) for a,b in re.findall(r'SOL profile: (open-start|shown|close-done)=(\d+)',t)];sym=[]
for l in (p/'wow32.map').read_text(errors='replace').splitlines():
 m=re.match(r'\s*0001:[0-9A-Fa-f]+\s+(\S+)\s+([0-9A-Fa-f]{8})\s+f\s+',l)
 if m:sym.append((int(m[2],16)-0x10000000,m[1]))
sym.sort();c=collections.Counter();records=[]
for l in (p/'returns.txt').read_text().splitlines():
 m=re.fullmatch(r'tick=(\d+) tid=(\d+) calls=(\d+) origin=([0-9a-f]+) base=([0-9a-f]+) target=([0-9a-f]+)',l,re.I)
 if not m or int(m[2])!=tid:continue
 tick=int(m[1]);addr=int(m[4],16);base=int(m[5],16);rva=addr-base;idx=bisect.bisect_right(sym,(rva,chr(0x10ffff)))-1; name=sym[idx][1] if base and idx>=0 and rva-sym[idx][0]<0x4000 else '?';phase='outside'
 for i in range(0,len(bounds)-2,3):
  a,b,d=(v for _,v in bounds[i:i+3]);phase=('open' if a<=tick<b else 'close' if b<=tick<=d else phase)
 c[(phase,name,hex(rva))]+=1;records.append({'tick':tick,'phase':phase,'rva':hex(rva),'symbol':name,'target':m[6]})
out={'samples':len(records),'counts':[{'phase':k[0],'symbol':k[1],'rva':k[2],'samples':v} for k,v in c.most_common()],'records':records,'map_sha256':hashlib.sha256((p/'wow32.map').read_bytes()).hexdigest().upper(),'limits':'1/16 return addresses, nearest map symbol, does not observe end_wait or direct inline native_call_end or callbacks and not exact frequency; diagnostics can affect execution.'};(p/'analysis.json').write_text(json.dumps(out,indent=2));print(json.dumps(out['counts'][:18],indent=2))
