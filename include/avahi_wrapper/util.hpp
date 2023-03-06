#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include <map>

#include "mdns.hpp"

namespace avahi_wrapper {

	class Parser
	{
	public:
		Parser();
		~Parser();
		
		std::shared_ptr<mdnsService> findByIp(std::string ip);
		std::vector<std::shared_ptr<mdnsService>> findByKeyValue(std::string key, std::string value);
		std::string findValue(std::string ip, std::string key);

	private:
		std::map<std::string, std::string> splitTxtRecords(std::shared_ptr<mdnsService> s);
		std::vector<std::string> split(std::string str, std::string delimiter);
	};
}
