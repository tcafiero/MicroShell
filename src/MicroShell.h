#ifndef __MICROSHELL_H
	#define __MICROSHELL_H

#ifdef __cplusplus
extern "C"{
#endif

#ifndef PublishFunctionStruct_DEFINED
#define PublishFunctionStruct_DEFINED
typedef struct
{
  char *name;
  void (*pfunc)(int argc, char **argv);
} PublishFunctionStruct;
#endif

extern PublishFunctionStruct PublishFunction[];


void InitMicroShell();
void enableExecution();
void disableExecution();
void MicroShell();
int argvscanf(  const char *format, ... );
void result(char *format, ...);
void ParserPutchar(const char ch[]);
int ParserGetchar();
void disableEcho();
void enableEcho();
#ifdef __cplusplus
}
#endif

#endif