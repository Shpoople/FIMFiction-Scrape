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

using namespace boost; 

#include "libs/structs.hpp"
#include "libs/menu.hpp"
#include "libs/prefs.hpp"
#include "libs/curl.hpp"
#include "libs/database.hpp"
#include "libs/picojson.h"
#include "libs/scrape.hpp"

int main() {
	int story, saved, checked = 0;
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
	
	loadPrefs();
	
	createDatabases();
	
	printw("Welcome to the ");
	attron(A_STANDOUT);
	printw("FIMFiction story scraper");
	attroff(A_STANDOUT);
	printw("!\n\n");
	refresh();
	
	time(&timeStart);
	
	for (story = settings.checkStart; story <= settings.checkLimit; story++) {
		saved += checkStory(story);
	}
	
	time(&timeEnd);
	
	checked = (settings.checkLimit - settings.checkStart)+1;
	
	duration = timeEnd-timeStart;
	percentage = ((float)saved/(float)checked)*100.0f;
	average = (float)duration/(float)checked;
	
	printw("\nFinished. Saved %i out of %i stories processed (%2.0f%)\nElapsed time %i seconds (average: %2.1f seconds)\n\n", saved, checked, percentage, duration, average);
	
	closeDatabases();
    
    endwin();
    refresh();
}
