/**
 * @file modem.h
 * @brief High-level cellular modem interface
 * @ingroup modem_driver
 *
 * This module provides high-level functions for controlling the cellular modem,
 * including call management, SMS operations, and network information.
 */

#ifndef MODEM_H_
#define MODEM_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "errornum.h"
#include "stm32_config.h"
#include "at_commands.h"

/**
 * @brief Modem event types
 * @ingroup modem_driver
 */
typedef enum
{
    MODEM_EVENT_NONE = 0,      /**< No event detected */
    MODEM_EVENT_INCOMING_CALL, /**< Incoming call event */
    MODEM_EVENT_INCOMING_SMS   /**< Incoming SMS event */
} ModemEventType;

/**
 * @ingroup modem_driver
 * @brief Initialize the modem
 * @return 0 on success, error code otherwise
 */
uint8_t modem_init(void);

/**
 * @ingroup modem_driver
 * @brief Put modem into sleep mode
 * @return 0 on success, error code otherwise
 */
uint8_t modem_sleep(void);

/**
 * @ingroup modem_driver
 * @brief Enable airplane mode (disable radio)
 * @return 0 on success, error code otherwise
 */
uint8_t modem_airplane_mode_on(void);

/**
 * @ingroup modem_driver
 * @brief Disable airplane mode (enable radio)
 * @return 0 on success, error code otherwise
 */
uint8_t modem_airplane_mode_off(void);

/**
 * @ingroup modem_driver
 * @brief Send an SMS message
 * @param sms_address Destination phone number
 * @param sms_message Message text to send
 * @return 0 on success, error code otherwise
 */
uint8_t modem_send_sms(const char *sms_address, const char *sms_message);

/**
 * @ingroup modem_driver
 * @brief Dial a phone number
 * @param dial_string Phone number to dial
 * @return 0 on success, error code otherwise
 */
uint8_t modem_dial(const char *dial_string);

/**
 * @ingroup modem_driver
 * @brief Get cellular signal strength
 * @param rssi Pointer to receive RSSI value (Received Signal Strength Indicator)
 * @param ber Pointer to receive BER value (Bit Error Rate)
 * @return 0 on success, error code otherwise
 */
uint8_t modem_get_signal_strength(int16_t *rssi, uint8_t *ber);

/**
 * @ingroup modem_driver
 * @brief Get network time and date
 * @param date Pointer to RTC date structure
 * @param time Pointer to RTC time structure
 * @return 0 on success, error code otherwise
 */
uint8_t modem_get_clock(RTC_DateTypeDef *date, RTC_TimeTypeDef *time);

/**
 * @ingroup modem_driver
 * @brief Read an SMS message from storage
 * @param index SMS storage index
 * @param sender Buffer to receive sender phone number
 * @param sender_size Size of sender buffer
 * @param message Buffer to receive message text
 * @param message_size Size of message buffer
 * @param memtype Pointer to a char array containing 2 items for the memory type 
 * @return 0 on success, error code otherwise
 */
uint8_t modem_read_sms(uint8_t index, char *sender, size_t sender_size,
                       char *message, size_t message_size, char memtype[3]);
/**
 * @ingroup modem_driver
 * @brief Check for incoming modem events
 * @param caller_id Buffer to receive caller ID (if incoming call)
 * @param caller_id_size Size of caller_id buffer
 * @param sms_index Pointer to receive SMS index (if incoming SMS)
 * @param memtype Pointer to a char array containing 2 items for the memory type 
 * @return Event type detected
 */
ModemEventType modem_check_event(char *caller_id, size_t caller_id_size, uint8_t *sms_index, char memtype[3]);

/**
 * @ingroup modem_driver
 * @brief Hang up active call
 * @return 0 on success, error code otherwise
 */
uint8_t modem_hang_up(void);

/**
 * @ingroup modem_driver
 * @brief Answer incomming call
 * @param result_code Result code for answer command
 * @return 0 on success, error code otherwise
 */
uint8_t modem_answer_call(enum ATV0ResultCodes *result_code);

#endif /* MODEM_H_ */