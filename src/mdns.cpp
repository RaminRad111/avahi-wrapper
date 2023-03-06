#include "avahi_wrapper/mdns.hpp"

namespace avahi_wrapper {

	std::vector<std::shared_ptr<mdnsService>> services;

	mDNS::mDNS() {
		mDNS::init();
	}

	mDNS::~mDNS() {
		if (client)
			avahi_client_free(client);

		if (simple_poll)
			avahi_simple_poll_free(simple_poll);

		if (sb)
			avahi_service_browser_free(sb);
	}

	void mDNS::init() {
		if (!(simple_poll = avahi_simple_poll_new())) {
			std::cout << "Failed to Create Simple Poll Object\n";
			delete(this);
			return;
		}

		client = avahi_client_new(avahi_simple_poll_get(simple_poll), (AvahiClientFlags)0, mDNS::clientCallback, NULL, nullptr);
	}

	void mDNS::clientCallback(AvahiClient *c, AvahiClientState state, AVAHI_GCC_UNUSED void * userdata) {
		switch (state) {
		case AVAHI_CLIENT_S_REGISTERING:
			std::cout << "Client State: AVAHI_CLIENT_S_REGISTERING\n";
			if (group)
				avahi_entry_group_reset(group);
			break;
		case AVAHI_CLIENT_S_RUNNING:
			std::cout << "Client State: AVAHI_CLIENT_S_RUNNING\n";
			break;
		case AVAHI_CLIENT_S_COLLISION:
			std::cout << "Client State: AVAHI_CLIENT_S_COLLISION\n";
			break;
		case AVAHI_CLIENT_FAILURE:
			std::cout << "Client State: AVAHI_CLIENT_FAILURE\n";
			avahi_simple_poll_quit(simple_poll);
			break;
		case AVAHI_CLIENT_CONNECTING:
			std::cout << "Client State: AVAHI_CLIENT_CONNECTING\n";
			break;
		}
	}

