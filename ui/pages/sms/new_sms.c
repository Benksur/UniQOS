#include "new_sms.h"

#define MAX_PHONE_NUMBER_LENGTH 10
#define MAX_SMS_LENGTH 140

typedef struct {
    Cursor cursor;
    char phone_number[MAX_PHONE_NUMBER_LENGTH + 1];
    char sms_content[MAX_SMS_LENGTH + 1];
} NewSMSState;

static Page* current_page = NULL;