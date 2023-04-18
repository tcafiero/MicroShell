#ifndef ioTWMicroShellSerialGlue_h
	#define ioTWMicroShellSerialGlue_h
	
	#include <MicroShell.h>
	
	extern char MicroShellEcho;	
	class BLEClass {
		public:
		void print(const char str[])
		{
			Serial.println(str);
		}
		
	};
	
	class SerialShellClass {
		public:
		BLEClass ble;
		void begin() {
			InitMicroShell();
			disableEcho();
		}
		void begin(const char *serviceName) {
			InitMicroShell();
			disableEcho();
		}
		
		void run() {
			while (Serial.available())
				MicroShell();
		}
		private:
		
	};
	
	SerialShellClass shell;
	
void ParserPutchar(const char ch[])
{
	if (MicroShellEcho)
	{
		static const char* endline = "\n\r";
		static char c[5];
		c[0] = ch[0];
		if (c[0] == '\r' || c[0] == '\n')
		{
			Serial.println();
			} else {
			c[1] = 0;
			Serial.print(c);
		}
	}
}
	
	
int ParserGetchar()
{
	return Serial.read();
}
	
#endif
