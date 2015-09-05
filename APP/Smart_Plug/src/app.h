/*****************************************************************************/
/*  Copyright (c) 2011 NXP B.V.  All rights are reserved.                    */
/*  Reproduction in whole or in part is prohibited without the prior         */
/*  written consent of the copyright owner.                                  */
/*                                                                           */
/*  This software and any compilation or derivative thereof is, and          */
/*  shall remain the proprietary information of NXP and is                   */
/*  highly confidential in nature. Any and all use hereof is restricted      */
/*  and is subject to the terms and conditions set forth in the              */
/*  software license agreement concluded with NXP B.V.                       */
/*                                                                           */
/*  Under no circumstances is this software or any derivative thereof        */
/*  to be combined with any Open Source Software, exposed to, or in any      */
/*  way licensed under any Open License Terms without the express prior      */
/*  written permission of the copyright owner.                               */
/*                                                                           */
/*  For the purpose of the above, the term Open Source Software means        */
/*  any software that is licensed under Open License Terms. Open             */
/*  License Terms means terms in any license that require as a               */
/*  condition of use, modification and/or distribution of a work             */
/*                                                                           */
/*  1. the making available of source code or other materials                */
/*     preferred for modification, or                                        */
/*                                                                           */
/*  2. the granting of permission for creating derivative                    */
/*     works, or                                                             */
/*                                                                           */
/*  3. the reproduction of certain notices or license terms                  */
/*     in derivative works or accompanying documentation, or                 */
/*                                                                           */
/*  4. the granting of a royalty-free license to any party                   */
/*     under Intellectual Property Rights                                    */
/*                                                                           */
/*  regarding the work and/or any work that contains, is combined with,      */
/*  requires or otherwise is based on the work.                              */
/*                                                                           */
/*  This software is provided for ease of recompilation only.                */
/*  Modification and reverse engineering of this software are strictly       */
/*  prohibited.                                                              */
/*                                                                           */
/*****************************************************************************/

#ifndef _APP_H_
#define _APP_H_

#include "rtk.h"
#include "metrology.h"

#define SW_VERSION    "2.01"

typedef struct meter_result_tag
{
    uint8_t             data_changed;
    unsigned int        sec_cnt;
    EM_mains_t          mains_freq;
    float               pcounter;   /* Produced energy counter (feed back into grid) */
    float               ecounter;   /* Consumed energy counter */
    metrology_result_t  meter_data;
} meter_result_t;

void    app_main(void *p_arg);
void    copy_calc_results(meter_result_t * ptr_calc_result);
void    print_meter_result(meter_result_t *meter_result);
uint8_t em_get_state(void);
void    em_StartCalibration(float *values);
void    em_NextCalibrationState(void);
void    em_PreviousCalibrationState(void);
void    em_CancelCalibrationState(void);
void    em_StopCalibrationState(void);
void    em_relay_open(void);
void    em_relay_close(void);
char    em_get_relay_state(void);
void    displayMessages();
#endif
