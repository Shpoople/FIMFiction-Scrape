prefs settings;

void loadPrefs();
bool checkPrefsExists();
bool createPrefs();
void savePrefs();
void checkPrefs();
void storyPrefs();
void imagePrefs();

bool saveFile();
bool loadFile();

void loadPrefs() {
	printw("Loading preferences file... ");
    refresh();
    
    if (!checkPrefsExists()) {
		printw("Error!\nPreferences file not found, creating... ");
		refresh();
		
		createPrefs();
	} else {
		printw("Found.\n");
		if (printMenu("Preference file found...", 2, "Use existing preferences", "Create new preferences")) {
			createPrefs();
		} else {
			loadFile();
		}
	}
	
	clear();
}

bool checkPrefsExists() {
	FILE *file;
	file = fopen("preferences.dat", "r");
	
	if (file == NULL) {
        return false;
    } else {
        fclose(file);
        return true;
    }
}

bool createPrefs() {
	int result = 0;
	
	//Create settingsault prefrences
	settings.saveCompleted = true;
	settings.saveIncomplete = true;
	settings.saveHiatus = true;
	settings.saveCancelled = true;
	
	settings.checkCompleted = true;
	settings.checkIncomplete = true;
	settings.checkHiatus = true;
	settings.checkCancelled = true;
	settings.checkInvalid = true;
	
	settings.saveExplicit = false;
	
	settings.checkStart = 1;
	settings.checkLimit = 400000;
	
	settings.saveStories = save_story::save_sql;
	settings.saveImages = save_image::save_all;
	
	settings.threads = 0;
	
	//Set up menu dialog
	const char *ExplicitMenuOptions[2] = {"Do not save explicit stories", "Save explicit stories"};
	const char *storyMenuOptions[3] = {"Save stories in SQL Database", "Save stories as eBooks", "Save stories as raw text"};
	const char *ImageMenuOptions[4] = {"Do not save images", "Save thumbnail images", "Save fullsize images", "Save all images"};
	char startMenuDialog[25];
	char limitMenuDialog[25];
	char threadMenuDialog[50];
	
	//Create menu and navigate
	while(1) {
		sprintf(startMenuDialog, "Start at story %i", settings.checkStart);
		sprintf(limitMenuDialog, "End at story %i", settings.checkLimit);
		sprintf(threadMenuDialog, "Use %i cURL threads (0 = disabled)", settings.threads);
		
		result = printMenu("Set preferences", 9, "Set stories to be saved", "Set stories to be rechecked", ExplicitMenuOptions[settings.saveExplicit], startMenuDialog, limitMenuDialog, storyMenuOptions[(int)settings.saveStories], ImageMenuOptions[(int)settings.saveImages], threadMenuDialog, "Save preferences and return");
		
		const char *respNum;
		
		switch(result) {
			case 0: //Change story fetching prefrences
				savePrefs();
				break;
			
			case 1: //Change story rechecking prefrences
				checkPrefs();
				break;
		
			case 2: //Change explicit fetching prefrences
				settings.saveExplicit = !settings.saveExplicit;
				break;
			
			case 3: //Change story start prefrence
				respNum = printInput("Set starting point for story check", true).c_str();
				settings.checkStart = atoi(respNum);
				break;
				
			case 4: //Change story limit prefrence
				respNum = printInput("Set limit for story check", true).c_str();
				settings.checkLimit = atoi(respNum);
				break;
			
			case 5: //Change story fetching prefrences
				storyPrefs();
				break;
				
			case 6: //Change image fetching prefrences
				imagePrefs();
				break;
				
			case 7: //Exit prefrences menu
				respNum = printInput("Set number of cURL threads to use", true).c_str();
				settings.threads = atoi(respNum);
				break;
				
			case 8:
				saveFile();
				return 1;
				break;
		}
	}
	
}
void savePrefs() {
	int result = 0;
	
	const char *saveCompletedOptions[2] = {"Do not save completed stories", "Save completed stories"};
	const char *saveIncompleteOptions[2] = {"Do not save incomplete stories", "Save incomplete stories"};
	const char *saveHiatusOptions[2] = {"Do not save stories on hiatus", "Save stories on hiatus"};
	const char *saveCancelledOptions[2] = {"Do not save cancelled stories", "Save cancelled stories"};
	
	//Create menu and navigate
	while(1) {
		result = printMenu("Set stories to be saved", 5, saveCompletedOptions[settings.saveCompleted], saveIncompleteOptions[settings.saveIncomplete], saveHiatusOptions[settings.saveHiatus], saveCancelledOptions[settings.saveCancelled], "Return to preferences");

		switch(result) {
			case 0: //Set completed
				settings.saveCompleted = !settings.saveCompleted;
				break;
			
			case 1: //Set incomplete
				settings.saveIncomplete = !settings.saveIncomplete;
				break;
		
			case 2: //Set Hiatus
				settings.saveHiatus = !settings.saveHiatus;
				break;
			
			case 3: //Set Cancelled
				settings.saveCancelled = !settings.saveCancelled;
				break;
			
			case 4: //Return to main menu
				return;
				break;
		}
	}
}

