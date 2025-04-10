#include "rc7620.h"

#define RC7620_DEFAULT_TIMEOUT_MS (1000UL)
#define RC7620_INIT_TIMEOUT_MS (5000UL)
#define RC7620_MAX_BAND_CONFIG_LENGTH (128)
#define RC7620_DEFAULT_AT_COMMAND_BUFFER_LENGTH (128)
#define RC7620_NETWORK_REGSTATUS_ATTEMPTS (20)
#define RC7620_SIGNAL_QUALITY_POLLING_INTERVAL_MS (1000UL)
#define RC7620_AT_COMMAND_RESPONSE_MAX_SIZE (500)

static CellularError_t prvRC7620UartInit(void);
static CellularError_t prvRC7620GpioInit(void);
static void prvRC7620ReleaseResourcesOnError(CellularContext_t *pContext);
static CellularError_t prvRC7620ConfigureModule(CellularContext_t *pContext);
static CellularError_t prvRC7620HandleUrcState(CellularContext_t *pContext);
static CellularError_t prvRC7620GetSignalInfo(CellularContext_t *pContext,
                                              CellularSignalInfo_t *pSignalInfo);
static CellularError_t prvRC7620GetNetworkRegistrationStatus(CellularContext_t *pContext,
                                                             CellularNetworkRegistrationStatus_t *pNetworkStatus);
static CellularError_t prvRC7620SetPDN(CellularContext_t *pContext,
                                       uint8_t contextId,
                                       const CellularPdnConfig_t *pPdnConfig);

CellularError_t rc7620_init(CellularContext_t *pContext)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if (pContext == NULL)
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Initialize UART and GPIO. */
        cellularStatus = prvRC7620UartInit();

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            cellularStatus = prvRC7620GpioInit();
        }

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Initialize the cellular common library. */
            cellularStatus = Cellular_CommonInit(pContext);
        }

        if (cellularStatus != CELLULAR_SUCCESS)
        {
            prvRC7620ReleaseResourcesOnError(pContext);
        }
    }

    return cellularStatus;
}

CellularError_t rc7620_configure(CellularContext_t *pContext,
                                 const CellularCommInterface_t *pCommInterface,
                                 const CellularTokenTable_t *pTokenTable)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if ((pContext == NULL) || (pCommInterface == NULL) || (pTokenTable == NULL))
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Initialize the module specific context. */
        pContext->pModuleContext = NULL;

        /* Configure the communication interface. */
        cellularStatus = Cellular_CommonSetCommIntf(pContext, pCommInterface);

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Setup token table. */
            cellularStatus = Cellular_CommonSetTokenTable(pContext, pTokenTable);
        }

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Configure the module. */
            cellularStatus = prvRC7620ConfigureModule(pContext);
        }

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Setup URC handlers. */
            cellularStatus = Cellular_CommonSetUrcHandlerTable(pContext, NULL, 0);
        }

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Handle initial URC state. */
            cellularStatus = prvRC7620HandleUrcState(pContext);
        }

        if (cellularStatus != CELLULAR_SUCCESS)
        {
            prvRC7620ReleaseResourcesOnError(pContext);
        }
    }

    return cellularStatus;
}

CellularError_t rc7620_send_at_command(CellularContext_t *pContext,
                                       const char *pAtCmd,
                                       char *pAtRsp,
                                       uint32_t atRspSize,
                                       uint32_t timeoutMs)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularPktStatus_t pktStatus = CELLULAR_PKT_STATUS_OK;
    CellularATCommandResponse_t atResp;

    if ((pContext == NULL) || (pAtCmd == NULL) || (pAtRsp == NULL) || (atRspSize == 0))
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        memset(&atResp, 0, sizeof(CellularATCommandResponse_t));

        /* Use common cellular API to send AT command. */
        pktStatus = Cellular_ATCommandRaw(pContext, atResp.pAtResp,
                                          atRspSize, pAtCmd, timeoutMs);

        if (pktStatus != CELLULAR_PKT_STATUS_OK)
        {
            cellularStatus = _Cellular_TranslatePktStatus(pktStatus);
        }
        else
        {
            /* Copy the response to the output buffer. */
            if (atResp.pAtResp != NULL)
            {
                strncpy(pAtRsp, atResp.pAtResp, atRspSize);
            }
        }
    }

    return cellularStatus;
}

