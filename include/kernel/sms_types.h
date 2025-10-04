/**
 * @file sms_types.h
 * @brief SMS message data structures
 * @ingroup sms_types
 *
 * Data structures for SMS message handling including
 * message composition, sending, and receiving.
 */

#ifndef SMS_TYPES_H
#define SMS_TYPES_H

#include <stdint.h>

/** @ingroup sms_types
 *  @brief Maximum phone number length (international format) */
#define SMS_MAX_PHONE_LENGTH 15

/** @ingroup sms_types
 *  @brief Maximum SMS message length (standard GSM) */
#define SMS_MAX_MESSAGE_LENGTH 160

/**
 * @brief Structure for sending SMS messages
 * @ingroup sms_types
 *
 * Used by UI components and cellular task to send SMS.
 */
typedef struct
{
    char recipient[SMS_MAX_PHONE_LENGTH + 1]; /**< Phone number/recipient address */
    char body[SMS_MAX_MESSAGE_LENGTH + 1];    /**< SMS message body */
} SmsMessage;

/**
 * @brief Structure for received SMS messages
 * @ingroup sms_types
 *
 * Used by cellular task to pass incoming SMS to display task.
 */
typedef struct
{
    char sender[SMS_MAX_PHONE_LENGTH + 1]; /**< Sender's phone number */
    char body[SMS_MAX_MESSAGE_LENGTH + 1]; /**< SMS message body */
} ReceivedSms;

#endif // SMS_TYPES_H
