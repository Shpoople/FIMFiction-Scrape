std::vector<std::string> tags = {"2nd Person", "Adventure", "Alternate Universe", "Anthro", "Comedy", "Crossover", "Dark", "Drama", "Equestria Girls", "Horror", "Human", "Mystery", "Random", "Romance", "Sad", "Sci-Fi", "Slice of Life", "Thriller", "Tragedy"};

enum class response {
	not_started,
	started,
	finished,
	error,
};

class threadPool {
	public:
		threadPool(int id, int chapters);
		~threadPool();
		void append(int chapNumber, const char *chapTitle, std::string chapUrl, int lastUpdated, int updated);
		void execute(int threads);
		void finish(int thread, response status);
		
		int finishedAll = 0;
	private:
		int _id;
		int _chapters;
		
		//Thread stuff
		std::thread *t;
		response *threadStatus;
		
		//Storage stuff
		std::vector<std::string> _chapTitle;
		std::vector<std::string> _chapUrl;
		int *_lastUpdated;
		int *_updated;
};

void threadDL(int id, int chapNumber, std::string chapTitle, std::string chapUrl, int lastUpdated, int updated, threadPool *pool);bool checkStory(int id);
bool scrapeStory(int id, const char *data, int scrape);
void scrapeState(const char *data, int &state, int &updated);
void scrapeExtraTags(int id);
void regexData(std::string s, std::string *ret);
void sanitize(std::string &str);

const char *states[5] = {"Completed", "Incomplete", "Hiatus", "Cancelled", "Invalid"};

bool checkStory(int id) {
	//just some simple variables
	int state, updated, newdate, success, scrape = 0;
	const char *APIData;
	
	//Fluff
	printw("Checking status of story %i of %i... ", id, settings.checkLimit);
	refresh();
	
	//Get state of story
	checkStoryStatus(id, state, updated);
	
	//Get story API data
	std::string scrapeUrl = "http://www.fimfiction.net/api/story.php?story=" + std::to_string(id);
	std::string dataString = dataFetch(scrapeUrl);
	APIData = dataString.c_str();
	
	//If it is 0, then we must not have it in our Database...
	if (!state) {
		scrapeState(APIData, state, updated);
			
		//Set the new state
		setStoryStatus(id, state, updated);
		
		//We can tell the user what state the story is in
		printw("[");
		
		if (state >= 4) {
			//Invalid, or cancelled
			attron(COLOR_PAIR(3));
			printw("%s", states[state-1]);
			attroff(COLOR_PAIR(3));
		} else if (state == 1) {
			//Completed
			attron(COLOR_PAIR(1));
			printw("%s", states[state-1]);
			attroff(COLOR_PAIR(1));
		} else {
			//Everything else
			attron(COLOR_PAIR(2));
			printw("%s", states[state-1]);
			attroff(COLOR_PAIR(2));
		}
		
		printw("], ");
		refresh();
		
		//Make sure the story isn't invalid...
		if (state == 5
		|| (state == 1 && settings.saveCompleted == false)
		|| (state == 2 && settings.saveIncomplete == false)
		|| (state == 3 && settings.saveHiatus == false)
		|| (state == 4 && settings.saveCancelled == false)) {
			printw("Skipping...\n");
			refresh();
					
			return 0;
		}
		
		scrape = 1;
		
	} else {
		//If the story's state is not to be rescraped, exit false. Else, continue...
		if ((state == 5 && settings.checkInvalid == false)
		|| (state == 1 && settings.checkCompleted == false)
		|| (state == 2 && settings.checkIncomplete == false)
		|| (state == 3 && settings.checkHiatus == false)
		|| (state == 4 && settings.checkCancelled == false)) {
			printw("Skipping rescrape...\n");
			refresh();
					
			return 0;
		}
			
		//Let's give this scrape another whorl
		scrapeState(APIData, state, newdate);
		
		//Set the new state
		updateStoryStatus(id, state, newdate);
		
		//We can tell the user what state the story is in
		printw("[");
		
		if (state >= 4) {
			//Invalid, or cancelled
			attron(COLOR_PAIR(3));
			printw("%s", states[state-1]);
			attroff(COLOR_PAIR(3));
		} else if (state == 1) {
			//Completed
			attron(COLOR_PAIR(1));
			printw("%s", states[state-1]);
			attroff(COLOR_PAIR(1));
		} else {
			//Everything else
			attron(COLOR_PAIR(2));
			printw("%s", states[state-1]);
			attroff(COLOR_PAIR(2));
		}
		
		printw("], ");
		refresh();
		
		if (newdate <= updated) {
			//The story has not been updated since our last scrape...
			//printw("No Update...\n");
			//refresh();
			
			scrape = 0;
		} else {
			scrape = 2;
		}
			
		//Make sure the story isn't invalid...
		if (state == 5
		|| (state == 1 && settings.saveCompleted == false)
		|| (state == 2 && settings.saveIncomplete == false)
		|| (state == 3 && settings.saveHiatus == false)
		|| (state == 4 && settings.saveCancelled == false)) {
			printw("Skipping...\n");
			refresh();
					
			return 0;
		}
	}
	
	//Let the user know we are scraping the story
	printw("Scraping... ");
	refresh();
	
	success = scrapeStory(id, APIData, scrape);
	
	return success;
}

