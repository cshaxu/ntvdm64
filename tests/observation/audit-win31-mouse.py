"""Read-only Win3.1 mouse NE/export audit; no implementation import."""
import argparse, hashlib, json, pathlib, struct
p=argparse.ArgumentParser()
p.add_argument('--driver',required=True)
p.add_argument('--output',required=True)
a=p.parse_args();path=pathlib.Path(a.driver);b=path.read_bytes()
out=pathlib.Path(a.output).resolve();repo=pathlib.Path(__file__).resolve().parents[2]
if not out.is_relative_to(repo/'build'):raise ValueError('build-owned output required')
def u16(o):return struct.unpack_from('<H',b,o)[0]
def u32(o):return struct.unpack_from('<I',b,o)[0]
if b[:2]!=b'MZ':raise ValueError('not MZ')
n=u32(60)
if b[n:n+2]!=b'NE':raise ValueError('not NE')
shift=u16(n+50);segments=[]
if shift>16:raise ValueError('invalid segment alignment')
for i in range(u16(n+28)):
    o=n+u16(n+34)+i*8;start=u16(o)<<shift;size=u16(o+2) or 65536
    if start+size>len(b):raise ValueError('truncated segment')
    segments.append({'number':i+1,'offset':start,'length':size,'flags':hex(u16(o+4))})
entries=[];pos=n+u16(n+4);end=pos+u16(n+6);ordinal=1
while pos<end:
    count=b[pos];pos+=1
    if not count:break
    segment=b[pos];pos+=1
    for _ in range(count):
        if segment:
            if segment==255:
                flags=b[pos];actual=b[pos+3];offset=u16(pos+4);pos+=6
                if b[pos-5:pos-3]!=b'\xcd\x3f':raise ValueError('invalid movable-entry marker')
            else:
                flags=b[pos];actual=segment;offset=u16(pos+1);pos+=3
            if pos>end or actual<1 or actual>len(segments):raise ValueError('invalid entry')
            s=segments[actual-1]
            if offset>=s['length']:raise ValueError('entry outside segment')
            entries.append({'ordinal':ordinal,'segment':actual,'offset':hex(offset),
                            'flags':hex(flags),'firstBytes':b[s['offset']+offset:s['offset']+offset+24].hex(' ')})
        ordinal+=1
def names(pos,limit):
    rows=[]
    while pos<limit:
        size=b[pos];pos+=1
        if not size:break
        if pos+size+2>limit:raise ValueError('truncated name')
        rows.append({'name':b[pos:pos+size].decode('ascii'),'ordinal':u16(pos+size)});pos+=size+2
    return rows
imports=[];table=n+u16(n+40)
for i in range(u16(n+30)):
    pos=n+u16(n+42)+u16(table+2*i);size=b[pos]
    imports.append(b[pos+1:pos+1+size].decode('ascii'))
result={'role':'read-only-NE-ABI-evidence-not-runtime-validation','driver':str(path),
        'sha256':hashlib.sha256(b).hexdigest(),'neVersion':hex(u16(n+62)),
        'flags':hex(u16(n+12)),'automaticDataSegment':u16(n+14),
        'segments':segments,'entries':entries,'imports':imports,
        'residentNames':names(n+u16(n+38),len(b)),
        'nonresidentNames':names(u32(n+44),u32(n+44)+u16(n+32))}
out.parent.mkdir(parents=True,exist_ok=True);out.write_text(json.dumps(result,indent=2))
print(json.dumps(result,indent=2))
