/*
 * Sensors.c
 *
 *  Created on: Feb 4, 2017
 *      Author: Oliver
 */

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "BCDS_SensorErrorType.h"
#include "ADC_ih.h"
#include "PTD_portDriver_ph.h"
#include "PTD_portDriver_ih.h"
#include "Sensors.h"

static Accelerometer_HandlePtr_T accelSensor; /**< structure variable to store accelerator sensor handle*/

static Gyroscope_HandlePtr_T gyroSensor; /** structure variable to store gyro sensor handle */

static LightSensor_HandlePtr_T lightSensor; /** structure variable to store light sensor handle*/

static Orientation_HandlePtr_T orientationSensor; //**structure variable to store orientation sensor handle*/

static Magnetometer_HandlePtr_T magnetometerSensor; /** structure variable to store magnetometer sensor handle */

static Environmental_HandlePtr_T environmentSensor; /** structure variable to store environmnet sensor handle */


static uint8_t setLightSensorRegisters(LightSensor_HandlePtr_T sensorHandle);

static void printSensorError(Retcode_T error) {
	BCDS_SensorErrorType_T returnValue = SENSOR_ERROR;

	returnValue = BCDS_getSensorErrorCode(error);
	if (returnValue == SENSOR_ERROR) {
		printf("SENSOR_ERROR\n");
	} else if (returnValue == SENSOR_INVALID_PARAMETER) {
		printf("SENSOR_INVALID_PARAMETER\n");
	} else if (returnValue == SENSOR_INIT_NOT_DONE) {
		printf("SENSOR_INIT_NOT_DONE\n");
	} else if (returnValue == SENSOR_UNSUPPORTED_PARAMETER) {
		printf("SENSOR_UNSUPPORTED_PARAMETER\n");
	} else {
		printf("Unknown sensor error! Error code: %i\n", returnValue);
	}
}


void accelerometerSensorInit(void) {

	/* Return value for Accel Sensor */
	Retcode_T accelReturnValue = (Retcode_T) RETCODE_FAILURE;

	/*initialize accel*/
	accelSensor = xdkAccelerometers_BMA280_Handle;
	accelReturnValue = Accelerometer_init(accelSensor);
	if (RETCODE_OK == accelReturnValue) {
		printf("accelerometerSensorInit Success\n\r");
	} else {
		assert(0);
	}

}

void readAccelerometerData(Accelerometer_XyzData_T* accelData) {

	Retcode_T accError = (Retcode_T) RETCODE_FAILURE;

	accError = Accelerometer_readXyzLsbValue(accelSensor, accelData);

	if ( RETCODE_OK == accError) {
#ifdef DEBUG_LOGGING
		/*print Accel data of BMA280 on serialport */
		printf("Accel Data Raw Data :\n\rx =%d\n\ry= %d\n\rz =%d\n\r",
				(uint)accelData->xAxisData, (uint)accelData->yAxisData, (uint)accelData->zAxisData);
#endif
	} else {
		printf("Error reading accelerometer sensor values!\n");
		printSensorError(accError);
		assert(0);
	}

	accError = Accelerometer_readXyzGValue(accelSensor, accelData);

	if ( RETCODE_OK == accError) {
#ifdef DEBUG_LOGGING
		/*print Accel data of BMA280 on serialport */
		printf("Accel Data G data :\n\rx =%d\n\ry= %d\n\rz =%d\n\r",
				(int)accelData->xAxisData, (int)accelData->yAxisData, (int)accelData->zAxisData);
#endif
	} else {
		printf("Error reading accelerometer sensor G values!\n");
		printSensorError(accError);
		assert(0);
	}

}

void gyroSensorInit(void) {
	Retcode_T gyroReturnValue = (Retcode_T) RETCODE_FAILURE;

	/*initialize Gyro sensor*/
	gyroSensor = xdkGyroscope_BMG160_Handle;
	gyroReturnValue = Gyroscope_init(gyroSensor);

	if ( RETCODE_OK == gyroReturnValue) {
		printf("gyroSensorInit Success\n\r");

	} else {
		assert(0);
	}

}