bool scrapeStory(int id, const char *data, int scrape) {
	char *filename = new char[100];
	char *storyUrl = new char[100];
	
	picojson::value v;
	std::string err;
		
	err = picojson::parse(v, data);
	if (!err.empty()) {
		printw("Error: %s", err.c_str());
	}
	
	//SOOOOO MANY VARIABLES!
	storySQL story;
	
	story.title = v.get("story").get("title").get<std::string>().c_str();
	
	if (!v.get("story").get("short_description").is<picojson::null>()) {
		story.short_desc = v.get("story").get("short_description").get<std::string>().c_str();
	} else {
		story.short_desc = "";
	}
	
	if (!v.get("story").get("description").is<picojson::null>()) {
		story.desc = v.get("story").get("description").get<std::string>().c_str();
	} else {
		story.desc = "";
	}
	
	const char *image;
	if (!v.get("story").get("image").is<picojson::null>()) {
		image = v.get("story").get("image").get<std::string>().c_str();
	} else {
		image = "";
	}
	
	const char *full_image;
	if (!v.get("story").get("full_image").is<picojson::null>()) {
		full_image = v.get("story").get("full_image").get<std::string>().c_str();
	} else {
		full_image = "";
	}
	
	const char *stateString;
	if (!v.get("story").get("status").is<picojson::null>()) {
		stateString = v.get("story").get("status").get<std::string>().c_str();
	} else {
		stateString = "Error";
	}
		
	if (!strcmp(stateString, "Complete")) {
		story.status = 1;
	} else if (!strcmp(stateString, "Incomplete")) {
		story.status = 2;
	} else if (!strcmp(stateString, "On Hiatus")) {
		story.status = 3;
	} else if (!strcmp(stateString, "Cancelled")) {
		story.status = 4;
	} else if (!strcmp(stateString, "Error")) {
		story.status = 0;
	}

	//We cannot, for some reason, get the story status
	
	story.author = v.get("story").get("author").get("name").get<std::string>().c_str();
	
	story.modified = (int)v.get("story").get("date_modified").get<double>();
	
	story.chapters = (int)v.get("story").get("chapter_count").get<double>();
	
	story.rating = (int)v.get("story").get("content_rating").get<double>();
	
	//Check if it is mature, and if we can download mature stories...
	if (story.rating == 2 && settings.saveExplicit == false) {
		printw("Mature, Skipping...\n");
		refresh();
		
		delete[] filename;
		delete[] storyUrl;
		
		return 0;
	}
	
	//Check to see if the story has any chapters. If not, skip...
	if (story.chapters == 0) {
		printw("Empty.\n");
		refresh();
		
		delete[] filename;
		delete[] storyUrl;
			
		return 0;
	}
	
	//Alright, now we can...
	//Save the story data itself.
	if (settings.saveStories != save_story::save_sql) {
		if (settings.saveStories != save_story::save_raw) {
			//Save as raw text
			sprintf(filename, "stories/%i.txt", id);
			sprintf(storyUrl, "http://www.fimfiction.net/download_story.php?story=%i", id);
		} else {
			sprintf(filename, "stories/%i.epub", id);
			sprintf(storyUrl, "http://www.fimfiction.net/download_epub.php?story=%i", id);
		}
		
		dataSave(storyUrl, filename);
	} else {
		if (v.get("story").get("chapters").is<picojson::array>()) {
			//Save each individual chapter
			const picojson::array list = v.get("story").get("chapters").get<picojson::array>();
			int chapterNum = 0;
			int updatedChap = 0;
			
			threadPool threads(id, story.chapters);
			
			for (picojson::array::const_iterator iter = list.begin(); iter != list.end(); ++iter) {
				const char *chapTitle = (*iter).get("title").get<std::string>().c_str();
				std::string chapUrl = (*iter).get("link").get<std::string>().c_str();
				int lastUpdated = (int)(*iter).get("date_modified").get<double>();
				
				//See if chapter has updated since we checked
				int lastCached = 0;
				checkChapterStatus(id, chapterNum, lastCached);
				
				//printw("API is reporting last chapter update date of %i\n", lastUpdated);
				//refresh();
				
				if (lastCached != lastUpdated) {
					//Dates do not match.
					//This must either be our first time checking, or the chapter has been updated
					
					if (settings.threads) {
						threads.append(chapterNum, chapTitle, chapUrl, lastCached, lastUpdated);
					} else {
						//Fetch data from chapter url
						std::string rawData = dataFetch(chapUrl);
						std::string chapData;
						std::string strTitle(chapTitle);
						
						//Process data for actual chapter via RegEx
						regexData(rawData, &chapData);
						
						//Strip of illegal characters, needs to be streamlined
						sanitize(chapData);
						sanitize(strTitle);
						
						//Save chapter to SQL
						if (lastCached == 0) {
							//We have never checked this chapter before.
							saveChapterSQL(id, chapterNum, lastUpdated, strTitle.c_str(), chapData.c_str());
						} else {
							updateChapterSQL(id, chapterNum, lastUpdated, strTitle.c_str(), chapData.c_str());
						}
					}
					updatedChap++;
				}
				
				chapterNum++;
			}
			
			if (settings.threads && updatedChap != 0) {
				//Execute threads and wait until they're finished
				threads.execute(settings.threads);
				while(!threads.finishedAll);
			}
		} else {
			//Story is empty, what's the use in doing anything else?
			printw("Empty.\n");
			refresh();
			
			delete[] filename;
			delete[] storyUrl;
			
			return 0;
		}
	}
	
	//If we want to scrape/rescrape the story info/images/tags
	if (scrape) {
		//Save thumbnail image
		if (strcmp(image, "") && scrape == true) {
			if (settings.saveImages == save_image::save_thumb || settings.saveImages == save_image::save_all) {
				sprintf(filename, "images/thumb/thumb_%i", id);
				
				dataSave(image, filename);
				story.image = filename;
			} else {
				story.image = "null";
			}
		} else {
			story.image = "null";
		}
		
		//Save fullsize image
		if (strcmp(full_image, "") && scrape == true) {
			if (settings.saveImages == save_image::save_full || settings.saveImages == save_image::save_all) {
				sprintf(filename, "images/story_%i", id);
				
				dataSave(full_image, filename);
				story.full_image = filename;
			} else {
				story.full_image = "null";
			}
		} else {
			story.full_image = "null";
		}
		
		
		//Sanitize all of the varchar variables
		std::string strTitle(story.title);
		sanitize(strTitle);
		story.title = strTitle.c_str();
		
		std::string strDesc(story.desc);
		sanitize(strDesc);
		story.desc = strDesc.c_str();
		
		std::string strSDesc(story.short_desc);
		sanitize(strSDesc);
		story.short_desc = strSDesc.c_str();
		
		std::string strAuthor(story.author);
		sanitize(strAuthor);
		story.author = strAuthor.c_str();
		
		//Save all of the tags that the story has
		for (size_t tag = 0; tag < tags.size(); tag++) {
			if (v.get("story").get("categories").get(tags[tag].c_str()).get<bool>()) {
				addTagSQL(id, tag + 1);
			}
		}
			
		//Since the FIMFiction API is such a piece of shit, we are forced to do this.
		scrapeExtraTags(id);
		
		//After we get everything done, it's time to save the story data...
		if (scrape == 1) {
			saveStorySQL(id, &story);
		} else if (scrape == 2) {
			updateStorySQL(id, &story);
		}
	}
	
	printw("Done.\n");
	refresh();
	
	delete[] filename;
	delete[] storyUrl;
	
	return 1;
	
}

