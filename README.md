# FIMFiction-Scrape

The **FIMFiction Story Scraper** is a simple command line program that allows you to download FIMFiction stories and their cover images for offline usage, or for the creation of an online backup of your own.

## Preferences

There are a wide range of preferences for which you can change, including:

- What stories you want to save, including *Complete* stories, *Incomplete* stories, stories on *Hiatus*, or even *Cancelled* stories.
- What stories to recheck on iterative runs, Including all of the above, as well as *Invalid* stories, which may simply not be available yet.
- Whether or not to save *Explicit* stories
- Where to start and stop your story scrape
- Whether to save your stories as raw text, eBooks, or in an SQL Database
- Whether or not to save the fullsize cover images, thumbnails, or both.

## Building

There are several libraries that are required in order to build, `ncurses`, `sqlite3`, `libcurl`, and `boost_regex`

Currently, the **FIMFiction Story Scraper** may be built with this command:

`g++ -Wall -o "main" "main.cpp" -std=c++11 -lncurses -lsqlite3 -lcurl -lboost_regex`

## WHAT TO DO

- Allow for multithreaded operation
- Check to see if iterative runs would like to begin from the last valid story.
- Skip stories that have not been modified at all.
- Exit if empty story for non-SQL scrapes.
- Use magic numbers from images to determine image type.
- Fix cURL errors crashing program.
- Allow retries of cURL downloads.
