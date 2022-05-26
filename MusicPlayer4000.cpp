#include <iostream>
#include <thread>
#include <Windows.h>
#include <mmsystem.h>
#include <vector>
#include <filesystem>
#include <fstream>
#include <queue>
#include <random>
#include <stdlib.h>
#include "Song.h"


#pragma comment(lib, "winmm.lib") // Used to tell the linker to include a DLL, where the dll lib goes in ""


class MusicPlayer3000 {
private:

	bool keyboardInUse{ false };
	std::vector<Song> library;
	std::queue<Song> songQueue;

public:

	MusicPlayer3000() {

		createUserLibrary();

	}

	int menu() {

		while (keyboardInUse == true) { // incase we have queue song func or musicplayer func using the keyboard to keyboard

			Sleep(500);

		}

		std::cout << "\n1.View Library\n2.Exit\n\nChoice : ";

		int choice{};

		std::cin >> choice;

		return choice;

	}

	bool fileExists(const std::string& fileName){
		
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

				library.push_back(temp);

			}
		}
		else {


			std::filesystem::create_directories("C:\\MusicPlayer4000\\");
			std::ifstream reader;
			
			files.open(filePath);

			std::cout << "\nPlease place any .WAV files you would like to add to your library into " << filePath << " and place each song name, artist name, and song length in seconds on a new line";

			std::string enterTest{ " " };


			while (reader >> songInfo) {

				std::string songName = songInfo;
				reader >> songInfo;
				std::string artistName = songInfo;
				reader >> songInfo;
				int songLength = std::stoi(songInfo);

				Song temp(songName, artistName, songLength);

				library.push_back(temp);

			}

		}
	}


	void printLibrary(const std::vector<Song>& library) {

		

		if (library.size() == 0) {

			std::string folderPath{ "C:\\MusicPlayer3000\\" };
		
			std::cout << "\nNo songs in library! Add files to " << folderPath << " and each song name, artist name, and song length in seconds on a new line on Songs.txt";

		}

		for (auto i{ 0 }; i < library.size(); ++i) {

			std::cout << "\n" << i + 1 << ". Song Name : " << library[i].getSongName() << "Artist name : " << library[i].getArtistName();

		}

	}


	void viewLibrary() {

		printLibrary(library);

		std::cout << "\n\n1. Play song\n2. Return\n\n Choice : ";

		int choice{};

		std::cin >> choice;

		if (choice != 2) {

		keyboardInUse = true; // lock access to prevent menu function from being able to proceed while were in songplayer or getsongtoqueues critical section using keyboard, if it was a sequential process or didnt use keyboard we wouldnt need this but since we will return to user menu which uses keyboard & both getsongqueue and songplayer use keyboard, we must lock

				if (songQueue.empty()) {
					std::thread t1(&MusicPlayer3000::songPlayer, this);
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
			std::string currentSongName{ songQueue.front().getSongName() };

			system("CLS");

			std::cout << "\nYOU ARE LISTENING TO " << currentSongName;
			int time{ 0 };

			std::cout << "F1. Queue Song F2. Repeat Song F3. Randomize Repeat F4. Skip Current Song ";

			std::string folderPath{ "C:\\MusicPlayer3000\\" + songQueue.front().getSongName() + ".wav" };

			std::wstring stemp = std::wstring(folderPath.begin(), folderPath.end());
			LPCWSTR sw = stemp.c_str();

			PlaySound(sw, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

			while (time < songQueue.front().getSongTime()) {

				if (keyboardInUse == false) {

					if (GetAsyncKeyState(VK_F1) && (songQueue.front().getSongTime()) - time > 10) {
						keyboardInUse = true;
						std::thread t1(&MusicPlayer3000::getSongToQueue, this);
						t1.detach();
					}
					else if (GetAsyncKeyState(VK_F2)) {
						repeatSong = true;
					}
					else if (GetAsyncKeyState(VK_F3)) {
						randomizeSong = true;
					}
					else if (GetAsyncKeyState(VK_F4)) {
						songQueue.pop();
					}
				}
				Sleep(1000);
				++time;

			}

			if (repeatSong == false) {
				songQueue.pop();
			}
			else if (randomizeSong == true) {
				songQueue.pop();
				songQueue.push(library[getRandomSong(library)]);
			}

		}

		PlaySound(NULL, 0, 0);

	}

	int getRandomSong(const std::vector<Song>& library) {

		std::mt19937 mt;

		return mt() % library.size();

	}
	int selectSongWNumber(const std::vector<Song>& library) {

		std::cout << "\nSelect song number : ";

		int songChoice;

		std::cin >> songChoice;

		--songChoice;

		while (songChoice < library.size() && songChoice != -1) {

			std::cout << "\nEnter a number from 1 - " << library.size() << " or -1 to exit : ";

			std::cin >> songChoice;

		}

		--songChoice;

		return songChoice;

	}
	int selectSongWName(const std::vector<Song>& library) {

		std::string song{ " " };
		std::cout << "\nEnter song name : ";
		std::cin >> song;

		int i{ 0 };

		while (i < library.size() && library[i].getSongName() != song) {

			++i;
		}

		if (i < library.size() && library[i].getSongName() == song) {
			return i;
		}
		else {
			std::cout << "\nNo song found!";
			return -1;
		}
	}

	void getSongToQueue() {

		std::cout << "\n1.Select song by number \n2.Select song by name \n3. Return";

		int choice{};

		std::cin >> choice;

		int songChoice{};

		switch (choice) {

		case 1: {

			songChoice = selectSongWNumber(library);

		}
		case 2: {

			songChoice = selectSongWName(library);

		}

		case 3: {
		
			songChoice = -1;

		}


		if (songChoice >= 0) {

				  songQueue.push(library[songChoice]);

			  }

		}

		keyboardInUse = false;

	}

	bool songQueueEmpty() {

		return songQueue.empty();

	}


};

int main()
{
	bool repeat{ true };

	MusicPlayer3000 m;

	while (repeat) {

			switch (m.menu()) {

				case 1:
					m.viewLibrary();
					break;
				default:
					repeat = false;

			}

		}


	}



/* TODO -
* Add songs to hashmap, where key is SongName + ArtistName, to make it faster to find?
* Add GUI? 
*/
