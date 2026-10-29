import hashlib,json,pathlib,subprocess
pkg=pathlib.Path('C:/Users/neko/AppData/Local/Microsoft/WinGet/Packages/BrechtSanders.WinLibs.POSIX.MSVCRT_Microsoft.Winget.Source_8wekyb3d8bbwe/mingw32/bin')
tmp=pathlib.Path('build/t473-s10/dem-diskreset-probe');tmp.mkdir(parents=True,exist_ok=True)
orig=pathlib.Path('build/.ninja/msvc-x86-release/worker/obj/dem/demmisc.obj');src=pathlib.Path('artifacts/mvdm-audit/host-ch-scope/dem-diskreset-fixture.c');exe=tmp/'probe.exe'
build=subprocess.run([str(pkg/'gcc.exe'),'-m32','-Wl,--gc-sections',str(src),str(orig),'-o',str(exe)],capture_output=True,text=True)
(tmp/'build.log').write_text(build.stdout+build.stderr);assert build.returncode==0,build.stderr
run=subprocess.run([str(exe.resolve())],capture_output=True,text=True);print(run.stdout+run.stderr);assert run.returncode==0
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest().upper()
meta={'object_sha256':sha(orig),'source_sha256':sha(pathlib.Path('src/mvdm/dos/dem/demmisc.c')),'fixture_sha256':sha(src),'exe_sha256':sha(exe),'output':run.stdout+run.stderr,'exit_code':run.returncode,'build_diagnostics':build.stderr}
pathlib.Path('artifacts/mvdm-audit/host-ch-scope/dem-diskreset-observation.json').write_text(json.dumps(meta,indent=2)+'\n')
