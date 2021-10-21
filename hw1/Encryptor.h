class Encryptor{
	public:
		static int encrypt(unsigned char* text, int text_len,unsigned char* key, unsigned char* cipher, int type){
			int cipher_len = 0;
			int len = 0;

			EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

			if(!ctx){
				perror("EVP_CIPHER_CTX_new() failed");
				exit(-1);
			}
			if(type == 1){
				if(!EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL)){
					perror("EVP_EncryptInit() failed");
                                	exit(-1);
				}
			}else
				if(!EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, NULL)){
                                        perror("EVP_EncryptInit() failed");
                                        exit(-1);
                                }

			if(!EVP_EncryptUpdate(ctx, cipher, &len, text, text_len)){
                                perror("EVP_EncryptInit() failed");
                                exit(-1);
                        }

			cipher_len +=len;

			if(!EVP_EncryptFinal(ctx, cipher + len, &len)){
                                perror("EVP_EncryptFinal() failed");
                                exit(-1);
                        }

			cipher_len +=len;

			EVP_CIPHER_CTX_free(ctx);

			return cipher_len;
		}
		
		static int decrypt(unsigned char* cipher, int cipher_len,unsigned char* key, unsigned char* text, int type){
                        int text_len = 0;
                        int len = 0;

                        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

                        if(!ctx){
                                perror("EVP_CIPHER_CTX_new() failed");
                                exit(-1);
                        }
                        if(type == 1){
                                if(!EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL)){
                                        perror("EVP_DecryptInit() failed");
                                        exit(-1);
                                }
                        }else
                                if(!EVP_DecryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, NULL)){
                                        perror("EVP_DecryptInit() failed");
                                        exit(-1);
                                }

                        if(!EVP_DecryptUpdate(ctx, text, &len, cipher, cipher_len)){
                                perror("EVP_DecryptInit() failed");
                                exit(-1);
                        }

                        text_len +=len;

                       /* if(!EVP_DecryptFinal(ctx, text + len, &len)){
                                perror("EVP_DecryptFinal() failed");
                                exit(-1);
                        }*/
			EVP_DecryptFinal(ctx, text + len, &len);

                        text_len +=len;

                        EVP_CIPHER_CTX_free(ctx);

                        return text_len;
                }

};