void scrapeState(const char *data, int &state, int &updated) {
	picojson::value v;
	std::string err;
		
	err = picojson::parse(v, data);
	if (!err.empty()) {
		printw("Error: %s", err.c_str());
	}
		
	if (!v.get("error").is<picojson::null>()) {
		
		//The query returns an invalid story, return invalid 
		state = 5;
		
	} else {
		//It's not as simple as that. We must go deeper...
		if (v.get("story").get("status").is<picojson::null>()) {
			//We will default to "Incomplete" for broken stories.
			//I'm looking at you, iloveportalz0r.
			state = 2;
			return;
		}
		
		//Now get the state string
		const char *stateString = v.get("story").get("status").get<std::string>().c_str();
		updated = (int)v.get("story").get("date_modified").get<double>();
		
		if (!strcmp(stateString, "Complete")) {
			state = 1;
		} else if (!strcmp(stateString, "Incomplete")) {
			state = 2;
		} else if (!strcmp(stateString, "On Hiatus")) {
			state = 3;
		} else if (!strcmp(stateString, "Cancelled")) {
			state = 4;
		}
	}
	
	//printw("API is reporting status of %i, and update date of %i\n", state, updated);
	//refresh();
}

void scrapeExtraTags(int id) {
	std::string scrapeUrl = "https://www.fimfiction.net/story/" + std::to_string(id);
	
	//regex expression and matching objects
	std::string raw = dataFetch(scrapeUrl);
	raw.erase(std::remove(raw.begin(), raw.end(), '\n'), raw.end());
	regex contRegex{"<div class=\"inner_data\">(.+?)<div class=\"story_stats\""};
	smatch container;
	
	if (regex_search(raw, container, contRegex)) {
		//regex expression and matching objects
		std::string newstring = container[1].str();
		regex tagRegex{"href=\"\\/tag\\/(.+?)\" class=\"character_icon\""};
		smatch tag;
		
		//if (regex_search(container[0].str(), tag, tagRegex)) {
			//printf("%s\n", tag[1].str().c_str());
		//}
		
		sregex_iterator m1(newstring.begin(), newstring.end(), tagRegex);
		sregex_iterator m2;
		for (; m1 != m2; ++m1){
			//printw("Extra tag: %s\n", m1->str(1).c_str());
			//refresh();
			std::string charName =  m1->str(1);
			
			addExtraTagSQL(id, charName);
		}
	}
}

