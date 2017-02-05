/*
 * Sensors.h
 *
 *  Created on: Feb 4, 2017
 *      Author: Oliver
 */

#ifndef SOURCE_SENSORS_H_
#define SOURCE_SENSORS_H_

#include <BCDS_Basics.h>
#include "XdkSensorHandle.h"

#define DEBUG_LOGGING
#define READ_GYRO_VALUES_DEGREE
#define NOISE_SENSOR_ACTIVATED

// Noise sensor constants
#define AKU_SENSITIVITY_VOLT    1
#define AKU_SENSITIVITY_DB    -38
#define AKU_PASCALTODBSPL      94
#define AKU_SENSORGAIN          1


void accelerometerSensorInit(void);
void readAccelerometerData(Accelerometer_XyzData_T* accelData);

void gyroSensorInit(void);
void readGyroData(Gyroscope_XyzData_T* gyroData);

void lightsensorInit(void);
void readLightSensor(uint32_t* milliLuxData);

void noiseSensorInit(void);
void readNoiseSensor(uint32_t* noiseRaw);

void magnetometerSensorInit(void);
void readMagnetometerSensor(Magnetometer_XyzData_T* magData);

void environmentSensorInit(void);
void readEnvironmentSensor(Environmental_Data_T* value);

void orientationSensorInit(void);
void readOrientationSensor(Orientation_QuaternionData_T* value);


#endif /* SOURCE_SENSORS_H_ */
