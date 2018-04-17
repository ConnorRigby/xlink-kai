/*******************************************
Copyright (c) 2004, Luís Miguel Fernandes <zipleen 'at' teamxlink.co.uk>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************/

#ifndef KAIUI__H
#define KAIUI__H

#define LIBKAIUI_VERSION                 "7.0.0.3"
#define MAX_CHAR 17 // max username (16+1)

#ifdef _XBOX
#define KAI_MAX_CHAR 1792
#else
#define KAI_MAX_CHAR 16128
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* events and their structs */

typedef struct {
    char* question;
    char* response;
} APP_SPECIFIC;

typedef struct {
    char* user;
    char* msg;
} ARENA_PM;

typedef struct {
	char* user; 
	unsigned int ping; 
	unsigned int status; // 0 -> IDLE | 1-> BUSY | 2-> HOSTING | 3-> DEDICATED
	unsigned int players; // means how many consoles are up to the fight :p
	unsigned int ui; // if set to 1 this is a UI
	unsigned int xbox_ui; // if set to 1 this is a xbox UI
	unsigned int console_and_voice; //  if set to 1 the user has at least 1 console and has voice capabilities enabled
	unsigned int text_chat; // if set to 1, the UI has text chat capabilities
	unsigned int bot; // if set to 1 this is a bot
	char* ui_caps; // for XBMC compatibility :P
} ARENA_CONTACT_PING;

typedef struct {
    int mode;
    int localusers;
} ARENA_STATUS;

typedef struct {
    char* user;
    char* url;
} AVATAR;

typedef struct {
    char* chatroom;
    char* user;
    char* msg;
} CHAT;

typedef struct {
	char* user;
	char* state;
	unsigned int ping;
	unsigned int ui; // if set to 1 this is a UI
	unsigned int xbox_ui; // if set to 1 this is a xbox UI
	unsigned int console_and_voice; //  if set to 1 the user has at least 1 console and has voice capabilities enabled
	unsigned int text_chat; // if set to 1, the UI has text chat capabilities
	unsigned int bot; // if set to 1 this is a bot
	char* ui_caps; // for XBMC compatibility :P
} CONTACT_PING;

typedef struct {
	char* engine_ip;
	int   engine_port;
} ENGINE_HERE;

typedef struct {
	char* user;
	char* vector;
	char* time;
	char* msg;
} INVITE;

typedef struct {
	char* chatroom;
	char* user;
	int admin;
	int mod;
} JOINS_CHAT;

typedef struct {
	char* chatroom;
	char* user;
} LEAVES_CHAT;          

typedef struct {
	char* orbname;
	char* reachable;
	char* myip;
	int myport;
	char* engine_version;
	char* engine_platform;
	char* engine_author;
	char* adapter;
	char* locked;
	unsigned int engine_up;
	unsigned int engine_down;
	unsigned int chat_up;
	unsigned int chat_down;
	unsigned int orb_up;
	unsigned int orb_down;
	char* captech;
} METRICS;

typedef struct {
	char* arena;
	char* mods;
} MODERATOR_PRIVILEGES;

typedef struct {
	char* user;
	char* pass;
} NOT_LOGGED_IN;

typedef struct {
	char* user;
	char* msg;
} PM;

typedef struct {
	char* user;
	char* device;
} REMOTE_ARENA_DEVICE;

typedef struct {
	char* vector;
	unsigned int user_count;
	unsigned int private_vector_count;
} SPECIFIC_COUNT;

typedef struct {
	char* user;
	char* data;
} SPEEX;

typedef struct {
	char* vector;
	unsigned int user_count;
	unsigned int private_vector_count;
	int ispass; // 0 = no pass, 1 = pass enabled
	unsigned int maxplayers;
} SUB_VECTOR;

typedef SPECIFIC_COUNT VECTOR_UPDATE;

typedef struct {
	char* name;
	unsigned int age;
	char* bandwidth;
	char* location;
	unsigned int consoles; // Bitwise field, 0 = no consoles, 1 = Xbox, 2 = ps2, 4 = gc, 8 = psp??, 16 = ??
	char* bio;
} USER_PROFILE;

typedef struct {
	// por acaso nao precisamos disto num vector criado por um user normal? depois deleta este comment ...
	char* vector;
	unsigned int user_count;
	unsigned int private_vector_count;
	int ispass; // 0 = no pass, 1 = pass enabled
	unsigned int maxplayers;
	
	char* description;
} USER_SUB_VECTOR;

typedef struct {
	char* vector;
	int cancreate;
} VECTOR;

typedef struct {
	char* vector;
	char* reason;
} VECTOR_DISALLOWED;