CellularError_t rc7620_power_on(CellularContext_t *pContext)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if (pContext == NULL)
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Set the power pin high. */
        /* TODO: Implement hardware-specific power control. */

        /* Wait for the module to power up. */
        vTaskDelay(pdMS_TO_TICKS(RC7620_INIT_TIMEOUT_MS));

        /* Check if the module is responsive. */
        cellularStatus = Cellular_CommonATCommandRaw(pContext, NULL, 0, "AT", RC7620_DEFAULT_TIMEOUT_MS);
    }

    return cellularStatus;
}

CellularError_t rc7620_power_off(CellularContext_t *pContext)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if (pContext == NULL)
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Send power down command. */
        cellularStatus = Cellular_CommonATCommandRaw(pContext, NULL, 0, "AT+CPWROFF", RC7620_DEFAULT_TIMEOUT_MS);

        /* Set the power pin low. */
        /* TODO: Implement hardware-specific power control. */

        /* Wait for the module to power down. */
        vTaskDelay(pdMS_TO_TICKS(RC7620_DEFAULT_TIMEOUT_MS));
    }

    return cellularStatus;
}

CellularError_t rc7620_check_status(CellularContext_t *pContext)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularNetworkRegistrationStatus_t networkStatus;

    if (pContext == NULL)
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Check if the module is responsive. */
        cellularStatus = Cellular_CommonATCommandRaw(pContext, NULL, 0, "AT", RC7620_DEFAULT_TIMEOUT_MS);

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Check network registration status. */
            cellularStatus = prvRC7620GetNetworkRegistrationStatus(pContext, &networkStatus);
        }
    }

    return cellularStatus;
}

CellularError_t rc7620_test_signal_strength(CellularContext_t *pContext,
                                            CellularSignalInfo_t *pSignalInfo)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if ((pContext == NULL) || (pSignalInfo == NULL))
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        cellularStatus = prvRC7620GetSignalInfo(pContext, pSignalInfo);
    }

    return cellularStatus;
}

CellularError_t rc7620_register_callbacks(CellularContext_t *pContext,
                                          const CellularUrcNetworkRegistrationCallback_t networkRegCallback,
                                          const CellularUrcPdnEventCallback_t pktDataCallback)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if (pContext == NULL)
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Register network registration callback. */
        cellularStatus = Cellular_CommonRegisterUrcNetworkRegistrationCallback(pContext, networkRegCallback, NULL);

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Register packet data callback. */
            cellularStatus = Cellular_CommonRegisterUrcPdnEventCallback(pContext, pktDataCallback, NULL);
        }
    }

    return cellularStatus;
}

CellularError_t rc7620_deinit(CellularContext_t *pContext)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if (pContext == NULL)
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Power off the module. */
        (void)rc7620_power_off(pContext);

        /* De-initialize the cellular common library. */
        cellularStatus = Cellular_CommonCleanup(pContext);
    }

    return cellularStatus;
}

/*-----------------------------------------------------------*/

/* Private function implementations */

static CellularError_t prvRC7620UartInit(void)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    /* TODO: Implement hardware-specific UART initialization. */

    return cellularStatus;
}

static CellularError_t prvRC7620GpioInit(void)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    /* TODO: Implement hardware-specific GPIO initialization. */

    return cellularStatus;
}

static void prvRC7620ReleaseResourcesOnError(CellularContext_t *pContext)
{
    if (pContext != NULL)
    {
        /* Cleanup cellular common resources. */
        (void)Cellular_CommonCleanup(pContext);
    }
}

