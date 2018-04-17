/*******************************************
Copyright (c) 2004, Luís Miguel Fernandes <zipleen 'at' teamxlink.co.uk>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************/

#ifdef UNIX
#include "linux/netcode.h"
#elif WIN32
#include "win32\netcode.h"
#elif _XBOX
#include "xbox\netcode.h"
#endif

char nickname[MAX_CHAR]; // our nick... stored because of EV_USER_DATA            36  // (casecorrectedusername)
char current_chat_room[KAID_STRING_LENGTH]; // we need this global because ArenaHelper functions need this info...
char current_arena[KAID_STRING_LENGTH];
int Logged_in; // logged in state. currently we only need 0=not logged 1=logged
int attached;
int discovered;

void parseDisc(char *msg_recv)
{
	char *opcode;
	opcode=strtok(msg_recv,";");
	if(opcode==NULL)
		return;
	if(parseopcode(opcode) == EV_ENGINE_HERE)
	{
		if(discovered==0) // only 1 time only.. the first engine_here is OUR engine now
			GoDiscovered();
	}
}

/* This function parses all the EV_ , while the other receives them from the socket and passes the message to this function... */
void parsePackets(char *msg_recv)
{
	char *opcode,msg[KAID_STRING_LENGTH];
	char c1[KAID_STRING_LENGTH],c2[KAID_STRING_LENGTH]; // we need some char's so we can pass pointers for the structs :P
	
	opcode=strtok(msg_recv,";");
	if(opcode==NULL)
		return;
	switch(parseopcode(opcode))
	{
		case EV_ENGINE_HERE:
		{
			if(discovered==0) // only 1 time only.. the first engine_here is OUR engine now
				GoDiscovered();
			break;
		}
		case EV_ENGINE_IN_USE:
		{
			if(discovered==1 && attached==0) // if we are attached this shouldn't be here...
				GoEngineInUse();
			break;
		}
		case EV_ATTACH: // KAI_CLIENT_ATTACH
		{
			if(discovered==1 && attached==0)
				GoAttach();
			break;
		}
		
		case EV_AUTH_FAILED:
			Dispatch(EV_AUTH_FAILED, NULL, 0);
			break;
		case EV_NOT_LOGGED_IN:  // NOT_LOGGED_IN
		{
			strcpy(c1, "");
			strcpy(c2, "");
			//lets see if we have auto-login enabled...
			opcode=strtok(NULL,";"); // now we have the login...
			if(opcode)
				strcpy(c1, opcode);
		
			opcode=strtok(NULL,";"); // now we have the password...
			if(opcode) 
				strcpy(c2, opcode);
		
			opcode = strtok(NULL,";"); // if 1 auto-login, if 0 -> manual login
		
			int login_mode = 0;
			if(opcode)
				login_mode = atoi(opcode);
		
			if(login_mode == 1) // auto login
				{
				sprintf(msg, "KAI_CLIENT_LOGIN;%s;%s;", c1, c2); 
				SendToEngine(msg);
				break;
				}
			NOT_LOGGED_IN nli = {c1, c2};
			Dispatch(EV_NOT_LOGGED_IN, &nli, sizeof(nli));
			break;
		}
		
		case EV_STATUS: // "KAI_CLIENT_STATUS"
			opcode=strtok(NULL,";");
			strcpy(c1,opcode);
			Dispatch(EV_STATUS, &c1, strlen(c1));
		break;
	
		case EV_DETACH: // "KAI_CLIENT_DETACH"
			Logged_in=0;
			attached=0;
			Dispatch(EV_DETACH, NULL, 0);
		break;
	
		case EV_ADD_CONTACT: // KAI_CLIENT_ADD_CONTACT
			opcode=strtok(NULL,";");
			Dispatch(EV_ADD_CONTACT,opcode , strlen(opcode));
			break;
	
		case EV_AVATAR: // avatar
		{
			opcode=strtok(NULL,";"); // user
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			
			opcode=strtok(NULL,";"); // this is the avatar url...
			if(opcode==NULL)
				return;
			strcpy(c2,opcode);
			AVATAR av = { c1, c2 };
			Dispatch(EV_AVATAR, &av, sizeof(av));	      
			break;
		}
		case EV_CONTACT_OFFLINE: // User offline  KAI_CLIENT_CONTACT_OFFLINE
			opcode=strtok(NULL,";"); // user
			strcpy(c1,opcode);
			Dispatch(EV_CONTACT_OFFLINE, c1, strlen(c1));
			break;
	
		case EV_CONTACT_ONLINE: // "KAI_CLIENT_CONTACT_ONLINE"
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			// new user in contact field, lets get the avatar :P doesn't get used but hey!whatever :P
			Avatar(c1);
			Dispatch(EV_CONTACT_ONLINE, c1, strlen(c1));
			break;
	
		case EV_CONTACT_PING: // "KAI_CLIENT_CONTACT_PING
		{
			int s1,i;
			char m1[KAID_STRING_LENGTH];
			
			opcode=strtok(NULL,";"); // username
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			opcode=strtok(NULL,";"); // state
			if(atoi(opcode)==0) // means that the user is in an arena... lets update it
				{
					strcpy(c2,opcode);
					opcode=strtok(NULL,";"); // ping
					s1=atoi(opcode);
				}
			else
				{
					s1=atoi(opcode);
					strcpy(c2,""); // messenger MODE
				}
			CONTACT_PING cp = { c1, c2, s1 };
			
			opcode=strtok(NULL,";");
			if(opcode==NULL) // we don't have caps enabled for this client yet...
			{
				cp.ui=0;
				cp.xbox_ui=0;
				cp.console_and_voice=0;
				cp.text_chat=0;
				cp.bot=0;
				strcpy(m1,"");
				cp.ui_caps=m1;
			}
			else
			{
				strcpy(m1,opcode);
				cp.ui_caps=m1;
				
				cp.ui=0;
				cp.xbox_ui=0;
				cp.console_and_voice=0;
				cp.text_chat=0;
				cp.bot=0;
				
				//lets see what we have..
				i=-1;
				while(opcode[++i]!=0)
				{
					switch(opcode[i])
					{
						case '0':
							cp.ui=1;
							break;
						case '1':
							cp.xbox_ui=1;
							break;
						case '2':
							cp.console_and_voice=1;
							break;
						case '3':
							cp.text_chat=1;
							break;
						case '4':
							cp.bot=1;
							break;
					}
				}
			}
			Dispatch(EV_CONTACT_PING, &cp, sizeof(cp));
			break;
		}
	
		case EV_PM: // "KAI_CLIENT_PM"
		{
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			opcode=strtok(NULL,";"); // message
			if(opcode==NULL)
				return;
			strcpy(c2,opcode);
			EscapeUIMsg(c2);
			PM pp = { c1 , c2 };
			Dispatch(EV_PM , &pp, sizeof(pp));	
			break;
		}
	
		case EV_CHAT: // "KAI_CLIENT_CHAT" EV_CHAT
		{
			
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			if(strcmp(opcode,current_chat_room)==0)
			{
				opcode=strtok(NULL,";");
				strcpy(c1,opcode);
				opcode=strtok(NULL,";");
				strcpy(c2,opcode);
				EscapeUIMsg(c2);
				CHAT ccc = { current_chat_room, c1 , c2 };
				Dispatch(EV_CHAT, &ccc, sizeof(ccc));
			}
			break;
		}
	
		case EV_REMOVE_CONTACT: //"KAI_CLIENT_REMOVE_CONTACT"
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			Dispatch(EV_REMOVE_CONTACT, c1,strlen(c1));
			break;
	
		case EV_JOINS_VECTOR: //"KAI_CLIENT_JOINS_VECTOR" aka arena join, someone entered on our current arena... that DOESN'T MEAN HE IS AVAILABLE to CHAT
			
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			Dispatch(EV_JOINS_VECTOR, c1, strlen(c1));
			break;
	
		case EV_LEAVES_VECTOR: // "KAI_CLIENT_LEAVES_VECTOR"  -- significa q saiu da arena
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			Dispatch(EV_LEAVES_VECTOR, c1, strlen(c1));
			break;
	
		case EV_USER_DATA: // KAI_CLIENT_USER_DATA - our nick
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(nickname,opcode);
			Logged_in=1;
			Dispatch(EV_USER_DATA,nickname, strlen(nickname));
			break;
	
		case EV_VECTOR: // KAI_CLIENT_VECTOR;vector;cancreate; (join arenas)  | EV_VECTOR
		{
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			if( opcode[0]=='0' || opcode[0]=='1' ) // we aren't in arena mode...
				strcpy(c1,"");
			else
				strcpy(c1,opcode);
			VECTOR v = { c1 } ;
			strcpy(current_arena,c1);
			opcode=strtok(NULL,";");
			if(opcode==NULL) 
				v.cancreate=0;
			else
				v.cancreate=atoi(opcode);
			
			// we need to go into CHATMODE in here! we only know if we changed to the correct arena if we receive this...
			if(strcmp(current_chat_room,"")!=0)
			{
				if(strcmp(current_arena,"")==0) // we are in general chat then...
					Chatmode("General Chat");
				else
					Chatmode(current_arena);
			}
			Dispatch(EV_VECTOR, &v, sizeof(v));
			break;
		}
	
		case EV_MODERATOR_PRIVILEGES: // KAI_CLIENT_MODERATOR_PRIVILEGES;;  32 chars 
		{	
			char *op;
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			if(msg_recv[32]!=';') // this means that the mods aren't global... we need this because of strtok() 
			{
				strcpy(c1,opcode); // arena
				opcode=strtok(NULL,";"); // if mods aren't global we need an aditional strtok()
			}
			else strcpy(c1,"");
			
			strcpy(c2,opcode); // list of mod users
			op=strtok(opcode,"/");
			if(opcode==NULL)
				return;

			clear_mods();
			while( op != NULL )
			{
				Insert_mod(op);
				op=strtok(NULL,"/");
			} 
			MODERATOR_PRIVILEGES mp = { c1 , c2 };
			Dispatch(EV_MODERATOR_PRIVILEGES, &mp, sizeof(mp));
			break;
		}
		
		case EV_SUB_VECTOR: // 'KAI_CLIENT_SUB_VECTOR;vector;users;subs;ispass;maxplayer - contacts of an arena
		{
			int users, subs, ispass, maxplayers;
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode); // chat channel name

			opcode=strtok(NULL,";"); // users
			if(opcode==NULL)
				return;
			users=atoi(opcode);

			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			subs=atoi(opcode);
			
			opcode=strtok(NULL,";"); // password ?
			if(opcode==NULL)
				return;
			ispass=atoi(opcode);
			
			opcode=strtok(NULL,";"); // max_players
			if(opcode==NULL)
				return;
			maxplayers=atoi(opcode);
			
			SUB_VECTOR sv = { c1 , users , subs , ispass , maxplayers };
			Dispatch(EV_SUB_VECTOR, &sv, sizeof(sv));
			break;
		}
	
		case EV_REMOVE_SUB_VECTOR: // KAI_CLIENT_REMOVE_SUB_VECTOR
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			Dispatch(EV_REMOVE_SUB_VECTOR,c1,strlen(c1)); 
			break;
	
		case EV_SUB_VECTOR_UPDATE:  // KAI_CLIENT_SUB_VECTOR_UPDATE -- mudou atributo canal EV_SUB_VECTOR_UPDATE
		{
			int users, subs;
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode); // chat channel for update
			
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			users=atoi(opcode);
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			subs=atoi(opcode); 
			VECTOR_UPDATE vu = { c1 ,  users , subs };
			Dispatch(EV_SUB_VECTOR_UPDATE, &vu, sizeof(vu));
			break;
		}
	
		case EV_USER_SUB_VECTOR: // KAI_CLIENT_USER_SUB_VECTOR" 
		{
			int users,subs,ispass,maxplayers;
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c2,opcode);
			
			opcode=strtok(NULL,";"); //users
			if(opcode==NULL)
				return;
			users=atoi(opcode); 

			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			subs=atoi(opcode);

			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			ispass=atoi(opcode);

			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			maxplayers=atoi(opcode);

			opcode=strtok(NULL,";");
			if(opcode!=NULL)
				strcpy(c1,opcode);
			else
				strcpy(c1,"");
			EscapeUIMsg(c1);
			USER_SUB_VECTOR usv = { c2 , users , subs , ispass , maxplayers , c1};
			Dispatch(EV_USER_SUB_VECTOR, &usv, sizeof(usv));
			break;
		}
	
		case EV_VECTOR_DISALLOWED: // KAI_CLIENT_VECTOR_DISALLOWED
		{
			
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c2,opcode);
			VECTOR_DISALLOWED vd = { c1 , c2 };
			Dispatch(EV_VECTOR_DISALLOWED, &vd, sizeof(vd));
			break;
		}
	
		case EV_ARENA_STATUS: // KAI_CLIENT_ARENA_STATUS ARENA_STATUS
		{
			ARENA_STATUS as;
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			as.mode=atoi(opcode);
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			as.localusers=atoi(opcode);
			Dispatch(EV_ARENA_STATUS, &as, sizeof(as));
			break;
		}
		
		case EV_CONNECTED_MESSENGER: // MESSENGER CONNECTED
			Dispatch(EV_CONNECTED_MESSENGER, NULL, 0);
			break;
		
		case EV_CONNECTED_ARENA: // ARENA MODE
			Dispatch(EV_CONNECTED_ARENA, NULL, 0);
			break;
	
		case EV_ARENA_PM: // KAI_CLIENT_ARENA_PM
		{
			opcode=strtok(NULL,";"); // user
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			opcode=strtok(NULL,";"); // msg
			if(opcode==NULL)
				return;
			strcpy(c2,opcode);
			EscapeUIMsg(c2);
			ARENA_PM ap = { c1 , c2 };
			Dispatch(EV_ARENA_PM, &ap, sizeof(ap));
			break;
		}
	
		case EV_APP_SPECIFIC:  // KAI_CLIENT_APP_SPECIFIC
		{
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			EscapeUIMsg(c1);
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c2,opcode);
			EscapeUIMsg(c2);
			APP_SPECIFIC asp = { c1 , c2 };
			Dispatch(EV_APP_SPECIFIC, &asp, sizeof(asp));
			break;
		}
		case EV_CHATMODE: // KAI_CLIENT_CHATMODE;roomanme
		{
			opcode=strtok(NULL,";");
			if(opcode!=NULL) // if != -> messenger mode... ignore the rest
				{
					strcpy(current_chat_room,opcode);
					Dispatch(EV_CHATMODE, current_chat_room, strlen(current_chat_room));
					break;
				}
			strcpy(current_chat_room,"\0");
			Dispatch(EV_CHATMODE, current_chat_room, strlen(current_chat_room));
			break;
		}
		case EV_LEAVES_CHAT:  // KAI_CLIENT_LEAVES_CHAT
		{
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			if(strcmp(opcode,current_chat_room)==0)
			{
				opcode=strtok(NULL,";");
				if(opcode==NULL)
					return;
				strcpy(c1,opcode);
				LEAVES_CHAT lc = { current_chat_room , c1 };
				Dispatch(EV_LEAVES_CHAT, &lc, sizeof(lc));
				break;
			}
			break;
		}
	
		case EV_JOINS_CHAT: // KAI_CLIENT_JOINS_CHAT  -- significa que já pode falar 
		{
			int admin=0, mod=0;
			MOD_LIST *m;
			ADM_LIST *p;
			
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			if(strcmp(opcode,current_chat_room)==0)
			{
				opcode=strtok(NULL,";");
				if(opcode==NULL)
					return;
				strcpy(c1,opcode);
				p=Search_adm(c1);
				if(p!=NULL)
					admin=1;
				m=Search_mods(c1);
				if(m!=NULL)
					mod=1;

				JOINS_CHAT jc = { current_chat_room , c1 , admin , mod};
				Dispatch(EV_JOINS_CHAT, &jc, sizeof(jc));
				break;
			}
			break;
		}
	
		case EV_ARENA_CONTACT_PING: // KAI_CLIENT_ARENA_PING
		{
			ARENA_CONTACT_PING acp;
			char m1[KAID_STRING_LENGTH];
			
			opcode=strtok(NULL,";"); // nick
			if(opcode==NULL)
				return;
			strcpy(c1,opcode); // the nick
			acp.user=c1;
			
			opcode=strtok(NULL,";"); // THA PING
			if(opcode==NULL)
				return;
			acp.ping=atoi(opcode);

			opcode=strtok(NULL,";"); // first int
			if(opcode==NULL)
				return;
			acp.status=atoi(opcode); 
			opcode=strtok(NULL,";"); // second int
			if(opcode==NULL)
				return;
			acp.players=atoi(opcode);
			
			opcode=strtok(NULL,";");
			if(opcode==NULL) // we don't have caps enabled for this client yet...
			{
				acp.ui=0;
				acp.xbox_ui=0;
				acp.console_and_voice=0;
				acp.text_chat=0;
				acp.bot=0;
				strcpy(m1,"");
				acp.ui_caps=m1;
			}
			else
			{
				strcpy(m1,opcode);
				acp.ui_caps=m1;
				
				acp.ui=0;
				acp.xbox_ui=0;
				acp.console_and_voice=0;
				acp.text_chat=0;
				acp.bot=0;
				
				//lets see what we have..
				int i=-1;
				while(opcode[++i]!=0)
				{
					switch(opcode[i])
					{
						case '0':
							acp.ui=1;
							break;
						case '1':
							acp.xbox_ui=1;
							break;
						case '2':
							acp.console_and_voice=1;
							break;
						case '3':
							acp.text_chat=1;
							break;
						case '4':
							acp.bot=1;
							break;
					}
				}
			}
			Dispatch(EV_ARENA_CONTACT_PING, &acp, sizeof(acp));

			break;
		}
		
		case EV_ADMIN_PRIVILEGES: // KAI_CLIENT_ADMIN_PRIVILEGES  -- vamos usar uma string separada por "/" ou temos de fazer uma lista ligada?
		{
			char *op;
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			op=strtok(opcode,"/");
			clear_admin();
			while( op!= NULL)
			{
				Insert_admin(op);
				op=strtok(NULL,"/");
			}
			//Dispatch(EV_ADMIN_PRIVILEGES, c1, strlen(c1));
			break;
		}
		
		case EV_DHCP_FAILURE: // KAI_CLIENT_DHCP_FAILURE  -- implemented but not used yet in kaiD
		{
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			Dispatch(EV_DHCP_FAILURE, c1, strlen(c1));
			break;
		}
		
		case EV_LOCAL_DEVICE: //KAI_CLIENT_LOCAL_DEVICE
		{
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			Dispatch(EV_LOCAL_DEVICE, c1, strlen(c1));
			break;
		}
		
		case EV_SPECIFIC_COUNT: // KAI_CLIENT_SPECIFIC_COUNT
		{
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			
			SPECIFIC_COUNT sc = { c1 } ;
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			sc.user_count=atoi(opcode);
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			sc.private_vector_count=atoi(opcode);
			Dispatch(EV_SPECIFIC_COUNT, &sc, sizeof(sc));
			break;
		}
		
		case EV_USER_PROFILE: // KAI_CLIENT_USER_PROFILE
		{
			USER_PROFILE up;
			char loc[KAID_STRING_LENGTH], bio[KAID_STRING_LENGTH],m1[KAID_STRING_LENGTH];
			
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode); // user
			up.name=c1;
			
			opcode=strtok(NULL,";"); // age
			if(opcode==NULL)
				return;
			up.age=atoi(opcode);
	
			opcode=strtok(NULL,";"); // bandwidth
			if(opcode==NULL)
				return;
			strcpy(c2,opcode);
			EscapeUIMsg(c2);
			up.bandwidth=c2;
			
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(loc,opcode);
			EscapeUIMsg(loc);
			up.location=loc;
			
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			up.consoles=0;
			strcpy(m1,opcode);
			if(strcmp(m1,"Yes")==0)
				up.consoles |= HAVE_XBOX; // 1 = xbox
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(m1,opcode);
			if(strcmp(m1,"Yes")==0)
				up.consoles |= HAVE_PS2;; // 2 = ps | 1+2=3 xbox e ps
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(m1,opcode);
			if(strcmp(m1,"Yes")==0)
				up.consoles |= HAVE_GC; // 4 = gc | 1+2+4=7 xbox e ps e gc | 2+4 = 6 ps e gc | 1+4 = 5 xbox e gc
			
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(bio,opcode);
			EscapeUIMsg(bio);
			up.bio=bio;
			Dispatch(EV_USER_PROFILE, &up, sizeof(up));
			break;
		}
		
		case EV_REMOTE_ARENA_DEVICE: // KAI_CLIENT_REMOVE_ARENA_DEVICE;user;device
		{
			
			opcode=strtok(NULL,";"); // user
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			opcode=strtok(NULL,";"); // device
			if(opcode==NULL)
				return;
			strcpy(c2,opcode);
			REMOTE_ARENA_DEVICE rad = { c1 , c2 };
			Dispatch(EV_REMOTE_ARENA_DEVICE, &rad, sizeof(rad));
			break;
		}
		
		case EV_LOGGED_IN: // KAI_CLIENT_LOGGED_IN
			Logged_in=1;
			Dispatch(EV_LOGGED_IN, NULL,0);
			break;
			
		case EV_METRICS:// KAI_CLIENT_METRICS
		{
			char m1[KAID_STRING_LENGTH],m2[KAID_STRING_LENGTH],m3[KAID_STRING_LENGTH],m4[KAID_STRING_LENGTH];
			char m5[KAID_STRING_LENGTH],m6[KAID_STRING_LENGTH],m7[KAID_STRING_LENGTH];
			METRICS m;
			
			/* acording to ^S^ ... and i'm going to follow this order blindlessly.. dont know what to expect in here */
			opcode=strtok(NULL,";"); // kai core
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			m.orbname=c1;
			
			opcode=strtok(NULL,";"); // yes?!
			if(opcode==NULL)
				return;
			strcpy(c2,opcode);
			m.reachable=c2;
			
			opcode=strtok(NULL,";"); // ip
			if(opcode==NULL)
				return;
			strcpy(m1,opcode);
			m.myip=m1;
			
			opcode=strtok(NULL,";"); //porta
			if(opcode==NULL)
				return;
			m.myport=atoi(opcode);
			
			opcode=strtok(NULL,";"); // versao
			if(opcode==NULL)
				return;
			strcpy(m2,opcode);
			m.engine_version=m2;
			
			opcode=strtok(NULL,";"); // sistema
			if(opcode==NULL)
				return;
			strcpy(m3,opcode);
			m.engine_platform=m3;
			
			opcode=strtok(NULL,";"); // gajo q fez :D
			if(opcode==NULL)
				return;
			strcpy(m4,opcode);
			m.engine_author=m4;
			
			opcode=strtok(NULL,";"); // device
			if(opcode==NULL)
				return;
			strcpy(m5,opcode);
			m.adapter=m5;
			
			opcode=strtok(NULL,";"); // locked
			if(opcode==NULL)
				return;
			strcpy(m6,opcode);
			m.locked=m6;
			
			opcode=strtok(NULL,";"); // engine up
			if(opcode==NULL)
				return;
			m.engine_up=atoi(opcode);
			
			opcode=strtok(NULL,";"); // engine down
			if(opcode==NULL)
				return;
			m.engine_down=atoi(opcode);
			
			opcode=strtok(NULL,";"); // chat up
			if(opcode==NULL)
				return;
			m.chat_up=atoi(opcode);
			
			opcode=strtok(NULL,";"); // chat down
			if(opcode==NULL)
				return;
			m.chat_down=atoi(opcode);
			
			opcode=strtok(NULL,";"); // orb up
			if(opcode==NULL)
				return;
			m.orb_up=atoi(opcode);
			
			opcode=strtok(NULL,";"); // orb down
			if(opcode==NULL)
				return;
			m.orb_down=atoi(opcode);
			
			opcode=strtok(NULL,";"); //technology
			if(opcode==NULL)
				return;
			strcpy(m7, opcode);
			m.captech=m7;
			
			Dispatch(EV_METRICS,&m,sizeof(m));
			break;
		}

		case EV_INVITE: // KAI_CLIENT_INVITE
		{
			char m1[KAID_STRING_LENGTH],m2[KAID_STRING_LENGTH];
			opcode=strtok(NULL,";"); // user
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			
			opcode=strtok(NULL,";"); // vector
			if(opcode==NULL)
				return;
			strcpy(c2,opcode);
			
			opcode=strtok(NULL,";"); // time
			if(opcode==NULL)
				return;
			strcpy(m1,opcode);
			
			opcode=strtok(NULL,";"); // message
			if(opcode!=NULL)
				strcpy(m2,opcode);
			else
				strcpy(m2,"");
			INVITE in = { c1 , c2 , m1 , m2 };
			Dispatch(EV_INVITE, &in, sizeof(in));
			break;
		}
		
		case EV_SPEEX_ON:
		{
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			Dispatch(EV_SPEEX_ON, &c1, strlen(c1));
		}

		case EV_SPEEX_OFF:
		{
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			Dispatch(EV_SPEEX_OFF, &c1, strlen(c1));
		}
						
		case EV_SPEEX_STOP:
			Dispatch(EV_SPEEX_STOP, NULL, 0);

		case EV_SPEEX_START:
			Dispatch(EV_SPEEX_START, NULL, 0);
		
		case EV_SPEEX_RING:
		{
			opcode=strtok(NULL,";");
			if(opcode==NULL)
				return;
			strcpy(c1,opcode);
			Dispatch(EV_SPEEX_RING, &c1, strlen(c1));
		}
		
		case EV_SPEEX:
		{
			// NOT CODED!!
			break;
		}
		default:
			#ifdef DEBUG_SOCK
			printf("Whats this?!?: %s\n",msg_recv);
			#endif
			break;
	}
}

