#ifndef SCALARRESULTS_H
#define SCALARRESULTS_H

#include "IScalarResultReceiver.h"
#include "ScalarResult.h"
#include <map>
#include <vector>
#include <optional>
#include <string>
#include <iterator>

class ScalarResults : public IScalarResultReceiver {
public:
    virtual ~ScalarResults();
    std::optional<ScalarResult> operator[](const std::string& tradeId) const;

    bool containsTrade(const std::string& tradeId) const;

    virtual void addResult(const std::string& tradeId, double result) override;

    virtual void addError(const std::string& tradeId, const std::string& error) override;

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = ScalarResult;
        using difference_type = std::ptrdiff_t;
        using pointer = ScalarResult*;
        using reference = ScalarResult&;

        Iterator() = default;
        Iterator(std::vector<std::string>::const_iterator it, const ScalarResults* parent)
            : it_(it), parent_(parent) {}

        // Iterator must be constructable from ScalarResults parent
        Iterator& operator++() { ++it_; return *this; }

        ScalarResult operator*() const {
            return (*parent_)[*it_].value(); // safe to unwrap, tradeIds_ vector will never return std::nullopt (internal iterator use guarantees string is in either results_ or errors_; see .cpp implementation)
        }

        bool operator!=(const Iterator& other) const {
            return it_ != other.it_;
        }

    private: 
        std::vector<std::string>::const_iterator it_;
        const ScalarResults* parent_ = nullptr;
    };

    Iterator begin() const { return Iterator(tradeIds_.begin(), this); }
    Iterator end() const { return Iterator(tradeIds_.end(), this); }

private:
    std::map<std::string, double> results_;
    std::map<std::string, std::string> errors_;
    std::vector<std::string> tradeIds_; // Minimal vector to track unique trades for iteration
};

#endif // SCALARRESULTS_H
