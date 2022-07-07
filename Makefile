include /usr/local/etc/PcapPlusPlus.mk

# All Target
all:
	g++ $(PCAPPP_BUILD_FLAGS) $(PCAPPP_INCLUDES) -c -o a.o Firewall.cpp
	g++ $(PCAPPP_LIBS_DIR) -static-libstdc++ -o a.out a.o $(PCAPPP_LIBS) -lcurl

# Clean Target
clean:
	rm a.o
	rm a.out