/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * MPFS HAL Embedded Software
 *
 */

 /*========================================================================*//**
  @mainpage Configuration for the MiV-IHC driver

    @section intro_sec Introduction
    Used to configure the driver

    @section

*//*==========================================================================*/


#ifndef MIV_IHC_ADD_MAPPING_H_
#define MIV_IHC_ADD_MAPPING_H_

#ifndef COMMON_AHB_BASE_ADD
#define COMMON_AHB_BASE_ADD     0x50000000UL
#endif
#ifndef IHC_HO_BASE_OFFSET
#define IHC_HO_BASE_OFFSET      0x00000000UL
#endif
#ifndef IHC_H1_BASE_OFFSET
#define IHC_H1_BASE_OFFSET      0x00000500UL
#endif
#ifndef IHC_H2_BASE_OFFSET
#define IHC_H2_BASE_OFFSET      0x00000A00UL
#endif
#ifndef IHC_H3_BASE_OFFSET
#define IHC_H3_BASE_OFFSET      0x00000F00UL
#endif
#ifndef IHC_H4_BASE_OFFSET
#define IHC_H4_BASE_OFFSET      0x00001400UL
#endif

/************** My Hart 0 ************/

#ifndef IHC_LOCAL_H0_REMOTE_H1
#define IHC_LOCAL_H0_REMOTE_H1      0x50000000
#endif

#ifndef IHC_LOCAL_H0_REMOTE_H2
#define IHC_LOCAL_H0_REMOTE_H2      0x50000100
#endif

#ifndef IHC_LOCAL_H0_REMOTE_H3
#define IHC_LOCAL_H0_REMOTE_H3      0x50000200
#endif

#ifndef IHC_LOCAL_H0_REMOTE_H4
#define IHC_LOCAL_H0_REMOTE_H4      0x50000300
#endif

#ifndef IHCIA_LOCAL_H0
#define IHCIA_LOCAL_H0               0x50000400
#endif

/************** My Hart 1 ************/

#ifndef IHC_LOCAL_H1_REMOTE_H0
#define IHC_LOCAL_H1_REMOTE_H0      0x50000500
#endif

#ifndef IHC_LOCAL_H1_REMOTE_H2
#define IHC_LOCAL_H1_REMOTE_H2      0x50000600
#endif

#ifndef IHC_LOCAL_H1_REMOTE_H3
#define IHC_LOCAL_H1_REMOTE_H3      0x50000700
#endif

#ifndef IHC_LOCAL_H1_REMOTE_H4
#define IHC_LOCAL_H1_REMOTE_H4      0x50000800
#endif

#ifndef IHCIA_LOCAL_H1
#define IHCIA_LOCAL_H1               0x50000900
#endif

/************** My Hart 2 ************/

#ifndef IHC_LOCAL_H2_REMOTE_H0
#define IHC_LOCAL_H2_REMOTE_H0      0x50000A00
#endif

#ifndef IHC_LOCAL_H2_REMOTE_H1
#define IHC_LOCAL_H2_REMOTE_H1      0x50000B00
#endif

#ifndef IHC_LOCAL_H2_REMOTE_H3
#define IHC_LOCAL_H2_REMOTE_H3      0x50000C00
#endif

#ifndef IHC_LOCAL_H2_REMOTE_H4
#define IHC_LOCAL_H2_REMOTE_H4      0x50000D00
#endif

#ifndef IHCIA_LOCAL_H2
#define IHCIA_LOCAL_H2               0x50000E00
#endif

/************** My Hart 3 ************/

#ifndef IHC_LOCAL_H3_REMOTE_H0
#define IHC_LOCAL_H3_REMOTE_H0      0x50000F00
#endif

#ifndef IHC_LOCAL_H3_REMOTE_H1
#define IHC_LOCAL_H3_REMOTE_H1      0x50001000
#endif

#ifndef IHC_LOCAL_H3_REMOTE_H2
#define IHC_LOCAL_H3_REMOTE_H2      0x50001100
#endif

#ifndef IHC_LOCAL_H3_REMOTE_H4
#define IHC_LOCAL_H3_REMOTE_H4      0x50001200
#endif

#ifndef IHCIA_LOCAL_H3
#define IHCIA_LOCAL_H3               0x50001300
#endif

/************** My Hart 4 ************/

#ifndef IHC_LOCAL_H4_REMOTE_H0
#define IHC_LOCAL_H4_REMOTE_H0      0x50001400
#endif

#ifndef IHC_LOCAL_H4_REMOTE_H1
#define IHC_LOCAL_H4_REMOTE_H1      0x50001500
#endif

#ifndef IHC_LOCAL_H4_REMOTE_H2
#define IHC_LOCAL_H4_REMOTE_H2      0x50001600
#endif

#ifndef IHC_LOCAL_H4_REMOTE_H3
#define IHC_LOCAL_H4_REMOTE_H3      0x50001700
#endif

#ifndef IHCIA_LOCAL_H4
#define IHCIA_LOCAL_H4               0x50001800
#endif

/*------------------------------------------------------------------------------
 * choose the interrupt mapping used in our system
 * Please see miv_ihc_regs.h for the defaults
 */
#define IHCIA_hart0_IRQHandler PLIC_f2m_63_IRQHandler
#define IHCIA_hart1_IRQHandler PLIC_f2m_62_IRQHandler
#define IHCIA_hart2_IRQHandler PLIC_f2m_61_IRQHandler
#define IHCIA_hart3_IRQHandler PLIC_f2m_60_IRQHandler
#define IHCIA_hart4_IRQHandler PLIC_f2m_59_IRQHandler

#define IHCIA_hart0_INT  FABRIC_F2H_63_PLIC
#define IHCIA_hart1_INT  FABRIC_F2H_62_PLIC
#define IHCIA_hart2_INT  FABRIC_F2H_61_PLIC
#define IHCIA_hart3_INT  FABRIC_F2H_60_PLIC
#define IHCIA_hart4_INT  FABRIC_F2H_59_PLIC

/* HSS to HART interrupts must use locals */
#define IHCIA_HSS_to_hart4_IRQHandler U54_f2m_28_local_IRQHandler
#define IHCIA_HSS_to_hart3_IRQHandler U54_f2m_29_local_IRQHandler
#define IHCIA_HSS_to_hart2_IRQHandler U54_f2m_30_local_IRQHandler
#define IHCIA_HSS_to_hart1_IRQHandler U54_f2m_31_local_IRQHandler

#define IHCIA_HSS_TO_HART4_INT   U54_F2M_28_INT_OFFSET
#define IHCIA_HSS_TO_HART3_INT   U54_F2M_29_INT_OFFSET
#define IHCIA_HSS_TO_HART2_INT   U54_F2M_30_INT_OFFSET
#define IHCIA_HSS_TO_HART1_INT   U54_F2M_31_INT_OFFSET

#endif /* MIV_IHC_ADD_MAPPING_H_ */

