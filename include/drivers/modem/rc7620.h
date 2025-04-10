#ifndef RC7620_CELLULAR_H_
#define RC7620_CELLULAR_H_

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_common.h"
#include "cellular_types.h"
#include "cellular_api.h"
#include "cellular_common_api.h"
#include "cellular_common_portable.h"

CellularError_t rc7620_init(CellularContext_t *pContext);

CellularError_t rc7620_configure(CellularContext_t *pContext,
                                 const CellularCommInterface_t *pCommInterface,
                                 const CellularTokenTable_t *pTokenTable);

CellularError_t rc7620_send_at_command(CellularContext_t *pContext,
                                       const char *pAtCmd,
                                       char *pAtRsp,
                                       uint32_t atRspSize,
                                       uint32_t timeoutMs);

CellularError_t rc7620_power_on(CellularContext_t *pContext);

CellularError_t rc7620_power_off(CellularContext_t *pContext);

CellularError_t rc7620_check_status(CellularContext_t *pContext);

CellularError_t rc7620_test_signal_strength(CellularContext_t *pContext,
                                            CellularSignalInfo_t *pSignalInfo);

CellularError_t rc7620_register_callbacks(CellularContext_t *pContext,
                                          const CellularUrcNetworkRegistrationCallback_t networkRegCallback,
                                          const CellularUrcPdnEventCallback_t pktDataCallback);

CellularError_t rc7620_deinit(CellularContext_t *pContext);

#endif
