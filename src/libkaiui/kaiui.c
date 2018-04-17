/*******************************************
Copyright (c) 2004, Lu�s Miguel Fernandes <zipleen 'at' teamxlink.co.uk>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************/

#include "errors.h"
#ifdef UNIX
#include "linux/netcode.h"
#elif WIN32
#include "win32/netcode.h"
#elif _XBOX
#include "xbox/netcode.h"
#endif

/******************* lists stuff ************************/
/*
 * Don't Need mutex's now.. cause all admins and mods are
 * processed in only one thread, one at a time...
 */
ADM_LIST *admins=NULL;
MOD_LIST *mods=NULL;

ADM_LIST *Search_adm(char *n)
{
	ADM_LIST *ptr_act;

	ptr_act=admins;
	if(ptr_act==NULL)
		return NULL;
	else
	{
		while(strcmp(ptr_act->nick,n) !=0 && ptr_act->next != NULL)
			ptr_act = ptr_act->next;
		return strcmp(ptr_act->nick,n) == 0 ? ptr_act : NULL;
	}
}

MOD_LIST *Search_mods(char *n)
{
	MOD_LIST *ptr_act;

	ptr_act=mods;
	if(ptr_act==NULL)
		return NULL;
	else
	{
		while(strcmp(ptr_act->nick,n) !=0 && ptr_act->next != NULL)
			ptr_act = ptr_act->next;
		return strcmp(ptr_act->nick,n) == 0 ? ptr_act : NULL;
	}
}

MOD_LIST *Insert_mod(char *n)
{
	MOD_LIST *ptr_aux, *ptr_act;

	//EnterCriticalSect(); // think it's stressing xbox build... we don't need this anymore cause we only process this "one packet/request at a time"
					// so we don't have problems about mutex's and threading code...

	ptr_act=Search_mods(n);
	if(ptr_act!=NULL)
  		return ptr_act;

	ptr_aux = (MOD_LIST *)malloc(sizeof(MOD_LIST));
	if(ptr_aux==NULL)
	{
		error_nr=1;
		return NULL;
	}
	ptr_aux->next=mods;
	mods=ptr_aux;
	strcpy(ptr_aux->nick,n);

	return ptr_aux;
}

ADM_LIST *Insert_admin(char *n)
{
	ADM_LIST *ptr_aux, *ptr_act;

	ptr_act=Search_adm(n);
	if(ptr_act!=NULL)
  		return ptr_act;

	ptr_aux = (ADM_LIST *)malloc(sizeof(ADM_LIST));
	if(ptr_aux==NULL)
	{
		error_nr=2;
		return NULL;
	}
	ptr_aux->next=admins;
	admins=ptr_aux;
	strcpy(ptr_aux->nick,n);

	return ptr_aux;
}

void clear_admin()
{
	ADM_LIST *ptr_act,*del;

	if(admins==NULL)
		return;

	ptr_act=admins;
	do
	{
		del=ptr_act->next;
		free(ptr_act);
		ptr_act=del;
	}while(ptr_act!=NULL);
	admins=NULL;
	return;
}

void clear_mods()
{
	ADM_LIST *ptr_act,*del;

	if(mods==NULL)
		return;

	ptr_act=mods;
	do
	{
		del=ptr_act->next;
		free(ptr_act);
		ptr_act=del;
	}while(ptr_act!=NULL);
	mods=NULL;
	return;
}
/**************************** Lists stuff end :P ***********************/