void regexData(std::string s, std::string *ret) {
	//First off, make sure there are no newlines in the string...
	s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
	
	//regex expression and matching object
	regex expr{"<div id=\"chapter_container\">(.+?)<\\/div>"};
	smatch what;
	
	//If successful, return data
	if (regex_search(s, what, expr)) {
		//printf("%s", what[1].str().c_str());
		*ret = what[1].str();
	}
}

void sanitize(std::string &str) {
	//Don't strangle me over this awful excuse for sanitization...
	const char *strLocate[4] = {"'", "\"", "\n", "\r"};
	const char *strReplace[4] = {"&apos;", "&quot;", "\\n", "\\r"};
	
	for (int i = 0; i < 4; i++) {
		size_t loc = str.find_first_of(strLocate[i]);

		while (loc != std::string::npos) {
			str.replace(loc, strlen(strLocate[i]), strReplace[i]);
			
			loc = str.find_first_of(strLocate[i], loc+1); // Relocate again.
		}
	}
}

///////////////////////////////////////////
/////MULTI-THREAD OPERATION////////////////
///////////////////////////////////////////


threadPool::threadPool(int id, int chapters) {
	//Create and assign shit
	this->_id = id;
	this->_chapters = chapters;
	
	this->t = new std::thread[chapters];
	this->threadStatus = new response[chapters];
	this->_lastUpdated = new int[chapters];
	this->_updated = new int[chapters];
	
	for (int i = 0; i < chapters; i++) {
		this->threadStatus[i] = response::not_started;
	}
}

