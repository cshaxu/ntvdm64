import pathlib,hashlib,json
fs=['src/mvdm/dos/v86/doskrnl/dos/lock.asm','src/mvdm/dos/dem/demlock.c','src/mvdm/dos/dem/demdisp.c','src/mvdm/dos/dem/demerror.c','tests/legacy/opennt/trace/fixtures/original_demlock_register_fixture.c','artifacts/mvdm-audit/shard-2/parallel-review-evidence.json','build/.ninja/msvc-x86-formal-release/build.ninja'];out=[]
for s in fs:
 p=pathlib.Path(s);out.append({'path':s,'sha256':hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None})
pathlib.Path('artifacts/mvdm-audit/nine-way/part-8/dem-lock-guest-host-evidence.json').write_text(json.dumps(out,indent=2),encoding='utf8')
