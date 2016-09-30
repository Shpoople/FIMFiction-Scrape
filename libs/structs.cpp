struct prefs {
	bool saveCompleted;
	bool saveIncomplete;
	bool saveHiatus;
	bool saveCancelled;
	
	bool checkCompleted;
	bool checkIncomplete;
	bool checkHiatus;
	bool checkCancelled;
	bool checkInvalid;
	
	bool saveExplicit;
	
	int checkStart;
	int checkLimit;
	
	int saveStories;
	int saveImages;
};

struct storySQL {
	const char *title;
	
	const char *desc;
	const char *short_desc;
	
	const char *image;
	const char *full_image;
	
	const char *author;
	
	int status;
	int rating;
	int modified;
	int chapters;
};