static CellularError_t prvRC7620ConfigureModule(CellularContext_t *pContext)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularAtReq_t atReqGetResult = {0};
    uint8_t attempts = 0;

    if (pContext == NULL)
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Disable echo. */
        atReqGetResult.pAtCmd = "ATE0";
        atReqGetResult.atCmdType = CELLULAR_AT_NO_RESULT;
        cellularStatus = Cellular_AtcmdRequestWithCallback(pContext, atReqGetResult);

        /* Configure PDN settings if needed. */
        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Apply the PDN configurations from cellular_config.h. */
            CellularPdnConfig_t pdnConfig;
            pdnConfig.apnName = CELLULAR_APN_NAME;
            pdnConfig.pdnContextType = CELLULAR_PDN_CONTEXT_IPV4;
            pdnConfig.username = CELLULAR_PDN_USERNAME;
            pdnConfig.password = CELLULAR_PDN_PASSWORD;

            cellularStatus = prvRC7620SetPDN(pContext, CELLULAR_PDN_CONTEXT_ID, &pdnConfig);
        }

        /* Check module registration to the network. */
        if (cellularStatus == CELLULAR_SUCCESS)
        {
            CellularNetworkRegistrationStatus_t networkStatus;

            /* Wait for network registration. */
            for (attempts = 0; attempts < RC7620_NETWORK_REGSTATUS_ATTEMPTS; attempts++)
            {
                cellularStatus = prvRC7620GetNetworkRegistrationStatus(pContext, &networkStatus);

                if ((cellularStatus == CELLULAR_SUCCESS) &&
                    ((networkStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME) ||
                     (networkStatus == CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING)))
                {
                    break;
                }

                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
    }

    return cellularStatus;
}

static CellularError_t prvRC7620HandleUrcState(CellularContext_t *pContext)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;

    if (pContext == NULL)
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Enable URCs for network registration. */
        cellularStatus = Cellular_CommonAtcmdRequestWithCallback(pContext, "AT+CREG=2", CELLULAR_AT_NO_RESULT, NULL, NULL);

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Enable URCs for packet service registration. */
            cellularStatus = Cellular_CommonAtcmdRequestWithCallback(pContext, "AT+CGREG=2", CELLULAR_AT_NO_RESULT, NULL, NULL);
        }
    }

    return cellularStatus;
}

