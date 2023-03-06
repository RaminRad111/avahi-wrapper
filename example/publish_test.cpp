#include "avahi_wrapper/mdns.hpp"

#define SERVICE_TYPE "_samim-ems._tcp"
#define SERVICE_PORT 10006

int main(int argc, char const *argv[]) {
	std::vector<std::string> txt;
	txt.push_back("key0=value0");
	txt.push_back("key1=value1");

	avahi_wrapper::mdnsService s;
	s.name = "MY_SERVICE"; // optional (default is hostname)
	s.type = SERVICE_TYPE; // optional (default is "_http._tcp")
	s.port = SERVICE_PORT; // optional (default is 6600)
	s.txt = txt; // optional (default is empty)

	avahi_wrapper::mDNS mdns;
	mdns.createService(s);
	mdns.loop();

	return 0;
}
