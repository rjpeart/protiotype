const noble = require('noble');

var batteryVoltage = 0xFFFF;
var temperature = 0xFFFF;
var pressure = 0xFFFF;


noble.on('stateChange', function(state) {
	if (state === 'poweredOn') {
		noble.startScanning([], true);
	} else {
		noble.stopScanning();
	}
});

noble.on('discover', function(peripheral) {

		var serviceData = peripheral.advertisement.serviceData;
		if (serviceData && serviceData.length) {
			for (var i in serviceData) {
				if ((serviceData[i].uuid == "feaa") && (serviceData[i].data[0] == 0x20)) {
					var newBatteryVoltage = (serviceData[i].data[2] << 8) + serviceData[i].data[3];
					batteryVoltage = newBatteryVoltage;
					var pduCnt = (serviceData[i].data[6] << 24) + (serviceData[i].data[7] << 16) + (serviceData[i].data[8] << 8) + serviceData[i].data[9];
					var secCnt = (serviceData[i].data[10] << 24) + (serviceData[i].data[11] << 16) + (serviceData[i].data[12] << 8) + serviceData[i].data[13];
					if (pduCnt % 2 == 1)
					{
						var newTemperature = (serviceData[i].data[4] << 8) + serviceData[i].data[5];
						temperature = newTemperature;
					}
					else
					{
						var newPressure = (serviceData[i].data[4] << 8) + serviceData[i].data[5];
						pressure = newPressure;
					}
					console.log("device = " + peripheral.address + ", cnt = " + pduCnt + ", temp = " + temperature + ", battery = " + batteryVoltage + ", pressure = " + pressure + ", time = " + secCnt + ", txPower = " + peripheral.advertisement.txPowerLevel + ", rssi = " + peripheral.rssi);
				}
			}
	}
});

