#ifndef ioTWMicroShellESP32BLEUartGlue_h
	#define ioTWMicroShellESP32BLEUartGlue_h
	
	#include <MicroShell.h>
	#include <BLEDevice.h>
	#include <BLEServer.h>
	#include <BLEUtils.h>
	#include <BLE2902.h>
	#include <CircularBuffer.h>
	#include <esp_task_wdt.h>
	
	extern char MicroShellEcho;
	extern char servicename[40];
	CircularBuffer<char, 40> myQueue;
	
	BLEServer *pServer = NULL;
	BLECharacteristic * pTxCharacteristic;
	BLECharacteristic * pRxCharacteristic;
	#define BLE_SERIAL_SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
	#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
	#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
	
	BLE2902 iBLE2902;
	bool deviceConnected;
	bool dataReady;
	void putstr(const char str[]);
	
	class MyServerCallbacksClass: public BLEServerCallbacks {
		void onConnect(BLEServer* pServer) {
			#ifdef WDT_TIMEOUT
				esp_task_wdt_init(WDT_TIMEOUT, false); //disable panic so ESP32 restarts
			#endif
			deviceConnected = true;
		};
		
		void onDisconnect(BLEServer* pServer) {
			#ifdef WDT_TIMEOUT
				esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
			#endif
			// Start advertising
			pServer->getAdvertising()->start();
			dataReady=false;
			deviceConnected = false;
		}
	};
	MyServerCallbacksClass MyServerCallbacks;
	
	class MyCharacteristicCallbacksClass: public BLECharacteristicCallbacks {
		void onWrite(BLECharacteristic *pCharacteristic) {
			dataReady=true;
		}
	};
	MyCharacteristicCallbacksClass MyCharacteristicCallbacks;
	
	class BLEClass {
		public:
		void print(const char str[])
		{
			putstr(str);
		}
		
	};


	class BLEShellClass {
		public:
		BLEClass ble;
		void begin() {
			deviceConnected = false;
			// Create the BLE Device
			BLEDevice::init(servicename);
			
			// Create the BLE Server
			pServer = BLEDevice::createServer();
			pServer->setCallbacks(&MyServerCallbacks);
			
			// Create the BLE Service
			BLEService *pService = pServer->createService(BLE_SERIAL_SERVICE_UUID);
			
			// Create a BLE Characteristic
			pTxCharacteristic = pService->createCharacteristic(
				CHARACTERISTIC_UUID_TX,
				BLECharacteristic::PROPERTY_NOTIFY
			);
			pTxCharacteristic->addDescriptor(&iBLE2902);
			pTxCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED);
			pTxCharacteristic->setReadProperty(true);
	
			pRxCharacteristic = pService->createCharacteristic(
				CHARACTERISTIC_UUID_RX,
				BLECharacteristic::PROPERTY_WRITE
			);			
			pRxCharacteristic->setCallbacks(&MyCharacteristicCallbacks);
			pRxCharacteristic->setAccessPermissions(ESP_GATT_PERM_WRITE_ENCRYPTED);
			pRxCharacteristic->addDescriptor(&iBLE2902);
			pRxCharacteristic->setWriteProperty(true);
			
			// Start the service
			pService->start();
			
			// Start advertising
			// Start advertising
			pAdvertising = BLEDevice::getAdvertising();
			pAdvertising->addServiceUUID(BLE_SERIAL_SERVICE_UUID);
			pAdvertising->setScanResponse(true);
			pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
			pAdvertising->setMinPreferred(0x12);			
			pAdvertising->start();
			//pServer->getAdvertising()->start();
			dataReady=false;
			InitMicroShell();
			disableEcho();
		}
		void begin(const char *serviceName) {
			deviceConnected = false;
			// Create the BLE Device
			BLEDevice::init(serviceName);
			
			// Create the BLE Server
			pServer = BLEDevice::createServer();
			pServer->setCallbacks(&MyServerCallbacks);
			
			// Create the BLE Service
			BLEService *pService = pServer->createService(BLE_SERIAL_SERVICE_UUID);
			
			// Create a BLE Characteristic
			pTxCharacteristic = pService->createCharacteristic(
				CHARACTERISTIC_UUID_TX,
				BLECharacteristic::PROPERTY_NOTIFY
			);
			pTxCharacteristic->addDescriptor(&iBLE2902);
			
			pRxCharacteristic = pService->createCharacteristic(
				CHARACTERISTIC_UUID_RX,
				BLECharacteristic::PROPERTY_WRITE
			);
			
			pRxCharacteristic->setCallbacks(&MyCharacteristicCallbacks);
			
			// Start the service
			pService->start();
			
			// Start advertising
			pAdvertising = BLEDevice::getAdvertising();
			pAdvertising->addServiceUUID(BLE_SERIAL_SERVICE_UUID);
			pAdvertising->setScanResponse(true);
			pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
			pAdvertising->setMinPreferred(0x12);			
			pAdvertising->start();
			//pServer->getAdvertising()->start();
			dataReady=false;
			InitMicroShell();
			disableEcho();
		}
		
		void run() {
			if (deviceConnected)
			{
				if (dataReady) {
					dataReady=false;
					std::string rxValue = pRxCharacteristic->getValue();
					static char c;
					for (int i = 0; i < rxValue.length(); i++)
					{
						c = rxValue[i];
						if (c != '\n')
						{
							myQueue.push(c);
						}
					}
				}
				while (!myQueue.isEmpty())
				MicroShell();
			}
		}
		private:
		BLEAdvertising *pAdvertising;
		
	};
	
	BLEShellClass shell;
	
	void ParserPutchar(const char ch[])
	{
		if (MicroShellEcho)
		{
			static const char endline[] = "\n\r";
			static char c[5];
			c[0] = ch[0];
			if (c[0] == '\r' || c[0] == '\n')
			{
				pTxCharacteristic->setValue((uint8_t*)endline, 2);
				pTxCharacteristic->notify();
				} else {
				pTxCharacteristic->setValue((uint8_t*)c, 1);
				pTxCharacteristic->notify();
			}
		}
	}
	
	void putstr(const char str[])
	{
		const int paylod_len = 20;
		const int len = strlen(str);
		int index = 0;
		for (int i = 0 ; i < len / paylod_len ; i++)
		{
			pTxCharacteristic->setValue((uint8_t*)&str[index], paylod_len);
			pTxCharacteristic->notify();
			delay(30);
			index += paylod_len;
		}
		if (len - index) {
			pTxCharacteristic->setValue((uint8_t*)&str[index], len - index);
			pTxCharacteristic->notify();
			delay(30);
		}
	}
	
	int ParserGetchar()
	{
		return myQueue.shift();
	}
	
#endif
