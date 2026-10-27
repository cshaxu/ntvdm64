import hashlib, json, subprocess
from pathlib import Path

root=Path('artifacts/mvdm-audit')
names=subprocess.check_output(['git','ls-files','--',str(root)],text=True).splitlines()
reader=subprocess.Popen(['git','cat-file','--batch'],stdin=subprocess.PIPE,stdout=subprocess.PIPE)
newline_only=[];semantic=[];same=0
try:
    for name in names:
        reader.stdin.write(('HEAD:'+name+'\n').encode());reader.stdin.flush()
        header=reader.stdout.readline().decode().strip().split()
        assert len(header)==3 and header[1]=='blob',(name,header)
        old=reader.stdout.read(int(header[2]));assert reader.stdout.read(1)==b'\n'
        current=Path(name).read_bytes()
        if current==old:same+=1;continue
        record=dict(path=name,working_sha256=hashlib.sha256(current).hexdigest().upper(),
                    git_blob_sha256=hashlib.sha256(old).hexdigest().upper(),bytes=len(current))
        if current.replace(b'\r\n',b'\n')==old.replace(b'\r\n',b'\n'):newline_only.append(record)
        else:semantic.append(record)
finally:
    reader.stdin.close();reader.wait();assert reader.returncode==0
result=dict(tracked=len(names),same=same,newline_only=newline_only,semantic_changes=semantic,
            scope='Read-only HEAD/working-byte comparison, no rewrite or Git mutation')
Path('build/t473-s10/audit-evidence-git-bytes.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(dict(tracked=len(names),same=same,newline_only=len(newline_only),
                     newline_only_bytes=sum(r['bytes'] for r in newline_only),semantic_changes=len(semantic))))
