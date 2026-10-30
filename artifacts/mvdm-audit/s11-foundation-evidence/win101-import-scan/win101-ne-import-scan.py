from pathlib import Path
import struct, hashlib, json

def scan(p):
    b=p.read_bytes()
    if len(b)<64 or b[:2]!=b'MZ': return None
    n=struct.unpack_from('<I',b,60)[0]
    if n+64>len(b) or b[n:n+2]!=b'NE': return None
    count,=struct.unpack_from('<H',b,n+0x1e)
    ref,imp=struct.unpack_from('<HH',b,n+0x28)
    names=[]
    for i in range(count):
        offset,=struct.unpack_from('<H',b,n+ref+i*2)
        pos=n+imp+offset
        size=b[pos]
        if pos+1+size>len(b): raise ValueError('truncated import name')
        names.append(b[pos+1:pos+1+size].decode('ascii'))
    return {'path':str(p),'sha256':hashlib.sha256(b).hexdigest().upper(),'imports':names}

control=scan(Path('O:/w31/CALC.EXE'))
assert control and 'WIN87EM' in control['imports']
rows=[];errors=[]
for p in sorted(Path('O:/W101').rglob('*')):
    if p.is_file() and p.suffix.lower() in ('.exe','.dll','.drv'):
        try:
            row=scan(p)
            if row: rows.append(row)
        except (OSError,ValueError,IndexError,UnicodeError,struct.error) as error:
            errors.append({'path':str(p),'error':str(error)})
data={'root':'O:/W101','count':len(rows),'rows':rows,'errors':errors,'control':control,
      'limitations':'Static module import names only; no dynamic load or runtime evidence.'}
Path('build/t473-s11/win101-ne-import-scan.json').write_text(json.dumps(data,indent=2))
print('count=',len(rows),'win87em=',sum('WIN87EM' in r['imports'] for r in rows),'errors=',len(errors))
