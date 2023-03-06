#pragma once

#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-client/publish.h>

#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/alternative.h>
#include <avahi-common/timeval.h>

#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <memory>

namespace avahi_wrapper {
	
	struct mdnsService {
		std::string name {"default"};
		std::string type {"_http._tcp"};
		std::string ip;
		unsigned int port {6600};
		std::vector<std::string> txt;
	};

	enum {
		PROTO_IPV4 = 0,     
		PROTO_IPV6 = 1,   
		PROTO_UNSPEC = -1  
	};

	static AvahiEntryGroup *group = NULL;
	static AvahiSimplePoll *simple_poll = NULL;
	static void (*serviceAddedCallback)(std::shared_ptr<mdnsService>) = nullptr;
	static void (*serviceRemovedCallback)(std::shared_ptr<mdnsService>) = nullptr;
	extern std::vector<std::shared_ptr<mdnsService>> services;
	void add(std::shared_ptr<mdnsService> s);
	void remove(std::shared_ptr<mdnsService> s);

	class mDNS {
	public:
		mDNS();
		~mDNS();

		void browse(const std::string t, void (*addCallback)(std::shared_ptr<mdnsService>), void (*removeCallback)(std::shared_ptr<mdnsService>), int protocol = PROTO_UNSPEC);
		void browse(const std::string t, int protocol = PROTO_UNSPEC);
		void createService(mdnsService &s);
		void updateService(mdnsService &s);
		void removeService();
		void loop();

	private:
		void init();
		static void clientCallback(AvahiClient *c, AvahiClientState state, AVAHI_GCC_UNUSED void * userdata);
		static void entryGroupCallback(AvahiEntryGroup *g, AvahiEntryGroupState state, AVAHI_GCC_UNUSED void *userdata);
		static void browseCallback(AvahiServiceBrowser *b,
			AvahiIfIndex interface,
			AvahiProtocol protocol,
			AvahiBrowserEvent event,
			const char *name,
			const char *type,
			const char *domain,
			AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
			void* userdata);
		static void resolveCallback (AvahiServiceResolver *r,
    		AVAHI_GCC_UNUSED AvahiIfIndex interface,
    		AVAHI_GCC_UNUSED AvahiProtocol protocol,
    		AvahiResolverEvent event,
    		const char *name,
    		const char *type,
    		const char *domain,
    		const char *host_name,
    		const AvahiAddress *address,
    		uint16_t port,
    		AvahiStringList *txt,
    		AvahiLookupResultFlags flags,
    		AVAHI_GCC_UNUSED void* userdata);

		AvahiClient *client = NULL;
		AvahiServiceBrowser *sb = NULL;
		int protocol = PROTO_UNSPEC;
	};
}
