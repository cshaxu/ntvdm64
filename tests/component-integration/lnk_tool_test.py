"""Exercise the real offline tool: malformed bounds, CP936, atomic refusal,
variable-length paths, metadata, opaque blocks and supplied original links.
No host COM, link resolution or program launch.
"""
import argparse, hashlib, json, struct, subprocess
from pathlib import Path

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--lnk',required=True);ap.add_argument('--output',required=True)
    ap.add_argument('--originals')
    a=ap.parse_args();out=Path(a.output);out.mkdir(parents=True,exist_ok=False)
    checks=[]
    def run(*args,ok=True):
        p=subprocess.run([a.lnk,*map(str,args)],capture_output=True,timeout=15)
        if ok:assert p.returncode==0,(args,p.returncode,p.stderr)
        else:assert p.returncode!=0,args
        return p.stdout.decode('utf-8').replace('\r','')
    def show(p):return dict(line.split('=',1) for line in run('show',p,'--codepage','936').splitlines())
    link=out/'sample.lnk'
    run('create',link,'--target',r'C:\WINDOWS\NOTEPAD.EXE','--directory',r'C:\WINDOWS',
        '--description','记事本','--arguments','hello world.txt','--icon',r'C:\WINDOWS\NOTEPAD.EXE',
        '--icon-index','-2','--show','3','--hotkey','0x241','--codepage','936')
    before=link.read_bytes();assert show(link)['DESCRIPTION']=='记事本'
    run('create',link,'--target',r'O:\OTHER.EXE',ok=False);assert link.read_bytes()==before
    checks.append('CP936 create, metadata and no-overwrite')
    target=r'O:\LONGROOT\TOOLS\WORDPAD.EXE'
    run('update',link,'--target',target,'--directory',r'O:\LONGROOT\TOOLS','--codepage','936')
    info=show(link)
    assert info['TARGET']==target and info['PIDL']=='1' and info['RELATIVE']==''
    assert info['ARGUMENTS']=='hello world.txt' and info['DESCRIPTION']=='记事本'
    assert info['ICON_INDEX']=='-2' and info['SHOW']=='3' and info['HOTKEY']=='577'
    checks.append('variable-length relocation preserves metadata')
    split=out/'split-path.lnk'
    base=b'C:\\WINDOWS\0';suffix=b'NOTEPAD.EXE\0';li=bytearray(45)
    struct.pack_into('<7I',li,0,45+len(base)+len(suffix),28,1,28,45,0,45+len(base))
    struct.pack_into('<4I',li,28,17,3,0,16)
    old=link.read_bytes();id_end=78+struct.unpack_from('<H',old,76)[0];oldlen=struct.unpack_from('<I',old,id_end)[0]
    split.write_bytes(old[:id_end]+li+base+suffix+old[id_end+oldlen:])
    assert show(split)['TARGET']==r'C:\WINDOWS\NOTEPAD.EXE'
    checks.append('local-base and path-suffix combination')
    good=link.read_bytes()
    for i,bad in enumerate((good[:30],b'junk',good[:76]+b'\xff\xff'+good[78:])):
        p=out/f'bad-{i}.lnk';p.write_bytes(bad)
        run('show',p,'--codepage','936',ok=False)
        run('update',p,'--target',target,'--codepage','936',ok=False);assert p.read_bytes()==bad
    checks.append('malformed header and offset rejection without writes')
    opaque=out/'opaque.lnk';payload=good[:-4]+struct.pack('<II',8,0xa000beef)+b'\0'*4;opaque.write_bytes(payload)
    assert show(opaque)['WRITABLE']=='0'
    run('update',opaque,'--target',target,'--codepage','936',ok=False)
    assert opaque.read_bytes()==payload
    checks.append('unknown extra block is read-only')
    p=out/'unrepresentable.lnk'
    run('create',p,'--target',r'O:\WINDOWS\NOTEPAD.EXE','--description','😀','--codepage','936',ok=False)
    assert not p.exists();checks.append('lossy encoding rejected')
    originals=[]
    if a.originals:
        for i,source in enumerate(sorted(Path(a.originals).rglob('*.LNK'))):
            p=out/f'original-{i}.lnk';p.write_bytes(source.read_bytes());meta=show(p)
            if meta['TARGET']:
                run('update',p,'--target',meta['TARGET'].replace('C:\\','O:\\'),'--codepage','936')
                assert show(p)['PIDL']=='1'
            originals.append(dict(source=str(source),sha256=hashlib.sha256(source.read_bytes()).hexdigest()))
        assert len(originals)>=20
        checks.append('original Win95 links parsed and canonicalized')
    result=dict(passed=True,checks=checks,originalLinks=len(originals),originals=originals)
    (out/'result.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps(dict(passed=True,checks=checks,originalLinks=len(originals))))

if __name__=='__main__':main()
