#include "Song.h"

int Song::getSongTime() const{

	return songLength;

}

std::string Song::getSongTimeMS() const{

	std::string songTime{ "" };
	int minutes = this->songLength / 60;
	int seconds = this->songLength % 60;
	songTime = minutes + " : " + seconds;

	return songTime;

}

const std::string& Song::getSongName() const {

	return songName;

}

const std::string& Song::getArtistName() const {

	return artistName;

}