// this one "resolves" a opcode to a number
int parseopcode(char *s)
{  // return codes aren't in the right order cause i've re-ordered this, so it's "faster" (less strcmp()) for the functions that get used more
  if(strcmp(s,"KAI_CLIENT_SPEEX")==0) // we want speex data to arrive as fast as possible :p
    return EV_SPEEX;
  else if(strcmp(s,"KAI_CLIENT_CONTACT_PING")==0) // this one comes second and then the other ping,because its the one thats get used *a lot*
    return EV_CONTACT_PING;
  else if(strcmp(s,"KAI_CLIENT_ARENA_PING")==0)
    return EV_ARENA_CONTACT_PING;
  else if(strcmp(s,"KAI_CLIENT_PM")==0) // chatting....
    return EV_PM;
  else if(strcmp(s,"KAI_CLIENT_CHAT")==0)
    return EV_CHAT;
  else if(strcmp(s,"KAI_CLIENT_JOINS_VECTOR")==0) // more chatting...
    return EV_JOINS_VECTOR;
  else if(strcmp(s,"KAI_CLIENT_LEAVES_VECTOR")==0)
    return EV_LEAVES_VECTOR;
  else if(strcmp(s,"KAI_CLIENT_USER_DATA")==0)
    return EV_USER_DATA;
  else if(strcmp(s,"KAI_CLIENT_VECTOR")==0)
    return EV_VECTOR;
  else if(strcmp(s,"KAI_CLIENT_SUB_VECTOR")==0)
    return EV_SUB_VECTOR;
  else if(strcmp(s,"KAI_CLIENT_LEAVES_CHAT")==0)
    return EV_LEAVES_CHAT;
  else if(strcmp(s,"KAI_CLIENT_JOINS_CHAT")==0)
    return EV_JOINS_CHAT;
  else if(strcmp(s,"KAI_CLIENT_SPEEX_ON")==0)
    return EV_SPEEX_ON;
  else if(strcmp(s,"KAI_CLIENT_SPEEX_OFF")==0)
    return EV_SPEEX_OFF;
  else if(strcmp(s,"KAI_CLIENT_SPEEX_STOP")==0)
    return EV_SPEEX_STOP;
  else if(strcmp(s,"KAI_CLIENT_SPEEX_START")==0)
    return EV_SPEEX_START;
  else if(strcmp(s,"KAI_CLIENT_SPEEX_RING")==0)
    return EV_SPEEX_RING;

  else if(strcmp(s,"KAI_CLIENT_SUB_VECTOR_UPDATE")==0)
    return EV_SUB_VECTOR_UPDATE;
  else if(strcmp(s,"KAI_CLIENT_REMOVE_SUB_VECTOR")==0)
    return EV_REMOVE_SUB_VECTOR;
  else if(strcmp(s,"KAI_CLIENT_USER_SUB_VECTOR")==0)
    return EV_USER_SUB_VECTOR;
  else if(strcmp(s,"KAI_CLIENT_VECTOR_DISALLOWED")==0)
    return EV_VECTOR_DISALLOWED;
  else if(strcmp(s,"KAI_CLIENT_ARENA_STATUS")==0)
    return EV_ARENA_STATUS;
  else if(strcmp(s,"KAI_CLIENT_ARENA_PM")==0)
    return EV_ARENA_PM;
  else if(strcmp(s,"KAI_CLIENT_APP_SPECIFIC")==0)
    return EV_APP_SPECIFIC;
  else if(strcmp(s,"KAI_CLIENT_CHATMODE")==0)
    return EV_CHATMODE;
  else if(strcmp(s,"KAI_CLIENT_INVITE")==0)
    return EV_INVITE;

  else if(strcmp(s,"KAI_CLIENT_CONNECTED_MESSENGER")==0)
    return EV_CONNECTED_MESSENGER;
  else if(strcmp(s,"KAI_CLIENT_CONNECTED_ARENA")==0)
    return EV_CONNECTED_ARENA;
  else if(strcmp(s,"KAI_CLIENT_STATUS")==0)
    return EV_STATUS;
  else if(strcmp(s,"KAI_CLIENT_AUTHENTICATION_FAILED")==0)
    return EV_AUTH_FAILED;
  else if(strcmp(s,"KAI_CLIENT_NOT_LOGGED_IN")==0)
    return EV_NOT_LOGGED_IN;
  else if(strcmp(s,"KAI_CLIENT_DETACH")==0)
    return EV_DETACH;
  else if(strcmp(s,"KAI_CLIENT_ADD_CONTACT")==0)
    return EV_ADD_CONTACT;
  else if(strcmp(s,"KAI_CLIENT_AVATAR")==0)
    return EV_AVATAR;
  else if(strcmp(s,"KAI_CLIENT_CONTACT_OFFLINE")==0)
    return EV_CONTACT_OFFLINE;
  else if(strcmp(s,"KAI_CLIENT_CONTACT_ONLINE")==0)
    return EV_CONTACT_ONLINE;
  else if(strcmp(s,"KAI_CLIENT_REMOVE_CONTACT")==0)
    return EV_REMOVE_CONTACT;
  else if(strcmp(s,"KAI_CLIENT_MODERATOR_PRIVILEGES")==0)
    return EV_MODERATOR_PRIVILEGES;
  else if(strcmp(s,"KAI_CLIENT_ADMIN_PRIVILEGES")==0)
    return EV_ADMIN_PRIVILEGES;
  else if(strcmp(s,"KAI_CLIENT_DHCP_FAILURE")==0)
    return EV_DHCP_FAILURE;
  else if(strcmp(s,"KAI_CLIENT_LOCAL_DEVICE")==0)
    return EV_LOCAL_DEVICE;
  else if(strcmp(s,"KAI_CLIENT_SPECIFIC_COUNT")==0)
    return EV_SPECIFIC_COUNT;
  else if(strcmp(s,"KAI_CLIENT_USER_PROFILE")==0)
    return EV_USER_PROFILE;
  else if(strcmp(s,"KAI_CLIENT_REMOTE_ARENA_DEVICE")==0)
    return EV_REMOTE_ARENA_DEVICE;
  else if(strcmp(s,"KAI_CLIENT_LOGGED_IN")==0)
    return EV_LOGGED_IN;
  else if(strcmp(s,"KAI_CLIENT_METRICS")==0)
    return EV_METRICS;
  else if(strcmp(s,"KAI_CLIENT_ATTACH")==0)
    return EV_ATTACH;
  else if(strcmp(s,"KAI_CLIENT_ENGINE_HERE")==0)
    return EV_ENGINE_HERE;
  else if(strcmp(s,"KAI_CLIENT_ENGINE_IN_USE")==0)
    return EV_ENGINE_IN_USE;

return 0;
}


