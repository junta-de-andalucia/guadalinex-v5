/*************************************************************************/
/* module:          SyncML WSP protocol driver                           */
/* file:            src/xpt/palm/xptiwsp.h                               */
/* target system:   palm                                                 */
/* target OS:       palm                                                 */
/*************************************************************************/


/*
 * Copyright Notice
 * Copyright (c) Ericsson, IBM, Lotus, Matsushita Communication 
 * Industrial Co., Ltd., Motorola, Nokia, Openwave Systems, Inc., 
 * Palm, Inc., Psion, Starfish Software, Symbian, Ltd. (2001).
 * All Rights Reserved.
 * Implementation of all or part of any Specification may require 
 * licenses under third party intellectual property rights, 
 * including without limitation, patent rights (such a third party 
 * may or may not be a Supporter). The Sponsors of the Specification 
 * are not responsible and shall not be held responsible in any 
 * manner for identifying or failing to identify any or all such 
 * third party intellectual property rights.
 * 
 * THIS DOCUMENT AND THE INFORMATION CONTAINED HEREIN ARE PROVIDED 
 * ON AN "AS IS" BASIS WITHOUT WARRANTY OF ANY KIND AND ERICSSON, IBM, 
 * LOTUS, MATSUSHITA COMMUNICATION INDUSTRIAL CO. LTD, MOTOROLA, 
 * NOKIA, PALM INC., PSION, STARFISH SOFTWARE AND ALL OTHER SYNCML 
 * SPONSORS DISCLAIM ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO ANY WARRANTY THAT THE USE OF THE INFORMATION 
 * HEREIN WILL NOT INFRINGE ANY RIGHTS OR ANY IMPLIED WARRANTIES OF 
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
 * SHALL ERICSSON, IBM, LOTUS, MATSUSHITA COMMUNICATION INDUSTRIAL CO., 
 * LTD, MOTOROLA, NOKIA, PALM INC., PSION, STARFISH SOFTWARE OR ANY 
 * OTHER SYNCML SPONSOR BE LIABLE TO ANY PARTY FOR ANY LOSS OF 
 * PROFITS, LOSS OF BUSINESS, LOSS OF USE OF DATA, INTERRUPTION OF 
 * BUSINESS, OR FOR DIRECT, INDIRECT, SPECIAL OR EXEMPLARY, INCIDENTAL, 
 * PUNITIVE OR CONSEQUENTIAL DAMAGES OF ANY KIND IN CONNECTION WITH 
 * THIS DOCUMENT OR THE INFORMATION CONTAINED HEREIN, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH LOSS OR DAMAGE.
 * 
 * The above notice and this paragraph must be included on all copies 
 * of this document that are made.
 * 
 */


/**
 * This module contains Palm OS specific definitions for the
 * WSP protocol handler for SyncML
 * invokes the miscellaneous protocol drivers.
 *
 */

#ifndef XPTIWSP_H
#define XPTIWSP_H

#include <DateTime.h>
#include <StringMgr.h>
#include <wspdef.h>

#define atoi(s) StrAToI((s))

unsigned long getTransactionID(void) WSP_SECTION;

unsigned long getTransactionID(void) {
   UInt32 id = TimGetSeconds();

   if (id == -1) {
      /**
       * Not sure what to use for unique ID if the time is unavailable...
       */
      id = 0;
   }

   return (unsigned long)id;

} /* End getTransactionID() */


#endif
