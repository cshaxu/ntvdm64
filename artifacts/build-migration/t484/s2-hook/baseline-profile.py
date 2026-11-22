from pathlib import Path
import subprocess
root=Path.cwd()
text=subprocess.check_output(['git','show','161b9dc27:test/build/test-authored-softpc-profile.py'],text=True)
filename=str(root/'test/build/test-authored-softpc-profile.py')
exec(compile(text,filename,'exec'),{'__name__':'__main__','__file__':filename})
