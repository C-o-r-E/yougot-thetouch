#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <unistd.h>

#include <linux/input.h>

typedef struct touchContact
{
  unsigned int id;
  unsigned int cnt;
  int pos_x;
  int pos_y;
  int last_x;
  int last_y;
} touch_contact;

typedef struct tagevent
{
    struct timeval time;
    unsigned short type;
    unsigned short code;
    int value;
} input_event;

#define BUFFSIZE 32
#define MAX_CONTACTS 2

input_event evBuf[BUFFSIZE];
touch_contact contacts[MAX_CONTACTS];

unsigned int event_count;
int slot;


int distance(touch_contact tc1, touch_contact tc2)
{
  double dist;
  double a, b;

  a = pow((double)(tc2.pos_x - tc1.last_x), 2.0);
  b = pow((double)(tc2.pos_y - tc1.last_y), 2.0);

  dist = sqrt(a + b);

  return (int) dist;
}

void zoomCheck()
{

  //we must have 2 active contacts
  if( ((contacts[0].id != -1) && (contacts[1].id != -1)) &&
      //we need at least 2 positions recorded
      ((contacts[0].cnt > 1) && (contacts[1].cnt > 1))
      )
    {
      printf("dist = %d\n", distance(contacts[0], contacts[1]));
    }

}

void init_machine()
{
  //zero everything
  memset(&contacts, 0, sizeof(touch_contact) * MAX_CONTACTS);
  memset(&evBuf, 0, sizeof(input_event) * BUFFSIZE);

  event_count = 0;
  slot = 0;
}

void parse(int num_events)
{
  unsigned int loc = 0;
  unsigned int id = 0;
  int i;
  int got_pos = 0;
  for(i=0; i<num_events; i++)
    { 
      switch(evBuf[loc].code)
	{
	case ABS_MT_SLOT:
	  //printf("SLOT %d\n", evBuf[loc].value);

	  //if we just got some position info in the last slot
	  if (got_pos)
	    {
	      contacts[slot].cnt++;
	      got_pos = 0;
	    }

	  slot = evBuf[loc++].value;
	  //is the id valid?
	  if( (slot < -1) || (slot >= MAX_CONTACTS) )
	    {
	      printf("Invalid slot = %d!\n", slot);
	      break;
	    }

	  break;
	  
	case ABS_MT_TRACKING_ID:
	  //printf("ID %d\n", evBuf[loc].value);
	  id = evBuf[loc++].value;
	  contacts[slot].id = id;
	  contacts[slot].cnt = 0;
	  break;
	  
	case ABS_MT_POSITION_X:
	  //printf("\tx: %d\n", evBuf[loc].value);
	  contacts[slot].last_x = contacts[slot].pos_x;
	  contacts[slot].pos_x = evBuf[loc++].value;
	  got_pos = 1;
	  break;
	  
	case ABS_MT_POSITION_Y:
	  //printf("\ty: %d\n", evBuf[loc].value);
	  contacts[slot].last_y = contacts[slot].pos_y;
	  contacts[slot].pos_y = evBuf[loc++].value;
	  got_pos = 1;
	  break;
	}
    }

  if (got_pos)
    {
      contacts[slot].cnt++;
    }

  //here we can actually checl for zoom
  zoomCheck();

}

void handleEvent(input_event ev)
{
  evBuf[event_count++] = ev;
  
  if(ev.code == SYN_REPORT)
    {
      parse(event_count);
      event_count = 0;
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
