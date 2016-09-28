sqlite3 *listDB;
sqlite3 *storyDB;

int storyStatusCheck = 0;

static int storyCheckCallback(void *NotUsed, int argc, char **argv, char **azColName){
	//This should only send one value, which should always be an integer, so we should be safe...
	storyStatusCheck = atoi(argv[0]);
	
	return 0;
}

bool createDatabases() {
	char *ErrMsg = 0;
	int RespCode;
	const char *listSql, *storySql, *tagSql, *chapterSql;

	RespCode = sqlite3_open("db/list.db", &listDB);
	
	printw("Opening list database... ");
	refresh();

	if(RespCode){
		printw("Can't open database: %s\n", sqlite3_errmsg(listDB));
		refresh();
		exit(-1);
	}else{
		printw("Success!\n");
		refresh();
	}
   
	listSql = "CREATE TABLE IF NOT EXISTS `list` (" \
		"`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," \
		"`storyid` INTEGER NOT NULL," \
		"`result` INTEGER NOT NULL);";
	
	printw("Creating list table... ");
	refresh();
	
	RespCode = sqlite3_exec(listDB, listSql, NULL, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}else{
		printw("Success!\n");
		refresh();
	}
	
	RespCode = sqlite3_open("db/stories.db", &storyDB);
	
	printw("Opening story database... ");
	refresh();

	if(RespCode){
		printw("Can't open database: %s\n", sqlite3_errmsg(storyDB));
		refresh();
		exit(-1);
	}else{
		printw("Success!\n");
		refresh();
	}
   
	storySql = "CREATE TABLE IF NOT EXISTS `stories` (" \
		"`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," \
		"`storyid` INTEGER NOT NULL," \
		"`title` CHAR(255) NOT NULL," \
		"`author` CHAR(100) NOT NULL," \
		"`desc` TEXT NOT NULL," \
		"`short_desc` CHAR(255) NOT NULL," \
		"`image` CHAR(100) NOT NULL," \
		"`full_image` CHAR(100) NOT NULL," \
		"`status` INTEGER NOT NULL," \
		"`modified` INTEGER NOT NULL," \
		"`rating` INTEGER NOT NULL," \
		"`chapters` INTEGER NOT NULL);";
	
	printw("Creating story table... ");
	refresh();
	
	RespCode = sqlite3_exec(storyDB, storySql, NULL, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}else{
		printw("Success!\n");
		refresh();
	}
	
	tagSql = "CREATE TABLE IF NOT EXISTS `tags` (" \
			"`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," \
			"`story` INTEGER NOT NULL," \
			"`tag` INTEGER NOT NULL);";
	
	printw("Creating tags table... ");
	refresh();
	
	RespCode = sqlite3_exec(storyDB, tagSql, NULL, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}else{
		printw("Success!\n");
		refresh();
	}
	
	if (settings.saveStories == SAVE_SQL) {
		chapterSql = "CREATE TABLE IF NOT EXISTS `chapters` (" \
			"`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," \
			"`parentid` INTEGER NOT NULL," \
			"`chapterid` INTEGER NOT NULL," \
			"`title` CHAR(255) NOT NULL," \
			"`body` TEXT NOT NULL);";
		
		printw("Creating chapter table... ");
		refresh();
		
		RespCode = sqlite3_exec(storyDB, chapterSql, NULL, 0, &ErrMsg);
	
		if( RespCode != SQLITE_OK ){
			printw("SQL error: %s\n", ErrMsg);
			refresh();
			
			sqlite3_free(ErrMsg);
		exit(-1);
		}else{
			printw("Success!\n");
			refresh();
		}
	   
		//sqlite3_close(storyDB);
	} else {
		printw("Opening chapter database... Aborted.\nChapters not saved in SQL database.\n");
		refresh();
	}
	
	refresh();
	
	clear();
	
	refresh();
	
	return 1;
}

void closeDatabases() {
	printw("Closing Databases... ");
	refresh();
		
	sqlite3_close(listDB);
	sqlite3_close(storyDB);
		
	printw("Success!\n");
	refresh();
}

int setStoryStatus(int id, int result) {
	char *ErrMsg = 0;
	int RespCode;
	char *sql = new char[100];
	
	sprintf(sql, "INSERT INTO `list` (`storyid`, `result`) VALUES (%i, %i);", id, result);
	
	RespCode = sqlite3_exec(listDB, (const char*)sql, NULL, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("API SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}else{
		//printw("Success!\n");
		//refresh();
	}
	
	return 1;
}

int updateStoryStatus(int id, int result) {
	char *ErrMsg = 0;
	int RespCode;
	char *sql = new char[100];
	
	sprintf(sql, "UPDATE `list` SET `result`=%i WHERE `storyid`=%i;", result, id);
	
	RespCode = sqlite3_exec(listDB, (const char*)sql, NULL, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("API SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}else{
		//printw("Success!\n");
		//refresh();
	}
	
	return 1;
}

int checkStoryStatus(int id) {
	char *ErrMsg = 0;
	int RespCode;
	char *sql = new char[100];
	
	storyStatusCheck = 0;
	
	sprintf(sql, "SELECT `result` FROM `list` WHERE `storyid`=%i LIMIT 1;", id);
	
	RespCode = sqlite3_exec(listDB, (const char*)sql, storyCheckCallback, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("API SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}else{
		//printw("Success!\n");
		//refresh();
	}
	
	return storyStatusCheck;
}

int saveStorySQL(int id, storySQL *story) {
	char *ErrMsg = 0;
	int RespCode;
	char *sql = new char[10000];
	
	sprintf(sql, "INSERT INTO `stories` (`storyid`, `title`, `author`, `desc`, `short_desc`, `image`, `full_image`, `status`, `modified`, `rating`, `chapters`) VALUES ('%i', '%s', '%s', '%s', '%s', '%s', '%s', %i, %i, %i, %i);", id, story->title, story->author, story->desc, story->short_desc, story->image, story->full_image, story->status, story->modified, story->rating, story->chapters);
	
	RespCode = sqlite3_exec(storyDB, (const char*)sql, NULL, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("Story SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}else{
		//printw("Great Success!\n");
		//refresh();
	}
	refresh();
	
	return 1;
}

int saveChapterSQL(int id, int chapter, const char *title, const char *body) {
	char *ErrMsg = 0;
	int RespCode;
	char *sql = new char[1000000];
	
	sprintf(sql, "INSERT INTO `chapters` (`parentid`, `chapterid`, `title`, `body`) VALUES (%i, %i, '%s', '%s');", id, chapter, title, body);
	
	RespCode = sqlite3_exec(storyDB, (const char*)sql, NULL, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("Chapter %i SQL error: %s\n", chapter, ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}else{
		//printw("Success! %s, %i\n", title, strlen(sql));
		//refresh();
	}
	
	return 1;
}

int addTagSQL(int id, int tag) {
	char *ErrMsg = 0;
	int RespCode;
	char *sql = new char[1000];
			
	sprintf(sql, "INSERT INTO `tags` (`story`, `tag`) VALUES (%i, %i);", id, tag);
	
	RespCode = sqlite3_exec(storyDB, (const char*)sql, NULL, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("Tag SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}else{
		//printw("Success!\n");
		//refresh();
	}
	refresh();
	
	return 1;
}