void readGyroData(Gyroscope_XyzData_T* gyroData) {

	Retcode_T gyroError = (Retcode_T) RETCODE_FAILURE;

	gyroError = Gyroscope_readXyzValue(gyroSensor, gyroData);

	if ( RETCODE_OK == gyroError) {
#ifdef DEBUG_LOGGING
		/*print chip id and gyro data on serialport */
		printf("Gyro Data Raw :\tx =%d\ty= %d\tz =%d\n\r",
				(int)gyroData->xAxisData, (int)gyroData->yAxisData, (int)gyroData->zAxisData);
#endif
	} else {
		printf("Error reading gyro sensor values!\n");
		printSensorError(gyroError);
		assert(0);
	}

#ifdef READ_GYRO_VALUES_DEGREE
	gyroError = Gyroscope_readXyzValue(gyroSensor, gyroData);
	if (RETCODE_OK ==gyroError )
	{
		/*print chip id and gyro data on serialport */
		printf("Gyro Data Degree :\tx =%d\ty= %d\tz =%d\n\r",
				(int)gyroData->xAxisData, (int)gyroData->yAxisData, (int)gyroData->zAxisData);

	}
	else
	{
		printf("Error reading gyro sensor values!\n");
		printSensorError(gyroError);
		assert(0);
	}
#endif
}

void lightsensorInit(void) {
	Retcode_T lightSensorReturnValue = (Retcode_T) RETCODE_FAILURE;

	/*initialize lightsensor*/
	lightSensor = xdkLightSensor_MAX44009_Handle;

	lightSensorReturnValue = LightSensor_init(lightSensor);
	if (RETCODE_OK == lightSensorReturnValue) {
		lightSensorReturnValue = setLightSensorRegisters(lightSensor);
		if (RETCODE_OK != lightSensorReturnValue) {
			/**
			 * @todo using printf is not thread safe, we need to consider using iprintf.
			 */
			printf("lightsensor register access Failed\n\r");
		}
	} else {
		assert(0);
	}
}

static uint8_t setLightSensorRegisters(LightSensor_HandlePtr_T sensorHandle) {
	Retcode_T lightSensorReturnValue = (Retcode_T) RETCODE_FAILURE;

	/** Manual Mode should be enable in order to configure the continuous mode, brightness  and integration time*/
	lightSensorReturnValue = LightSensor_setManualMode(sensorHandle,
			LIGHTSENSOR_MAX44009_ENABLE_MODE);
	if (RETCODE_OK == lightSensorReturnValue) {
		lightSensorReturnValue = LightSensor_setIntegrationTime(sensorHandle,
				LIGHTSENSOR_100MS);
		if (RETCODE_OK != lightSensorReturnValue) {
			return (lightSensorReturnValue);
		}
		lightSensorReturnValue = LightSensor_setBrightness(sensorHandle,
				LIGHTSENSOR_HIGH_BRIGHTNESS);
		if (RETCODE_OK != lightSensorReturnValue) {
			return (lightSensorReturnValue);
		}
	} else {
		return (lightSensorReturnValue);
	}
	lightSensorReturnValue = LightSensor_setContinuousMode(sensorHandle,
			LIGHTSENSOR_MAX44009_ENABLE_MODE);

	return (lightSensorReturnValue);
}

void readLightSensor(uint32_t* milliLuxData) {
	Retcode_T lightSensorError = (Retcode_T) RETCODE_FAILURE;

	/* read sensor data in milli lux*/
	lightSensorError = LightSensor_readLuxData(lightSensor, milliLuxData);
	if (lightSensorError != RETCODE_OK) {
		printf("lightsensorReadInMilliLux Failed\n\r");
		printSensorError(lightSensorError);
		assert(0);
	} else {
#ifdef DEBUG_LOGGING
		printf("sensor data obtained in milli lux :%d \n\r", (unsigned int) milliLuxData);
#endif
	}

}

void noiseSensorInit() {

#ifdef NOISE_SENSOR_ACTIVATED
	/* configure the ADC input pin of AKU340 sensor  */
	PTD_pinModeSet(PTD_GET_PORT_PIN_MODE_DOUT(AKU340_ADC_OUT));

	/* initialize the Noise Sensor*/
	/* This is how it should be done, once the driver is ready */
	/* AKU_init(&noiseSensor); */


	// Sets the supply voltage for the acoustic sensor
	PTD_pinOutSet(PTD_PORT_AKU340_VDD,PTD_PIN_AKU340_VDD);

	ADC_node_t data;
	ADC_Init_TypeDef init = ADC_INIT_DEFAULT;

	data.adcFreq = 7000000;
	data.registerBaseAddress = ADC0;
	data.cmuClk = cmuClock_ADC0;
	data.init = init;

	ADC_init(&data);

#endif
}

