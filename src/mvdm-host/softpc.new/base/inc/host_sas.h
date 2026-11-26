/*[
 *      Product:        SoftPC-AT Revision 3.0
 *
 *      Name:           host_sas.h
 *
 *      Derived From:   New
 *
 *      Authors:        A. Guthrie
 *
 *      Created On:     Wed Apr 24 18:33:01 BST 1991
 *
 *      Sccs ID:        @(#)host_sas.h	1.18 08/10/92
 *
 *      Purpose:        Defines the function pointer interface to host sas
 *			routines.
 *
 *      (c)Copyright Insignia Solutions Ltd., 1991. All rights reserved.
 *
]*/
#if defined(BASE_SAS) && defined(HOST_SAS)

#error Unsupported excluded original configuration
#endif /* BASE_SAS && HOST_SAS */
#line 67

#ifndef BASE_SAS

#ifdef HOST_SAS

#error Excluded unsupported HOST_SAS interface profile
#endif /* HOST_SAS */
#line 200

#endif /* BASE_SAS */
