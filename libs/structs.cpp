enum class save_image {
	save_none, 
	save_thumb, 
	save_full, 
	save_all,
};

enum class save_story {
	save_sql,
	save_ebook,
	save_raw,
};

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
	
	save_story saveStories;
	save_image saveImages;
	
	int threads;
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
