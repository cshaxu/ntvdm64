"""Run as an owned background process; no physical serial endpoint is opened."""
import argparse
from pathlib import Path
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('--build-root', required=True)
args = parser.parse_args()
repo = Path(__file__).resolve().parents[2]
out = (repo / args.build_root).resolve()
if not out.is_relative_to(repo / 'build') or out.exists():
    raise SystemExit('Fresh repository build root required')
out.mkdir(parents=True)
source = (repo / 'src/mvdm-host/softpc.new/host/src/nt_com.c').read_text(encoding='utf-8')
arm = source.split('case HOST_COM_BAUD:', 1)[1].split('/*:::::::::::::::::::::::::::::::::::::::: Process DTR line requests */', 1)[0]
(out / 'serial_baud_arm.inc').write_text('case HOST_COM_BAUD:' + arm, encoding='utf-8')
fixture = repo / 'test/mvdm-host/serial_baud_test.c'
driver = repo / 'build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'
with (out / 'compile.log').open('w', encoding='utf-8') as log:
    subprocess.run([str(driver), 'cl.exe', '/nologo', '/W4', '/WX', '/D_CRT_SECURE_NO_WARNINGS', '/TC', '/I' + str(out),
                    '/Fo' + str(out / 'fixture.obj'), '/Fe' + str(out / 'fixture.exe'),
                    str(fixture)], cwd=out, stdout=log, stderr=subprocess.STDOUT,
                   timeout=120, check=True)
result = subprocess.run([str(out / 'fixture.exe')], capture_output=True, text=True, timeout=15)
(out / 'result.txt').write_text(result.stdout + result.stderr, encoding='utf-8')
print(result.stdout + result.stderr, end='')
raise SystemExit(result.returncode)
