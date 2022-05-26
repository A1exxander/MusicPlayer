#pragma once

#include <string>

class Song {
private:
	std::string songName;
	std::string artistName;
	int songLength;
public:
	Song(std::string songName, std::string artistName, int songLength) : songName{ songName }, artistName{ artistName }, songLength{ songLength } {} // No need for getters, as we wont want to and shouldnt change the data for this class, we must simply initilize it with the data we want
	int getSongTime() const;
	const std::string& getSongName() const;
	const std::string& getArtistName() const;
	std::string getSongTimeMS() const; 
};