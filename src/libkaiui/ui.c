#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef UNIX
#include <unistd.h>
#elif (WIN32 || _XBOX)
#include <windows.h>
#define sleep Sleep
#endif

#include "kaiui.h"
int go;

// see the events ? you only need to implement the right way you are going to do stuff with this:P
// in here i only print it...
void OnEvent(KAIEVENT id, void* data, int data_size)
{
    switch(id)
    {
		case EV_USER_DATA:
			go=100;
			break;
        case EV_ENGINE_HERE:
        {
		ENGINE_HERE* en;
		en = (ENGINE_HERE*)data;
		go=6;
            	printf("EV_ENGINE_HERE: '%s:%d'\n",en->engine_ip,en->engine_port);
	    	Attach(); // not a simulation ;)
            	break;
        }
	case EV_APP_SPECIFIC:
	{
		APP_SPECIFIC* as;
		as = (APP_SPECIFIC*)data;
		printf("EV_APP_SPECIFIC: question='%s', response='%s'.\n",
			as->question, as->response);
		break;
	}
        case EV_ARENA_STATUS:
        {
            	if(!data) {
			printf("Can't use a null pointer, sorry.\n");
			break;
			}

		ARENA_STATUS* st;
           	st = (ARENA_STATUS*)data;
            	printf("EV_ARENA_STATUS: mode=%d, localusers=%d.\n", st->mode,
                   st->localusers);
            	break;
        }
	case EV_DETACH:
	{
		printf("Houston... I'm shutting down!\n");
		Discover();
		//exit(0);
		break;
	}
	case EV_NOT_LOGGED_IN:
	{
		NOT_LOGGED_IN* st;
        st = (NOT_LOGGED_IN*)data;
		go=6;
        printf("EV_NOT_LOGGED_IN: mode=%s, localusers=%s.\n", st->user, st->pass);
	    break;
	}
	case EV_STATUS:
		printf("EV_STATUS:%s\n",(char*)data);
		break;
	case EV_CONTACT_ONLINE:
		printf("contacto ONLINE:%s\n",(char*)data);
		break;
	case EV_ADD_CONTACT:
		printf("new contact: %s\n",(char*)data);
		break;

	case EV_AVATAR: {
		AVATAR* bb;
		bb = (AVATAR*)data;
		printf("AVATAR: user:%s url: %s\n", bb->user, bb->url);
		break;
		}

	case EV_CONNECTED_ARENA:
	{
		char a[255];
		GetCurrentArena(a);
	 	printf("ARENA MODE. your arena: %s.\n",a);
		break;
	 }
	 case EV_CONNECTED_MESSENGER:
		printf("MESSENGER MODE\n");
		break;
	case EV_CHAT:
	{
		CHAT *ab;
		ab=(CHAT*)data;
		printf("chatroom(%s)|%s:%s\n",ab->chatroom,ab->user,ab->msg);
		break;
	}
	case EV_METRICS:
	{
		METRICS *m;
		m=(METRICS*)data;
		printf("c1:%s,c2:%s,m1:%s,I:%d,m2:%s,m3:%s,m4:%s,m5:%s,m6:%s,I:%d,I:%d,I:%d,I:%d,I:%d,I:%d,m7:%s\n",m->orbname,m->reachable,m->myip,m->myport,m->engine_version,m->engine_platform,m->engine_author,m->adapter,m->locked,m->engine_up,m->engine_down,m->chat_up,m->chat_down,m->orb_up,m->orb_down,m->captech);
		break;
        }
	case EV_JOINS_CHAT:
	{
		JOINS_CHAT *jj;
		jj=(JOINS_CHAT*)data;
		printf("joined %s the user %s. admin?:%d mod?:%d\n",jj->chatroom,jj->user,jj->admin,jj->mod);
		break;
	}
	case EV_LEAVES_CHAT:
	{
		LEAVES_CHAT *jj;
		jj=(LEAVES_CHAT*)data;
		printf("Left %s the user %s.\n",jj->chatroom,jj->user);
		break;
	}
	case EV_MODERATOR_PRIVILEGES:
	{
		MODERATOR_PRIVILEGES *mp;
		mp=(MODERATOR_PRIVILEGES*)data;
		printf("moderators in %s arena are:%s\n",mp->arena,mp->mods);
		break;
	}

	case EV_ENGINE_IN_USE:
	{
		go=6;
		printf("sending takeover\n");
		Takeover(); // for this to work you need to uncomment exit() in EV_DETACH :P
		break;
	}

	case EV_ARENA_CONTACT_PING:
	{
		ARENA_CONTACT_PING *acp;
		acp=(ARENA_CONTACT_PING*)data;
		printf("ping from %s with:%d | caps: ui:%d xbox_ui:%d voice:%d text:%d bot:%d\n",acp->user,acp->ping,acp->ui,acp->xbox_ui,acp->console_and_voice,acp->text_chat,acp->bot);
		break;
	}
	case EV_CONTACT_PING:
	{
		CONTACT_PING *acp;
		acp=(CONTACT_PING*)data;
		printf("CONTACT ping from %s with:%d | caps: ui:%d xbox_ui:%d voice:%d text:%d bot:%d\n",acp->user,acp->ping,acp->ui,acp->xbox_ui,acp->console_and_voice,acp->text_chat,acp->bot);
		break;
	}
	case EV_USER_PROFILE:
	{
		USER_PROFILE *pp;
		pp=(USER_PROFILE*)data;
		printf("user: %s, consoles:%d\n",pp->name,pp->consoles);
		break;
	}
	default:
        {
		//printf("Got event #%d, and data is %d byte(s) long.\n", id,data_size);
      		break;
        }
    }
}

int main(void)
{
	char h[256];
	Init(OnEvent); // Set libkaiui callback

	printf("HI! make sure you have kaiD or win32 kai engine running :D i'll wait for you..\n");

	for(go=1;go<5;go++)
	{
		Discover();
		#ifdef UNIX
		sleep(1);
		#elif (WIN32 || _XBOX)
		Sleep(500);
		#endif
	}
	if(go==5)
	{
		printf("discover wasn't good... specify ip:");
		  do{
			fgets(h,256,stdin);
		}while(h[0]=='\n');
		h[(strlen(h))-1]=0;
		Connect(h);
	}
	#ifdef UNIX
	sleep(1);
	#elif (WIN32 || _XBOX)
	Sleep(500);
	#endif
	if(go<6)
	{
		printf("the ip didn't answered... i'm exiting..\n");
		exit(0);
	}
	// very ugly bad way of doing this, but i'm only testing it :P
	// we only want to process commands after we've logged in
	while(go<99){

		#ifdef UNIX
		sleep(1);
		#elif (WIN32 || _XBOX)
		Sleep(1000);
		#endif

	};
//for(;;) sleep(15);
	//Chatmode("General Chat"); // you now enter the general chat mode :P
	//sleep(100); // lets wait there for 10 seconds :P

	GoArena("/Arena",""); // you now are in Arena/XBox :D
//Chat("/orb summon ^S^");
for(;;)	sleep(15); // 15 seconds there ? :P

	GetProfile("^S^");

	GetMetrics();

	sleep(5);

	GetCurrentArena(h);
	printf("current arena:%s\n",h); // your current arena

	GoUp(); // you should now be in "arena"
	sleep(7);
	GetCurrentArena(h);
	printf("current arena:%s\n",h);

	sleep(10); // more 10 seconds and then bye bye :D

	End();

    return 0;

}