static CellularError_t prvRC7620GetSignalInfo(CellularContext_t *pContext,
                                              CellularSignalInfo_t *pSignalInfo)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularAtReq_t atReqQuerySignalInfo = {0};
    CellularAtReq_t atReqQuerySignalRsrp = {0};
    char signalInfo[CELLULAR_AT_MULTI_DATA_WO_PREFIX_STRING_MAX_SIZE] = {'\0'};
    char signalRsrp[CELLULAR_AT_MULTI_DATA_WO_PREFIX_STRING_MAX_SIZE] = {'\0'};
    char *pToken = NULL;

    if ((pContext == NULL) || (pSignalInfo == NULL))
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Query signal quality using AT+CSQ. */
        atReqQuerySignalInfo.pAtCmd = "AT+CSQ";
        atReqQuerySignalInfo.respCallback = NULL;
        atReqQuerySignalInfo.pData = signalInfo;
        atReqQuerySignalInfo.dataLen = CELLULAR_AT_MULTI_DATA_WO_PREFIX_STRING_MAX_SIZE;
        atReqQuerySignalInfo.atCmdType = CELLULAR_AT_WITH_PREFIX;
        atReqQuerySignalInfo.pAtRspPrefix = "+CSQ:";

        cellularStatus = Cellular_AtcmdRequestWithCallback(pContext, atReqQuerySignalInfo);

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Parse the signal quality response. */
            pToken = strtok(signalInfo, ",");
            if (pToken != NULL)
            {
                pSignalInfo->rssi = atoi(pToken);

                /* Convert to dBm. */
                if (pSignalInfo->rssi == 99)
                {
                    pSignalInfo->rssi = -99; /* Unknown */
                }
                else
                {
                    pSignalInfo->rssi = -113 + (2 * pSignalInfo->rssi);
                }

                /* Get the BER. */
                pToken = strtok(NULL, ",");
                if (pToken != NULL)
                {
                    pSignalInfo->ber = atoi(pToken);
                }
                else
                {
                    pSignalInfo->ber = 99; /* Unknown */
                }
            }
            else
            {
                cellularStatus = CELLULAR_INTERNAL_FAILURE;
            }
        }

        /* Query LTE signal parameters. */
        if (cellularStatus == CELLULAR_SUCCESS)
        {
            atReqQuerySignalRsrp.pAtCmd = "AT+CESQ";
            atReqQuerySignalRsrp.respCallback = NULL;
            atReqQuerySignalRsrp.pData = signalRsrp;
            atReqQuerySignalRsrp.dataLen = CELLULAR_AT_MULTI_DATA_WO_PREFIX_STRING_MAX_SIZE;
            atReqQuerySignalRsrp.atCmdType = CELLULAR_AT_WITH_PREFIX;
            atReqQuerySignalRsrp.pAtRspPrefix = "+CESQ:";

            cellularStatus = Cellular_AtcmdRequestWithCallback(pContext, atReqQuerySignalRsrp);

            if (cellularStatus == CELLULAR_SUCCESS)
            {
                /* Parse the LTE signal parameters response. */
                /* Format: +CESQ: <rxlev>,<ber>,<rscp>,<ecno>,<rsrq>,<rsrp> */
                char *pParams[6] = {0};
                uint8_t paramCount = 0;

                pToken = strtok(signalRsrp, ",");
                while ((pToken != NULL) && (paramCount < 6))
                {
                    pParams[paramCount++] = pToken;
                    pToken = strtok(NULL, ",");
                }

                if (paramCount == 6)
                {
                    /* Parse RSRQ - Value range is 0-34, where 0 = -20dB, 1 = -19.5dB, ..., 34 = -3dB */
                    int rsrq = atoi(pParams[4]);
                    if (rsrq != 255) /* 255 is not available */
                    {
                        pSignalInfo->rsrq = -20.0f + (rsrq * 0.5f);
                    }

                    /* Parse RSRP - Value range is 0-97, where 0 = -140dBm, 1 = -139dBm, ..., 97 = -43dBm */
                    int rsrp = atoi(pParams[5]);
                    if (rsrp != 255) /* 255 is not available */
                    {
                        pSignalInfo->rsrp = -140 + rsrp;
                    }
                }
            }
        }
    }

    return cellularStatus;
}

static CellularError_t prvRC7620GetNetworkRegistrationStatus(CellularContext_t *pContext,
                                                             CellularNetworkRegistrationStatus_t *pNetworkStatus)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    CellularAtReq_t atReqQueryRegStatus = {0};
    char regStatus[CELLULAR_AT_MULTI_DATA_WO_PREFIX_STRING_MAX_SIZE] = {'\0'};
    char *pToken = NULL;
    int32_t regStatusValue = 0;

    if ((pContext == NULL) || (pNetworkStatus == NULL))
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Query network registration status using AT+CREG? */
        atReqQueryRegStatus.pAtCmd = "AT+CREG?";
        atReqQueryRegStatus.respCallback = NULL;
        atReqQueryRegStatus.pData = regStatus;
        atReqQueryRegStatus.dataLen = CELLULAR_AT_MULTI_DATA_WO_PREFIX_STRING_MAX_SIZE;
        atReqQueryRegStatus.atCmdType = CELLULAR_AT_WITH_PREFIX;
        atReqQueryRegStatus.pAtRspPrefix = "+CREG:";

        cellularStatus = Cellular_AtcmdRequestWithCallback(pContext, atReqQueryRegStatus);

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Parse the registration status response. */
            /* Format: +CREG: <n>,<stat>[,<lac>,<ci>,<AcT>] */
            /* We need the <stat> value. */
            pToken = strtok(regStatus, ",");
            if (pToken != NULL)
            {
                /* Skip <n>. */
                pToken = strtok(NULL, ",");
                if (pToken != NULL)
                {
                    regStatusValue = atoi(pToken);

                    /* Map the registration status value to the corresponding enum value. */
                    switch (regStatusValue)
                    {
                    case 0:
                        *pNetworkStatus = CELLULAR_NETWORK_REGISTRATION_STATUS_NOT_REGISTERED;
                        break;
                    case 1:
                        *pNetworkStatus = CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_HOME;
                        break;
                    case 2:
                        *pNetworkStatus = CELLULAR_NETWORK_REGISTRATION_STATUS_SEARCHING;
                        break;
                    case 3:
                        *pNetworkStatus = CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTRATION_DENIED;
                        break;
                    case 4:
                        *pNetworkStatus = CELLULAR_NETWORK_REGISTRATION_STATUS_UNKNOWN;
                        break;
                    case 5:
                        *pNetworkStatus = CELLULAR_NETWORK_REGISTRATION_STATUS_REGISTERED_ROAMING;
                        break;
                    default:
                        *pNetworkStatus = CELLULAR_NETWORK_REGISTRATION_STATUS_UNKNOWN;
                        break;
                    }
                }
                else
                {
                    cellularStatus = CELLULAR_INTERNAL_FAILURE;
                }
            }
            else
            {
                cellularStatus = CELLULAR_INTERNAL_FAILURE;
            }
        }
    }

    return cellularStatus;
}

