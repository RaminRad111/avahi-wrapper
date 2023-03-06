#include <avahi_wrapper/util.hpp>

namespace avahi_wrapper {

	Parser::Parser() {
	}

	Parser::~Parser() {
	}

	std::shared_ptr<mdnsService> Parser::findByIp(std::string ip) {
		for (auto s : services)
			if (s->ip == ip)
				return s;

		return nullptr;
	}

	std::vector<std::shared_ptr<mdnsService>> Parser::findByKeyValue(std::string key, std::string value) {
		std::vector<std::shared_ptr<mdnsService>> result;

		for (auto s : services) {
			std::map<std::string, std::string> records = splitTxtRecords(s);
			if (records[key] == value)
				result.push_back(s);
		}

		return result;
	}

	std::string Parser::findValue(std::string ip, std::string key) {
		std::map<std::string, std::string> records;

		for (auto &s : services) {
			if (s->ip == ip) {
				records = splitTxtRecords(s);
				return (records.find(key) != records.end()) ? records[key] : NULL;
			}
		}

		return NULL;
	}

	std::map<std::string, std::string> Parser::splitTxtRecords(std::shared_ptr<mdnsService> s) {
		std::map<std::string, std::string> result;
		std::vector<std::string> records;

		records = split(s->txt[0], "\"");	

		for (auto &r : records) {
			std::vector<std::string> keyvalue = split(r, "=");
			result.insert({keyvalue[0], keyvalue[1]});
		}

		return result;
	}

	std::vector<std::string> Parser::split(std::string str, std::string delimiter) {
		std::vector<std::string> result;
		std::string substr;
		int start = 0;
		int end = str.find(delimiter);

		while (end != std::string::npos) {
			substr = str.substr(start, end - start);
			if (substr != " " && !substr.empty())
				result.push_back(substr);
			start = end + delimiter.size();
			end = str.find(delimiter, start);
		}

		substr = str.substr(start, end - start);
		if (!substr.empty())
			result.push_back(substr);

		return result;
	}
}
