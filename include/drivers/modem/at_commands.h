/**
 * @file at_commands.h
 * @brief AT command interface for cellular modem
 * @ingroup at_commands
 *
 * Provides AT command definitions, timeouts, and result codes for
 * communicating with the cellular modem. Based on RC76xx AT command reference.
 */

#ifndef AT_COMMANDS_H_
#define AT_COMMANDS_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "errornum.h"
#include "stm32_config.h"

/** @ingroup at_commands
 *  @brief Modem type selection (RC7620) */
#define MODEM_RC7620 // move this to some master config later

#ifdef MODEM_RC7620
#include "rc7620_api.h"
#endif

/** @ingroup at_commands
 *  @brief Standard command timeout values (see Table A-3 of RC76xx AT Command Reference) */
#define TIMEOUT_1S 1000     /**< 1 second timeout */
#define TIMEOUT_2S 2000     /**< 2 second timeout */
#define TIMEOUT_5S 5000     /**< 5 second timeout */
#define TIMEOUT_30S 30000   /**< 30 second timeout */
#define TIMEOUT_60S 60000   /**< 60 second timeout */
#define TIMEOUT_120S 120000 /**< 120 second timeout */

/** @ingroup at_commands
 *  @brief Maximum string size for phone numbers */
#define MAXNUMBERSTR 16

/**
 * @brief Function mode enumeration
 * @ingroup at_commands
 */
enum FunctionModes
{
    MODE_MIN = 0,      /**< Minimum functionality, low power draw */
    MODE_FULL = 1,     /**< Full functionality, high power draw */
    MODE_AIRPLANE = 4, /**< Airplane mode, low power draw */
    MODE_TEST = 5,     /**< Factory Test Mode */
    MODE_RST = 6,      /**< Reset UE */
    MODE_OFFLINE = 7,  /**< Offline mode */
};

/**
 * @brief Text mode enumeration
 * @ingroup at_commands
 */
enum TextModes
{
    TEXTMODE_PDU = 0,  /**< PDU mode for SMS */
    TEXTMODE_TEXT = 1, /**< Text mode for SMS */
};

/**
 * @brief ATV0 result codes enumeration
 * @ingroup at_commands
 */
enum ATV0ResultCodes
{
    ATV0_OK = 0,          /**< Command executed successfully */
    ATV0_CONNECT = 1,     /**< Connection established */
    ATV0_RING = 2,        /**< Incoming call */
    ATV0_NO_CARRIER = 3,  /**< No carrier detected */
    ATV0_ERROR = 4,       /**< Command error */
    ATV0_NO_DAILTONE = 6, /**< No dial tone */
    ATV0_BUSY = 7,        /**< Busy signal */
    ATV0_NO_ANSWER = 8,   /**< No answer */
};

/**
 * @brief Call direction enumeration
 * @ingroup at_commands
 */
enum CallDir
{
    DIR_MO = 0, /**< Mobile Originated (outgoing) */
    DIR_MT = 1, /**< Mobile Terminated (incoming) */
};

/**
 * @brief Call status enumeration
 * @ingroup at_commands
 */
enum CallStat
{
    STAT_ACTIVE = 0,    /**< Call is active */
    STAT_HELD = 1,      /**< Call is on hold */
    STAT_DIALING = 2,   /**< Dialing in progress */
    STAT_ALERTING = 3,  /**< Alerting (ringing) */
    STAT_INCOMMING = 4, /**< Incoming call */
    STAT_WAITING = 5,   /**< Call waiting */
};

/**
 * @brief Call mode enumeration
 * @ingroup at_commands
 */
enum CallMode
{
    MODE_VOICE = 0,   /**< Voice call mode */
    MODE_DATA = 1,    /**< Data call mode */
    MODE_FAX = 2,     /**< Fax call mode */
    MODE_VFBD_VM = 3, /**< Voice followed by data, voice mode */
    MODE_AVD_VM = 4,  /**< Alternating voice/data, voice mode */
    MODE_AVF_VM = 5,  /**< Alternating voice/fax, voice mode */
    MODE_VFBD_DM = 6, /**< Voice followed by data, data mode */
    MODE_AVD_DM = 7,  /**< Alternating voice/data, data mode */
    MODE_AVF_FM = 8,  /**< Alternating voice/fax, fax mode */
    MODE_UNKNOWN = 9, /**< Unknown mode */
};

/**
 * @brief Multiparty call enumeration
 * @ingroup at_commands
 */
enum CallMPTY
{
    MPTY_NO = 0,  /**< Not a multiparty call */
    MPTY_YES = 1, /**< Multiparty call */
};