/* All the func's that the UI will call */
void AddContact(char* user)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(user);
		sprintf(msg,"KAI_CLIENT_ADD_CONTACT;%s;",user);
		SendToEngine(msg);
	}
}

void AppSpecific(char* question)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(question);
		sprintf(msg,"KAI_CLIENT_APP_SPECIFIC;%s;",question);
		SendToEngine(msg);
	}
}

void ArenaBan(char* user)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(user);
		sprintf(msg,"KAI_CLIENT_ARENA_BAN;%s;",user);
		SendToEngine(msg);
	}
}

void ArenaBreakStream(char* user)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(user);
		sprintf(msg,"KAI_CLIENT_ARENA_BREAK_STREAM;%s;",user);
		SendToEngine(msg);
	}
}

void ArenaKick(char* user)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(user);
		sprintf(msg,"KAI_CLIENT_ARENA_KICK;%s;",user);
		SendToEngine(msg);
	}
}

void ArenaPM(char* user, char* message)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(user);
		EscapeUIMsg(message);
		sprintf(msg,"KAI_CLIENT_ARENA_PM;%s;%s;",user,message);
		SendToEngine(msg);
	}
}

void ArenaStatus(unsigned int status,unsigned int players)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		sprintf(msg,"KAI_CLIENT_ARENA_STATUS;%d;%d;",status,players);
		SendToEngine(msg);
	}
}

void Attach()
{
	if(discovered==1 && attached==0 && Logged_in==0)
		SendToEngine("KAI_CLIENT_ATTACH;");
}
void Avatar(char* user)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(user);
		sprintf(msg,"KAI_CLIENT_AVATAR;%s;",user);
		SendToEngine(msg);
	}
}

void Chat(char* msg)
{
	char msg1[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(msg);
		sprintf(msg1,"KAI_CLIENT_CHAT;%s;",msg);
		SendToEngine(msg1);
	}
}

void Chatmode(char* chatroom)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(chatroom);
		sprintf(msg,"KAI_CLIENT_CHATMODE;%s;",chatroom);
		SendToEngine(msg);
	}
}

void CreateVector(char* password, int maxplayers, char* description)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(password);
		EscapeUIMsg(description);
		sprintf(msg,"KAI_CLIENT_CREATE_VECTOR;%d;%s;%s;",maxplayers,description,password);
		SendToEngine(msg);
	}
}

void GetMetrics()
{
	if(Logged_in==1)
	{
		SendToEngine("KAI_CLIENT_GET_METRICS;");
	}
}

void GetProfile(char* user)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(user);
		sprintf(msg,"KAI_CLIENT_GET_PROFILE;%s;",user);
		SendToEngine(msg);
	}
}

void GetVectors(char* vector)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(vector);
		sprintf(msg,"KAI_CLIENT_GET_VECTORS;%s;",vector);
		SendToEngine(msg);
	}
}

void GetState()
{
	SendToEngine("KAI_CLIENT_GETSTATE;");
}

void Invite(char* user, char* vector, char *message)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(user);
		EscapeUIMsg(vector);
		EscapeUIMsg(msg);
		sprintf(msg,"KAI_CLIENT_INVITE;%s;%s;%s;",user,vector,message);
		SendToEngine(msg);
	}
}

void Login(char* user, char* password)
{
	char msg[KAID_STRING_LENGTH];
	EscapeUIMsg(user);
	EscapeUIMsg(password);
	sprintf(msg,"KAI_CLIENT_LOGIN;%s;%s;",user,password);
	SendToEngine(msg);
}

void SendClientCapabilities(int ui, int xbox_ui, int console_and_chat, int text_chat, int bot)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		sprintf(msg,"KAI_CLIENT_CAPS;");
		if(ui==1)
			strcat(msg,"0");
		if(xbox_ui==1)
			strcat(msg,"1");
		if(console_and_chat==1)
			strcat(msg,"2");
		if(text_chat==1)
			strcat(msg,"3");
		if(bot==1)
			strcat(msg,"4");
		strcat(msg,";");
		SendToEngine(msg);
	}
}

