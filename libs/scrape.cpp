void scrapeState(int id, int &state, int &updated);
bool checkStory(int id);
bool scrapeStory(int id, int scrape);
void regexData(std::string s, std::string *ret);
void sanitize(std::string &str);

bool checkStory(int id) {
	//just some simple variables
	int state, updated, newdate, success, scrape = 0;
	const char *states[5] = {"Completed", "Incomplete", "Hiatus", "Cancelled", "Invalid"};
	
	//Fluff
	printw("Checking status of story %i of %i... ", id, settings.checkLimit);
	refresh();
	
	//Get state of story
	checkStoryStatus(id, state, updated);
	
	//If it is 0, then we must not have it in our Database...
	if (!state) {
		scrapeState(id, state, updated);
		
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
		if (state == 5) {
			printw("Skipping...\n");
			refresh();
					
			return 0;
			
		} else if (state == 1 && settings.saveCompleted == false) {
			printw("Skipping...\n");
			refresh();
					
			return 0;
			
		} else if (state == 2 && settings.saveIncomplete == false) {
			printw("Skipping...\n");
			refresh();
					
			return 0;
			
		} else if (state == 3 && settings.saveHiatus == false) {
			printw("Skipping...\n");
			refresh();
					
			return 0;
		} else if (state == 4 && settings.saveCancelled == false) {
			printw("Skipping...\n");
			refresh();
					
			return 0;
		}
		
		scrape = 1;
		
	} else {
		//If the story's state is not to be rescraped, exit false. Else, continue...
		if (state == 1 && settings.checkCompleted == false) {
			printw("Skipping rescrape...\n");
			refresh();
						
			return 0;
		}
		
		if (state == 2 && settings.checkIncomplete == false) {
			printw("Skipping rescrape...\n");
			refresh();
						
			return 0;
		}
		
		if (state == 3 && settings.checkHiatus == false) {
			printw("Skipping rescrape...\n");
			refresh();
						
			return 0;
		}
		
		if (state == 4 && settings.checkCancelled == false) {
			printw("Skipping rescrape...\n");
			refresh();
						
			return 0;
		}
		
		if (state == 5 && settings.checkInvalid == false) {
			printw("Skipping rescrape...\n");
			refresh();
						
			return 0;
		}
			
		//Let's give this scrape another whorl
		scrapeState(id, state, newdate);
		
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
		if (state == 5) {
			printw("Skipping...\n");
			refresh();
					
			return 0;
				
		} else if (state == 1 && settings.saveCompleted == false) {
			printw("Skipping...\n");
			refresh();
						
			return 0;
				
		} else if (state == 2 && settings.saveIncomplete == false) {
			printw("Skipping...\n");
			refresh();
					
			return 0;
				
		} else if (state == 3 && settings.saveHiatus == false) {
			printw("Skipping...\n");
			refresh();
						
			return 0;
		} else if (state == 4 && settings.saveCancelled == false) {
			printw("Skipping...\n");
			refresh();
						
			return 0;
		}
	}
	
	//Let the user know we are scraping the story
	printw("Scraping... ");
	refresh();
	
	success = scrapeStory(id, scrape);
	
	return success;
}

