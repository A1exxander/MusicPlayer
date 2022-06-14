#include <iostream>
#include <thread>
#include <Windows.h>
#include <mmsystem.h>
#include <map>
#include <filesystem>
#include <fstream>
#include <queue>
#include <random>
#include <stdlib.h>
#include "Song.h"


#pragma comment(lib, "winmm.lib") // Used to tell the linker to include a DLL, where the dll lib goes in ""

class MusicPlayer4000 {
private:

	bool keyboardInUse{ false };
	std::map<std::string, Song> library;
	std::string folderPath{ "C:\\MusicPlayer4000\\" };
	std::queue<Song> songQueue;

public:

	MusicPlayer4000() {

		createUserLibrary();

	}

	void printMenu() const { // Used for our main menu func OR music player where we may clear console on a seperate thread but we are still waiting for input from our previous mainmenu function running, instead of asking for another vars input, we will simply reprint the menu to remind the user 

		std::cout << "\n\n1. View Library\n2. Exit\n\nChoice : ";

	}

	int menu() const {

		while (keyboardInUse == true) { // Incase we have queueSong OR musicPlayer function running on a seperate thread using the keyboard

			Sleep(500);

		}
		
		printMenu();
		int choice{};
		std::cin >> choice;

		return choice;

	}


	bool fileExists(const std::string& fileName) const {

		std::ifstream file(fileName);
		return file.good();

	}

	void createUserLibrary() {

		std::ifstream reader;
		std::ofstream files;
		std::string songInfo{ " " };
		std::string filePath{ "C:\\MusicPlayer4000\\Songs.txt" };

		if (fileExists(filePath)) {

			reader.open(filePath);

			while (reader >> songInfo) {

				std::string songName = songInfo;
				reader >> songInfo;
				std::string artistName = songInfo;
				reader >> songInfo;
				int songLength = std::stoi(songInfo);

				Song temp(songName, artistName, songLength);

				library.insert({ temp.getSongName() + temp.getArtistName(), temp });

			}
		}

		else {

			std::filesystem::create_directories("C:\\MusicPlayer4000\\");
			std::ifstream reader;
			files.open(filePath);

			printEmptyLibraryError();

			while (reader >> songInfo) {

				std::string songName = songInfo;
				reader >> songInfo;
				std::string artistName = songInfo;
				reader >> songInfo;
				int songLength = std::stoi(songInfo);

				Song temp(songName, artistName, songLength);

				library.insert({ temp.getSongName() + temp.getArtistName(), temp });

			}

		}

	}

	const std::string& getFolderPath() const {
	
		return folderPath;

	}

	void printEmptyLibraryError() const {
	
		std::cerr << "\nNo songs in library! Add files to " << getFolderPath() << " and the song name, artist name, and song length in seconds for each song on a new line inside Songs.txt";
		std::cin.get();

	}

	void printLibrary(const std::map<std::string, Song>& library) const {

		int songNumber{ 1 };

		if (library.size() == 0) {

			printEmptyLibraryError();

		}

		for (auto i{ library.begin() }; i != library.end(); ++i) {

			std::cout << "\n" << songNumber << ". Song Name : " << i->second.getSongName() << " Artist name : " << i->second.getArtistName();
			++songNumber;

		}

	}

	void viewLibrary() {

		printLibrary(library);

		std::cout << "\n\n1. Play song\n2. Return\n\nChoice : ";
		int choice{};
		std::cin >> choice;

		if (choice == 1) {
			
			keyboardInUse = true; // lock access to prevent menu function from being able to proceed while were in songplayer or getsongtoqueues critical section using keyboard, if it was a sequential process or didnt use keyboard we wouldnt need this but since we will return to user menu which uses keyboard & both getsongqueue and songplayer use keyboard, we must lock
			
			if (songQueue.empty()) {
				std::thread t1(&MusicPlayer4000::songPlayer, this);
				t1.detach();
			}
			else {
				getSongToQueue();
			}

		}

	}