void readNoiseSensor(uint32_t* noiseRaw) {

#ifndef NOISE_SENSOR_ACTIVATED
	(void) noiseDbSpl;
#endif

#ifdef NOISE_SENSOR_ACTIVATED

	/* noiseSensorError = AKU_getSensorData(&sensorData); */
	ADC_singleAcq_t result = ADC_SINGLE_ACQ_DEFAULT;

	// ADC single sample channel 4
	result.adcChannel = adcSingleInpCh4;

	result.initSingle.acqTime = adcAcqTime16;

	ADC_pollSingleData(&result);

	//printf("ADC raw value: %lu\n\r", result.data);

	*noiseRaw = ADC_scaleAdcValue(&result);

	//printf("ADC Acoustic Voltage: %lu mV\n\r", *noiseRaw);

#endif
}

void magnetometerSensorInit(void) {
	Retcode_T magReturnValue = (Retcode_T) RETCODE_FAILURE;
	Retcode_T modeReturnStatus = (Retcode_T) RETCODE_FAILURE;

	/*initialize Gyro sensor*/
	magnetometerSensor = xdkMagnetometer_BMM150_Handle;
	magReturnValue = Magnetometer_init(magnetometerSensor);

	if (RETCODE_OK == magReturnValue) {
		printf("magnetometerSensorInit Success\n\r");
		modeReturnStatus = Magnetometer_setPowerMode(magnetometerSensor,
				MAGNETOMETER_BMM150_POWERMODE_NORMAL);

		if (RETCODE_OK == modeReturnStatus) {
			printf("Magnetometer mode successfully set \n");
		} else {
			printf("Error setting magnetormeter mode! \n");
			assert(0);
		}
	} else {
		assert(0);
	}
}

void readMagnetometerSensor(Magnetometer_XyzData_T* magData) {
	Retcode_T magnetometerSensorError = (Retcode_T) RETCODE_FAILURE;

	/* read magnetometer sensor data*/

	magnetometerSensorError = Magnetometer_readXyzTeslaData(
			xdkMagnetometer_BMM150_Handle, magData);

	if (magnetometerSensorError != RETCODE_OK) {
		printf("magnetometerReadMicroTeslaXyz Failed\n\r");
		assert(0);
	} else {
#ifdef DEBUG_LOGGING
		/*print magnetometer sensor data on serialport */
		printf("Magnetometer micro tesla data :\n\rx = %d\n\ry = %d\n\rz = %d\n\r",
				(int)magData->xAxisData, (int)magData->yAxisData, (int)magData->zAxisData);
		printf("Magnetometer sensor data resistance: %d \n\r", (int)magData->resistance );
#endif
	}

}

void environmentSensorInit(void) {
	Retcode_T environmentSensorReturnValue = (Retcode_T) RETCODE_FAILURE;

	/*initialize Environmental sensor*/
	environmentSensor = xdkEnvironmental_BME280_Handle;
	environmentSensorReturnValue = Environmental_init(environmentSensor);

	if (RETCODE_OK == environmentSensorReturnValue) {
		printf("environmentSensorInit Success\n\r");
	} else {
		assert(0);
	}

}

void readEnvironmentSensor(Environmental_Data_T* value) {
	Retcode_T environmentSensorError = (Retcode_T) RETCODE_FAILURE;

	environmentSensorError = Environmental_readData(environmentSensor, value);

	if (RETCODE_OK == environmentSensorError) {
#ifdef DEBUG_LOGGING
		/*Environment data on serialport */
		printf("Environmental Conversion Data :\n\rp = %ld Pa\n\rt = %ld mDeg\n\rh = %ld %%rh\n\r",
				(long int) value->pressure, (long int) value->temperature, (long int) value->humidity);
#endif
	} else {
		printf("Error reading environment sensor values!\n");
		printSensorError(environmentSensorError);
		assert(0);
	}
}


void orientationSensorInit(void) {
	Retcode_T orientationSensorReturnValue = (Retcode_T) RETCODE_FAILURE;

	/*initialize orientation sensor*/
	orientationSensor = xdkOrientationSensor_Handle;
	orientationSensorReturnValue = Orientation_init(orientationSensor);

	if (RETCODE_OK == orientationSensorReturnValue) {
		printf("orientationSensorInit Success\n\r");

	} else {
		assert(0);
	}

}

void readOrientationSensor(Orientation_QuaternionData_T* value) {
	Retcode_T orientationSensorError = (Retcode_T) RETCODE_FAILURE;

	orientationSensorError = Orientation_readQuaternionValue(value);

	if (RETCODE_OK == orientationSensorError) {
		/*orientation data on serialport */
#ifdef DEBUG_LOGGING
		printf("Orientation quaternion data : %3.4f, %3.4f, %3.4f, %3.4f\n\r",
				value->w, value->x, value->y, value->z);
#endif
	} else {
		printf("Error reading orientation sensor values!\n");
		printSensorError(orientationSensorError);
		assert(0);
	}
}
