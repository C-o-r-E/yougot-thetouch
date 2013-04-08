#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/input.h>

typedef struct tagevent
{
    struct timeval time;
    unsigned short type;
    unsigned short code;
    int value;
} input_event;

#define BUFFSIZE 32
input_event evBuf[BUFFSIZE];
unsigned int cnt = 0;

void parse()
{
  unsigned int loc = 0;
  unsigned int slot = 0;
  unsigned int id = 0;

  switch(evBuf[loc].code)
    {
    case ABS_MT_SLOT:
      slot = evBuf[loc++].value;
      break;

    case ABS_MT_TRACKING_ID:
      id = evBuf[loc++].value;
      break;

    case ABS_MT_POSITION_X:
      break;

    case ABS_MT_POSITION_Y:
      break;
    }
}

void handleEvent(input_event ev)
{
  evBuf[cnt++] = ev;
  
  if(ev.code == SYN_REPORT)
    {
      parse();
      cnt = 0;
    }
  
}

/*
void ParseEvent( input_event ev )
{
    static unsigned short X, Y, State;
    static unsigned short X2, Y2, State2;
    static unsigned char bIsSecond;

    static unsigned int  count;

    count++;
    printf("got event %d\n", count);

    if( ev.type == EV_ABS ) 
	  {
	    switch( ev.code ) 
	      {
		
	      case ABS_MT_SLOT:
		printf("	[ABS_MT_SLOT]           code = %3d, value = %4d\n", ev.code , ev.value);
		break;
		
	      case ABS_MT_PRESSURE: 
		printf("	[ABS_MT_PRESSURE]       code = %3d, value = %4d\n", ev.code , ev.value);
		break;
		
	      case ABS_MT_TRACKING_ID:
		printf("	[ABS_MT_TRACKING_ID]    code = %3d, value = %4d\n", ev.code , ev.value);
		break;
	      case ABS_MT_POSITION_X: 
		printf("	[ABS_MT_POSITION_X]     code = %3d, value = %4d\n", ev.code , ev.value);
		break;
	      case ABS_MT_POSITION_Y: 
		printf("	[ABS_MT_POSITION_Y]     code = %3d, value = %4d\n", ev.code , ev.value);
		break;
	      case ABS_MT_TOUCH_MAJOR:
		printf("	[ABS_MT_TOUCH_MAJOR]    code = %3d, value = %4d\n", ev.code , ev.value);
		break;
	      case ABS_MT_WIDTH_MAJOR:
		printf("	[ABS_MT_WIDTH_MAJOR]    code = %3d, value = %4d\n", ev.code , ev.value);
		break;
		
	      case ABS_X: 
		printf("	[ABS_X]                 code = %3d, value = %4d\n", ev.code , ev.value);
		break;
	      case ABS_Y: 
		printf("	[ABS_Y]                 code = %3d, value = %4d\n", ev.code , ev.value);
		break;
	      case ABS_RX: 
		printf("	[ABS_RX]                code = %3d, value = %4d\n", ev.code , ev.value);
		break;
	      case ABS_RY: 
		printf("	[ABS_RY]                code = %3d, value = %4d\n", ev.code , ev.value);
		break;
	      case ABS_PRESSURE: 
		printf("	[ABS_PRESSURE]          code = %3d, value = %4d\n", ev.code , ev.value);
		break;
	      default:
		break;
	      }
	  }
	else if( ev.type == EV_KEY )
	  {
	    switch( ev.code )
	      {
	      case BTN_TOUCH:
		printf("	[BTN_TOUCH]		code = %3d, value = %4d\n", ev.code , ev.value);
		break;
	      case BTN_LEFT:
		printf("	[BTN_LEFT]		code = %3d, value = %4d\n", ev.code , ev.value);
		break;
	      case BTN_EXTRA:
		printf("	[BTN_EXTRA]		code = %3d, value = %4d\n", ev.code , ev.value);
		break;
	      default:
		break;
	      }
		
	  }
	else if( ev.type == EV_SYN )
	  {
	    switch( ev.code )
	      {
	      case SYN_REPORT:
		printf("	[SYN_REPORT]		code = %3d, value = %4d\n\n", ev.code , ev.value);
		break;

	      case SYN_MT_REPORT:
		printf("	[SYN_MT_REPORT]		code = %3d, value = %4d\n", ev.code , ev.value);
		break;

	      default:
		break;
	      }	
	  }
	else if( ev.type == EV_MSC )
	  {
	    if( ev.code == MSC_SCAN )
	      printf("	[MSC_SCAN]		code = %3d, value = %4d\n", ev.code , ev.value);
	  }

    
}
*/



int main( int argc, char **argv )
{
    int fd;
    unsigned char byFlag = 0; /* 0: Point */
    char strPort[64] = { 0 };
    fd_set readfds;

    if( argc == 1 || argc > 2 ) 
      {
        printf(" \nUsage:\n \tGetEvent [Port]\n");
        printf("Example:\n \tGetEvent /dev/input/event1\n\n");
        return 0;
      }
	
    sprintf( strPort, "%s", argv[1] );
    printf("Event Port = %s ", strPort);
    fd = open(strPort, O_RDONLY );
    if( fd >= 0 ) 
      {	
        printf(" open: ok\n" );
        FD_ZERO( &readfds );
        FD_SET( fd , &readfds );
        while( 1 ) 
	  {
            if( select( fd+1, &readfds, NULL, NULL, NULL ) > 0 ) {
	      input_event ev;
	      if( sizeof( input_event ) == read( fd, &ev, sizeof( input_event ) ) ) 
		{
		  handleEvent(ev);
		  //ParseEvent( ev );
                } else 
		{
		  printf(" Nothing read \n" );
                }
            }
	  }	
      }
    return 0;
}