/* Event IDs to be used in callback */
/*
	Events with single char* param should use second param from callback, straight!
	structs for composite (more than one param) are defined accordingly, for ex.
	for EV_JOINS_CHAT, theres a struct JOINS_CHAT.
	for UI coders, an easy switch would be:

		void OnEvent(KAIEVENT id, void* data, int size)
		{
			switch(id)
			{
				case EV_JOINS_CHAT:
				{
					JOINS_CHAT* jc;
					jc = (JOINS_CHAT*)data;
					printf("%s joined the room %s\n", jc->user, jc->chatroom);
					break;
				}
			}
		}

*/
#define EV_ADD_CONTACT          1   // (char* user)
#define EV_ADMIN_PRIVILEGES     2   // (char* list) w/ "/" delim
#define EV_APP_SPECIFIC         3   // (char* question, char* response)
#define EV_ARENA_CONTACT_PING	5
#define EV_ARENA_PM             6   // (char* user,char* msg)
#define EV_ARENA_STATUS         7   // (int mode, int local)
#define EV_ATTACH               8
#define EV_AUTH_FAILED          9
#define EV_AVATAR               10   // (char* user,char* url)
#define EV_CHAT                 11  // (char* chatroom,char* user,char* msg)
#define EV_CHATMODE             12  // (char* roomname)
#define EV_CONNECTED_ARENA      13
#define EV_CONNECTED_MESSENGER	14
#define EV_CONTACT_OFFLINE      15  // (char* user)
#define EV_CONTACT_ONLINE       16  // (char* user)
#define EV_CONTACT_PING         17  // (char* user, unsigned int state, unsigned int ping)
#define EV_DETACH               18
#define EV_DHCP_FAILURE         19 // (char* mac)
#define EV_ENGINE_HERE          20  // (char* engine_ip, int engine_port)
#define EV_ENGINE_IN_USE        21
#define EV_INVITE		22 // (char* user, char* vector, char* time, char* msg)
#define EV_JOINS_CHAT           23  // (char* chatroom,char* user)
#define EV_JOINS_VECTOR         24  // (char* user)
#define EV_LEAVES_CHAT          25  // (char* chatroom,chat* user)
#define EV_LEAVES_VECTOR        26  // (char* user)
#define EV_LOCAL_DEVICE         27  // (char* mac)
#define EV_LOGGED_IN            28
#define EV_METRICS              29  // (char* list_of_metrics) Or struct
#define EV_MODERATOR_PRIVILEGES 30  // (arena, list_of_mods)
#define EV_NOT_LOGGED_IN        31  // (user, pass) If auto, lib will login
#define EV_PM                   32  // (user, msg)
#define EV_REMOTE_ARENA_DEVICE  33  // (user, device)
#define EV_REMOVE_CONTACT       34  // (user)
#define EV_REMOVE_SUB_VECTOR	35  // (vector)
#define EV_SPECIFIC_COUNT       36  // (vector, count, subs)
#define EV_STATUS               37  // (msg)
#define EV_SUB_VECTOR           38  // (vector, users, subs, ispass, maxplayers)
#define EV_SUB_VECTOR_UPDATE	39  // (vector, count, subs)
#define EV_USER_DATA            40  // (casecorrectedusername)
#define EV_USER_PROFILE         41  // (user, age, bandwidth, location, xbox, gcn, ps2, bio)
#define EV_USER_SUB_VECTOR      42  // (vector, users, subs, ispass, maxplayers, description)
#define EV_VECTOR               43  // (vector, cancreate)
#define EV_VECTOR_DISALLOWED	44  // (vector, reason)
/*speex*/
#define EV_SPEEX                45 // 
#define EV_SPEEX_OFF            46 // (char* user)
#define EV_SPEEX_ON             47 // (char* user)
#define EV_SPEEX_START          48
#define EV_SPEEX_STOP           49
#define EV_SPEEX_RING		50 // (char* user)


/* Postponed events */
#define EV_RESERVED             100
#define EV_SPEEX_CONNECTED      EV_RESERVED + 2
#define EV_SPEEX_DISCONNECTED   EV_RESERVED + 3

/* Event callback */
typedef int KAIEVENT;
typedef void (*callback_t)(KAIEVENT, void*, int);
/* callback dispatcher */
extern callback_t Dispatch;

/* callback init function */
void Init(callback_t callback);
void ReInit(callback_t callback);
void End();
int LibError(char* descr);

/* function prototypes */
void AddContact(char* user);
void AppSpecific(char* question);
void ArenaBan(char* user);
void ArenaBreakStream(char* user); // ban user from your current game
void ArenaKick(char* user);
void ArenaPM(char* user, char* message);
void ArenaStatus(unsigned int status,unsigned int players); // STATUS=={ 1-> Looking for a game | 2-> Hosting | 3-> Dedicated } players==number of players you set
void Attach(); // this function should be in your onEvent under EV_ENGINE_HERE
void Avatar(char* user);
void Chat(char* msg);
void Chatmode(char* chatroom);
void CreateVector(char* password, int maxplayers, char* description);
void Detach();
void Discover(); // this one sends ONE discover message. you need to put this in a loop for consecutive discover's
void GetMetrics();
void GetProfile(char* user);
void GetVectors(char* vector);
void GetState();
void Invite(char* user, char* vector, char *msg);
void Login(char* user, char* password);
void SendClientCapabilities(int ui, int xbox_ui, int console_and_chat, int text_chat, int bot);
void SendPrivateMsg(char* user, char* message);
void RemoveContact(char* user);
void SpecificCount(char* vector);
void Takeover();
void Vector(char* vector, char* password);

int Connect(char* ip); // this is special. if you want to specify an ip to connect (without using discover) call this function with the ip. 
/* Connect() returns:
0 -> if sucessefull
-1 -> blank address or something wrong with the pointer..
-2 -> The specified host is unknown.
-3 -> The requested name is valid but does not have an IP address.
-4 -> A non-recoverable name server error occurred.
-5 -> A temporary error occurred on an authoritative name server.  Try again later. 
-6 -> Error in sendto() */

/* Postponed */
void Speex(char* user, char* data, int len);
void SpeexOff(char* user);
void SpeexOn(char* user);

/* Helper functions */
void GoUp(); // go to the previous arena. if you are allready in the root arena it won't do anything.
void GoArena(char *arena, char *pass); // GoArena("XBox","") -> goes from the current arena, to the XBox subarena. GoArena("/Arena/XBox","") -> the path is from the root, it goes directly to Arena/XBox
void GetCurrentArena(char* arena);
int GetChatmode(); // return 0 if chatmode is disabled, else return 1

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
