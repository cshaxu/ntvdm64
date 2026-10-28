import hashlib, json, pathlib, subprocess, sys
root = pathlib.Path(__file__).resolve().parents[3]
out = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else root / 'build/t473-s10/wordbreak-object'
if not out.is_absolute(): out = root / out
paths = [root/'src/mvdm/wow32/wmsgem.c', root/'build/.ninja/msvc-x86-release/wow32/obj/wmsgem.obj', pathlib.Path(__file__), pathlib.Path(__file__).with_name('wordbreak-object-build.cmd'), pathlib.Path(__file__).with_name('wordbreak-object-fixture.c'), pathlib.Path(__file__).with_name('tabstop-link-seams.c'), root/'artifacts/mvdm-audit/shard-2/tabstop-object-types.h', out/'fixture.exe', out/'fixture.map']
results=[]
for case in ('zero','guest-ldt','guest-high','native-fail','native-success','native-cached'):
    p=subprocess.run([str(out/'fixture.exe'),case],text=True,capture_output=True)
    if p.returncode: raise RuntimeError(f'{case}: {p.returncode}: {p.stdout} {p.stderr}')
    results.append({'case':case,'exit':p.returncode,'output':p.stdout.strip()})
print(json.dumps({'sha256':{str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},'results':results},indent=2))
