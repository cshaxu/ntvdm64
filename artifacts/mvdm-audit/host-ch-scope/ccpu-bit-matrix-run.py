import subprocess,pathlib,os,hashlib,json
w=pathlib.Path('build/.ninja/msvc-x86-release/worker').resolve();tmp=pathlib.Path('build/t473-s10/ccpu-bit-matrix-probe').resolve()
def get(t):return subprocess.check_output(['ninja','-C',str(w),'-t','commands',t],text=True).splitlines()[-1]
def compile_command(obj,src,out):
 cmd=get(obj); old='O:/repos.hobby/ntvdm64/tests/mvdm-host/'+src
 assert '/Fo'+obj in cmd and old in cmd, 'unexpected compiler command; refusing to overwrite a cached target'
 return cmd.replace('/Fo'+obj,'/Fo"'+str(tmp/out)+'"').replace(old,'"'+str(tmp/('seams.c' if 'seams' in src else 'ccpu-fixture.c'))+'"')
a=compile_command('obj/tests/ccpu_bounded_execution_fixture.obj','ccpu_bounded_execution_fixture.c','matrix.obj');b=compile_command('obj/tests/ccpu_bounded_execution_fixture_seams.obj','ccpu_bounded_execution_fixture_seams.c','matrix-seams.obj')
l=get('ccpu-high-linear-page-test.exe')
for token in ('obj/tests/ccpu_bounded_execution_fixture_seams.obj','obj/tests/ccpu_bounded_execution_fixture.obj','/map:ccpu-high-linear-page-test.exe.map','/out:ccpu-high-linear-page-test.exe'):
 assert token in l, 'unexpected linker command; refusing to overwrite a cached target: '+token
l=l.replace('obj/tests/ccpu_bounded_execution_fixture_seams.obj','"'+str(tmp/'matrix-seams.obj')+'" obj/tests/ccpu_host_fixture_seams.obj').replace('obj/tests/ccpu_bounded_execution_fixture.obj','"'+str(tmp/'matrix.obj')+'"').replace('/map:ccpu-high-linear-page-test.exe.map','/map:"'+str(tmp/'matrix.map')+'"').replace('/out:ccpu-high-linear-page-test.exe','/out:"'+str(tmp/'matrix.exe')+'"')
(tmp/'matrix-build.cmd').write_text('@echo off\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\n'+a+'\nif errorlevel 1 exit /b %errorlevel%\n'+b+'\nif errorlevel 1 exit /b %errorlevel%\n'+l+'\nexit /b %errorlevel%\n');r=subprocess.run(['cmd','/c',str(tmp/'matrix-build.cmd')],cwd=w,capture_output=True,text=True,errors='replace');(tmp/'build-output.txt').write_text(r.stdout+r.stderr);assert r.returncode==0, (r.returncode,(r.stdout+r.stderr)[-3000:])
results=[]
for i in range(7):
 env=os.environ.copy();env['BIT_CASE']=str(i);r=subprocess.run([str(tmp/'matrix.exe')],cwd=w,env=env,capture_output=True,text=True,errors='replace',timeout=30);out=r.stdout+r.stderr;(tmp/f'case-{i}.txt').write_text(out);hits=[x for x in out.splitlines() if 'BITCASE=' in x or 'PIC16 vector=' in x];print(i,r.returncode,hits);assert r.returncode==0 and len(hits)==2,(i,r.returncode,out[-1000:]);results.append({'case':i,'exit':r.returncode,'lines':hits})
(tmp/'result.json').write_text(json.dumps({'exe_sha256':hashlib.sha256((tmp/'matrix.exe').read_bytes()).hexdigest().upper(),'results':results},indent=2))
