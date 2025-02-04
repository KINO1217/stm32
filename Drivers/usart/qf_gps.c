#include "qf_gps.h"
#include "qf_uart.h"
#include "stdlib.h"
#include "string.h"

static void QF_GPS_Callback(u8* data, u16 data_len);
static double glon = 0.0; // 经度
static double glat = 0.0; // 纬度

void QF_GPS_Init(void)
{
    QF_UART3_Init(9600);
    UART3_Callback = QF_GPS_Callback;
}

static void QF_GPS_Callback(u8* data, u16 data_len)
{
    char* ptr = NULL;

    if ((ptr = strstr((char*)data, "$GNRMC")) != NULL) {
        char* ptok = NULL;
        ptok = strtok(ptr, ",");
        ptok = strtok(NULL, ",");
        if (!strcmp(ptok, "A") || !strcmp(ptok, "V")) {
            glon = 0.0;
            glat = 0.0;
        } else { // 获取到信号
            ptok = strtok(NULL, ",");
            if (!strcmp(ptok, "A")) {
                ptok = strtok(NULL, ",");
                glat = atof(ptok);
                ptok = strtok(NULL, ",");
                ptok = strtok(NULL, ",");
                glon = atof(ptok);
            } else if (!strcmp(ptok, "V")) {
                glon = 0.0;
                glat = 0.0;
            }
        }
        ptok = NULL;
    }
    ptr = NULL;
    return;
}

void QF_GPS_GetData(double* lon, double* lat)
{
    *lon = glon;
    *lat = glat;
}
