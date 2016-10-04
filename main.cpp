#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sqlite3.h>
#include <string>
#include <string.h>
#include <curl/curl.h>
#include <boost/regex.hpp>

#if defined(_WIN32)
	#include <direct.h>
#else 
	#include <sys/stat.h>
#endif

using namespace boost; 

#include "libs/picojson.h"
#include "libs/structs.cpp"
#include "libs/menu.cpp"
#include "libs/prefs.cpp"
#include "libs/curl.cpp"
#include "libs/database.cpp"
#include "libs/scrape.cpp"

void makeDirs() {
	#if defined(_WIN32)
		_mkdir("db");
		_mkdir("images");
		_mkdir("images\thumb");
		_mkdir("stories");
	#else 
		mkdir("db", 0777);
		mkdir("images", 0777);
		mkdir("images/thumb", 0777);
		mkdir("stories", 0777);
	#endif
}

int main() {
	int story, saved, checked, lastStory, startAt = 0;
	float percentage, average = 0.00;
	time_t timeStart;
	time_t timeEnd;
	time_t duration;
	
	initscr();
    scrollok(stdscr,TRUE);
    
    if(has_colors() != FALSE) {
		use_default_colors();
		start_color();
		
		init_pair(1, COLOR_GREEN, -1);
		init_pair(2, COLOR_YELLOW, -1);
		init_pair(3, COLOR_RED, -1);
	}
	
	makeDirs();
	
	loadPrefs();
	
	createDatabases();
	
	lastStory = findLastStory();
	
	if (lastStory) {
		char proceedOpt[50];
		
		sprintf(proceedOpt, "Start from latest story (%i)", lastStory);
		startAt = printMenu("Existing database detected", 2, "Start from beginning", proceedOpt);
		
		if (!startAt) {
			startAt = settings.checkStart;
		} else {
			startAt = lastStory;
		}
	} else {
		startAt = settings.checkStart;
	}
	
	clear();
	refresh();
	
	printw("Welcome to the ");
	attron(A_STANDOUT);
	printw("FIMFiction story scraper");
	attroff(A_STANDOUT);
	printw("!\n\n");
	refresh();
	
	time(&timeStart);
	
	saved = 0;
	
	for (story = startAt; story <= settings.checkLimit; story++) {
		saved += checkStory(story);
	}
	
	//After we are done, we may close the ncurses window
	closeDatabases();
    
    refresh();
    endwin();
    
    //Calculate and print stats
	
	time(&timeEnd);
	
	checked = (settings.checkLimit - settings.checkStart)+1;
	
	duration = timeEnd-timeStart;
	percentage = ((float)saved/(float)checked)*100.0f;
	average = (float)duration/(float)checked;
	
	printf("Finished. Saved %i out of %i stories processed (%2.0f%%)\nElapsed time %i seconds (average: %2.1f seconds)\n\n", saved, checked, percentage, (int)duration, average);
}
