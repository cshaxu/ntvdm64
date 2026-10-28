from pathlib import Path
s=Path('artifacts/mvdm-audit/nine-way/part-2/rasprint-mini-observer.py').read_text(encoding='utf-8-sig')
s=s.replace("OUT=A/'nine-way/part-2'","OUT=A/'nine-way/part-8'").replace("root=Path('O:/repos.external/OpenNT-4.5')","root=Path('O:/repos.external/OpenNT')").replace("prefix='nt/private/ntos/w32/ntgdi/printers/rasprint/mini/'","prefix='windows/core/ntgdi/printers/rasprint/mini/'").replace("x['bucket']=='2'","x['bucket']=='8'").replace('len(own)==4208','len(own)==4773').replace("c['edition']=='OpenNT-4.5'","c['edition']=='OpenNT'")
Path('artifacts/mvdm-audit/nine-way/part-8/rasprint-mini-observer.py').write_text(s,encoding='utf-8')
