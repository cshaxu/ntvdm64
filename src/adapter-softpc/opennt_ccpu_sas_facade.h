#ifndef BX_NTVDM_OPENNT_CCPU_SAS_FACADE_H
#define BX_NTVDM_OPENNT_CCPU_SAS_FACADE_H

/*
 * Source-shaped CCPU/SAS mechanical facade for directly imported OpenNT
 * provider bodies.  This is deliberately an alias surface over the one
 * existing call-scoped implementation: it allocates no second frame, guest
 * pointer mapper, or handle table.  The historical provider selects its own
 * service semantics; this component transports only copied registers, checked
 * spans and opaque handles for the duration of that one call.
 */
#include "opennt_dem_ccpu_sas_facade.h"

typedef bx_ntvdm_demhndl_call bx_ntvdm_ccpu_sas_call;

#define BX_NTVDM_CCPU_SAS_CALL_MAGIC BX_NTVDM_DEMHNDL_CALL_MAGIC
#define BX_NTVDM_CCPU_SAS_CALL_VERSION BX_NTVDM_DEMHNDL_CALL_VERSION
#define bx_ntvdm_ccpu_sas_call_valid bx_ntvdm_demhndl_call_valid
#define bx_ntvdm_ccpu_sas_invoke_body bx_ntvdm_demhndl_invoke_body
#define bx_ntvdm_ccpu_sas_invoke_body_with_resume bx_ntvdm_demhndl_invoke_body_with_resume
#define bx_ntvdm_ccpu_sas_get_ax bx_ntvdm_demhndl_get_ax
#define bx_ntvdm_ccpu_sas_get_bx bx_ntvdm_demhndl_get_bx
#define bx_ntvdm_ccpu_sas_get_cx bx_ntvdm_demhndl_get_cx
#define bx_ntvdm_ccpu_sas_get_dx bx_ntvdm_demhndl_get_dx
#define bx_ntvdm_ccpu_sas_get_si bx_ntvdm_demhndl_get_si
#define bx_ntvdm_ccpu_sas_get_di bx_ntvdm_demhndl_get_di
#define bx_ntvdm_ccpu_sas_get_ds bx_ntvdm_demhndl_get_ds
#define bx_ntvdm_ccpu_sas_get_es bx_ntvdm_demhndl_get_es
#define bx_ntvdm_ccpu_sas_set_ax bx_ntvdm_demhndl_set_ax
#define bx_ntvdm_ccpu_sas_set_bx bx_ntvdm_demhndl_set_bx
#define bx_ntvdm_ccpu_sas_set_cx bx_ntvdm_demhndl_set_cx
#define bx_ntvdm_ccpu_sas_set_dx bx_ntvdm_demhndl_set_dx
#define bx_ntvdm_ccpu_sas_set_si bx_ntvdm_demhndl_set_si
#define bx_ntvdm_ccpu_sas_set_di bx_ntvdm_demhndl_set_di
#define bx_ntvdm_ccpu_sas_set_es bx_ntvdm_demhndl_set_es
#define bx_ntvdm_ccpu_sas_set_cf bx_ntvdm_demhndl_set_cf
#define bx_ntvdm_ccpu_sas_get_vdm_addr bx_ntvdm_demhndl_get_vdm_addr
#define bx_ntvdm_ccpu_sas_copy_guest bx_ntvdm_demhndl_copy_guest
#define bx_ntvdm_ccpu_sas_write_guest bx_ntvdm_demhndl_write_guest

#endif
