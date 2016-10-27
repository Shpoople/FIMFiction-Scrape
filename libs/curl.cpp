static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void setFileExt(const char *baseFilename, const char file[8]) {
	//jpeg = ff d8 ff
	//png = 89 50 4e 47 0d 0a 1a 0a
	//gif = GIF
	char filename[50];
	
	if (file[1] == 0x50) {
		//printw("File is a PNG (0x%x)\n", file[1]);
		//refresh();
				
		sprintf(filename, "%s.png", baseFilename);
	} else if (file[1] == 0x49) {
		//printw("File is a GIF (0x%x)\n", file[1]);
		//refresh();
				
		sprintf(filename, "%s.gif", baseFilename);
	} else {
		//printw("File is a JPEG (0x%x)\n", file[1]);
		//refresh();
				
		sprintf(filename, "%s.jpg", baseFilename);
	}
	
	rename(baseFilename , filename);
}

std::string dataFetch(const char *url) {
	std::string readBuffer;
	
	CURL *curl;
	CURLcode res;
	readBuffer = "";
	
	int tries = 1;
	
	retryFetch:

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(curl, CURLOPT_COOKIE, "view_mature=true;");

		#ifdef SKIP_PEER_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		#endif

		#ifdef SKIP_HOSTNAME_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		#endif
	
		res = curl_easy_perform(curl);
		
		//This was the source of a massive memory leak, just if you're wondering...
		curl_easy_cleanup(curl);
		curl_global_cleanup();
    
		if(res != CURLE_OK) {
			if (tries <= 3) {
				printw("\nTry %i failed... Retrying... ", tries);
				refresh();
				tries++;
				
				sleep(5);
				
				goto retryFetch;
			} else {
				printw("curl_easy_perform() failed: %s\nCheck your internet connection, and hit ENTER!!! to try again...\n", curl_easy_strerror(res));
				refresh();
				
				//Set keypad mode, and hide cursor
				keypad(stdscr, TRUE);
				curs_set(0);
				
				int c = wgetch(stdscr);
				
				switch(c){	
					case 10: //Go ahead and retry
						tries = 1;
						goto retryFetch;
						
						break;
					default: //Exit, instead
						exit(-1);
						break;
				}
			}
		} else {
			return readBuffer;
		}
	}

	return 0;
}

int dataSave(const char *url, const char *file) {
	std::string readBuffer;
	
	CURL *curl;
	CURLcode res;
	FILE *fp;
	char byteBuffer[8];
	
	int tries = 1;
	
	retrySave:

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if(curl) {
		// Open file 
		fp = fopen(file, "w+b"); 
		
		if( fp == NULL ) {
			printw("File cannot be opened! (%s)\n", file);
			refresh();
			
			return 0;
		}
		
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL); 
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(curl, CURLOPT_COOKIE, "view_mature=true;");

		#ifdef SKIP_PEER_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		#endif

		#ifdef SKIP_HOSTNAME_VERIFICATION
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		#endif
	
		res = curl_easy_perform(curl);
		
		//This was the source of a massive memory leak, just if you're wondering...
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		
		fseek(fp, SEEK_SET, 0);
		fread(byteBuffer, sizeof(char), sizeof(byteBuffer)-1, fp);
		
		fclose(fp);
    
		if(res != CURLE_OK) {
			if (tries <= 3) {
				printw("\nTry %i failed... Retrying... ", tries);
				refresh();
				tries++;
				
				sleep(5);
				
				goto retrySave;
			} else {
				printw("curl_easy_perform() failed: %s\nCheck your internet connection, and hit ENTER to try again...\n", curl_easy_strerror(res));
				refresh();
				
				//Set keypad mode, and hide cursor
				keypad(stdscr, TRUE);
				curs_set(0);
				
				int c = wgetch(stdscr);
				
				switch(c){	
					case 10: //Go ahead and retry
						tries = 1;
						goto retrySave;
						
						break;
					default: //Exit, instead
						exit(-1);
						break;
				}
				//return 0;
			}
		} else {
			setFileExt(file, byteBuffer);
			
			return 1;
		}
	}

	return 0;
}