bool scrapeStory(int id, int scrape) {
	char *filename = new char[100];
	char *storyUrl = new char[100];
	char *scrapeUrl = new char[100];
	const char *jsonData;
	picojson::value v;
	std::string err;
		
	sprintf(scrapeUrl, "http://www.fimfiction.net/api/story.php?story=%i", id);
	jsonData = dataFetch(scrapeUrl);
		
	err = picojson::parse(v, jsonData);
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
	
	const char *stateString = v.get("story").get("status").get<std::string>().c_str();
		
	if (!strcmp(stateString, "Complete"))
		story.status = 1;
			
	if (!strcmp(stateString, "Incomplete"))
		story.status = 2;
				
	if (!strcmp(stateString, "On Hiatus"))
		story.status = 3;
			
	if (!strcmp(stateString, "Cancelled"))
		story.status = 4;
	
	story.author = v.get("story").get("author").get("name").get<std::string>().c_str();
	
	story.modified = (int)v.get("story").get("date_modified").get<double>();
	
	story.chapters = (int)v.get("story").get("chapter_count").get<double>();
	
	story.rating = (int)v.get("story").get("content_rating").get<double>();
	
	//Check if it is mature, and if we can download mature stories...
	if (story.rating == 2 && settings.saveExplicit == false) {
		printw("Mature, Skipping...\n");
		refresh();
		
		return 0;
	}
	
	//Check to see if the story has any chapters. If not, skip...
	if (story.chapters == 0) {
		printw("Empty.\n");
		refresh();
			
		return 0;
	}
	
	//Note: Due to the way that FIMFiction saves images, I can't actually tell what type they are
	//unless I actually look at the first few bytes. Which I don't wanna do.
	//Fortunately, webrowsers do that on their own, so all I have to do is save them with an arbitrary extension.
	//Save thumbnail image
	if (strcmp(image, "") && scrape == true) {
		if (settings.saveImages == SAVE_THUMB || settings.saveImages == SAVE_ALL) {
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
		if (settings.saveImages == SAVE_FULL || settings.saveImages == SAVE_ALL) {
			sprintf(filename, "images/story_%i", id);
			
			dataSave(full_image, filename);
			story.full_image = filename;
		} else {
			story.full_image = "null";
		}
	} else {
		story.full_image = "null";
	}
	
	//Save the story itself.
	if (!settings.saveStories == SAVE_SQL) {
		
		if (settings.saveStories == SAVE_RAW) {
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
			
			for (picojson::array::const_iterator iter = list.begin(); iter != list.end(); ++iter) {
				const char *chapTitle = (*iter).get("title").get<std::string>().c_str();
				const char *chapUrl = (*iter).get("link").get<std::string>().c_str();
				int lastUpdated = (int)(*iter).get("date_modified").get<double>();
				
				//See if chapter has updated since we checked
				int lastCached = 0;
				checkChapterStatus(id, chapterNum, lastCached);
				
				//printw("API is reporting last chapter update date of %i\n", lastUpdated);
				//refresh();
				
				if (lastCached != lastUpdated) {
					//Dates do not match.
					//This must either be our first time checking, or the chapter has been updated
					
					//Fetch data from chapter url
					std::string rawData(dataFetch(chapUrl));
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
				
				chapterNum++;
			}
		} else {
			//Story is empty, what's the use in doing anything else?
			printw("Empty.\n");
			refresh();
			
			return 0;
		}
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
	
	//Categories
	if (v.get("story").get("categories").get("2nd Person").get<bool>())
		addTagSQL(id, 1);
	if (v.get("story").get("categories").get("Adventure").get<bool>())
		addTagSQL(id, 2);
	if (v.get("story").get("categories").get("Alternate Universe").get<bool>())
		addTagSQL(id, 3);
	if (v.get("story").get("categories").get("Anthro").get<bool>())
		addTagSQL(id, 4);
	if (v.get("story").get("categories").get("Comedy").get<bool>())
		addTagSQL(id, 5);
	if (v.get("story").get("categories").get("Crossover").get<bool>())
		addTagSQL(id, 6);
	if (v.get("story").get("categories").get("Dark").get<bool>())
		addTagSQL(id, 7);
	if (v.get("story").get("categories").get("Drama").get<bool>())
		addTagSQL(id, 8);
	if (v.get("story").get("categories").get("Equestria Girls").get<bool>())
		addTagSQL(id, 9);
	if (v.get("story").get("categories").get("Horror").get<bool>())
		addTagSQL(id, 10);
	if (v.get("story").get("categories").get("Human").get<bool>())
		addTagSQL(id, 11);
	if (v.get("story").get("categories").get("Mystery").get<bool>())
		addTagSQL(id, 12);
	if (v.get("story").get("categories").get("Random").get<bool>())
		addTagSQL(id, 13);
	if (v.get("story").get("categories").get("Romance").get<bool>())
		addTagSQL(id, 14);
	if (v.get("story").get("categories").get("Sad").get<bool>())
		addTagSQL(id, 15);
	if (v.get("story").get("categories").get("Sci-Fi").get<bool>())
		addTagSQL(id, 16);
	if (v.get("story").get("categories").get("Slice of Life").get<bool>())
		addTagSQL(id, 17);
	if (v.get("story").get("categories").get("Thriller").get<bool>())
		addTagSQL(id, 18);
	if (v.get("story").get("categories").get("Tragedy").get<bool>())
		addTagSQL(id, 19);
	
	//After we get everything done, it's time to save the story data...
	if (scrape == 1) {
		saveStorySQL(id, &story);
	} else if (scrape == 2) {
		updateStorySQL(id, &story);
	}
	
	printw("Done.\n");
	refresh();
	
	return 1;
	
}

void scrapeState(int id, int &state, int &updated) {
	
	char *scrapeUrl = new char[100];
	const char *jsonData;
	picojson::value v;
	std::string err;
		
	sprintf(scrapeUrl, "http://www.fimfiction.net/api/story.php?story=%i", id);
	jsonData = dataFetch(scrapeUrl);
		
	err = picojson::parse(v, jsonData);
	if (!err.empty()) {
		printw("Error: %s", err.c_str());
	}
		
	if (!v.get("error").is<picojson::null>()) {
		
		//The query returns an invalid story, return invalid 
		state = 5;
		
	} else {
		//It's not as simple as that. We must go deeper...
		const char *stateString = v.get("story").get("status").get<std::string>().c_str();
		updated = (int)v.get("story").get("date_modified").get<double>();
		
		if (!strcmp(stateString, "Complete"))
			state = 1;
			
		if (!strcmp(stateString, "Incomplete"))
			state = 2;
				
		if (!strcmp(stateString, "On Hiatus"))
			state = 3;
			
		if (!strcmp(stateString, "Cancelled"))
			state = 4;
	}
	
	//printw("API is reporting status of %i, and update date of %i\n", state, updated);
	//refresh();
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
