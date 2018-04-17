#include <signal.h>
#include <sys/prctl.h>
#include <linux/prctl.h>
#include <stdio.h>
#include <unistd.h>

#include "kaiui.h"

size_t running = 1;

void OnEvent(KAIEVENT id, void* data, int data_size);

void OnEvent(KAIEVENT id, void* data, int data_size) {
  switch(id) {
    case EV_ENGINE_HERE: {
      ENGINE_HERE* eh = (ENGINE_HERE*)data;
      fprintf(stderr, "EV_ENGINE_HERE: %s:%d\r\n", eh->engine_ip, eh->engine_port);
      Attach();
      break;
    }
    case EV_ENGINE_IN_USE: {
      Takeover();
      break;
    }
    case EV_DETACH: {
      fprintf(stderr, "Detached. \r\n");
      Discover();
    }
    case EV_ATTACH: {break;}
    default:
      fprintf(stderr, "Got event #%d, and data is %d byte(s) long.\r\n", id,data_size);
      break;
  }
}

int main(int argc, char const *argv[]) {
  prctl(PR_SET_PDEATHSIG, SIGHUP);
  Init(OnEvent); // Set libkaiui callback
  Discover();
  char data[100];

  // I know i know. use select or something.
  while(running == 1) {
    scanf("%[^\n]%*c", data);
    fprintf(stderr, "got data %s \r\n", data);
    sleep(1);
  }

  return running;
}
