/**
 * This software is copyrighted by Bosch Connected Devices and Solutions GmbH, 2016.
 * The use of this software is subject to the XDK SDK EULA
 */
//lint -esym(956,*) /* Suppressing "Non const, non volatile static or external variable" lint warning*/

/* module includes ********************************************************** */

/* system header files */
#include <stdio.h>
/* additional interface header files */
#include "simplelink.h"
#include "BCDS_Basics.h"
#include "BCDS_Assert.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "BLE_stateHandler_ih.h"
#include "BLE_serialDriver_ih.h"
#include "BCDS_WlanConnect.h"
#include "BCDS_NetworkConfig.h"
#include <Serval_Types.h>
#include <Serval_Basics.h>
#include <Serval_Ip.h>

#include "URU_usbResetUtility_ih.h"

/* own header files */
#include "SendDataOverUdp.h"

/* constant definitions ***************************************************** */

#define BUFFER_SIZE        UINT8_C(60)

/* local variables ********************************************************** */

static Accelerometer_XyzData_T accelData;
static Gyroscope_XyzData_T gyroData;
static uint32_t milliLuxData;
static uint8_t noiseDbSpl;
static Magnetometer_XyzData_T magData;
static Environmental_Data_T environmentData;

/**
 * This buffer holds the data to be sent to server via UDP
 * */
static uint8_t bsdBuffer_mau[BUFFER_SIZE] = { (uint8_t) ZERO };
/**
 * Timer handle for connecting to wifi and obtaining the IP address
 */
xTimerHandle wifiConnectTimerHandle_gdt = NULL;
/**
 * Timer handle for periodically sending data over wifi
 */
xTimerHandle wifiSendTimerHandle = NULL;

/* global variables ********************************************************* */

/* inline functions ********************************************************* */



/* local functions ********************************************************** */


/**
 *  @brief
 *      Function to initialize the wifi network send application. Create timer task
 *      to start WiFi Connect and get IP function after one second. After that another timer
 *      to send data periodically.
 */
void init(void)
{
	printf("Init 0\r\n");
    uint32_t Ticks = PERIOD*ONESECONDDELAY;

    if (Ticks != UINT32_MAX) /* Validated for portMAX_DELAY to assist the task to wait Infinitely (without timing out) */
    {
        Ticks /= portTICK_RATE_MS;
    }
    if (UINT32_C(0) == Ticks) /* ticks cannot be 0 in FreeRTOS timer. So ticks is assigned to 1 */
    {
        Ticks = UINT32_C(1);
    }

    printf("Init 1\r\n");

    /* Initialize sensors */

    accelerometerSensorInit();
    gyroSensorInit();
    lightsensorInit();
    noiseSensorInit();
    magnetometerSensorInit();
    environmentSensorInit();
    orientationSensorInit();


    /* create timer task*/
    wifiConnectTimerHandle_gdt = xTimerCreate((char * const ) "wifiConnect", Ticks, TIMER_AUTORELOAD_OFF, NULL, wifiConnectGetIP);
    wifiSendTimerHandle = xTimerCreate((char * const ) "wifiSend", Ticks, TIMER_AUTORELOAD_ON, NULL, wifiSend);

    if ((wifiConnectTimerHandle_gdt != NULL) && (wifiSendTimerHandle != NULL))
    {
        /*start the wifi connect timer*/
        if ( xTimerStart(wifiConnectTimerHandle_gdt, TIMERBLOCKTIME) != pdTRUE)
        {
            assert(false);
        }
    }
    else
    {
        /* Assertion Reason: "Failed to create timer task during initialization"   */
        assert(false);
    }
}

/**
 * @brief This is a template function where the user can write his custom application.
 *
 */
void appInitSystem(xTimerHandle xTimer)
{
    BCDS_UNUSED(xTimer);

    printf("Hello world\r\n");
    /*Call the WNS module init API */
    init();
}

static void sampleAndStoreSensorData(void) {

	readAccelerometerData(&accelData);
	//accelData.xAxisData
	//accelData.yAxisData
	//accelData.zAxisData

	readGyroData(&gyroData);
	//gyroData.xAxisData
	//gyroData.yAxisData
	//gyroData.zAxisData

	readLightSensor(&milliLuxData);
	//milliLuxData

	readNoiseSensor(&noiseDbSpl);
	//noiseDbSpl

	readMagnetometerSensor(&magData);
	//magData.xAxisData
	//magData.yAxisData
	//magData.zAxisData
	//magData.resistance

	readEnvironmentSensor(&environmentData);
	//environmentData.pressure
	//environmentData.temperature
	//environmentData.humidity
}


/**
 * @brief Opening a UDP client side socket and sending data on a server port
 *
 * This function opens a UDP socket and tries to connect to a Server SERVER_IP
 * waiting on port SERVER_PORT.
 * Then the function will send periodic UDP packets to the server.
 * 
 * @param[in] port
 *					destination port number
 *
 * @return         returnTypes_t:
 *                                  SOCKET_ERROR: when socket has not opened properly
 *                                  SEND_ERROR: when 0 transmitted bytes or send error
 *                                  STATUS_OK: when UDP sending was successful
 */