/* -------------------------------- Call Status -------------------------------- */

typedef struct call_status_t
{
    int ccidx;
    enum CallDir dir;
    enum CallStat stat;
    enum CallMode mode;
    enum CallMPTY mpty;
    char number[MAXNUMBERSTR];
} call_status_t;

/* -------------------------------- Phone Book -------------------------------- */

// TODO: Determine max string sizes to set a fixed size
typedef struct phonebook_entry_t
{
    uint16_t index; // index in the source phone book
    char *number;   // string type phone number of format `type`
    uint16_t type;  // 145 when dialling includes "+", otherwise 129
    char *text;     // string type field of maximum length <tlength>; character set as specified by command select TE character set +CSCS
    // supported char sets +CSCS: ("IRA","GSM","UCS2")
} phonebook_entry_t;

typedef struct phonebook_t
{
    uint16_t index_min; // min index value
    uint16_t index_max; // max index value
    uint16_t nlength;   // maximum length of rc7620_phonebook_entry_t field `number`
    uint16_t tlength;   // maximum length of rc7620_phonebook_entry_t field `text`
    phonebook_entry_t *entries;
} phonebook_t;

/* ------------------------------- Sim Card info ------------------------------ */

#define RC7620_MCC_MAX // TODO: give these values
#define RC7620_MNC_MAX
#define RC7620_IMSI_MAX

typedef struct plminfo_t
{
    char mcc[RC7620_MCC_MAX + 1]; // Mobile Country Code
    char mnc[RC7620_MNC_MAX + 1]; // Mobile Network Code
} plminfo_t;

typedef struct siminfo_t
{
    char imsi[RC7620_IMSI_MAX + 1]; // International Mobile Subscriber Identity
    plminfo_t plmn;                 // Public Land Mobile Network Info
} siminfo_t;

/* -------------------------------- Modem info -------------------------------- */

#define RC7620_FW_VERSION_MAX_SIZE // TODO: give these values
#define RC7620_IMEI_MAX_SIZE
#define RC7620_MANUFACTURE_ID_MAX_SIZE
#define RC7620_MODEL_ID_MAX_SIZE

typedef struct modeminfo_t
{
    char firmwareVersion[RC7620_FW_VERSION_MAX_SIZE + 1];   // Firmware version number
    char imei[RC7620_IMEI_MAX_SIZE + 1];                    // International Mobile Equipment Identity
    char manufactureId[RC7620_MANUFACTURE_ID_MAX_SIZE + 1]; // Manufacture Identity
    char modelId[RC7620_MODEL_ID_MAX_SIZE + 1];             // Model Identity
} modeminfo_t;

/* -------------------------------- Functions -------------------------------- */

/* Governed By 3GPP TS 27.007*/
uint8_t at_set_function_mode(enum FunctionModes mode);

uint8_t at_set_auto_timezone(bool set_atz);
uint8_t at_get_clock(RTC_DateTypeDef *date, RTC_TimeTypeDef *time);

uint8_t at_get_signal_strength(int16_t *rssi, uint8_t *ber);

uint8_t at_select_char_set(char *chset);

uint8_t at_select_phonebook_memory(char storage[2], char *password);
uint8_t at_query_phonebook_memory(int *used, int *total);
uint8_t at_get_phonebook_info(phonebook_t *phonebook);
uint8_t at_get_phonebook_entry_range(uint16_t index1, uint16_t index2, phonebook_t *phonebook);
uint8_t at_write_phonebook_entry_index(uint16_t index, phonebook_entry_t *entry);
uint8_t at_write_phonebook_entry_first(phonebook_entry_t *entry);
uint8_t at_delete_phonebook_entry(uint16_t index);

uint8_t at_check_cpin(void);
uint8_t at_get_sim_info(siminfo_t *sim_info);
uint8_t at_get_modem_info(modeminfo_t *modem_info);
uint8_t at_check_net_reg(void);
uint8_t at_check_eps_net_reg(void);

uint8_t at_call_status(call_status_t **status, int max_items);

/* Governed By 3GPP TS 27.005*/
uint8_t at_set_message_format(enum TextModes mode);
uint8_t at_get_sms_textmode(int index, char *sms_buff, int bufflen);
uint8_t at_send_sms_textmode(const char *sms_address, const char *sms_message);

/* Governed By ITU-T Recommendation V.250*/
uint8_t at_call_dial(char *dial_string, enum ATV0ResultCodes *result_code);
uint8_t at_call_answer(enum ATV0ResultCodes *result_code);
uint8_t at_call_hook(void);
uint8_t at_set_echo(bool echo);

#endif