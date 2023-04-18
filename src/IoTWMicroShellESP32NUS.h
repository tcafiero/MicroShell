#ifndef IoTWMicroShellESP32NUS_h
	#define IoTWMicroShellESP32NU_h
	
	#include <BleSerial.h>
	#include <MicroShell.h>
	char *inputString = buffer;
	bool stringComplete = false;  // whether the string is complete
	
	extern char servicename[40];
	
	class BLEShellClass {
		public:
		BleSerial ble;
		void begin() {
			inputString = buffer;
			InitMicroShell();
			ble.begin(servicename, true, D2);
			stringComplete = false;
			i = 0;
			disableEcho();
		}
		void run() {
			bleEvent();
			if (stringComplete) {
				//ble.println(inputString);
				//Serial.println(inputString);
				Execute(inputString);
				// clear the string:
				i = 0;
				stringComplete = false;
			}
		}
		private:
		bool stringComplete = false;  // whether the string is complete
		int i;
		void bleEvent() {
			while (ble.available()) {
				// get the new byte:				
				if (i >= (BUFSIZE - 1)) return;
				char inChar = (char)ble.read();
				// add it to the inputString:
				if ((inChar == '\r') || (inChar == '\n')) {
					stringComplete = true;
				}
				else {
					inputString[i++] = inChar;
					inputString[i] = 0;
				}
			}
		}
	};
	
	BLEShellClass shell;
	
	
#endif
