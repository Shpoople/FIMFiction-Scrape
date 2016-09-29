scrape: 
	g++ -Wall -o "scrape" "main.cpp" -std=c++11 -lncurses -lsqlite3 -lcurl -lboost_regex
