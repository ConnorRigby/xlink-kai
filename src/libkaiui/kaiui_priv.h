/*******************************************
Copyright (c) 2004, Luís Miguel Fernandes <zipleen 'at' teamxlink.co.uk>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************/

#ifndef KAIUIPRIV__H
#define KAIUIPRIV__H

/* Private file for kaiui.c */
/* This will have all my internal functions used in the library.. no need to put trash in the kaiui.h :P */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define HAVE_XBOX 1
#define HAVE_PS2 2
#define HAVE_GC 4

#define KAI_PORT 34522

#ifdef _XBOX
#define KAID_STRING_LENGTH 2048
#else
#define KAID_STRING_LENGTH 16384
#endif

#define MAX_CHAR 17 // max username (16+1)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Data structures and typedefs */
extern struct sockaddr_in server,temp_rcv; // server is where we send data to, rcv is where data is received
extern char nickname[MAX_CHAR]; // our nick... stored because of EV_USER_DATA            36  // (casecorrectedusername)
extern char current_chat_room[KAID_STRING_LENGTH]; // we need this global because ArenaHelper functions need this info...
extern char current_arena[KAID_STRING_LENGTH];
extern int Logged_in; // logged in state. currently we only need 0=not logged 1=logged
extern int attached;
extern int discovered;

typedef struct _adm{
	char nick[MAX_CHAR];
	struct _adm *next;
}ADM_LIST;

typedef ADM_LIST MOD_LIST;

/*List functions */
ADM_LIST *Search_adm(char *n);
MOD_LIST *Search_mods(char *n);
MOD_LIST *Insert_mod(char *n);
ADM_LIST *Insert_admin(char *n);
void clear_admin();
void clear_mods();

// parsing code
int parseopcode(char *s);
void parseDisc(char *msg_recv); // special one for discover
void parsePackets(char *);
void EscapeUIMsg(char *msg); // use this before sending messages or receiving them

// sending data
void SendToEngine(char *msg);
void SendDataToEngine(void *bytes, int len);
// discover, takeover and attach
void GoDiscovered();
void GoEngineInUse();
void GoAttach();

void mysleep(int s);

/* mutex - they are still used in xbox, and we have them in here from precaution with recvfrom() and sendto()*/
void EnterCriticalSect();
void LeaveCriticalSect();
void InitCriticalSect();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
