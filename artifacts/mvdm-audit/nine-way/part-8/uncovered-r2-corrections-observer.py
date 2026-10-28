import csv,pathlib
p=pathlib.Path('artifacts/mvdm-audit/nine-way/part-8')
with (p/'existing-classifications-uncovered-r2.csv').open(encoding='utf-8-sig',newline='') as f:rows=list(csv.DictReader(f))
source={'.asm','.inc','.c','.h','.ext'};messages={'.ctl','.cl1','.cl2','.cl3','.cl4','.cla','.clb','.clf','.skl'};out=[]
for r in rows:
 suffix=pathlib.PurePosixPath(r['path']).suffix.lower()
 if r['classification'] not in ('excluded','整体排除') or suffix not in source|messages:continue
 kind='source/interface' if suffix in source else 'message/build data'
 out.append({'path':r['path'],'shard':r['shard'],'current':r['classification'],'recommended':'pending','category':kind,'basis':r['evidence'],'limit':r['limit'],'reason':'Existing evidence limits exclusion to guest build/file role; applicable guest capabilities and source-to-image correspondence remain unresolved.'})
with (p/'existing-classifications-uncovered-r2-corrections.csv').open('w',encoding='utf-8',newline='') as f:
 w=csv.DictWriter(f,fieldnames=['path','shard','current','recommended','category','basis','limit','reason']);w.writeheader();w.writerows(out)
print(len(out),'source/data recommendations; excludes metadata and executable images')
