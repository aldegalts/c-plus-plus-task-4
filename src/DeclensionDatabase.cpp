#include "DeclensionDatabase.h"
#include "Logger.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>

const std::vector<std::string>& DeclensionDatabase::caseNames() {
    static const std::vector<std::string> names = {
        "\u0418\u043c\u0435\u043d\u0438\u0442\u0435\u043b\u044c\u043d\u044b\u0439",
        "\u0420\u043e\u0434\u0438\u0442\u0435\u043b\u044c\u043d\u044b\u0439",
        "\u0414\u0430\u0442\u0435\u043b\u044c\u043d\u044b\u0439",
        "\u0412\u0438\u043d\u0438\u0442\u0435\u043b\u044c\u043d\u044b\u0439",
        "\u0422\u0432\u043e\u0440\u0438\u0442\u0435\u043b\u044c\u043d\u044b\u0439",
        "\u041f\u0440\u0435\u0434\u043b\u043e\u0436\u043d\u044b\u0439"
    };
    return names;
}

void DeclensionDatabase::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u043e\u0442\u043a\u0440\u044b\u0442\u044c \u0444\u0430\u0439\u043b: " + filePath);
    }

    clear();

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        std::string header = line;
        DeclensionEntry entry;

        const auto& cases = caseNames();
        for (size_t i = 0; i < cases.size(); ++i) {
            if (!std::getline(file, line)) {
                throw std::invalid_argument(
                    "\u041d\u0435\u043e\u0436\u0438\u0434\u0430\u043d\u043d\u044b\u0439 \u043a\u043e\u043d\u0435\u0446 \u0444\u0430\u0439\u043b\u0430 \u043f\u0440\u0438 \u0447\u0442\u0435\u043d\u0438\u0438 \u043f\u0430\u0434\u0435\u0436\u0435\u0439 \u0434\u043b\u044f \u0441\u043b\u043e\u0432\u0430 '" + header +
                    "' (\u0441\u0442\u0440\u043e\u043a\u0430 " + std::to_string(lineNumber) + ")"
                );
            }
            ++lineNumber;
            line = trim(line);

            auto colonPos = line.find(':');
            if (colonPos == std::string::npos) {
                throw std::invalid_argument(
                    "\u041e\u0448\u0438\u0431\u043a\u0430 \u0444\u043e\u0440\u043c\u0430\u0442\u0430 \u0432 \u0441\u0442\u0440\u043e\u043a\u0435 " + std::to_string(lineNumber) +
                    ": \u043e\u0436\u0438\u0434\u0430\u043b\u0441\u044f ':' (\u0441\u0442\u0440\u043e\u043a\u0430: '" + line + "')"
                );
            }

            std::string caseName = trim(line.substr(0, colonPos));
            std::string wordForm = trim(line.substr(colonPos + 1));

            if (wordForm.empty()) {
                throw std::invalid_argument(
                    "\u041f\u0443\u0441\u0442\u0430\u044f \u0444\u043e\u0440\u043c\u0430 \u0441\u043b\u043e\u0432\u0430 \u0432 \u0441\u0442\u0440\u043e\u043a\u0435 " + std::to_string(lineNumber)
                );
            }

            entry[caseName] = wordForm;
        }

        entries_.push_back(std::move(entry));

        Logger::instance().log(LogLevel::INFO,
            "\u0417\u0430\u0433\u0440\u0443\u0436\u0435\u043d\u043e \u0441\u043b\u043e\u0432\u043e: " + header);
    }

    buildReverseLookup();

    Logger::instance().log(LogLevel::INFO,
        "\u0417\u0430\u0433\u0440\u0443\u0436\u0435\u043d\u043e " + std::to_string(entries_.size()) +
        " \u0441\u043b\u043e\u0432 \u0438\u0437 \u0444\u0430\u0439\u043b\u0430 " + filePath);
}

std::pair<std::string, std::string> DeclensionDatabase::lookup(const std::string& wordForm) const {
    std::string trimmed = trim(wordForm);
    std::string lower = toLower(trimmed);

    auto it = reverseLookup_.find(lower);
    if (it == reverseLookup_.end()) {
        throw std::runtime_error("\u0421\u043b\u043e\u0432\u043e \u043d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d\u043e: " + trimmed);
    }

    size_t idx = it->second.first;
    const std::string& caseName = it->second.second;

    const auto& entry = entries_.at(idx);
    auto nomIt = entry.find("\u0418\u043c\u0435\u043d\u0438\u0442\u0435\u043b\u044c\u043d\u044b\u0439");
    if (nomIt == entry.end()) {
        throw std::runtime_error("\u0418\u043c\u0435\u043d\u0438\u0442\u0435\u043b\u044c\u043d\u044b\u0439 \u043f\u0430\u0434\u0435\u0436 \u043d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d \u0434\u043b\u044f \u0441\u043b\u043e\u0432\u0430: " + trimmed);
    }

    return {nomIt->second, caseName};
}

const std::vector<DeclensionEntry>& DeclensionDatabase::getAllEntries() const {
    return entries_;
}

std::string DeclensionDatabase::getNominative(size_t index) const {
    if (index >= entries_.size()) {
        throw std::out_of_range("\u0418\u043d\u0434\u0435\u043a\u0441 \u0437\u0430 \u043f\u0440\u0435\u0434\u0435\u043b\u0430\u043c\u0438 \u0434\u0438\u0430\u043f\u0430\u0437\u043e\u043d\u0430: " + std::to_string(index));
    }
    auto it = entries_.at(index).find("\u0418\u043c\u0435\u043d\u0438\u0442\u0435\u043b\u044c\u043d\u044b\u0439");
    if (it == entries_.at(index).end()) {
        return "???";
    }
    return it->second;
}

bool DeclensionDatabase::isEmpty() const {
    return entries_.empty();
}

void DeclensionDatabase::clear() {
    entries_.clear();
    reverseLookup_.clear();
}

size_t DeclensionDatabase::wordCount() const {
    return entries_.size();
}

void DeclensionDatabase::buildReverseLookup() {
    reverseLookup_.clear();

    for (size_t i = 0; i < entries_.size(); ++i) {
        for (const auto& [caseName, wordForm] : entries_[i]) {
            std::string lower = toLower(wordForm);
            if (reverseLookup_.find(lower) != reverseLookup_.end()) {
                Logger::instance().log(LogLevel::WARNING,
                    "\u0414\u0443\u0431\u043b\u0438\u043a\u0430\u0442 \u0444\u043e\u0440\u043c\u044b '" + wordForm +
                    "' \u043f\u0440\u0438 \u043f\u043e\u0441\u0442\u0440\u043e\u0435\u043d\u0438\u0438 \u0438\u043d\u0434\u0435\u043a\u0441\u0430");
            }
            reverseLookup_[lower] = {i, caseName};
        }
    }
}

std::string DeclensionDatabase::trim(const std::string& s) {
    auto start = std::find_if_not(s.begin(), s.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });
    auto end = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base();

    if (start >= end) {
        return "";
    }
    return std::string(start, end);
}

std::string DeclensionDatabase::toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char ch) { return std::tolower(ch); });
    return result;
}
