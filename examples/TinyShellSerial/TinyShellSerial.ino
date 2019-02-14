#include <MicroShell.h>
Stream *Interface;

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
  Interface=&Serial;
  InitMicroShell();
}

void loop() {
  // put your main code here, to run repeatedly:
  while ( Interface->available() )
  {
    MicroShell();
  }
}
