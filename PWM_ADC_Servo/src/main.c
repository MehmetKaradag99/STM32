#include "myLib.h"

int main(void)
{
	StartConfig();
	waitingForInterrupt();
  while (1)
  {
	  RunningServo();
  }
}



