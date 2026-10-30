import re,pathlib,statistics,json
base=pathlib.Path('build/t473-s11/retire-profile'); skip=int((base/'prior-event-lines.txt').read_text()); lines=(base/'events.log').read_text().splitlines()[skip:]; ev=[]
for l in lines:
 m=re.fullmatch(r'tick=(\d+) thread=(\d+) calls=(\d+) slots=(\d+) pending=(\d+) us=(\d+)',l)
 if m:ev.append(tuple(map(int,m.groups())))
probe=pathlib.Path('build/t473-s11/retire-profile-sol-r2/probe.txt').read_text();bounds=[(k,int(v)) for k,v in re.findall(r'SOL profile: (open-start|shown|close-done)=(\d+)',probe)]; rows=[]
for i in range(0,len(bounds),3):
 if [k for k,v in bounds[i:i+3]]!=['open-start','shown','close-done']:continue
 x,y,z=(v for k,v in bounds[i:i+3]);
 for label,start,end in [('open',x,y),('close',y,z)]:
  a=[e for e in ev if start<=e[0]<=end and e[2]>=64 and e[2]%64==0]; slots=[e[3] for e in a]; times=[e[5] for e in a]
  rows.append({'round':i//3+1,'phase':label,'wall_ms':end-start,'sample_count':len(a),'estimated_scans':len(a)*64,'median_slots':statistics.median(slots) if slots else None,'median_sample_us':statistics.median(times) if times else None,'estimated_inclusive_ms':round(sum(times)*64/1000,1),'pending_samples':sum(e[4]>0 for e in a)})
out={'samples_total':len(ev),'phases':rows,'limitations':'Every 64th scan sampled; scan durations are inclusive and can double-count nested scans, sample estimates assume representative calls, file logging perturbs execution; cross-thread events filtered only by time.'};(base/'analysis-r2.json').write_text(json.dumps(out,indent=2));print(json.dumps(out,indent=2))
