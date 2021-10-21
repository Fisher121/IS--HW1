#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/conf.h>
#include <string.h>
#include "Encryptor.h"


class MC{

	public: unsigned char key[16] = "123456789012345";
	public: unsigned char key1[16] = "123456789054321";
	public: unsigned char key2[16] = "098765432112345";
	int sockfd;
	sockaddr_in sockaddr;
	const int port = 9999;
	int connection;

	MC(){
		init();
	}

	private: void init(){
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
 		if (sockfd == -1) {
    			std::cout << "Failed to create socket. errno: " << errno << std::endl;
    			exit(EXIT_FAILURE);
  		}

		sockaddr.sin_family = AF_INET;
  		sockaddr.sin_addr.s_addr = INADDR_ANY;
  		sockaddr.sin_port = htons(port);

		if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
    			std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
   			exit(EXIT_FAILURE);
  		}

        	if (listen(sockfd, 5) < 0) {
                	std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
                	exit(EXIT_FAILURE);
        	}
		loop();
	
	}
	
	public: void loop(){
		do{
        		auto addrlen = sizeof(sockaddr);
        		connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
        		if (connection < 0) {
                		std::cout << "Failed to grab connection. errno: " << errno << std::endl;
                		exit(EXIT_FAILURE);
        		}

        		int msg;
        		read(connection,&msg,sizeof(int));
			
        		if(msg == 1){
				unsigned char result[100];
				int len = Encryptor::encrypt(key1,strlen((const char*)key1),key,result,1);
				write(connection,&len,sizeof(len));
                		write(connection,result,len);
			}
        		else{
				unsigned char result[100];
                                int len = Encryptor::encrypt(key2,strlen((const char*)key2),key,result,2);
				write(connection,&len,sizeof(len));
                                write(connection,result,len);

			}
        		close(connection);
  		}while(1);
  	close(sockfd);
	}

};


int main() {
  	new MC();
}
