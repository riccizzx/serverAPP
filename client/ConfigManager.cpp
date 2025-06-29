#include "ConfigManager.hpp"
#include <algorithm>

//initializes the file path
ConfigManager::ConfigManager(const std::string& filepath): filePath_(filepath) {

}

// Utility function to trim leading/trailing whitespace from a string.
std::string ConfigManager::trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\n\r\f\v");
	if (std::string::npos == first) {
		return str;
	}
	size_t last = str.find_last_not_of(" \t\n\r\f\v");
	return str.substr(first, (last - first + 1));
}

// load the configuration data from INI file

bool ConfigManager::loading() {
	std::ifstream file(filePath_); // open the configuration file
	if (!file.is_open()) {//check if file is not open
		std::cerr << "ERROR: Could not open the INI file: " << filePath_ << "\n";
		return false; // file not found
	}
	std::string line; 
	std::string currentSection;

	// loop for verify the INI file: check until they find the IP and PORT
	while (std::getline(file, line)) { //read the file line by line
		line = trim(line);

		if (line.empty() || line[0] == '#' || line[0] == ';') {
			//skip empty lines or comment lines
			continue;
		}
		if (line[0] == '[' && line.back() == ']') {
			//section for the [Network]
			currentSection = line.substr(1, line.length() - 2);
			// Optionally store section info, but for simple key-value, not strictly needed.
			continue;
		}
		size_t delimiterPos = line.find('=');
		if (delimiterPos != std::string::npos) {
			//extract key and value from the file
			std::string key = trim(line.substr(0, delimiterPos));
			std::string value = trim(line.substr(delimiterPos + 1));
			configData_[key] = value;
		}
 	}
	file.close(); 
	return true; // config LOAD sucessfull
}

// retrives a string value by key
std::string ConfigManager::getString(const std::string& key) const {
	auto it = configData_.find(key);
	if (it != configData_.end()) {
		return it->second;
	}
	return ""; // return empty string if KEY cant be find
}

//retrives an integer value by key
int ConfigManager::getInt(const std::string& key) const {
	std::string valueStr = getString(key);
	if (!valueStr.empty()) {
		try {
			return std::stoi(valueStr); // convert string to INT
		}
		catch (const std::exception& e) {
			std::cerr << "WARNING: Falied to convert value for key " << key << " to integer. ERROR: " << e.what() << "\n";
		}
	}
	return 0; // return 0 if key not found or conversion fails.
}

