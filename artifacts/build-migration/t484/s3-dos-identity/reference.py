from pathlib import Path
root=Path(__file__).resolve().parents[2]
out=Path(__file__).resolve().parent/'reference'
out.mkdir(exist_ok=True)
import subprocess
old=subprocess.check_output(['git','show','b9bf7a44f:test/observation/prepare-dos-identity-probe.py'],cwd=root,text=True)
exec(old[old.index("s=(root/"):])