scrape: 
	g++ -Wall -o "scrape" "main.cpp" -std=c++11 -lncurses -lsqlite3 -lcurl -lboost_regex -pthread

clean:
	rm -rf db
	rm -rf images
	rm -rf stories
	rm scrape
	g++ -Wall -o "scrape" "main.cpp" -std=c++11 -lncurses -lsqlite3 -lcurl -lboost_regex -pthread
