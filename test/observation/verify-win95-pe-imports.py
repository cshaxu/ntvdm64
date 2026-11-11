"""Read-only import binding check against the supplied original Win95 DLLs."""
import argparse, hashlib, json, struct
from pathlib import Path

def pe(b):
    u=lambda at:struct.unpack_from('<I',b,at)[0]
    at=u(60);assert b[at:at+4]==b'PE\0\0'
    count=struct.unpack_from('<H',b,at+6)[0];opt=at+24
    assert struct.unpack_from('<H',b,opt)[0]==0x10b
    size=struct.unpack_from('<H',b,at+20)[0]
    sections=[struct.unpack_from('<8s6I',b,opt+size+i*40) for i in range(count)]
    def offset(rva):
        for s in sections:
            if s[2]<=rva<s[2]+max(s[1],s[3]):return s[4]+rva-s[2]
        raise ValueError(hex(rva))
    def string(rva):a=offset(rva);return b[a:b.index(0,a)].decode('ascii')
    return u,opt,offset,string

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--image',required=True);ap.add_argument('--system-directory',required=True);ap.add_argument('--output',required=True);a=ap.parse_args()
    image=Path(a.image);b=image.read_bytes();u,opt,off,string=pe(b);at=off(u(opt+104));rows=[]
    while any(b[at:at+20]):
        original,_,_,module,first=struct.unpack_from('<5I',b,at);at+=20
        name=string(module);thunk=off(original or first);imports=[]
        while u(thunk):
            value=u(thunk);thunk+=4;assert not value&0x80000000
            imports.append(string(value+2))
        source=Path(a.system_directory)/name.upper();dll=source.read_bytes();v,o,f,s=pe(dll)
        export=f(v(o+96));count=v(export+24);names=f(v(export+32));available={s(v(names+i*4)) for i in range(count)}
        missing=sorted(set(imports)-available)
        rows.append(dict(module=name,source=str(source),sha256=hashlib.sha256(dll).hexdigest(),imports=imports,missing=missing))
    result=dict(passed=not any(r['missing'] for r in rows),imageSha256=hashlib.sha256(b).hexdigest(),bindings=rows)
    Path(a.output).write_text(json.dumps(result,indent=2));print(json.dumps(result));assert result['passed']

if __name__=='__main__':main()
