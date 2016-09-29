std::string readBuffer;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

const char *dataFetch(const char *url) {
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
    
		if(res != CURLE_OK) {
			if (tries <= 3) {
				printw("Try %i failed... Retrying.\n", tries);
				refresh();
				tries++;
				
				sleep(1);
				
				goto retryFetch;
			} else {
				printw("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
				refresh();
				
				exit(-1);
			}
		} else {
			return readBuffer.c_str();
		}

		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();

	return 0;
}

int dataSave(const char *url, const char *file) {
	CURL *curl;
	CURLcode res;
	FILE *fp; 
	
	int tries = 1;
	
	retrySave:

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if(curl) {
		// Open file 
		fp = fopen(file, "wb"); 
		
		if( fp == NULL ) {
			printw("File cannot be opened!\n");
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
    
		if(res != CURLE_OK) {
			if (tries <= 3) {
				printw("Try %i failed... Retrying.\n", tries);
				refresh();
				tries++;
				
				sleep(1);
				
				goto retrySave;
			} else {
				//printw("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
				//refresh();
				
				//exit(-1);
				return 0;
			}
		} else {
			return 1;
		}

		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();

	return 0;
}
//static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
    //((std::string*)userp)->append((char*)contents, size * nmemb);
    //return size * nmemb;
//}

//const char *dataFetch(const char *url) {
	//CURL *curl;
	//CURLcode res;
	//std::string readBuffer;

	//curl = curl_easy_init();
	//if(curl) {
		//curl_easy_setopt(curl, CURLOPT_URL, url);
		//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		//res = curl_easy_perform(curl);
		//curl_easy_cleanup(curl);
		
		//(void)res;

		//return readBuffer.c_str();
	//} else {
		//return "Error";
	//}
	//return 0;
//}
