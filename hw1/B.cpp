#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/conf.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "Encryptor.h"
#define PORT 9999

   
class B{
public:
	unsigned char key[16] = "123456789012345";
	unsigned char encKey[16];
	int mode;
	int sock;
	struct sockaddr_in serv_addr;

	int A_sock;
	struct sockaddr_in A_serv_addr;

	B(){
		initAComm();
	}


	int initToMC(){
		//initializare client catre MC
		sock = 0;
    		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    		{
       	 		printf("\n Socket creation error \n");
        		return -1;
    		}

    		serv_addr.sin_family = AF_INET;
    		serv_addr.sin_port = htons(PORT);

  
    		if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    		{
        		printf("\nInvalid address/ Address not supported \n");
        		return -1;
    		}
		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    		{
        		printf("\nConnection Failed \n");
        		return -1;
    		}

	}

	int initAComm(){
		//initializare client catre A
		A_sock = 0;
    		sockaddr_in A_serv_addr;
    		if ((A_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    		{
       	 		printf("\n Socket creation error \n");
        		return -1;
    		}

    		A_serv_addr.sin_family = AF_INET;
    		A_serv_addr.sin_port = htons(8888);

  
    		if(inet_pton(AF_INET, "127.0.0.1", &A_serv_addr.sin_addr)<=0)
    		{
        		printf("\nInvalid address/ Address not supported \n");
        		return -1;
    		}
		if (connect(A_sock, (struct sockaddr *)&A_serv_addr, sizeof(A_serv_addr)) < 0)
    		{
        		printf("\nConnection Failed \n");
        		return -1;
    		}

		communicate();

	}

	void communicate(){
		read(A_sock,&mode,sizeof(int)); // citim modul de criptare de la A

		initToMC(); // initiem comunicarea cu MC si procuram cheea

		write(sock,&mode,sizeof(int));
		int len;
		unsigned char* encrypted;
		read(sock,&len,sizeof(int));
		encrypted = new unsigned char[len];
		read(sock,encrypted,len);
		
		int len_decrypted = Encryptor::decrypt(encrypted,len,key,encKey,mode);

		write(A_sock,&mode,sizeof(int)); // trimitem mesaj lui A ca suntem pregatiti pentru comunicare

		readFile();
	}

	void readFile(){
		int size = -1;
		do{
			read(A_sock,&size,sizeof(int));
			if(size!=-1){
				unsigned char encrypted[100];
				read(A_sock,encrypted,size);
				unsigned char decrypted[50];
				Encryptor::decrypt(encrypted,strlen((const char*)encrypted),encKey,decrypted,mode);
				std::cout<<decrypted;
			}
		}while(size!=-1);
	}

};


int main(int argc, char const *argv[])
{

	new B();
    return 0;
}