static CellularError_t prvRC7620SetPDN(CellularContext_t *pContext,
                                       uint8_t contextId,
                                       const CellularPdnConfig_t *pPdnConfig)
{
    CellularError_t cellularStatus = CELLULAR_SUCCESS;
    char cmdBuf[CELLULAR_AT_COMMAND_MAX_SIZE] = {'\0'};

    if ((pContext == NULL) || (pPdnConfig == NULL))
    {
        cellularStatus = CELLULAR_BAD_PARAMETER;
    }
    else
    {
        /* Build the AT command to set the PDN configuration. */
        if (pPdnConfig->pdnContextType == CELLULAR_PDN_CONTEXT_IPV4)
        {
            (void)snprintf(cmdBuf, CELLULAR_AT_COMMAND_MAX_SIZE, "AT+CGDCONT=%d,\"IP\",\"%s\"",
                           contextId, pPdnConfig->apnName);
        }
        else if (pPdnConfig->pdnContextType == CELLULAR_PDN_CONTEXT_IPV6)
        {
            (void)snprintf(cmdBuf, CELLULAR_AT_COMMAND_MAX_SIZE, "AT+CGDCONT=%d,\"IPV6\",\"%s\"",
                           contextId, pPdnConfig->apnName);
        }
        else if (pPdnConfig->pdnContextType == CELLULAR_PDN_CONTEXT_IPV4V6)
        {
            (void)snprintf(cmdBuf, CELLULAR_AT_COMMAND_MAX_SIZE, "AT+CGDCONT=%d,\"IPV4V6\",\"%s\"",
                           contextId, pPdnConfig->apnName);
        }
        else
        {
            cellularStatus = CELLULAR_BAD_PARAMETER;
        }

        if (cellularStatus == CELLULAR_SUCCESS)
        {
            /* Send the AT command. */
            cellularStatus = Cellular_CommonAtcmdRequestWithCallback(pContext, cmdBuf, CELLULAR_AT_NO_RESULT, NULL, NULL);
        }

        /* Set the authentication if needed. */
        if ((cellularStatus == CELLULAR_SUCCESS) &&
            ((pPdnConfig->username != NULL) && (pPdnConfig->password != NULL)))
        {
            /* Build the AT command to set the authentication. */
            (void)snprintf(cmdBuf, CELLULAR_AT_COMMAND_MAX_SIZE, "AT+CGAUTH=%d,1,\"%s\",\"%s\"",
                           contextId, pPdnConfig->username, pPdnConfig->password);

            /* Send the AT command. */
            cellularStatus = Cellular_CommonAtcmdRequestWithCallback(pContext, cmdBuf, CELLULAR_AT_NO_RESULT, NULL, NULL);
        }
    }

    return cellularStatus;
}
