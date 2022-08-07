# DOE Project

This is a github repository for my doe project. This is essentially a automatic firewall written in C++. This is my first program written in C++ so don't expect much in the way of quality. There will be a release when my doe deadline hits but even after that I will continue development allbeit less.

## Building and running the project

### You need pcapplusplus in order to run the project, the included code installs it with the default options and only makes the libraries. if you want to change what you want to install check out [the pcapplusplus installation guide for linux](https://pcapplusplus.github.io/docs/install/linux).  

  
## PcapPlusPlus Installation
```
sudo apt-get install libpcap-dev libcurl4-openssl-dev
git clone https://github.com/seladb/PcapPlusPlus.git
cd PcapPlusPlus/
./configure-linux.sh --default
make libs
sudo make install
```

## Building the project

    git clone https://github.com/swanserquack/doe-project
    cd doe-project
    make
    sudo ./Firewall.out