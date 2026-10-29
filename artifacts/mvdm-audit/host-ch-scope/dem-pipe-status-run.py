import hashlib,json,os,pathlib,shutil,subprocess
pkg=os.environ.get('MVDM_AUDIT_MINGW32_BIN')
def tool(name):
    candidate = str(pathlib.Path(pkg) / (name + '.exe')) if pkg else shutil.which(name + '.exe') or shutil.which(name)
    if not candidate or not pathlib.Path(candidate).is_file():
        raise RuntimeError('Set MVDM_AUDIT_MINGW32_BIN to the 32-bit MinGW bin directory or put ' + name + ' on PATH')
    return candidate
tmp=pathlib.Path('build/t473-s10/dem-pipe-status-probe');tmp.mkdir(parents=True,exist_ok=True)
orig=pathlib.Path('build/.ninja/msvc-x86-release/worker/obj/dem/demhndl.obj');src=pathlib.Path('artifacts/mvdm-audit/host-ch-scope/dem-pipe-status-fixture.c');mod=tmp/'demhndl-hook.obj';exe=tmp/'probe.exe'
subprocess.run([tool('objcopy'),'--redefine-sym','__imp__GetFileSize@8=_hook_size',str(orig),str(mod)],check=True)
build=subprocess.run([tool('gcc'),'-m32','-Wl,--gc-sections',str(src),str(mod),'-o',str(exe)],capture_output=True,text=True)
(tmp/'build.log').write_text(build.stdout+build.stderr);assert build.returncode==0,build.stderr
run=subprocess.run([str(exe.resolve())],capture_output=True,text=True);print(run.stdout+run.stderr);assert run.returncode==0
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest().upper()
meta={'object_sha256':sha(orig),'source_sha256':sha(pathlib.Path('src/mvdm/dos/dem/demhndl.c')),'fixture_sha256':sha(src),'exe_sha256':sha(exe),'rebound_import':'GetFileSize','output':run.stdout+run.stderr,'exit_code':run.returncode,'build_diagnostics':build.stderr}
pathlib.Path('artifacts/mvdm-audit/host-ch-scope/dem-pipe-status-observation.json').write_text(json.dumps(meta,indent=2)+'\n')
