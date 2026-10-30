from pathlib import Path
import re,json
p=Path('build/t473-s11/page-projection-profile');q=Path('build/t473-s11/page-projection-profile-sol2');t=(q/'probe.txt').read_text();tid=int(re.search(r'game-thread=(\d+)',t)[1]);bounds=[(a,int(b)) for a,b in re.findall(r'SOL profile: (open-start|shown|close-done)=(\d+)',t)];samples=[]
for l in (p/'events2.txt').read_text().splitlines():
 m=re.fullmatch(r'tick=(\d+) tid=(\d+) calls=(\d+) total=(\d+) validation=(\d+) prepare=(\d+) replace=(\d+) hz=(\d+)',l)
 if m and int(m[2])==tid:samples.append(list(map(int,m.groups())))
res=[]
for i in range(0,len(bounds)-2,3):
 a,b,c=(v for _,v in bounds[i:i+3])
 for phase,start,end in [('open',a,b),('close',b,c)]:
  rows=[x for x in samples if start<=x[0]<=end]
  if len(rows)<2:continue
  x,y=rows[0],rows[-1]; hz=x[7];ms=lambda i: round((y[i]-x[i])*1000/hz,1)
  res.append({'round':i//3+1,'phase':phase,'wall_ms':end-start,'covered_ms':y[0]-x[0],'calls':y[2]-x[2],'total_ms':ms(3),'validation_ms':ms(4),'preparation_ms':ms(5),'replacement_ms':ms(6)})
out={'phases':res,'limitations':'64-call cumulative counter deltas omit edges; inclusive timings may include nested calls and instrumentation; preparation excludes cleanup; all results diagnostic.'};(p/'analysis2.json').write_text(json.dumps(out,indent=2));print(json.dumps(out,indent=2))
