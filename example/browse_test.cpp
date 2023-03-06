#include "avahi_wrapper/mdns.hpp"
#include "avahi_wrapper/util.hpp"
#include <chrono>

#define SERVICE_TYPE "_http._tcp"

static void serviceAddedCallback(std::shared_ptr<avahi_wrapper::mdnsService> s) {
	std::cout << "\n*** Service Added ***\n";
	std::cout << s->name << std::endl;
	std::cout << s->type << std::endl;
	std::cout << s->ip << std::endl;
	std::cout << s->port << std::endl;
	for (auto &str : s->txt)
		std::cout << str << " ";
	std::cout << std::endl;
}

static void serviceRemovedCallback(std::shared_ptr<avahi_wrapper::mdnsService> s) {
	std::cout << "\n*** Service Removed ***\n";
	std::cout << s->name << std::endl;
	std::cout << s->type << std::endl;
	std::cout << s->ip << std::endl;
	std::cout << s->port << std::endl;
	for (auto &str : s->txt)
		std::cout << str << " ";
	std::cout << std::endl;
}

int main(int argc, char const *argv[]) {
	avahi_wrapper::mDNS mdns;
	
	mdns.browse(SERVICE_TYPE, &serviceAddedCallback, &serviceRemovedCallback, avahi_wrapper::PROTO_IPV4);
	// mdns.browse(SAMIM_SERVICE_TYPE); // it is possible to browse without callbacks and use Parser(util.hpp & util.cpp) to search in founded service 
	mdns.loop();
	
	return 0;
}
