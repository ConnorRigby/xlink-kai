/*******************************************
Copyright (c) 2004, Luís Miguel Fernandes <zipleen 'at' teamxlink.co.uk>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************/

#ifndef _ERRORS_H_
#define _ERRORS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
int error_nr;
//char* error_descriptions;

/*isto pode-se fazer em C normaleco? agora o gajo queixa-se de q o netcode.c nao usa esta variavel.. e ta correcto, é só usado no kaiui.c mas 
 n sei q fazer com isto.. se definir normalmente sem static ele começa a dizer q ta definido em varios locais ...*/
static char* error_descriptions[] = { 
	"No Error",
	"Error in malloc() inserting mod user",
	"Error in malloc() inserting admin user",
	"Error in malloc() inserting user",
	"Error in malloc() inserting chat user",
	"Error in malloc() inserting chat channel",
	"Error in sendto() - SendingBC Discover",
	"Error in sendto() - SendToEngine",
	"Error in sendto() - Attaching to server",
	"I tried 5 times to Attach...but the server refused it?!.. going to exit...",
	"Bind error! Your port number could be in use if you specified it...",
	"Select error",
	"Error in SIOCGIFCONF ioctl() - no network?!?!",
	"Error in SIOCGIFFLAGS ioctl() - no flags set in network adapter??",
	"Error in SIOCGIFBRDADDR ioctl() - no broadcast address?!"
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
