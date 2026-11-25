#include "PricingConfigLoader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string PricingConfigLoader::getConfigFile() const {
    return configFile_;
}

void PricingConfigLoader::setConfigFile(const std::string& file) {
    configFile_ = file;
}

PricingEngineConfig PricingConfigLoader::parseXml(const std::string& content) {
    PricingEngineConfig config;
    std::istringstream stream(content);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.find("<Engine") != std::string::npos) {
            PricingEngineConfigItem item;

            // lambda to extract attribute values safely
            auto extractAttr = [&](const std::string& attrName) {
                std::string search = attrName + "=\"";
                size_t start = line.find(search);
                if (start != std::string::npos) {
                    start += search.length();
                    size_t end = line.find("\"", start);
                    if (end != std::string::npos) {
                        return line.substr(start, end - start);
                    }
                }
                return std::string();
            };

            item.setTradeType(extractAttr("tradeType"));
            item.setAssembly(extractAttr("assembly"));
            item.setTypeName(extractAttr("pricingEngine"));
            
            // basic validation
            if (!item.getTradeType().empty()) {
                config.push_back(item);
            }
        }
    }
    return config;
}

PricingEngineConfig PricingConfigLoader::loadConfig() {
    if (configFile_.empty()) {
        throw std::invalid_argument("Config file path is empty");
    }

    std::ifstream file(configFile_);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + configFile_);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return parseXml(buffer.str());
}
