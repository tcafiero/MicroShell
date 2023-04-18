#ifndef ioTWMicroShellBLEGlue_h
	#define ioTWMicroShellBLEGlue_h
	
	#include <BleSerial.h>
	#include <MicroShell.h>

	
	extern char MicroShellEcho;	
	
	class SerialShellClass {
		public:
		BleSerial ble;
		void begin() {
			ble.begin("MicroShell");
			InitMicroShell();
			disableEcho();
		}
		void begin(const char *serviceName) {
			ble.begin(serviceName);
			InitMicroShell();
			disableEcho();
		}
		
		void run() {
			while (ble.available())
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
			shell.ble.print(endline);
			} else {
			c[1] = 0;
			shell.ble.print(c);
		}
	}
}
	
	
int ParserGetchar()
{
	return shell.ble.read();
}
	
#endif
