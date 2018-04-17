/*******************************************
Copyright (c) 2004, Luís Miguel Fernandes <zipleen 'at' teamxlink.co.uk>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************/

/* Netcode.c for linux and *nix based OS - Networking and threads */
#include "netcode.h"
extern int error_nr;
pthread_t bcreceive;
pthread_mutex_t mutex;
int sock; // my socket :D
struct sockaddr_in server,rcv; // server is where we send data to, rcv is where data is received
/* Dispatcher func */
callback_t Dispatch;

// broadcast discover... helping in attaching.... only ONE broadcast is sent. don't flood the server too much...
void Discover()
{
	struct sockaddr_in bc;
	int adl=sizeof(server),n;
	struct ifconf ifc;
	struct ifreq *ifr;
	char buf[BUFSIZ];
	int did_something;

	// first comes 127.0.0.1 ...
	bc.sin_family=AF_INET;
	bc.sin_addr.s_addr=inet_addr("127.0.0.1");
	bc.sin_port=htons(KAI_PORT);
	if(-1==sendto(sock,"KAI_CLIENT_DISCOVER;",strlen("KAI_CLIENT_DISCOVER;"),0,(struct sockaddr *)&bc, adl))
	{
		#ifdef DEBUG_SOCK
		printf("Error in sending KAI_CLIENT_DISCOVER to 127.0.0.1\n");
		#endif
		error_nr=6;
	}
	pthread_testcancel();	
	did_something=0; // will explain few lines later what this is..
	
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if (ioctl(sock,SIOCGIFCONF, (char *) &ifc) < 0)
	{
		error_nr=11;
		return;
	}
	ifr=ifc.ifc_req;
	
	for(n=ifc.ifc_len/sizeof(struct ifreq); --n >=0; ifr++)
	{
		if(ifr->ifr_addr.sa_family!=AF_INET) // we only want to search for adapters in the AF_INET family....
			continue;
		if(ioctl(sock,SIOCGIFFLAGS, (char *) ifr)<0)
			error_nr=12;

		if((ifr->ifr_flags & IFF_UP) == 0 ||
			(ifr->ifr_flags & IFF_LOOPBACK ) ||
			(ifr->ifr_flags & (IFF_BROADCAST | IFF_POINTOPOINT)) == 0)
			continue; // lets ditch the ones that we don't want... loopback, down interfaces, network adapters that don't have BC

		if(ifr->ifr_flags & IFF_BROADCAST) // if we have broadcast set...
		{
			if(ioctl(sock, SIOCGIFBRDADDR, (char *) ifr)<0)
			{
				error_nr=13;
				continue;
			}
			did_something++;
			memcpy((char *) &bc, (char *) &ifr->ifr_broadaddr, sizeof(ifr->ifr_broadaddr));
			bc.sin_port=htons(KAI_PORT);
			#ifdef DEBUG_SOCK
			printf("Sending DISCOVER to %s\n",inet_ntoa(bc.sin_addr));
			#endif
			if(-1==sendto(sock,"KAI_CLIENT_DISCOVER;",strlen("KAI_CLIENT_DISCOVER;"),0,(struct sockaddr *)&bc, adl))
			{	
				#ifdef DEBUG_SOCK
				printf("ERROR sending DISCOVER to %s\n",inet_ntoa(bc.sin_addr));
				#endif
				error_nr=6;
			}
		}
	}
	// well there is a problem with macOS and freebsd(?) with the "new" broadcast finding address method.. so we need a fallback
	// if we send something, then "did_something" will be !=0 so we will never enter the cycle, else we'll send the broadcast
	// for the INADDR_BROADCAST that is set from the OS.
	if(did_something==0)
	{
		bc.sin_family=AF_INET;
		bc.sin_addr.s_addr=INADDR_BROADCAST;
		bc.sin_port=htons(KAI_PORT);
		#ifdef DEBUG_SOCK
			printf("Couldn't find valid BC interfaces, sending to default (255.255.255.255)\n");
		#endif
		if(-1==sendto(sock,"KAI_CLIENT_DISCOVER;",strlen("KAI_CLIENT_DISCOVER;"),0,(struct sockaddr *)&bc, adl))
		{
			#ifdef DEBUG_SOCK
				printf("ERROR sending DISCOVER to %s\n",inet_ntoa(bc.sin_addr));
			#endif
			error_nr=6;
		}
	}			

}

