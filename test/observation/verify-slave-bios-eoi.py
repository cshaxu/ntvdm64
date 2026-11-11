"""Build a bounded fixture against the current production PIC/BIOS archives.
Run this driver as an owned background process. Nonzero fixture is preserved.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess

p=argparse.ArgumentParser()
p.add_argument('--build-root',required=True)
a=p.parse_args()
repo=Path(__file__).resolve().parents[2]
out=(repo/a.build_root).resolve()
if not out.is_relative_to(repo/'build') or out.exists():
    raise SystemExit('Fresh repository build root required')
out.mkdir(parents=True)
cache=repo/'build/.ninja/msvc-x86-release/worker'
with (out/'dependencies.txt').open('w',encoding='utf-8') as log:
    subprocess.run(['cmd.exe','/d','/c',str(cache/'run-ninja-parallel.cmd'),
                    'pic-reset-eoi-test.exe'],cwd=repo,stdout=log,
                   stderr=subprocess.STDOUT,check=True)
graph=(cache/'build.ninja').read_text()
flags=re.search(r'^cflags = (.*)$',graph,re.M)[1].replace('$:',':')
release=re.search(r'^release_cflags = (.*)$',graph,re.M)[1]
inputs=re.search(r'^build pic-reset-eoi-test.exe: memory_test_link (.*)$',graph,re.M)[1].split()
assert inputs[0]=='obj/tests/pic_reset_eoi_test.obj'
source=repo/'test/mvdm-host/slave_bios_eoi_test.c'
obj=out/'fixture.obj'
exe=out/'fixture.exe'
(out/'compile.rsp').write_text(f'{flags} {release} /Fo"{obj}" "{source}"',encoding='utf-8')
inputs=[f'"{obj}"']+[f'"{cache/i}"' for i in inputs[1:]]
(out/'link.rsp').write_text('/nologo /force:multiple '+f'/map:"{exe}.map" /out:"{exe}" '+
    ' '.join(inputs)+' kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib rpcrt4.lib libcmt.lib libvcruntime.lib libucrt.lib',encoding='utf-8')
driver=repo/'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
for step,tool in [('compile','cl.exe'),('link','link.exe')]:
    with (out/f'{step}.txt').open('w',encoding='utf-8') as log:
        subprocess.run([str(driver),tool,'@'+str(out/f'{step}.rsp')],cwd=repo,
                       stdout=log,stderr=subprocess.STDOUT,timeout=120,check=True)
mapped=Path(str(exe)+'.map').read_text()
for name,owner in [('D11_int','original-softpc-bios:slave_bs.obj'),
                   ('ica_intack','original-softpc-system:ica.obj'),
                   ('ica_outb','original-softpc-system:ica.obj')]:
    assert re.search(r'_'+name+r'\s+[^\n]*'+re.escape(owner),mapped),name
result=subprocess.run([str(exe)],capture_output=True,text=True,timeout=15)
(out/'result.txt').write_text(result.stdout+result.stderr,encoding='utf-8')
sha=lambda path:hashlib.sha256(path.read_bytes()).hexdigest().upper()
paths=[source,repo/'src/mvdm-host/softpc.new/base/bios/slave_bs.c',
       repo/'src/mvdm-host/softpc.new/base/system/ica.c',exe,Path(str(exe)+'.map'),
       cache/'original-softpc-bios.lib',cache/'original-softpc-system.lib']
(out/'result.json').write_text(json.dumps(dict(exit=result.returncode,
    identities={str(path):sha(path) for path in paths}),indent=2),encoding='utf-8')
print(result.stdout+result.stderr,end='')
raise SystemExit(result.returncode)
