# avahi-wrapper
C++ wrapper for Avahi (mDNS/DNS-SD)

## Introduction
This is a C++ wrapper around official [avahi](https://github.com/lathiat/avahi) system. It can be used for DNS-SD discovery and publishing new mDNS service.

## Build
```Bash
mkdir build
cd build
cmake ..
make -j4
```

## Usage
You can add this library as a submodule to your main project.<br/>
Add these lines to your root CMakeLists.txt file:
```CMake
add_subdirectory(avahi-wrapper)
target_link_libraries(${PROJECT_NAME} avahi-wrapper)
```
If you are cross-compiling your project, don't forget to send libavahi-wrapper.so file to /usr/lib directory on the target.

## Examples
### Publish
It is easy to publish a new mdns service.
```C++
#include "avahi_wrapper/mdns.hpp"

int main(int argc, char const *argv[]) {
	avahi_wrapper::mdnsService s;
	avahi_wrapper::mDNS mdns;
  
	mdns.createService(s);
	mdns.loop();

	return 0;
}
```
This will publish a mdns service with your device's hostname as service name and "_http._tcp" as service type on port 6600 without txt records.<br/>
Of course you can change the service's attributes. check the publish_test.cpp under examples directory.
### Browse
You can simply run a DNS-DS discovery to observe services on network.
```C++
#include "avahi_wrapper/mdns.hpp"

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
	mdns.loop();
	
	return 0;
}
```
Two callbacks has been provided for service add and remove. In this example they are just used to print the added or removed service.<br/>
Using ```avahi_wrapper::PROTO_IPV4``` is optional, adding it will limit the browser to just ipv4 services.<br/>
You even can browse without callbacks:
```C++
#include "avahi_wrapper/mdns.hpp"
#include "avahi_wrapper/util.hpp"

#define SERVICE_TYPE "_http._tcp"

int main(int argc, char const *argv[]) {
	avahi_wrapper::mDNS mdns;
	
	mdns.browse(SERVICE_TYPE);
	mdns.loop();
	
	return 0;
}
```
Then You can find your desired services with Parser.
```C++
avahi_wrapper::Parser parser;
std::shared_ptr<avahi_wrapper::mdnsService> service;
std::string ip = "192.168.1.100";
service = parser.findByIp(ip);
```
