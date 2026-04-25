#ifndef DECLENSION_DATABASE_H
#define DECLENSION_DATABASE_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <utility>

using DeclensionEntry = std::map<std::string, std::string>;

class DeclensionDatabase {
public:
    void loadFromFile(const std::string& filePath);

    std::pair<std::string, std::string> lookup(const std::string& wordForm) const;

    const std::vector<DeclensionEntry>& getAllEntries() const;
    std::string getNominative(size_t index) const;

    bool isEmpty() const;
    void clear();
    size_t wordCount() const;

    static const std::vector<std::string>& caseNames();

private:
    std::vector<DeclensionEntry> entries_;
    std::unordered_map<std::string, std::pair<size_t, std::string>> reverseLookup_;

    void buildReverseLookup();
    static std::string trim(const std::string& s);
    static std::string toLower(const std::string& s);
};

#endif