void checkPrefs() {
	int result = 0;
	
	const char *checkCompletedOptions[2] = {"Do not recheck completed stories", "Recheck completed stories"};
	const char *checkIncompleteOptions[2] = {"Do not recheck incomplete stories", "Recheck incomplete stories"};
	const char *checkHiatusOptions[2] = {"Do not recheck stories on hiatus", "Recheck stories on hiatus"};
	const char *checkCancelledOptions[2] = {"Do not recheck cancelled stories", "Recheck cancelled stories"};
	const char *checkInvalidOptions[2] = {"Do not recheck invalid stories", "Recheck invalid stories"};
	
	//Create menu and navigate
	while(1) {
		result = printMenu("Set stories to be rechecked", 6, checkCompletedOptions[settings.checkCompleted], checkIncompleteOptions[settings.checkIncomplete], checkHiatusOptions[settings.checkHiatus], checkCancelledOptions[settings.checkCancelled], checkInvalidOptions[settings.checkInvalid], "Return to preferences");

		switch(result) {
			case 0: //Set completed
				settings.checkCompleted = !settings.checkCompleted;
				break;
			
			case 1: //Set incomplete
				settings.checkIncomplete = !settings.checkIncomplete;
				break;
		
			case 2: //Set Hiatus
				settings.checkHiatus = !settings.checkHiatus;
				break;
			
			case 3: //Set Cancelled
				settings.checkCancelled = !settings.checkCancelled;
				break;
				
			case 4: //Set Invalid
				settings.checkInvalid = !settings.checkInvalid;
				break;
			
			case 5: //Return to main menu
				return;
				break;
		}
	}
}
void storyPrefs() {
	int result = 0;
	
	result = printMenu("Set how stories are saved", 3, "Save stories in SQL Database", "Save stories as eBooks", "Save stories as raw text");
	
	switch(result) {
		case 0: //save as SQL database
			settings.saveStories = save_story::save_sql;
			break;
		
		case 1: //Save as eBook
			settings.saveStories = save_story::save_ebook;
			break;

		case 2: //Save as raw story
			settings.saveStories = save_story::save_raw;
			break;
	}
}

void imagePrefs() {
	int result = 0;
	
	result = printMenu("Set images to be saved", 4, "Do not save images", "Save thumbnail images", "Save fullsize images", "Save all images");
	
	switch(result) {
		case 0: //Do not save images
			settings.saveImages = save_image::save_none;
			break;
		
		case 1: //Save thumbs
			settings.saveImages = save_image::save_thumb;
			break;

		case 2: //Save fullsize
			settings.saveImages = save_image::save_full;
			break;

		case 3: //Save All
			settings.saveImages = save_image::save_all;
			break;
	}
}

bool saveFile() {
	std::ofstream out;
	out.open("preferences.dat", std::ios::binary);
	out.write(reinterpret_cast<char*>(&settings), sizeof(prefs));
	out.close();
	
	return 1;
}

bool loadFile() {
	std::ifstream in;
	in.open("preferences.dat", std::ios::binary);
	in.read(reinterpret_cast<char*>(&settings), sizeof(prefs));
	in.close();
	
	return 1;
}
