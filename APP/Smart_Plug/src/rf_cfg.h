#ifndef RF_CFG_H_INCLUDED
#define RF_CFG_H_INCLUDED

#include "bsp.h"

#if defined(SMARTPLUG_COORDINATOR)
    #define ZNP_DEVICETYPE    (COORDINATOR)
#elif defined(SMARTPLUG_ROUTER)
    #define ZNP_DEVICETYPE    (ROUTER)
#else
    #define ZNP_DEVICETYPE    (END_DEVICE)
#endif

#define DEFAULT_CHANNEL                      11
#define ZNP_START_DELAY_IF_FAIL_MS           5000
#define ZNP_RESTART_DELAY_IF_MESSAGE_FAIL_MS 5000
#define TEST_CLUSTER                         0x77
#endif /* end RF_CFG_H_INCLUDED */
