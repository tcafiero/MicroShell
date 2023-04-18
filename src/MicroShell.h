#ifndef __MICROSHELL_H
	#define __MICROSHELL_H

#ifdef __cplusplus
extern "C"{
#endif

#ifndef PublishFunctionStruct_DEFINED
#define PublishFunctionStruct_DEFINED
#define BUFSIZE 80
#define PARAMETERSNUM 10

extern char buffer[];

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
void Execute(char *buffer);
void MicroShell();
int argvscanf(  const char *format, ... );
void result(char *format, ...);
int ParserGetchar();
void disableEcho();
void enableEcho();
#ifdef __cplusplus
}
#endif

#endif