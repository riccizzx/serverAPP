#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

class ConfigManager {
public:
	// A simple class to read key-value pairs from an INI-like configuration file.
	// class to read key-value from INI file
	ConfigManager(const std::string& filePath);

	// loads the config from the file
	// returns true on success, false on failure.
	bool loading();

	// gets the string value associated with a given key.
	// returns an empty string if the key is not found.
	std::string getString(const std::string& key) const;

	// gets the integer value associated with a given key.
	// returns 0 if the key is not found or if conversion fails.
	int getInt(const std::string& key)const;

private:
	std::string filePath_;// path to the configuration file.
	std::map<std::string, std::string> configData_;// Stores key-value pairs.

	// trims whitespace from both ends of a string.
	std::string trim(const std::string& str);
};

#endif // CONFIG_MANAGER_HPP