	void songPlayer() {

		getSongToQueue();

		while (songQueue.size() > 0) {

			bool repeatSong{ false };
			bool randomizeSong{ false };

			system("CLS");

			while (!songQueue.empty()) {

				std::string currentSongName{ songQueue.front().getSongName() };
				std::string folderPath{ "C:\\MusicPlayer4000\\" + currentSongName + ".wav" };
				int currentSongTime{ songQueue.front().getSongTime() };
				int currentElapasedTime{ 0 };

				std::cout << "\nYOU ARE LISTENING TO " << currentSongName << "\n";
				std::cout << "F8. Repeat Song F9. Randomize Repeat F10. Skip Current Song ";

				std::wstring stemp = std::wstring(folderPath.begin(), folderPath.end());
				LPCWSTR sw = stemp.c_str();

				PlaySound(sw, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

				while (currentElapasedTime < currentSongTime) {

					if (keyboardInUse == false) {

						if (GetAsyncKeyState(VK_F8))
							repeatSong = true;
						else if (GetAsyncKeyState(VK_F9)) 
							randomizeSong = true;
						else if (GetAsyncKeyState(VK_F10)) 
							currentElapasedTime = 99999;
					}

					Sleep(1000);
					++currentElapasedTime;

				}

				system("CLS");
				printMenu();

				if (repeatSong == true)
					songQueue.push(songQueue.front());

				else if (randomizeSong == true)
					songQueue.push(*getRandomSong(library));

				songQueue.pop();

			}

		}

		PlaySound(NULL, 0, 0);
		system("CLS");
		printMenu();

	}

	Song* getRandomSong(const std::map<std::string, Song>& library) const {

		auto song = library.begin();
		std::mt19937 mt;
		unsigned int songChoice{ mt() % library.size() };

		for (int i{ 0 }; i < songChoice; ++i) {

			++song;

		}

		Song* temp = new Song(song->second.getSongName(), song->second.getArtistName(), song->second.getSongTime());

		return temp;

	}

	Song* selectSongWNumber(const std::map<std::string, Song>& library) const {

		auto song = library.begin();
		int songChoice{};
		std::cout << "\nEnter a song number : ";
		std::cin >> songChoice;

		while (songChoice > library.size()) {

			std::cout << "\nEnter a number from 1 - " << library.size();
			std::cin >> songChoice;

		}
		--songChoice;
		
		for (int i{ 0 }; i < songChoice; ++i) {

			++song;

		}

		Song* temp = new Song(song->second.getSongName(), song->second.getArtistName(), song->second.getSongTime()); // We need to do this as we cant return ptr local object, nor can we get the addr of song->second for some reason, its not the best but its fine as we delete it after

		return temp;

	}

	Song* selectSongWName(std::map<std::string, Song>& library)  {

		std::string songName{ " " };
		std::string artistName{ " " };
		std::cout << "\nEnter song name : ";
		std::cin >> songName;
		std::cout << "\nEnter artist name : ";
		std::cin >> artistName;
		std::string key{ songName + artistName };

		Song* song{ nullptr };

		if (library.find(key) != library.end()) {
		
			song = &library.find(key)->second;

		}

		return song;

	}

	void getSongToQueue() {
		
		int choice{};
		std::cout << "\n1. Select song by number \n2. Select song by name \n3. Return\n\nChoice : ";
		std::cin >> choice;
		Song* songChoice{ nullptr };

		switch (choice) {

			case 1: {
				songChoice = selectSongWNumber(library);
				break;
			}
			case 2: {
				songChoice = selectSongWName(library);
				break;
			}
			default: {
				songChoice = nullptr;
				break;
			}

		}

		if (songChoice != nullptr) {

			songQueue.push(*songChoice);
			delete songChoice;
			songChoice = nullptr;

		}

		keyboardInUse = false;

	}

	bool songQueueEmpty() const {

		return songQueue.empty();

	}


};

int main()
{
	bool repeat{ true };

	MusicPlayer4000 m;

	while (repeat) {

		switch (m.menu()) {

		case 1: {
			m.viewLibrary();
			break;
		}
		default: {
			repeat = false;
		}
		}

	}


}



/* TODO -
* Add GUI?
*/
