from pathlib import Path
import re
p=Path(__file__).resolve().parent
result=(p/'negative-result.txt').read_text(errors='replace'); trace=(p/'negative-trace.txt').read_text(errors='replace'); m=(p/'negative-map-excerpt.txt').read_text(errors='replace')
for label,stage in [('ROOT','required-root-capture'),('MENU_OWNER','menu-owner-info')]:
 x=re.search(r'EXPECTED_DEAD_'+label+r'=([0-9A-F]{8})',result); assert x,(label,'missing expected'); assert re.search(r'stage='+stage+r' hwnd='+x.group(1)+r' status=1400\b',trace),(label,'missing actual')
for x in ['WOW_GRAPH_SNAPSHOT_CCPU errors=0','WOW_MENU_DEAD_ENROLLMENT_CCPU errors=0 error=1400','WOW_PAGE_DOMAIN_LIFECYCLE_OK']: assert x in result
for x in ['_wow_user_window_project_graph@8','_wow_menu_client_refresh_all@4']: assert x in m
print('DIAGNOSTIC_IDENTITY_NEGATIVE_OK exact required root and menu HWND, 1400, selected fixture')
