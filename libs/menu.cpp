int printMenu(const char *title, int count, ...) {
	int pos = 0;
	va_list ap;
	
	//First off, we must clear the screen
	clear();
	
	//Then, we must move to the tope line, and print the title
	move(0, 0);
	
	printw(" %s ", title);
	hline(0, 500);
	refresh();
	
	//Set keypad mode, and hide cursor
	keypad(stdscr, TRUE);
	curs_set(0);
	
	while(1) {
		move(2, 0);
		
		//Start options list thingie
		va_start(ap, count);
		
		//Print out the options list
		for (int i = 0; i < count; i++) {
			char *option = va_arg(ap, char*);
			
			
			//If current option is selected, highlight it
			if (pos == i) {
				printw(" ");
				attron(A_STANDOUT);
			}
			
			//Print out the current option
			printw(" %s ", option);
			attroff(A_STANDOUT);
			printw("\n");
		}
		
		//End the options list thingie
		va_end(ap);
		refresh();
		
		//Wait for input, and determine if it is valid
		int c = wgetch(stdscr);
		switch(c){	
			case KEY_UP: //Left arrow key
				if (pos != 0)
					pos--;
				break;
			case KEY_DOWN: //Right arrow key
				if (pos != (count-1))
					pos++;
				break;
			case 10: //Enter key
				return pos;
				break;
			}
	}
	
	return 0;
}

char *printInput(const char *title, bool integer = false) {
	//I know this a memory leak, but I don't care.
	//It's not like we're gonna be running this function a million times
	char *str = new char[80];
	
	pistart:
	
	//Clear entire screen
	clear();
	
	//Then, we must move to the tope line, and print the title
	move(0, 0);
	
	printw(" %s ", title);
	hline(0, 500);
	refresh();
	
	//Set keypad mode, and show cursor
	keypad(stdscr, FALSE);
	curs_set(1);
	
	//move to last line on the screen, and enter text
	move(2, 0);
	
	printw(" >");
	getnstr(str, 80);
	
	//Undo earler keypad mode and cursor visibility
	keypad(stdscr, TRUE);
	curs_set(0);
	
	if (integer) {
		if (sscanf(str, "%*u%*c") == 0) {
			clear();
			mvprintw(0, 0, "Error: Input was not a numerical value!");
			refresh();
			sleep(3);
			
			goto pistart;
		}
	}
	
	return str;
}
