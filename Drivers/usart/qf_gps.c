#include "qf_gps.h"
#include "qf_uart.h"
#include "stdlib.h"
#include "string.h"

static void QF_GPS_Callback(u8* data, u16 data_len);
static double glon = 0.0; // ¾­¶È
static double glat = 0.0; // Î³¶È

void QF_GPS_Init(void)
{
    QF_UART1_Init(9600);
    UART1_Callback = QF_GPS_Callback;
}

static void QF_GPS_Callback(u8* data, u16 data_len)
{
    char* ptr = NULL;

    if ((ptr = strstr((char*)data, "$GNGGA")) != NULL) {
        if (strstr(ptr, "N") != NULL || strstr(ptr, "E") != NULL || strstr(ptr, "W") != NULL || strstr(ptr, "S") != NULL) {
            char* ptok = NULL;
            ptok = strtok(ptr, ",");
            ptok = strtok(NULL, ",");
            ptok = strtok(NULL, ",");
            glat = atof(ptok);
            ptok = strtok(NULL, ",");
            ptok = strtok(NULL, ",");
            glon = atof(ptok);
            ptok = NULL;
        } else {
            glon = 0.0;
            glat = 0.0;
        }
    }
    ptr = NULL;
    return;
}

void QF_GPS_GetData(double* lon, double* lat)
{
    *lon = glon;
    *lat = glat;
}
