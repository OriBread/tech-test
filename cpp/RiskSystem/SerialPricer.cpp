#include "SerialPricer.h"
#include <stdexcept>
#include <iostream>

void SerialPricer::loadPricers() {
    // Clean up existing pricers if this is called multiple times (unique_ptr handles old engines)
    pricers_.clear();

    // Load configuration
    PricingConfigLoader pricingConfigLoader;
    pricingConfigLoader.setConfigFile("./PricingConfig/PricingEngines.xml");
    PricingEngineConfig pricerConfig = pricingConfigLoader.loadConfig();
    
    // Manual factory pattern (C++ has no runtime reflection or dynamic assembly loading)
    for (const auto& configItem : pricerConfig) {
        const std::string& tradeType = configItem.getTradeType();
        const std::string& typeName = configItem.getTypeName();

        std::unique_ptr<IPricingEngine> engine = nullptr;

        if (typeName.find("GovBondPricingEngine") != std::string::npos) {
            engine = std::make_unique<GovBondPricingEngine>();
        }
        else if (typeName.find("CorpBondPricingEngine") != std::string::npos) {
            engine = std::make_unique<CorpBondPricingEngine>();
        }
        else if (typeName.find("FxPricingEngine") != std::string::npos) {
            engine = std::make_unique<FxPricingEngine>();
        }
        
        if (engine) {
            pricers_[tradeType] = std::move(engine);
        }

    }
}

void SerialPricer::price(const std::vector<std::vector<ITrade*>>& tradeContainers, 
                         IScalarResultReceiver* resultReceiver) {
    // Make sure pricers are loaded in before pricing
    if (pricers_.empty()) {
        loadPricers();
    }
    
    for (const auto& tradeContainer : tradeContainers) {
        for (ITrade* trade : tradeContainer) {
            // Original code does 2 lookups on the pricers_ map, replaced with a direct iterator access (1 lookup)
            auto it = pricers_.find(trade->getTradeType());
            if (it == pricers_.end()) {
                resultReceiver->addError(trade->getTradeId(), "No Pricing Engines available for this trade type");
                continue;
            }

            // it->second is the IPricingEngine
            it->second.get()->price(trade, resultReceiver);
        }
    }
}
