#include "../../src/bx-vdm/bx_ntvdm_dem_virtual_mutation_backend_v1.h"
int main(void) { bx_ntvdm_dem_overlay_store_v1 s; DWORD e; int empty;
 if(!bx_ntvdm_dem_overlay_store_v1_initialize(&s)) return 1;
 if(!bx_ntvdm_dem_virtual_mutation_backend_v1_create_directory(&s,2,L"D",&e)||e) return 2;
 if(!bx_ntvdm_dem_virtual_namespace_view_v1_directory_empty(&s,2,L"D",&empty,&e)||!empty) return 3;
 if(!bx_ntvdm_dem_overlay_store_v1_put_file(&s,2,L"D\\A.TXT",0,0,0)) return 4;
 if(!bx_ntvdm_dem_virtual_mutation_backend_v1_remove_directory(&s,2,L"D",&e)||e!=ERROR_DIR_NOT_EMPTY) return 5;
 if(!bx_ntvdm_dem_virtual_mutation_backend_v1_delete_file(&s,2,L"D\\A.TXT",&e)||e) return 6;
 if(!bx_ntvdm_dem_virtual_mutation_backend_v1_remove_directory(&s,2,L"D",&e)||e) return 7;
 bx_ntvdm_dem_overlay_store_v1_teardown(&s); return 0; }
