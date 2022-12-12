#include <lmic.h>
#include <hal/hal.h>
#include <LowPower.h> //low power library Include
//#include <credentials.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define testPin 2
#define mosfetPin 4
#define ledPin 13
#define voltPin A3
#define tempPin 9
#define turbPin A0
#define tdsPin A1
#define phPin A2
/*
#ifdef CREDENTIALS
static const u1_t NWKSKEY[16] = NWKSKEY1;
static const u1_t APPSKEY[16] = APPSKEY1;
static const u4_t DEVADDR = DEVADDR1;
#else
static u1_t NWKSKEY[16] = { 0x70, 0x13, 0x44, 0xDE, 0xF3, 0x71, 0x91, 0xB7, 0x55, 0xFE, 0x11, 0x97, 0xA0, 0xA7, 0x7B, 0x48 };
static u1_t APPSKEY[16] = { 0xF4, 0x27, 0x48, 0xE1, 0xC5, 0x84, 0xCD, 0xA1, 0x7C, 0xBD, 0x2D, 0x42, 0x58, 0x7C, 0x14, 0xFD };
static u4_t DEVADDR = 0x26011063;
#endif
*/
// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]={ 0x00, 0x00, 0xC8, 0x1E, 0x12, 0xD6, 0x76, 0x98 };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
// The key shown here is the semtech default key.
static const u1_t PROGMEM APPKEY[16] = { 0xF0, 0xBC, 0x5D, 0x35, 0x60, 0x17, 0xAC, 0xB9, 0x92, 0x9E, 0xD2, 0x06, 0xE3, 0xEF, 0x83, 0x0C };

void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}


static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 1;


// Pin mapping Arduino Pro mini (Mosi-11) (Miso-12)
const lmic_pinmap lmic_pins = {
  .nss = 6,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 5,
  .dio = {7, 8, LMIC_UNUSED_PIN},
};

byte buffer[16];

int count = 0;
void onEvent (ev_t ev) {
  if (ev == EV_TXCOMPLETE) {
    Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
    // Schedule next transmission
    
    Serial.println(F("Watchdog Timer - "));
     
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);
    delay(1000);
    digitalWrite(5, LOW);
    Serial.println(F("Gehe Schlafen für 16 seconds")); 
    //Wichtig da ansonsten deutlich höheren Sleep current
    pinMode(12, INPUT_PULLUP);
    pinMode(13, INPUT_PULLUP);
    delay(1000);
    for (int i = 0; i <= 2; i++) {
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }
    //Wichtig da ansonsten kein zweiter wert gesendet wird
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
    digitalWrite(4, HIGH);
    delay(2000);
    os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
  }
}

void do_send(osjob_t* j) {
  // Payload to send (uplink)

  refreshSensorData();
  LMIC_setTxData2(1, buffer, sizeof(buffer), 0);

  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, buffer, sizeof(buffer), 0);
    // LMIC_setTxData2(1, mydata, sizeof(mydata) - 1, 0);
    Serial.println(F("Packet queued"));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}


void setup() {
  Serial.begin(9600);
  Serial.println(F("Starting..."));

  pinMode(mosfetPin, OUTPUT);
  digitalWrite(mosfetPin, HIGH);
  
  delay(10000);
  pinMode(tdsPin, INPUT);
  pinMode(tempPin, INPUT);
  pinMode(voltPin, INPUT);
  pinMode(turbPin, INPUT);

  pinMode(testPin, INPUT_PULLUP);

  // LMIC init
  os_init();

  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  // Set static session parameters.
  //LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);

  // Disable link check validation
  //LMIC_setLinkCheckMode(0);

  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;

  // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF9, 14);

  // Start job
  do_send(&sendjob);
}

void loop() {
  os_runloop_once();
}

void refreshSensorData() {
  OneWire oneWire(tempPin);
  DallasTemperature sensors(&oneWire);
  sensors.begin();
  sensors.requestTemperatures();
  int16_t battery = analogRead(voltPin);
  int16_t tds = analogRead(tdsPin);
  int16_t turbidity = analogRead(turbPin);
  int16_t ph = analogRead(phPin);
  int16_t temp1 = int16_t(sensors.getTempCByIndex(0));
  int16_t temp2 = int16_t(sensors.getTempCByIndex(1));
  int16_t temp3 = int16_t(sensors.getTempCByIndex(2));

  count++;

  Serial.print(F("|| Batterie: "));
  Serial.print(battery);
  Serial.print(F("|| TDS: "));
  Serial.print(tds);
  Serial.print(F("|| Trübung: "));
  Serial.print(turbidity);
  Serial.print(F("|| PH: "));
  Serial.print(ph);
  Serial.print(F("|| Temperatur1: "));
  Serial.print(temp1);
  Serial.print(F("|| Temperatur2: "));
  Serial.print(temp2);
  Serial.print(F("|| Temperatur3: "));
  Serial.print(temp3);
  Serial.print(F("|| Count: "));
  Serial.println(count);

  buffer[0] = battery >> 8;
  buffer[1] = battery;
  buffer[2] = tds >> 8;
  buffer[3] = tds;
  buffer[4] = turbidity >> 8;
  buffer[5] = turbidity;
  buffer[6] = ph >> 8;
  buffer[7] = ph;
  buffer[8] = temp1 >> 8;
  buffer[9] = temp1;
  buffer[10] = temp2 >> 8;
  buffer[11] = temp2;
  buffer[12] = temp3 >> 8;
  buffer[13] = temp3;
  buffer[14] = count >> 8;
  buffer[15] = count;
}
/*
  float getPH() {
  unsigned long int avgValue;  //Store the average value of the sensor feedback
  float b;
  int buf[10], temp;


  for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor for smooth the value
  {
    buf[i] = analogRead(phPin);
    delay(10);
  }
  for (int i = 0; i < 9; i++) //sort the analog from small to large
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)               //take the average value of 6 center sample
    avgValue += buf[i];
  float phValue = (float)avgValue * 5.0 / 1024 / 6; //convert the analog into millivolt
  phValue = 3.5 * phValue;                  //convert the millivolt into pH value
  Serial.print("    pH:");
  Serial.print(phValue, 2);
  Serial.println(" ");
  digitalWrite(13, HIGH);
  delay(800);
  digitalWrite(13, LOW);

  }
*/