/* Receives from the network the packages and passes them to "parsePackets" for them to be handled */
void *ReceivingPackets(void *ptr1)
{
	char *op;
	int adl=sizeof(server),i,ready=0;
	char msg_recv[KAID_STRING_LENGTH];
	fd_set fds;//struct timeval tt;
	
	for(;;)
	{	
		FD_ZERO(&fds);FD_SET(sock,&fds);
		//tt.tv_sec=0;tt.tv_usec=50000;
		ready=select(sock+1,&fds,NULL,NULL,NULL);
		if(ready<0) 
		{
			error_nr=11;
			continue;
		}
		else if(ready && FD_ISSET(sock,&fds))
		{
			if(discovered==0) // only for discover
			{
				i=recvfrom(sock,msg_recv,sizeof(msg_recv)-1,0,(struct sockaddr *)&rcv,(socklen_t *)&adl);
				if(i>0)
					msg_recv[i]=0;
				#ifdef DEBUG_SOCK
				printf("ENGINE HERE EXPECTED: %s\n",msg_recv);
				#endif
				op=strtok(msg_recv,";");
				if(op==NULL)
					continue;
				if(parseopcode(op) == EV_ENGINE_HERE)
						GoDiscovered();
			}
			else // other one's (this has an aditional check if the message came from *our* attached engine
			{
			
				EnterCriticalSect(); // just to be sure there's no weird errors, although socks are full duplex...
				i=recvfrom(sock,msg_recv,sizeof(msg_recv)-1,0,(struct sockaddr *)&rcv,(socklen_t *)&adl);
				LeaveCriticalSect();
				// lets discharge the packet if it's not from "our" attached server.
				if( (rcv.sin_addr.s_addr != server.sin_addr.s_addr) && (rcv.sin_port != server.sin_port)) continue;
				if(i<=0) continue;
				else if(msg_recv[0]=='K') // someone could be sending some trash... with this little if we can "filter" only packets that start with K..(an additional check to not crash strtok() in some weird 'i-dont-know' way..
					{
						msg_recv[i]=0;
						#ifdef DEBUG_SOCK
							printf("Receiving(%s:%d):%s\n",inet_ntoa(rcv.sin_addr),ntohs(rcv.sin_port),msg_recv);fflush(stdout);
						#endif
						parsePackets(msg_recv);
					}
			}
		}
		pthread_testcancel();
	}
}

/* SendToEngine(msg) */
void SendToEngine(char *msg)
{
	int adl=sizeof(server);
	#ifdef DEBUG_SOCK
		printf("SendingToServer(%s:%d):%s\n",inet_ntoa(server.sin_addr),ntohs(server.sin_port),msg);
	#endif
	EnterCriticalSect();
	if(-1 == sendto(sock,msg,strlen(msg),0,(struct sockaddr *)&server, adl))
		error_nr=7;
	LeaveCriticalSect();
}

void SendDataToEngine(void *bytes, int len)
{
	int adl=sizeof(server);
	EnterCriticalSect();
	if(-1 == sendto(sock,bytes,len,0,(struct sockaddr *)&server, adl))
		error_nr=7;
	LeaveCriticalSect();
}	
/* Callback Dispatcher */
void Init(callback_t callback)
{
	int v=1;
	int adl=sizeof(server);
	sock=socket(AF_INET,SOCK_DGRAM,0);
	
	// initialize error code
	error_nr=0;

	// initialize callback function
	Dispatch = callback;
    
	// initialize mutex
	InitCriticalSect();
	bzero((char *)&server,adl);

	// initialize logged_in
	Logged_in=0;
	attached=0;
	discovered=0;
	// broadcast setting code now in here
	// broadcast, to find servers...
	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char *)&v,sizeof(v));
	
	// now we are going to receive all THA shit :P
	pthread_create(&bcreceive,NULL,ReceivingPackets,NULL);
}

