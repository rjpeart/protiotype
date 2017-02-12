/**
 * This software is copyrighted by Bosch Connected Devices and Solutions GmbH, 2016.
 * The use of this software is subject to the XDK SDK EULA
 */

/* header definition ******************************************************** */
#ifndef XDK110_SENDDATAOVERUDP_H_
#define XDK110_SENDDATAOVERUDP_H_

#include "Sensors.h"

/* local interface declaration ********************************************** */

/* local type and macro definitions */
typedef enum returnTypes_e
{
    STATUS_NOT_OK,
    STATUS_OK,
    SOCKET_ERROR,
    SEND_ERROR
} returnTypes_t;

#define ONESECONDDELAY UINT32_C(1000) 	            /**< Macro to represent one second time unit*/
#define FIVESECONDDELAY UINT32_C(5000)              /**< Macro to represent five second time unit*/

#define TIMERBLOCKTIME UINT32_MAX             /**< Macro used to define block time of a timer*/
#define ZERO    UINT8_C(0)  			            /**< Macro to define value zero*/
#define TIMER_AUTORELOAD_ON             UINT32_C(1)             /**< Auto reload of timer is enabled*/
#define TIMER_AUTORELOAD_OFF            UINT32_C(0)             /**< Auto reload of timer is disabled*/

#warning Please provide WLAN related configurations, with valid SSID & WPA key and server ip address where packets are to be sent in the below macros.
/** Network configurations */
#define WLAN_CONNECT_WPA_SSID                "PiForDinner"         /**< Macros to define WPA/WPA2 network settings */
#define WLAN_CONNECT_WPA_PASS                "P@ssw0rd123"      /**< Macros to define WPA/WPA2 network settings */

/** IP addressed of server side socket.Should be in long format, E.g: 0xc0a80071 == 192.168.0.113 */
//#define SERVER_IP         UINT32_C(0xC0A80071)
#define SERVER_IP         UINT32_C(0xFFFFFFFF)
#define SERVER_PORT        UINT16_C(6666)           /**< Port number on which server will listen */

#define PERIOD             10


/* local function prototype declarations */
/**
 *  @brief
 *      Function to connect to wifi network and obtain IP address
 *
 *  @param [in ] xTimer
 */
static void wifiConnectGetIP(xTimerHandle xTimer);
/**
 *  @brief
 *      Function to periodically send data over WiFi as UDP packets. This is run by an Auto-reloading timer.
 *
 *  @param [in ] xTimer
 */
static void wifiSend(xTimerHandle xTimer);

/* local module global variable declarations */

/* local inline function definitions */

#endif /* XDK110_SENDDATAOVERUDP_H_ */

/** ************************************************************************* */
