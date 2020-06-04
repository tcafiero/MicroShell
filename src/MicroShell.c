//delete comment in following statement if you need debug print
//#define DEBUG 1

// Do not touch following lines end comment
#include "Arduino.h"
#include "MicroShell.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#define BUFSIZE 80
#define PARAMETERSNUM 10
static char executionEnabled;
char buffer[BUFSIZE];
char *argv[PARAMETERSNUM];
int argc;
int i;
char MicroShellEcho;
const char BS=0x8;
const char BELL=0x7;
const char LF=0xa;
const char CR=0xd;
const char PROMPT='>';


int CallFormat(char* src)
{
  int index, i;
  i = 0;
  do
  {
    for ( index = 0; *src != '\0' && *src != ' ' && i < PARAMETERSNUM; index++, src++)
      if (index == 0) argv[i++] = src;
    if ( *src == ' ' ) *src = '\0';
  }
  while (*++src != '\0');
  argc = i;
  return i;
}

int argvscanf(  const char *format, ... )
{
  va_list ap;
  int conv = 1, *i;
  float *f;
  char *a;
  const char *fp;
  va_start ( ap, format );
  for ( fp = format; *fp != '\0'; fp++ ) {
    if ( *fp == '%' ) {
      switch ( *++fp ) {
        case 'd':
          i = va_arg ( ap, int * );
          *i = atoi ( argv[conv] );
          break;
		case 'f':
		  f = va_arg ( ap, float * );
		  *f = atof( argv[conv] );
		  break;
        case 's':
          a = va_arg ( ap, char * );
          strncpy ( a, argv[conv], strlen ( argv[conv] ) + 1 );
          break;
      }
      conv++;
    }
  }
  va_end ( ap );
  return conv;
}

static void Scanner()
{
  int i;
  if (argc == 0) return;
  //for(i=0; i < argc; i++) printf("%d -> %s\n", i, argv[i]);
  for (i = 0; strlen(PublishFunction[i].name); i++)
  {
    if (strcmp(PublishFunction[i].name, argv[0]) == 0)
    {
	  ParserPutchar(&CR);
      if(executionEnabled || (strcmp(argv[0], "askAccess") ==0))(*PublishFunction[i].pfunc)(0, argv);
	  for(int j=0; j < BUFSIZE; j++) buffer[j]=0;
	  ParserPutchar(&CR);
	  ParserPutchar(&PROMPT);
      return;
    }
  }
    for(int j=0; j < BUFSIZE; j++) buffer[j]=0;
    ParserPutchar(&CR);
    ParserPutchar(&PROMPT);
}

static void Execute(char *buffer)
{
  CallFormat(buffer);
  Scanner();
}



void InitMicroShell()
{
  i = 0;
  MicroShellEcho = 1;
  executionEnabled = 1;
  for(int j=0; j < BUFSIZE; j++) buffer[j]=0;
}

void enableExecution()
{
	executionEnabled=1;
}

void disableExecution()
{
	executionEnabled=0;
}

void disableEcho()
{
	MicroShellEcho = 0;
}

void enableEcho()
{
	MicroShellEcho = 1;
}

void MicroShell()
{
  char ch = (char)ParserGetchar();
  switch (ch)
  {
    //case 0x8:   if (i)
    case '<':   if (i)
      {
		i--;
		ParserPutchar(&BS);
      }
      break;
    case 0x0a:
    case 0x0d:
      buffer[i] = 0;
      Execute(buffer);
      i = 0;
      buffer[0] = 0;
      break;
    default:    if (i >= BUFSIZE - 2)
      {
        ParserPutchar(&BELL);
        break;
      }
	  if (isprint(ch)) {
		buffer[i++] = ch;
		ParserPutchar(&ch);
		break;
		}
		
  }
}