void End()
{
	if(Logged_in==1) Detach();
	
	// clear the lists...
	clear_mods();
	clear_admin();
	
	// close the socket
	close(sock);
}

void GoAttach() 
{
	// we only want opcodes from *our* discovered server
	if( (rcv.sin_addr.s_addr != server.sin_addr.s_addr) && (rcv.sin_port != server.sin_port)) 
	{
		#ifdef DEBUG_SOCK
		printf("Attach rejected from %s:%d\n",inet_ntoa(rcv.sin_addr),ntohs(rcv.sin_port));
		#endif
		return;
	}
	
	attached=1;
	Dispatch(EV_ATTACH, NULL, 0);
	GetState();
}

void GoEngineInUse()
{
	// we need to create the thread so we can receive "takeover" command
	pthread_create(&bcreceive,NULL,ReceivingPackets,NULL);
	Dispatch(EV_ENGINE_IN_USE, NULL, 0);
}

void Detach()
{

//	Chatmode(""); // just leave chatmode.. otherwise that would could be serious disasters lol, oh and you could be getting messages lol
	
	Logged_in=0;
	attached=0;
	discovered=0;
	SendToEngine("KAI_CLIENT_DETACH;");
	// Clean up global vars, free in-use pointers
	pthread_cancel(bcreceive); // cancel the thread...
	
}

void GoDiscovered() 
{
	char ip[16];
	int v;
	//getsockname(sock,(struct sockaddr *)&me,(socklen_t *)&adl);

	// we got the response, lets put it in the "server" -> this is now OUR official server
	memcpy(&server,&rcv,sizeof(rcv));
	
	// we are now discovered
	discovered=1;
	
	// we dont want broadcast anymore...
	v=0;
	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char *)&v,sizeof(v));

	strcpy(ip, inet_ntoa(server.sin_addr));
	ENGINE_HERE en;
	en.engine_ip = ip;
	en.engine_port=ntohs(server.sin_port);
	Dispatch(EV_ENGINE_HERE, &en, sizeof(en));
}

int Connect(char* ip)
{
	struct sockaddr_in tmp;
	struct hostent *host;
	int adl=sizeof(server);
	
	if(discovered==1 || Logged_in==1 || attached==1)
		return -1;
	if(ip==NULL)
		return -1;
	if(strlen(ip)<1) // ?!??!
		return -1;
	
	host=gethostbyname(ip);
	if(host==NULL) // wrong ip..
	{
		switch(h_errno)
		{
			case HOST_NOT_FOUND:
				return -2;
			case NO_ADDRESS:
				return -3;
			case NO_RECOVERY:
				return -4;
			case TRY_AGAIN:
				return -5;
			default:
				return -1;
		}
	}
	
	tmp.sin_family=AF_INET;
	tmp.sin_addr=*(struct in_addr *) *host->h_addr_list;
	tmp.sin_port=htons(KAI_PORT);
	
	#ifdef DEBUG_SOCK
		printf("Sending DISCOVER to %s\n",inet_ntoa(tmp.sin_addr));
	#endif
	if(-1==sendto(sock,"KAI_CLIENT_DISCOVER;",strlen("KAI_CLIENT_DISCOVER;"),0,(struct sockaddr *)&tmp, adl))
	{
		#ifdef DEBUG_SOCK
			printf("ERROR sending DISCOVER to %s\n",inet_ntoa(tmp.sin_addr));
		#endif
		return -6;
	}
	
	return 0;
}

void InitCriticalSect()
{
	 pthread_mutex_init(&mutex,NULL);
}

void EnterCriticalSect()
{
	pthread_mutex_lock(&mutex);
}

void LeaveCriticalSect()
{
	pthread_mutex_unlock(&mutex);
}
