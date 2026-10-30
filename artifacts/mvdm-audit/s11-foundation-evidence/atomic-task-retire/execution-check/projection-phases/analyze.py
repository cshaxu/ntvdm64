import pathlib,re,json
p=pathlib.Path('build/t473-s11/projection-phase-current');q=pathlib.Path('build/t473-s11/projection-phase-current-sol');txt=(q/'probe.txt').read_text();tid=int(re.search(r'game-thread=(\d+)',txt)[1]);bounds=[(a,int(b)) for a,b in re.findall(r'SOL profile: (open-start|shown|close-done)=(\d+)',txt)]; rows=[]
pat=r'tick=(\d+) tid=(\d+) calls=(\d+) total_ticks=(\d+) captures=(\d+) capture_ticks=(\d+) normal_ticks=(\d+) commit_ticks=(\d+) hz=(\d+)'
for line in (p/'phases.txt').read_text().splitlines():
 m=re.fullmatch(pat,line)
 if m:
  a=list(map(int,m.groups()))
  if a[1]==tid:rows.append(a)
results=[]
for i in range(0,len(bounds),3):
 if len(bounds[i:i+3])<3:continue
 a,b,c=(v for _,v in bounds[i:i+3])
 for phase,start,end in [('open',a,b),('close',b,c)]:
  inside=[z for z in rows if start<=z[0]<=end]
  if len(inside)<2:continue
  x,y=inside[0],inside[-1];hz=x[8]; ms=lambda idx:round((y[idx]-x[idx])*1000/hz,1)
  results.append({'round':i//3+1,'phase':phase,'wall_ms':end-start,'covered_ms':y[0]-x[0],'projections':y[2]-x[2],'projection_inclusive_ms':ms(3),'capture_inclusive_ms':ms(5),'normalize_inclusive_ms':ms(6),'commit_inclusive_ms':ms(7)})
out={'thread':tid,'phases':results,'limits':'Cumulative inclusive timings over first-to-last 64-call flush within each phase, so omit partial intervals; profiler adds QPC and file I/O overhead, nesting can double count, and commit/capture are subsets of projection. Not production wall-time attribution.'};(p/'phase-analysis.json').write_text(json.dumps(out,indent=2));print(json.dumps(out,indent=2))
