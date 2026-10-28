import runpy,pathlib,shutil,hashlib,csv,json
root=pathlib.Path('artifacts/mvdm-audit'); dest=pathlib.Path('build/t473-s10/retention-replay');dest.mkdir(parents=True,exist_ok=True)
for n in ('combined-initial-status.csv','supplemental-original-provenance.csv','supplemental-original-provenance-summary.json','frozen-file-reconciliation.csv'): shutil.copyfile(root/n,dest/n)
g=runpy.run_path('tools/audit/EstimateMirrorDiffRetention.py');g['main'].__globals__['AUDIT']=dest;g['main']()
