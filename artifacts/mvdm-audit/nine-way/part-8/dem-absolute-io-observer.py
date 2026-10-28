import pathlib,hashlib,json
files=['src/mvdm/dos/v86/doskrnl/dos/mscode.asm','src/mvdm/dos/dem/demdisp.c','src/mvdm/dos/dem/demdasd.c','src/mvdm/softpc.new/host/src/nt_fdisk.c','O:/repos.external/OpenNT/base/mvdm/softpc.new/host/src/nt_fdisk.c','O:/repos.external/OpenNT-4.5/nt/private/mvdm/softpc.new/host/src/nt_fdisk.c','tests/component-integration/fixtures/t230_s7_demdasd_ioctl_direct_import_fixture.c','tests/legacy/opennt/trace/fixtures/contained_media_fdisk_test.c'];out=[]
for x in files:
 p=pathlib.Path(x);out.append({'path':x,'sha256':hashlib.sha256(p.read_bytes()).hexdigest().upper()})
pathlib.Path('artifacts/mvdm-audit/nine-way/part-8/dem-absolute-io-evidence.json').write_text(json.dumps(out,indent=2),encoding='utf8')
