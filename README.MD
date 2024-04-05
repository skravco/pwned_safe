sudo apt-get update
sudo apt-get install libsqlite3-dev
sudo apt-get install libcrypto++-dev


to compile:
g++ -std=c++11 -o prog  prog.cpp -lcryptopp -lsqlite3

