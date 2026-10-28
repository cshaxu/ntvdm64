import hashlib, json, pathlib, subprocess, sys
root = pathlib.Path(__file__).resolve().parents[3]
out = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else root / 'build/t473-s10/tabstop-release-seams'
if not out.is_absolute(): out = root / out
paths = [root/'src/mvdm/wow32/wmsgem.c', root/'build/.ninja/msvc-x86-release/wow32/obj/wmsgem.obj', pathlib.Path(__file__), pathlib.Path(__file__).with_name('tabstop-release-build.cmd'), pathlib.Path(__file__).with_name('tabstop-release-fixture.c'), pathlib.Path(__file__).with_name('tabstop-release-seams.c'), root/'artifacts/mvdm-audit/shard-2/tabstop-object-types.h', out/'fixture.exe', out/'fixture.map']
results = []
for count in (1, 0, -1):
    for heap in (0, 1):
        for result in (0, 1):
            p = subprocess.run([str(out/'fixture.exe'), str(count), str(heap), str(result)], text=True, capture_output=True)
            if p.returncode: raise RuntimeError(f'{count} {heap} {result}: {p.returncode}: {p.stdout} {p.stderr}')
            results.append({'count':count, 'heap':heap, 'native_result':result, 'exit':p.returncode, 'output':p.stdout.strip()})
print(json.dumps({'sha256':{str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths}, 'results':results}, indent=2))
