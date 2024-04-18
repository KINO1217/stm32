#ifndef __DRIVER_CONF_H__
#include "clock_conf.h"
#include "string.h"
#include "delay.h"
#include "sys.h"
#include "stdio.h"
#include "my_uart.h"
/**********************ALL******************/
//#define SWITCH_DRIVER_ENABLE
#define KEY_BOARD_DRIVER_ENABLE

/**********************ALL******************/
#ifdef SWITCH_DRIVER_ENABLE
    #include "switch.h"
#endif

#ifdef KEY_BOARD_DRIVER_ENABLE
    #include "key_board.h"
#endif
#endif
