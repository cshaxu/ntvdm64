from pathlib import Path
import hashlib,json,struct
root=Path.cwd()
stage=root/'build/t484-s2-native-generators/stage-input'
manifest=json.loads((Path(__file__).with_name('previous-manifest.json')).read_text(encoding='utf-8-sig'))
targets={'WOW32.DLL':'msvc-x86-release/wow32/wow32.dll'}
for n in ['ntvdm.exe','VCDEX.DLL','VDMREDIR.DLL','nthook32.dll']:targets[n]='msvc-x86-release/worker/'+n
for n,d in [('run16.exe','launcher'),('ntsrv.exe','service'),('ntcon.exe','frontend'),('ntvwm.exe','worker'),('ntmon.exe','monitor'),('nthook64.dll','hook')]:targets[n]='msvc-x64-release/'+d+'/'+n
results=[]
for row in manifest:
    name=row['Name'];p=root/'build/.ninja'/targets[name];b=p.read_bytes();h=hashlib.sha256(b).hexdigest().upper()
    item=dict(image=name,sha256=h,previous_input_sha256=row['InputSha256'],classification='raw-identical' if h==row['InputSha256'] else 'unclassified')
    old=stage/name
    if item['classification']=='unclassified' and old.exists():
        a=old.read_bytes()
        assert hashlib.sha256(a).hexdigest().upper()==row['InputSha256'],name
        if len(a)==len(b):
            differences=[i for i,(x,y) in enumerate(zip(a,b)) if x!=y]
            pe=struct.unpack_from('<I',a,60)[0]
            assert a[pe:pe+4]==b'PE\0\0' and b[pe:pe+4]==b'PE\0\0'
            item.update(differing_bytes=len(differences),offsets= differences[:80])
            def stamps(data):
                p=struct.unpack_from('<I',data,60)[0]; opt=p+24
                magic=struct.unpack_from('<H',data,opt)[0]
                directory=opt+(112 if magic==0x20b else 96)
                sections=p+24+struct.unpack_from('<H',data,p+20)[0]
                def offset(rva):
                    for j in range(struct.unpack_from('<H',data,p+6)[0]):
                        s=sections+j*40
                        size,va,raw,ptr=struct.unpack_from('<IIII',data,s+8)
                        if va<=rva<va+max(size,raw):return ptr+rva-va
                    raise ValueError('unmapped data directory')
                result=[p+8]
                export,size=struct.unpack_from('<II',data,directory)
                if export and size:result.append(offset(export)+4)
                debug,size=struct.unpack_from('<II',data,directory+6*8)
                if debug and size:
                    assert size%28==0
                    result.extend(offset(debug)+j+4 for j in range(0,size,28))
                return result
            astamps=stamps(a);bstamps=stamps(b);assert astamps==bstamps
            allowed={i for stamp in astamps for i in range(stamp,stamp+4)}
            if all(i in allowed for i in differences):
                item['classification']='pe-link-timestamps-only'
                item['timestamp_fields']=astamps
    results.append(item)
out=root/'build/t484-s2-tib/raw-comparison.json'
out.write_text(json.dumps(results,indent=2)+'\n',encoding='utf-8',newline='\n')
print(json.dumps(results,indent=2))
