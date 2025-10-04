#include <stdio.h>
#include <string.h>
#include <stdint.h>

static int hexCharToInt(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 10 + (c - 'A');
    if (c >= 'a' && c <= 'f')
        return 10 + (c - 'a');
    return 0;
}

static uint8_t hexToByte(const char *hex)
{
    return (hexCharToInt(hex[0]) << 4) | hexCharToInt(hex[1]);
}

static void byteToHex(uint8_t val, char *out)
{
    const char hex[] = "0123456789ABCDEF";
    out[0] = hex[(val >> 4) & 0xF];
    out[1] = hex[val & 0xF];
    out[2] = '\0';
}

static int encodeSemiOctetNumber(const char *number, char *out)
{
    int len = strlen(number), outIdx = 0;
    for (int i = 0; i < len; i += 2)
    {
        char c1 = number[i];
        char c2 = (i + 1 < len) ? number[i + 1] : 'F';
        out[outIdx++] = c2;
        out[outIdx++] = c1;
    }
    out[outIdx] = '\0';
    return outIdx;
}

static void decodeSemiOctetNumber(const char *hex, int length, char *out, int outSize)
{
    int pos = 0;
    for (int i = 0; i < length && pos < outSize - 1; i += 2)
    {
        if (hex[i + 1] != 'F' && pos < outSize - 1)
            out[pos++] = hex[i + 1];
        if (hex[i] != 'F' && pos < outSize - 1)
            out[pos++] = hex[i];
    }
    out[pos] = '\0';
}

static int pack7bit(const char *input, uint8_t *output, int maxlen)
{
    int len = strlen(input), outIndex = 0, carryBits = 0, carry = 0;
    for (int i = 0; i < len; i++)
    {
        uint8_t septet = input[i] & 0x7F;
        uint8_t val = (septet << carryBits) | carry;
        output[outIndex++] = val;
        carry = septet >> (8 - carryBits);
        carryBits++;
        if (carryBits == 7)
        {
            output[outIndex++] = carry;
            carry = 0;
            carryBits = 0;
        }
        if (outIndex >= maxlen)
            break;
    }
    if (carryBits > 0 && outIndex < maxlen)
        output[outIndex++] = carry;
    return outIndex;
}

static void decode7bit(const uint8_t *data, int septetCount, char *out, int outSize)
{
    int outPos = 0;
    for (int i = 0; i < septetCount && outPos < outSize - 1; i++)
    {
        int bytePos = (i * 7) / 8;
        int bitInByte = (i * 7) % 8;
        uint16_t val = data[bytePos] >> bitInByte;
        if (bitInByte > 1)
            val |= data[bytePos + 1] << (8 - bitInByte);
        out[outPos++] = (char)(val & 0x7F);
    }
    out[outPos] = '\0';
}
static int encodeUCS2(const char *ascii, uint8_t *out, int maxlen)
{
    int len = strlen(ascii), outIndex = 0;
    for (int i = 0; i < len && outIndex + 1 < maxlen; i++)
    {
        out[outIndex++] = 0x00;
        out[outIndex++] = (uint8_t)ascii[i];
    }
    return outIndex;
}
static void decodeUCS2(const uint8_t *data, int byteCount, char *out, int outSize)
{
    int outPos = 0;
    for (int i = 0; i + 1 < byteCount && outPos < outSize - 1; i += 2)
    {
        uint16_t ch = (data[i] << 8) | data[i + 1];
        out[outPos++] = (ch < 128) ? (char)ch : '?';
    }
    out[outPos] = '\0';
}

int encodePDU(const char *smscNumber,
              const char *destNumber,
              const char *message,
              int useUCS2,
              char *outPdu,
              int outPduSize)
{
    char *p = outPdu, tmp[64], semi[64];
    int totalLen = 0;
    strcpy(p, "00");
    p += 2;
    totalLen += 2; // SMSC
    strcpy(p, "11");
    p += 2;
    totalLen += 2;
    strcpy(p, "00");
    p += 2;
    totalLen += 2;
    sprintf(tmp, "%02X", (int)strlen(destNumber));
    strcpy(p, tmp);
    p += 2;
    totalLen += 2;
    strcpy(p, "91");
    p += 2;
    totalLen += 2;
    int semiLen = encodeSemiOctetNumber(destNumber, semi);
    strncpy(p, semi, semiLen);
    p += semiLen;
    totalLen += semiLen;
    strcpy(p, "00");
    p += 2;
    totalLen += 2;
    strcpy(p, useUCS2 ? "08" : "00");
    p += 2;
    totalLen += 2;
    uint8_t ud[280];
    int udLen;
    if (useUCS2)
    {
        udLen = encodeUCS2(message, ud, sizeof(ud));
        sprintf(tmp, "%02X", udLen);
    }
    else
    {
        udLen = pack7bit(message, ud, sizeof(ud));
        sprintf(tmp, "%02X", (int)strlen(message));
    }
    strcpy(p, tmp);
    p += 2;
    totalLen += 2;
    for (int i = 0; i < udLen; i++)
    {
        char hex[3];
        byteToHex(ud[i], hex);
        strcpy(p, hex);
        p += 2;
        totalLen += 2;
    }
    *p = '\0';
    return totalLen;
}

void decodePDU(const char *pdu,
               char *sender, int senderSize,
               char *timestamp, int tsSize,
               char *message, int msgSize)
{
    const char *p = pdu;
    int smscLen = hexToByte(p);
    p += (1 + smscLen) * 2;
    int firstOctet = hexToByte(p);
    p += 2;
    int oaLen = hexToByte(p);
    p += 2;
    int oaType = hexToByte(p);
    p += 2;
    int oaSemiLen = ((oaLen + 1) / 2) * 2;
    decodeSemiOctetNumber(p, oaSemiLen, sender, senderSize);
    p += oaSemiLen;
    int pid = hexToByte(p);
    p += 2;
    int dcs = hexToByte(p);
    p += 2;
    int tsPos = 0;
    for (int i = 0; i < 7 && tsPos + 2 < tsSize; i++)
    {
        timestamp[tsPos++] = p[1];
        timestamp[tsPos++] = p[0];
        p += 2;
    }
    timestamp[tsPos] = '\0';
    int udl = hexToByte(p);
    p += 2;
    int remainingHex = strlen(p), udBytes = remainingHex / 2;
    uint8_t buf[280];
    for (int i = 0; i < udBytes; i++)
        buf[i] = hexToByte(p + i * 2);
    if (dcs == 0x00)
        decode7bit(buf, udl, message, msgSize);
    else if (dcs == 0x08)
        decodeUCS2(buf, udBytes, message, msgSize);
    else
        snprintf(message, msgSize, "[DCS %02X]", dcs);
}