#include <bluefruit.h>
#include <MicroShell.h>

// BLE Service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery

BLEUart *Interface;

// Put here function ParserPutchar to output character
void ParserPutchar(int ch)
{
  Interface->write( (char *)&ch, 1 );
}

// Put here function ParserGetchar to input character
int ParserGetchar()
{
  return (int) Interface->read();
}


// Put here function prototype to expose with micro shell
char* ExampleFunc(int a, char* b);

// Put here function wrapper
void ExampleFunc_wrapper(int argc, char **argv)
{
  int a;
  char s[20];
  argvscanf("%d %s", &a, s);
  #ifdef DEBUG
  result("Result: %s\n", ExampleFunc(a, s));
  Serial.printf("Result: %s\n", ExampleFunc(a, s));
  #else
  ExampleFunc(a, s);  
  #endif
}

//Put here Publish function table
PublishFunctionStruct PublishFunction[] =
{
  {"ExampleFunc", ExampleFunc_wrapper},
  {"", 0}
};

char* ExampleFunc(int a, char* b)
{
  static char buf[50];
  sprintf(buf, "a+1=%d and b=%s\n", a + 1, b);
  Interface->write( buf, strlen(buf) );
  return buf;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  
  // Setup the BLE LED to be enabled on CONNECT
  // Note: This is actually the default behaviour, but provided
  // here in case you want to control this LED manually via PIN 19
  Bluefruit.autoConnLed(true);

  // Config the peripheral connection with maximum bandwidth 
  // more SRAM required by SoftDevice
  // Note: All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(4);
  Bluefruit.setName("TopViewEasyStroke");
  //Bluefruit.setName(getMcuUniqueID()); // useful testing with multiple central connections
  Bluefruit.setConnectCallback(connect_callback);
  Bluefruit.setDisconnectCallback(disconnect_callback);

  // Configure and Start Device Information Service
  bledis.setManufacturer("IoThingsWare");
  bledis.setModel("TopView nRF52Token");
  bledis.begin();

  // Configure and Start BLE Uart Service
  Interface=&bleuart;
  Interface->begin();
  // Start BLE Battery Service
  blebas.begin();
  blebas.write(100);

  // Set up and start advertising
  startAdv();

  Serial.println("Please use TopView's LE app to connect in UART mode");
  Serial.println("Once connected, enter character(s) that you wish to send");
  InitMicroShell();
}

void loop() {
  // put your main code here, to run repeatedly:
  while ( Interface->available() )
  {
    MicroShell();
  }
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  char central_name[32] = { 0 };
  Bluefruit.Gap.getPeerName(conn_handle, central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}



/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 * https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/cores/nRF5/nordic/softdevice/s140_nrf52_6.1.1_API/include/ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.println("Disconnected");
}