	void mDNS::entryGroupCallback(AvahiEntryGroup *g, AvahiEntryGroupState state, AVAHI_GCC_UNUSED void *userdata) {
		switch (state) {
		case AVAHI_ENTRY_GROUP_UNCOMMITED:
			std::cout << "Entry Group State: AVAHI_ENTRY_GROUP_UNCOMMITED\n";
			break;
		case AVAHI_ENTRY_GROUP_REGISTERING:
			std::cout << "Entry Group State: AVAHI_ENTRY_GROUP_REGISTERING\n";
			break;
		case AVAHI_ENTRY_GROUP_ESTABLISHED:
			std::cout << "Entry Group State: AVAHI_ENTRY_GROUP_ESTABLISHED\n";
			break;
		case AVAHI_ENTRY_GROUP_COLLISION:
			std::cout << "Entry Group State: AVAHI_ENTRY_GROUP_COLLISION\n";
			break;
		case AVAHI_ENTRY_GROUP_FAILURE:
			std::cout << "Entry Group State: AVAHI_ENTRY_GROUP_FAILURE\n";
			std::cout << avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g))) << std::endl;
			avahi_simple_poll_quit(simple_poll);
			break;
		}
	}

	void mDNS::resolveCallback (AvahiServiceResolver *r,
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
    	AVAHI_GCC_UNUSED void* userdata) {
		

		switch (event) {
		case AVAHI_RESOLVER_FAILURE:
			std::cout << "Resolver State: AVAHI_RESOLVER_FAILURE\n";
			for (auto s : services) {
				if (s->name == name && s->type == type) {
					remove(s);
					break;
				}
			}
			avahi_service_resolver_free(r);
			break;
		case AVAHI_RESOLVER_FOUND:
			// std::cout << "Resolver State: AVAHI_RESOLVER_FOUND\n";
			char a[AVAHI_ADDRESS_STR_MAX], *t;
			avahi_address_snprint(a, sizeof(a), address);
			std::shared_ptr<mdnsService> service;
			service = std::shared_ptr<mdnsService>(new mdnsService());
			
			service->name = std::string(name, strlen(name));
			service->type = std::string(type, strlen(type));
			service->port = (unsigned int)(port);
			service->ip = std::string(a, strlen(a));
			t = avahi_string_list_to_string(txt);
			service->txt.push_back(std::string(t, strlen(t)));
			add(service);
			break;
		}

		// avahi_service_resolver_free(r);
	}

	void mDNS::browseCallback(AvahiServiceBrowser *b,
		AvahiIfIndex interface,
		AvahiProtocol protocol,
		AvahiBrowserEvent event,
		const char *name,
		const char *type,
		const char *domain,
		AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
		void* userdata) {

		AvahiClient *c = (AvahiClient*)userdata;

		switch (event) {
		case AVAHI_BROWSER_NEW:
			// std::cout << "Browser State: AVAHI_BROWSER_NEW\n";
			if (!(avahi_service_resolver_new(c, interface, protocol, name, type, domain, protocol, (AvahiLookupFlags)0, resolveCallback, userdata)))
        		std::cout << "Failed to resolve service\n";
			break;
		case AVAHI_BROWSER_REMOVE:
			std::cout << "Browser State: AVAHI_BROWSER_REMOVE\n";
			for (auto s : services) {
				if (s->name == name && s->type == type) {
					remove(s);
					break;
				}
			}
			break;
		case AVAHI_BROWSER_CACHE_EXHAUSTED:
			std::cout << "Browser State: AVAHI_BROWSER_CACHE_EXHAUSTED\n";
			break;
		case AVAHI_BROWSER_ALL_FOR_NOW:
			std::cout << "Browser State: AVAHI_BROWSER_ALL_FOR_NOW\n";
			break;
		case AVAHI_BROWSER_FAILURE:
			std::cout << "Browser State: AVAHI_BROWSER_FAILURE\n";
			break;
		}
	}

	void add(std::shared_ptr<mdnsService> s) {
		services.push_back(s);

		if (serviceAddedCallback != nullptr)
			serviceAddedCallback(s);
	}

	void remove(std::shared_ptr<mdnsService> s) {
		auto it = std::find(services.begin(), services.end(), s);
		services.erase(it);

		if (serviceRemovedCallback != nullptr)
			serviceRemovedCallback(s);
	}

	void mDNS::createService(mdnsService &s) {
		int ret;

		if (!group) {
    		if (!(group = avahi_entry_group_new(client, mDNS::entryGroupCallback, NULL))) {
    			std::cout << "avahi_entry_group_new() failed\n";
    			avahi_simple_poll_quit(simple_poll);
    			return;
    		}
    	}

    	AvahiStringList *list = avahi_string_list_new(NULL, NULL);
    	if (!s.txt.empty())
	    	for (auto &str : s.txt)
    			list = avahi_string_list_add(list, str.c_str());

    	if (s.name == "default")
    		s.name = (std::string)avahi_client_get_host_name(client);

    	if ((ret = avahi_entry_group_add_service_strlst(group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, (AvahiPublishFlags)0, s.name.c_str(), s.type.c_str(), NULL, NULL, (uint16_t)s.port, list)) < 0) {
   			if (ret == AVAHI_ERR_COLLISION) {
   				std::cout << "Name Collision\n";
   				return;
   			}

    		std::cout << "Failed to add service\n";
    		avahi_simple_poll_quit(simple_poll);
    		return;
    	}

    	if ((ret = avahi_entry_group_commit(group)) < 0) {
    		std::cout << "Failed to commit entry group: " << avahi_strerror(ret) << std::endl;
    		avahi_simple_poll_quit(simple_poll);
    		return;
    	}

		avahi_string_list_free(list);
    	return;
	}

	void mDNS::updateService(mdnsService &s) {
		if (avahi_client_get_state(client) != AVAHI_CLIENT_S_RUNNING || avahi_entry_group_get_state(group) == AVAHI_ENTRY_GROUP_FAILURE) {
			std::cout << "Update Failed with Error\n";
			return;
		}

		removeService();
		createService(s);
	}

	void mDNS::removeService() {
		if (avahi_client_get_state(client) == AVAHI_CLIENT_S_RUNNING) {
			if (group)
				avahi_entry_group_reset(group);
		}
	}

	void mDNS::browse(const std::string t, void (*addCallback)(std::shared_ptr<mdnsService> s), void (*removeCallback)(std::shared_ptr<mdnsService> s), int protocol) {
  		browse(t, protocol);

  		serviceAddedCallback = addCallback;
  		serviceRemovedCallback = removeCallback;
	}

	void mDNS::browse(const std::string t, int protocol) {
		if (avahi_client_get_state(client) != AVAHI_CLIENT_S_RUNNING)
			return;

		if (!(sb = avahi_service_browser_new(client, AVAHI_IF_UNSPEC, protocol, t.c_str(), NULL, (AvahiLookupFlags)0, browseCallback, client))) {
    		std::cout << "Failed to create service browser\n";
  		}
	}

	void mDNS::loop() {
		avahi_simple_poll_loop(simple_poll);
	}
}
