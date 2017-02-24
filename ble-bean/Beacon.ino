static uint8_t customAdvertData[] = {
    // General discoverable mode advertises indefinitely
    0x02,  // Length
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
    0x03, // Length
    GAP_ADTYPE_16BIT_COMPLETE,  // Param: Service List
    0xAA, 0xFE, // Eddystone ID
    0x11,  // Length
    0x16,  // Service Data
    0xAA, 0xFE, // Eddystone ID
    0x20,  // TLM flag
    0x00,  // TLM version
    0x00,  // Battery voltage
    0x00,  //
    0x00,  // Beacon temperature
    0x00,  //
    0x00,  // Adv PDU count
    0x00,  //
    0x00,  //
    0x00,  //
    0x00,  // Time since power on
    0x00,  //
    0x00,  //
    0x00   //
};

static uint32_t pduCnt;

void setBatteryLevel() {
    int batteryVoltage = Bean.getBatteryVoltage();
    customAdvertData[13] = ((batteryVoltage*10) >> 8) & 0xFF;
    customAdvertData[14] = (batteryVoltage*10) & 0xFF;
}

void setTemperature() {
    int temperature = Bean.getTemperature();
    customAdvertData[15] = ((temperature) >> 8) & 0xFF;
    customAdvertData[16] = (temperature) & 0xFF;
}

void setPressure() {
    Bean.setLed(255, 0, 0);
    digitalWrite(0, HIGH);
    delay(1);
    int sensorValue = analogRead(A0);
    digitalWrite(0, LOW);
    customAdvertData[15] = (sensorValue >> 8) & 0xFF;
    customAdvertData[16] = (sensorValue) & 0xFF;
    Bean.setLed(0, 0, 0);
}

void setup() {
  pduCnt = 0;
    
  pinMode(0, OUTPUT);
    Bean.setCustomAdvertisement(customAdvertData, sizeof(customAdvertData));
  Bean.enableCustom();
  Bean.enableConfigSave(false);
  Bean.setAdvertisingInterval(1000);
}

void loop() {
    
    // Set PDU_CNT
    
    customAdvertData[17] = (pduCnt >> 24) & 0xFF;
    customAdvertData[18] = (pduCnt >> 16) & 0xFF;
    customAdvertData[19] = (pduCnt >> 8) & 0xFF;
    customAdvertData[20] = (pduCnt) & 0xFF;
    
    // Set SEC_CNT
    
    uint32_t time = millis() / 100;
    customAdvertData[21] = (time >> 24) & 0xFF;
    customAdvertData[22] = (time >> 16) & 0xFF;
    customAdvertData[23] = (time >> 8) & 0xFF;
    customAdvertData[24] = (time) & 0xFF;
    
    // Set battery level
    setBatteryLevel();
    
    // Multiplex temperature and pressure
    if (pduCnt % 2 == 1) {
        setTemperature();
    }
    else
    {
        setPressure();
    }
        Bean.setCustomAdvertisement(customAdvertData, sizeof(customAdvertData));
    
    Bean.sleep(1000);
    
    pduCnt++;
}