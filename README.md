# MusicPlayer
Multi threaded music player app supporting a song queue, randomization, & repeat song. Stores song in a hashmap. Uses WinAPI to play song.
To add songs, go to C:\MusicPlayer4000 and add .wav files, along with the songname, artistname, & songlength(in seconds) each on their own line to a file called Songs.txt. The folder and file will be created if they do not exist on the first programs runtime. Attached a sample .txt file.

Mainly created to practice multithreading programming with a semaphore.
Originally planned on implementing a QT based GUI to the app, but decided it would be better to learn frontend web development than GUIs in C++

Must be compiled using C++17 Standard
