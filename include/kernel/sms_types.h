#ifndef SMS_TYPES_H
#define SMS_TYPES_H

#include <stdint.h>

// Maximum lengths for SMS data
#define SMS_MAX_PHONE_LENGTH 15    // International phone number format (up to 15 digits)
#define SMS_MAX_MESSAGE_LENGTH 160 // Standard GSM SMS length

/**
 * @brief Structure for sending SMS messages
 * Used by UI components and cellular task to send SMS
 */
typedef struct
{
    char recipient[SMS_MAX_PHONE_LENGTH + 1]; // Phone number/recipient address
    char body[SMS_MAX_MESSAGE_LENGTH + 1];    // SMS message body
} SmsMessage;

/**
 * @brief Structure for received SMS messages
 * Used by cellular task to pass incoming SMS to display task
 */
typedef struct
{
    char sender[SMS_MAX_PHONE_LENGTH + 1]; // Sender's phone number
    char body[SMS_MAX_MESSAGE_LENGTH + 1]; // SMS message body
} ReceivedSms;

#endif // SMS_TYPES_H
