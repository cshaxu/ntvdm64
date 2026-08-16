#include "../../src/bx-vdm/bx_ntvdm_dem_virtual_mutation_backend_v1.h"
int main(void) { bx_ntvdm_dem_overlay_store_v1 s; DWORD e; int empty;
 bx_ntvdm_dem_overlay_namespace_node_v1 n;
 if(!bx_ntvdm_dem_overlay_store_v1_initialize(&s)) return 1;
 if(!bx_ntvdm_dem_virtual_mutation_backend_v1_create_directory(&s,2,L"D",&e)||e) return 2;
 if(!bx_ntvdm_dem_virtual_namespace_view_v1_directory_empty(&s,2,L"D",&empty,&e)||!empty) return 3;
 if(!bx_ntvdm_dem_overlay_store_v1_put_file(&s,2,L"D\\A.TXT",0,0,0)) return 4;
 if(!bx_ntvdm_dem_virtual_mutation_backend_v1_remove_directory(&s,2,L"D",&e)||e!=ERROR_DIR_NOT_EMPTY) return 5;
 if(!bx_ntvdm_dem_virtual_mutation_backend_v1_delete_file(&s,2,L"D\\A.TXT",&e)||e) return 6;
 if(!bx_ntvdm_dem_virtual_mutation_backend_v1_remove_directory(&s,2,L"D",&e)||e) return 7;
 if(!bx_ntvdm_dem_virtual_mutation_backend_v1_create_directory(&s,2,L"SRC",&e)||e) return 8;
 if(!bx_ntvdm_dem_virtual_mutation_backend_v1_create_directory(&s,2,L"SRC\\SUB",&e)||e) return 9;
 if(!bx_ntvdm_dem_overlay_store_v1_put_file(&s,2,L"SRC\\SUB\\A.TXT",0,0,0)) return 10;
 if(!bx_ntvdm_dem_overlay_store_v1_tombstone(&s,2,L"SRC\\SUB\\HIDE.TXT")) return 11;
 if(!bx_ntvdm_dem_virtual_mutation_backend_v1_rename(&s,2,L"SRC",2,L"DST",&e)||e) return 12;
 if(!bx_ntvdm_dem_virtual_namespace_view_v1_query(&s,2,L"SRC",&n,&e)||n.kind!=BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) return 13;
 if(!bx_ntvdm_dem_virtual_namespace_view_v1_query(&s,2,L"DST\\SUB\\A.TXT",&n,&e)||n.kind!=BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_FILE) return 14;
 if(!bx_ntvdm_dem_virtual_namespace_view_v1_query(&s,2,L"DST\\SUB\\HIDE.TXT",&n,&e)||n.kind!=BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT) return 15;
 if(!bx_ntvdm_dem_virtual_mutation_backend_v1_rename(&s,2,L"DST",3,L"OTHER",&e)||e!=ERROR_NOT_SAME_DEVICE) return 16;
 if(!bx_ntvdm_dem_virtual_mutation_backend_v1_rename(&s,2,L"DST",2,L"DST\\LOOP",&e)||e!=ERROR_ACCESS_DENIED) return 17;
 bx_ntvdm_dem_overlay_store_v1_teardown(&s); return 0; }