static returnTypes_t bsdUdpClient(uint16_t port)
{
    static uint16_t counter = UINT16_C(0);
    SlSockAddrIn_t Addr;
    uint16_t AddrSize = (uint16_t) ZERO;
    int16_t SockID = (int16_t) ZERO;
    int16_t Status = (int16_t) ZERO;

    /* copies the dummy data to send array , the first array element is the running counter to track the number of packets send so far*/
    uint8_t i = 1;
    bsdBuffer_mau[i++] = (accelData.xAxisData & 0xFF);
    bsdBuffer_mau[i++] = ((accelData.xAxisData >>  8) & 0xFF);
    bsdBuffer_mau[i++] = ((accelData.xAxisData >> 16) & 0xFF);
    bsdBuffer_mau[i++] = ((accelData.xAxisData >> 24) & 0xFF);
    bsdBuffer_mau[i++] = (accelData.yAxisData & 0xFF);
    bsdBuffer_mau[i++] = ((accelData.yAxisData >>  8) & 0xFF);
    bsdBuffer_mau[i++] = ((accelData.yAxisData >> 16) & 0xFF);
    bsdBuffer_mau[i++] = ((accelData.yAxisData >> 24) & 0xFF);
    bsdBuffer_mau[i++] = (accelData.zAxisData & 0xFF);
    bsdBuffer_mau[i++] = ((accelData.zAxisData >>  8) & 0xFF);
    bsdBuffer_mau[i++] = ((accelData.zAxisData >> 16) & 0xFF);
    bsdBuffer_mau[i++] = ((accelData.zAxisData >> 24) & 0xFF);

    bsdBuffer_mau[i++] = (gyroData.xAxisData & 0xFF);
    bsdBuffer_mau[i++] = ((gyroData.xAxisData >>  8) & 0xFF);
    bsdBuffer_mau[i++] = ((gyroData.xAxisData >> 16) & 0xFF);
    bsdBuffer_mau[i++] = ((gyroData.xAxisData >> 24) & 0xFF);
    bsdBuffer_mau[i++] = (gyroData.yAxisData & 0xFF);
    bsdBuffer_mau[i++] = ((gyroData.yAxisData >>  8) & 0xFF);
    bsdBuffer_mau[i++] = ((gyroData.yAxisData >> 16) & 0xFF);
    bsdBuffer_mau[i++] = ((gyroData.yAxisData >> 24) & 0xFF);
    bsdBuffer_mau[i++] = (gyroData.zAxisData & 0xFF);
    bsdBuffer_mau[i++] = ((gyroData.zAxisData >>  8) & 0xFF);
    bsdBuffer_mau[i++] = ((gyroData.zAxisData >> 16) & 0xFF);
    bsdBuffer_mau[i++] = ((gyroData.zAxisData >> 24) & 0xFF);

    bsdBuffer_mau[i++] = (milliLuxData & 0xFF);
    bsdBuffer_mau[i++] = ((milliLuxData >>  8) & 0xFF);
    bsdBuffer_mau[i++] = ((milliLuxData >> 16) & 0xFF);
    bsdBuffer_mau[i++] = ((milliLuxData >> 24) & 0xFF);

    bsdBuffer_mau[i++] = noiseDbSpl;

    bsdBuffer_mau[i++] = (magData.xAxisData & 0xFF);
    bsdBuffer_mau[i++] = ((magData.xAxisData >>  8) & 0xFF);
    bsdBuffer_mau[i++] = ((magData.xAxisData >> 16) & 0xFF);
    bsdBuffer_mau[i++] = ((magData.xAxisData >> 24) & 0xFF);
    bsdBuffer_mau[i++] = (magData.yAxisData & 0xFF);
    bsdBuffer_mau[i++] = ((magData.yAxisData >>  8) & 0xFF);
    bsdBuffer_mau[i++] = ((magData.yAxisData >> 16) & 0xFF);
    bsdBuffer_mau[i++] = ((magData.yAxisData >> 24) & 0xFF);
    bsdBuffer_mau[i++] = (magData.zAxisData & 0xFF);
    bsdBuffer_mau[i++] = ((magData.zAxisData >>  8) & 0xFF);
    bsdBuffer_mau[i++] = ((magData.zAxisData >> 16) & 0xFF);
    bsdBuffer_mau[i++] = ((magData.zAxisData >> 24) & 0xFF);
    bsdBuffer_mau[i++] = magData.resistance & 0xFF;
    bsdBuffer_mau[i++] = (magData.resistance >> 8) & 0xFF;

    bsdBuffer_mau[i++] = (environmentData.pressure & 0xFF);
    bsdBuffer_mau[i++] = ((environmentData.pressure >>  8) & 0xFF);
    bsdBuffer_mau[i++] = ((environmentData.pressure >> 16) & 0xFF);
    bsdBuffer_mau[i++] = ((environmentData.pressure >> 24) & 0xFF);
    bsdBuffer_mau[i++] = (environmentData.temperature & 0xFF);
    bsdBuffer_mau[i++] = ((environmentData.temperature >>  8) & 0xFF);
    bsdBuffer_mau[i++] = ((environmentData.temperature >> 16) & 0xFF);
    bsdBuffer_mau[i++] = ((environmentData.temperature >> 24) & 0xFF);
    bsdBuffer_mau[i++] = (environmentData.humidity & 0xFF);
    bsdBuffer_mau[i++] = ((environmentData.humidity >>  8) & 0xFF);
    bsdBuffer_mau[i++] = ((environmentData.humidity >> 16) & 0xFF);
    bsdBuffer_mau[i++] = ((environmentData.humidity >> 24) & 0xFF);

    bsdBuffer_mau[0] = i;

    Addr.sin_family = SL_AF_INET;
    Addr.sin_port = sl_Htons((uint16_t) port);
    Addr.sin_addr.s_addr = sl_Htonl(SERVER_IP);
    AddrSize = sizeof(SlSockAddrIn_t);

    SockID = sl_Socket(SL_AF_INET, SL_SOCK_DGRAM, (uint32_t) ZERO); /**<The return value is a positive number if successful; other wise negative*/
    if (SockID < (int16_t) ZERO)
    {
        /* error case*/
        return (SOCKET_ERROR);
    }

    Status = sl_SendTo(SockID, bsdBuffer_mau, BUFFER_SIZE * sizeof(uint16_t), (uint32_t) ZERO, (SlSockAddr_t *) &Addr, AddrSize);/**<The return value is a number of characters sent;negative if not successful*/
    /*Check if 0 transmitted bytes sent or error condition*/
    if (Status <= (int16_t) ZERO)
    {
        Status = sl_Close(SockID);
        if (Status < 0)
        {
            return (SEND_ERROR);
        }
        return (SEND_ERROR);
    }
    Status = sl_Close(SockID);
    if (Status < 0)
    {
        return (SEND_ERROR);
    }
    counter++;
    return (STATUS_OK);
}
/**
 *  @brief
 *      Function to periodically send data over WiFi as UDP packets. This is run as an Auto-reloading timer.
 *
 *  @param [in ] xTimer - necessary parameter for timer prototype
 */