threadPool::~threadPool() {
	//Delet this
	delete[] this->t;
	delete[] this->threadStatus;
	
	delete[] this->_lastUpdated;
	delete[] this->_updated;
}

void threadPool::append(int chapNumber, const char *chapTitle, std::string chapUrl, int lastUpdated, int updated) {
	//Add chapter information to the queue
	this->_chapTitle.emplace_back(chapTitle);
	this->_chapUrl.emplace_back(chapUrl);
	this->_updated[chapNumber] = updated;
	this->_lastUpdated[chapNumber] = lastUpdated;
}

void threadPool::execute(int threads) {
	int maxThreads = threads;
	if (this->_chapters < threads) {
		//We have less chapters than threads, execute chapters directly
		maxThreads = this->_chapters;
	}
	
	for (int i = 0; i < maxThreads; ++i) {
		t[i] = std::thread(threadDL, this->_id, i, this->_chapTitle[i], this->_chapUrl[i], this->_lastUpdated[i], this->_updated[i], this);
		this->threadStatus[i] = response::started;
		t[i].detach();
	}
}

void threadPool::finish(int thread, response status) {
	this->threadStatus[thread] = status;
	int finishedThreads = 0;
	
	//Now we look for an unstarted thread, and execute it if there is
	for (int i = 0; i < this->_chapters; i++) {
		if (this->threadStatus[i] == response::not_started) {
			t[i] = std::thread(threadDL, this->_id, i, this->_chapTitle[i], this->_chapUrl[i], this->_lastUpdated[i], this->_updated[i], this);
			t[i].detach();
			this->threadStatus[i] = response::started;
			
			return;
		}
	}
	
	//Now we check to see if there are any threads still running
	for (int n = 0; n < this->_chapters; n++) {
		if (this->threadStatus[n] == response::finished || this->threadStatus[n] == response::error) {
			finishedThreads++;
		}
	}
	
	//If there isn't, then we're all finished
	if (finishedThreads == this->_chapters) {
		this->finishedAll = 1;
	}
}

void threadDL(int id, int chapNumber, std::string chapTitle, std::string chapUrl, int lastUpdated, int updated, threadPool *pool) {
	//Fetch data from chapter url
	//If you're wondering about it, I haven't gotten around to making the function return errors to the threadpool itself.
	std::string rawData = dataFetch(chapUrl);
	std::string chapData;
	std::string strTitle(chapTitle);
					
	//Process data for actual chapter via RegEx
	regexData(rawData, &chapData);
					
	//Strip of illegal characters, needs to be streamlined
	sanitize(chapData);
	sanitize(chapTitle);
					
	//Save chapter to SQL
	if (lastUpdated == 0) {
		//We have never checked this chapter before.
		saveChapterSQL(id, chapNumber, updated, chapTitle.c_str(), chapData.c_str());
	} else {
		updateChapterSQL(id, chapNumber, lastUpdated, chapTitle.c_str(), chapData.c_str());
	}
	
	usleep(chapNumber*200);
	pool->finish(chapNumber, response::finished);
}
