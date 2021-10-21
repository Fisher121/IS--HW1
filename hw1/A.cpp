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
#include <fcntl.h>`
#include "Encryptor.h"
#define PORT 9999

   
class A{
public:
	unsigned char key[16] = "123456789012345";
	unsigned char encKey[16];
	int mode;
	int sock;
	struct sockaddr_in serv_addr;

	int BComm_sockfd;
        sockaddr_in BComm_sockaddr;
	int connection;

	A(int m){
		initBComm(m);
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

	void initBComm(int mode){
		this->mode = mode;
		BComm_sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if (BComm_sockfd == -1) {
                        std::cout << "Failed to create socket. errno: " << errno << std::endl;
                        exit(EXIT_FAILURE);
                }

                BComm_sockaddr.sin_family = AF_INET;
                BComm_sockaddr.sin_addr.s_addr = INADDR_ANY;
                BComm_sockaddr.sin_port = htons(8888);

                if (bind(BComm_sockfd, (struct sockaddr*)&BComm_sockaddr, sizeof(BComm_sockaddr)) < 0) {
                        std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
                        exit(EXIT_FAILURE);
                }

                if (listen(BComm_sockfd, 5) < 0) {
                        std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
                        exit(EXIT_FAILURE);
                }

		auto addrlen = sizeof(BComm_sockaddr);
                connection = accept(BComm_sockfd, (struct sockaddr*)&BComm_sockaddr, (socklen_t*)&addrlen);
                if (connection < 0) {
                        std::cout << "Failed to grab connection. errno: " << errno << std::endl;
                        exit(EXIT_FAILURE);
                }

		communicate();

	}

	void communicate(){
		write(connection,&mode,sizeof(int)); // transmitem lui B modul de comunicare

		initToMC();// initiem comunicarea cu MC si procuram cheea

		write(sock,&mode,sizeof(int));
		int len;
		unsigned char* encrypted;
		read(sock,&len,sizeof(int));
		encrypted = new unsigned char[len];
		read(sock,encrypted,len);
		
		int len_decrypted = Encryptor::decrypt(encrypted,len,key,encKey,mode);
		
		read(connection,&mode,sizeof(int)); // asteptam ca B sa fie pregatit
		
		sendFile();

	}

	void sendFile(){
		int fd = open("words.txt",O_RDONLY);
		unsigned char temp[50];
		int bytes_read = 0;
		do{
			bytes_read = read(fd,temp,50);
			if(bytes_read!=0){
				std::cout<<"sending: "<<temp<<'\n';
				unsigned char encrypted[100];
				int size = Encryptor::encrypt(temp,strlen((const char*)temp),encKey,encrypted,mode);
			
				write(connection,&size,sizeof(int));
				write(connection,encrypted,size);
			}
		}while(bytes_read!=0);
		int k = -1;
		write(connection,&k,sizeof(int));
	}

};


int main(int argc, char const *argv[])
{
	new A(1);
    return 0;
}
