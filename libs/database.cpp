sqlite3 *listDB;
sqlite3 *storyDB;

int storyStatus[2];
int chapterStatus;

static int storyCheckCallback(void *NotUsed, int argc, char **argv, char **azColName){
	//This should only send two values, which should always be integers, so we should be safe...
	storyStatus[0] = atoi(argv[0]);
	storyStatus[1] = atoi(argv[1]);
	
	return 0;
}

static int lastStoryCallback(void *NotUsed, int argc, char **argv, char **azColName){
	//This should only send two values, which should always be integers, so we should be safe...
	storyStatus[0] = atoi(argv[0]);
	
	return 0;
}
static int chapterCheckCallback(void *NotUsed, int argc, char **argv, char **azColName){
	//This should only send one value, which should always be an integer, so we should be safe...
	chapterStatus = atoi(argv[0]);
	
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
		"`result` INTEGER NOT NULL," \
		"`updated` INTEGER NOT NULL);";
	
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
			"`tag` INTEGER NOT NULL," \
			"`extra` CHAR(100) DEFAULT '');";
	
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
	
	if (settings.saveStories == save_story::save_sql) {
		chapterSql = "CREATE TABLE IF NOT EXISTS `chapters` (" \
			"`id` INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," \
			"`parentid` INTEGER NOT NULL," \
			"`updated` INTEGER NOT NULL," \
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

int findLastStory() {
	char *ErrMsg = 0;
	int RespCode;

	std::string sql = "SELECT `storyid` FROM `list` ORDER BY `storyid` DESC LIMIT 1;";
	
	RespCode = sqlite3_exec(listDB, sql.c_str(), lastStoryCallback, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("API SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}else{
		//printw("Success!\n");
		//refresh();
	}
	
	return storyStatus[0];
}

int setStoryStatus(int id, int result, int updated) {
	char *ErrMsg = 0;
	int RespCode;
	
	std::stringstream ss;

	ss << "INSERT INTO `list` (`storyid`, `result`, `updated`) VALUES (" << id << ", " << result << ", " << updated << ");";

	std::string sql = ss.str();
	
	RespCode = sqlite3_exec(listDB, sql.c_str(), NULL, 0, &ErrMsg);
	
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

int updateStoryStatus(int id, int result, int updated) {
	char *ErrMsg = 0;
	int RespCode;
	
	std::stringstream ss;

	ss << "UPDATE `list` SET `result`=" << result << ", `updated`=" << updated << " WHERE `storyid`=" << id << ";";

	std::string sql = ss.str();
	
	RespCode = sqlite3_exec(listDB, sql.c_str(), NULL, 0, &ErrMsg);
	
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

void checkStoryStatus(int id, int &status, int &updated) {
	char *ErrMsg = 0;
	int RespCode;
	
	storyStatus[0] = 0;
	storyStatus[1] = 0;
	
	std::stringstream ss;

	ss << "SELECT `result`, `updated` FROM `list` WHERE `storyid`=" << id << " LIMIT 1;";

	std::string sql = ss.str();
	
	RespCode = sqlite3_exec(listDB, sql.c_str(), storyCheckCallback, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("API SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}else{
		//printw("Success!\n");
		//refresh();
	}
	
	status = storyStatus[0];
	updated = storyStatus[1];
	
	//printw("Database is reporting status of %i, and update date of %i\n", storyStatus[0], storyStatus[1]);
	//refresh();
}

int saveStorySQL(int id, storySQL *story) {
	char *ErrMsg = 0;
	int RespCode;
	
	std::stringstream ss;

	ss << "INSERT INTO `stories` (`storyid`, `title`, `author`, `desc`, `short_desc`, `image`, `full_image`, `status`, `modified`, `rating`, `chapters`) VALUES (" << id << ", '" << story->title << "', '" << story->author << "', '" << story->desc << "', '" << story->short_desc << "', '" << story->image << "', '" << story->full_image << "', " << story->status << ", " << story->modified << ", " << story->rating << ", " << story->chapters << ");";

	std::string sql = ss.str();
	
	RespCode = sqlite3_exec(storyDB, sql.c_str(), NULL, 0, &ErrMsg);
	
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

int updateStorySQL(int id, storySQL *story) {
	char *ErrMsg = 0;
	int RespCode;
	
	std::stringstream ss;

	ss << "UPDATE `stories` SET `title`='" << story->title << "', `author`='" << story->author << "', `desc`='" << story->desc << "', `short_desc`='" << story->short_desc << "', `image`='" << story->image << "', `full_image`='" << story->full_image << "', `status`=" << story->status << ", `modified`=" << story->modified << ", `rating`=" << story->rating << ", `chapters`=" << story->chapters << " WHERE `storyid`=" << id << ";";

	std::string sql = ss.str();
	
	RespCode = sqlite3_exec(storyDB, sql.c_str(), NULL, 0, &ErrMsg);
	
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

void checkChapterStatus(int id, int chapterNum, int &updated) {
	char *ErrMsg = 0;
	int RespCode;
	
	storyStatus[0] = 0;
	storyStatus[1] = 0;
	
	std::stringstream ss;

	ss << "SELECT `updated` FROM `chapters` WHERE `parentid`=" << id << " AND `chapterid`=" << chapterNum << " LIMIT 1;";

	std::string sql = ss.str();
	
	RespCode = sqlite3_exec(storyDB, sql.c_str(), chapterCheckCallback, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("API SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}else{
		//printw("Success!\n");
		//refresh();
	}
	
	updated = chapterStatus;
	
	//printw("Database is reporting chapter update date of %i\n", chapterStatus);
	//refresh();
}

int saveChapterSQL(int id, int chapter, int updated, const char *title, const char *body) {
	char *ErrMsg = 0;
	int RespCode;
	
	std::stringstream ss;

	ss << "INSERT INTO `chapters` (`parentid`, `chapterid`, `updated`, `title`, `body`) VALUES (" << id << ", " << chapter << ", " << updated << ", '" << title << "', '" << body << "');";

	std::string sql = ss.str();
	
	RespCode = sqlite3_exec(storyDB, sql.c_str(), NULL, 0, &ErrMsg);
	
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

int updateChapterSQL(int id, int chapter, int updated, const char *title, const char *body) {
	char *ErrMsg = 0;
	int RespCode;
	
	std::stringstream ss;

	ss << "UPDATE `chapters`  SET `updated`=" << updated << ", `title`='" << title << "', `body`='" << body << "' WHERE `parentid`=" << id << " AND `chapterid`=" << chapter << ";";

	std::string sql = ss.str();
	
	RespCode = sqlite3_exec(storyDB, sql.c_str(), NULL, 0, &ErrMsg);
	
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
	
	std::stringstream ss;

	ss << "INSERT INTO `tags` (`story`, `tag`) VALUES (" << id << ", " << tag << ");";

	std::string sql = ss.str();
	
	RespCode = sqlite3_exec(storyDB, sql.c_str(), NULL, 0, &ErrMsg);
	
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

int addExtraTagSQL(int id, std::string extra) {
	char *ErrMsg = 0;
	int RespCode;
	
	std::stringstream ss;

	ss << "INSERT INTO `tags` (`story`, `tag`, `extra`) VALUES (" << id << ", 0, '" << extra.c_str() << "');";

	std::string sql = ss.str();
	
	RespCode = sqlite3_exec(storyDB, sql.c_str(), NULL, 0, &ErrMsg);
	
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

int deleteStorySQL(int id) {
	char *ErrMsg = 0;
	int RespCode;
	std::stringstream ss;
	std::string sql;
	
	printw("Deleting incomplete data for story %i... ", id);
	refresh();
			
	//DELETE ENTRY FROM STORY LIST
	ss << "DELETE FROM `list` WHERE `storyid`=" << id << ";";

	sql = ss.str();
	RespCode = sqlite3_exec(listDB, sql.c_str(), NULL, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("List SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}
	refresh();
	
	//DELETE ENTRY FROM STORY DATABASE
	ss << "DELETE FROM `stories` WHERE `storyid`=" << id << ";";

	sql = ss.str();
	
	RespCode = sqlite3_exec(storyDB, sql.c_str(), NULL, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("Story SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}
	refresh();
	
	//DELETE ENTRIES FROM TAG DATABASE
	ss << "DELETE FROM `tags` WHERE `story`=" << id << ";";

	sql = ss.str();
	
	RespCode = sqlite3_exec(storyDB, sql.c_str(), NULL, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("Tag SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}
	refresh();
	
	//DELETE ENTRY FROM STORY DATABASE
	ss << "DELETE FROM `chapters` WHERE `parentid`=" << id << ";";

	sql = ss.str();
	
	RespCode = sqlite3_exec(storyDB, sql.c_str(), NULL, 0, &ErrMsg);
	
	if( RespCode != SQLITE_OK ){
		printw("Chapter SQL error: %s\n", ErrMsg);
		refresh();
		
		sqlite3_free(ErrMsg);
		exit(-1);
	}
	
	printf("Done.\n");
	refresh();
	
	return 1;
}
