import pathlib,hashlib,json
p=pathlib.Path('artifacts/mvdm-audit/nine-way/part-8'); files=['src/mvdm/dpmi32/i386/dpmimisc.c','O:/repos.external/OpenNT/base/mvdm/dpmi32/i386/dpmimisc.c','O:/repos.external/OpenNT-4.5/nt/private/mvdm/dpmi32/i386/dpmimisc.c','src/mvdm/dpmi32/dpmimscr.c','src/mvdm/dpmi32/dpmi32.c','src/mvdm/dpmi32/sources','tools/build/New-T310OriginalSoftpcNinja.ps1','build/.ninja/msvc-x86-formal-release/build.ninja'];result=[]
for x in files:
 b=pathlib.Path(x).read_bytes();result.append({'path':x,'sha256':hashlib.sha256(b).hexdigest().upper()})
assert result[0]['sha256']==result[1]['sha256']==result[2]['sha256'];(p/'dpmi-iret-provider-quality-evidence.json').write_text(json.dumps(result,indent=2),encoding='utf8')