static void wifiSend(xTimerHandle xTimer)
{
    BCDS_UNUSED(xTimer);

    sampleAndStoreSensorData();

    if (STATUS_OK != bsdUdpClient(SERVER_PORT))
    {
        /* assertion Reason:  "Failed to  send udp packet" */
        assert(false);
    }
}

/**
 *  @brief
 *      Function to connect to wifi network and obtain IP address
 *
 *  @param [in ] xTimer
 */
static void wifiConnectGetIP(xTimerHandle xTimer)
{
    BCDS_UNUSED(xTimer);

    NetworkConfig_IpSettings_T myIpSettings;
    memset(&myIpSettings, (uint32_t) 0, sizeof(myIpSettings));
    char ipAddress[PAL_IP_ADDRESS_SIZE] = { 0 };
    Ip_Address_T* IpaddressHex = Ip_getMyIpAddr();
    WlanConnect_SSID_T connectSSID;
    WlanConnect_PassPhrase_T connectPassPhrase;
    Retcode_T ReturnValue = (Retcode_T)RETCODE_FAILURE;
    int32_t Result = INT32_C(-1);

    if (RETCODE_OK != WlanConnect_Init())
    {
        printf("Error occurred initializing WLAN \r\n ");
        return;
    }

    printf("Connecting to %s \r\n ", WLAN_CONNECT_WPA_SSID);

    connectSSID = (WlanConnect_SSID_T) WLAN_CONNECT_WPA_SSID;
    connectPassPhrase = (WlanConnect_PassPhrase_T) WLAN_CONNECT_WPA_PASS;
    ReturnValue = NetworkConfig_SetIpDhcp(NULL);
    if (ReturnValue)
    {
        printf("Error in setting IP to DHCP\n\r");
        return;
    }

    if (RETCODE_OK == WlanConnect_WPA(connectSSID, connectPassPhrase, NULL))
    {
        ReturnValue = NetworkConfig_GetIpSettings(&myIpSettings);
        if (RETCODE_OK == ReturnValue)
        {
            *IpaddressHex = Basics_htonl(myIpSettings.ipV4);
            Result = Ip_convertAddrToString(IpaddressHex, ipAddress);
            if (Result < 0)
            {
                printf("Couldn't convert the IP address to string format \r\n ");
                return;
            }
            printf("Connected to WPA network successfully. \r\n ");
            printf(" Ip address of the device: %s \r\n ", ipAddress);
        }
        else
        {
            printf("Error in getting IP settings\n\r");
            return;
        }
    }
    else
    {
        printf("Error occurred connecting %s \r\n ", WLAN_CONNECT_WPA_SSID);
        return;
    }

    /* After connection start the wifi sending timer*/
    if (xTimerStart(wifiSendTimerHandle, TIMERBLOCKTIME) != pdTRUE)
    {
        assert(false);
    }
}

/* global functions ********************************************************* */

/** ************************************************************************* */