void SendPrivateMsg(char* user, char* message)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(user);
		EscapeUIMsg(message);
		sprintf(msg,"KAI_CLIENT_PM;%s;%s;",user,message);
		SendToEngine(msg);
	}
}

void RemoveContact(char* user)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(user);
		sprintf(msg,"KAI_CLIENT_REMOVE_CONTACT;%s;",user);
		SendToEngine(msg);
	}
}

/* if we ever need to redirect the callback function to another function, use this */
void ReInit(callback_t callback)
{
	Dispatch = callback;
}

void SpecificCount(char* vector)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(vector);
		sprintf(msg,"KAI_CLIENT_SPECIFIC_COUNT;%s;",vector);
		SendToEngine(msg);
	}
}

void Takeover()
{
	/* there is a little bug in this.. well not a bug, but a non-sense... if your program crashes, in linux, if you start it up again, it will re-use
	    your previous port.. so after TAKEOVER, the server will send a DETACH to the port where the program crashed... and that's YOUR
	    current port... so you get a DETACH first and then you get an attach and the other stuff.. so we must ignore the first detach... */
	SendToEngine("KAI_CLIENT_TAKEOVER;");
}

void Vector(char* vector, char* password)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(vector);
		sprintf(msg,"KAI_CLIENT_VECTOR;%s;%s;",vector,password);
		SendToEngine(msg);
	}
}
/* Postponed data speex */
void SpeexOff(char* user)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(user);
		sprintf(msg,"KAI_CLIENT_SPEEX;%s;OFF;",user);
		SendToEngine(msg);
	}
}
void SpeexOn(char* user)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(user);
		sprintf(msg,"KAI_CLIENT_SPEEX;%s;ON;",user);
		SendToEngine(msg);
	}
}

/* THIS ISN'T RIGHT AND UNTESTED! char* voice is only a test... */
void Speex(char* user, char* voice, int len)
{
	char msg[KAID_STRING_LENGTH];
	if(Logged_in==1)
	{
		EscapeUIMsg(user);
		sprintf(msg,"KAI_CLIENT_SPEEX;%s;%s",user,voice);
		SendDataToEngine(voice,len);
	}
}

/* below are some helper functions ... */

void GoUp()
{
	char tmp[KAID_STRING_LENGTH];
	int i,j=0;

	if(Logged_in==0)
		return;

	if(current_arena==NULL)
		return;
	strcpy(tmp,current_arena);
	for(i=strlen(tmp);i>0;i--)
	{
		if(tmp[i]=='/')
		{
			tmp[i]=0;
			j=1;
			break;
		}
	}
	if(j==0) return; // you probably are in "Arena" or in "messenger mode" so you can't change "up"
	Vector(tmp,"");
	GetVectors(tmp);
	//if(strcmp(current_chat_room,"")!=0)
	//	Chatmode(tmp);
}

void GetCurrentArena(char *arena)
{
	strcpy(arena, current_arena);
}

int GetChatmode()
{
	if(strcmp(current_chat_room,"")==0)
		return 0;
	else
		return 1;
}

void GoArena(char *arena, char *pass)
{
	char msg[KAID_STRING_LENGTH];

	if(Logged_in==0)
		return;

	if(current_arena==NULL)
		return;
	if(strcmp(current_arena,"")==0)
	{
		Vector("Arena","");
		//mysleep(1);
	}
	EscapeUIMsg(arena);
	if(arena[0]=='/') // we have a absolute path... lets pass it
	{
		arena=&arena[1];
		Vector(arena,pass);
		GetVectors(arena);
		// SEE EV_VECTOR -> we need to change the chatroom ONLY after we received EV_VECTOR, cause we may be changing
		// to a room that we aren't...
		//if(strcmp(current_chat_room,"")!=0)
		//	Chatmode(arena);
	}
	else
	{
		sprintf(msg,"%s/%s",current_arena, arena);
		Vector(msg,pass);
		GetVectors(msg);
		//if(strcmp(current_chat_room,"")!=0)
		//	Chatmode(msg);
	}
}

int LibError(char *descr)
{
	strcpy(descr, error_descriptions[error_nr]);
	return error_nr;
}

void mysleep(int s)
{
	#ifdef UNIX
	sleep(s);
	#elif (WIN32 || _XBOX)
	Sleep(s*1000);
	#endif
}

void EscapeUIMsg(char *msg)
{
	int i=-1; int len=strlen(msg); // -1 because when you enter the cycle you get i=0
	if(len==0) return;
	while((++i)<len)
		if(msg[i]==';') msg[i]=2;
		else if(msg[i]==2) msg[i]=';';
}
