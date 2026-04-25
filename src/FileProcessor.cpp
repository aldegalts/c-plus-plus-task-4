#include "FileProcessor.h"
#include "Logger.h"
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <numeric>

FileProcessor::FileProcessor(const DeclensionDatabase& db)
    : database_(db) {}

std::vector<std::string> FileProcessor::processFile(
    const std::string& inputPath,
    const std::string& outputPath)
{
    std::ifstream inFile(inputPath);
    if (!inFile.is_open()) {
        throw std::runtime_error("\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u043e\u0442\u043a\u0440\u044b\u0442\u044c \u0432\u0445\u043e\u0434\u043d\u043e\u0439 \u0444\u0430\u0439\u043b: " + inputPath);
    }

    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        throw std::runtime_error("\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u043e\u0442\u043a\u0440\u044b\u0442\u044c \u0432\u044b\u0445\u043e\u0434\u043d\u043e\u0439 \u0444\u0430\u0439\u043b: " + outputPath);
    }

    Logger::instance().log(LogLevel::INFO,
        "\u041d\u0430\u0447\u0430\u043b\u043e \u043e\u0431\u0440\u0430\u0431\u043e\u0442\u043a\u0438 \u0444\u0430\u0439\u043b\u0430: " + inputPath);

    std::vector<std::string> inputWords;
    std::string line;
    while (std::getline(inFile, line)) {
        auto start = std::find_if_not(line.begin(), line.end(),
            [](unsigned char ch) { return std::isspace(ch); });
        auto end = std::find_if_not(line.rbegin(), line.rend(),
            [](unsigned char ch) { return std::isspace(ch); }).base();
        if (start < end) {
            inputWords.emplace_back(start, end);
        }
    }

    std::vector<std::string> results;
    results.reserve(inputWords.size());

    int successCount = 0;

    std::for_each(inputWords.begin(), inputWords.end(),
        [this, &results, &outFile, &successCount](const std::string& word) {
            std::string resultLine;
            try {
                auto [nominative, caseName] = database_.lookup(word);
                resultLine = nominative + " (" + caseName + ")";
                ++successCount;
                Logger::instance().log(LogLevel::INFO,
                    "\u0421\u043b\u043e\u0432\u043e '" + word + "' -> " + resultLine);
            } catch (const std::exception& e) {
                resultLine = word + " (\u043d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d\u043e)";
                Logger::instance().log(LogLevel::WARNING,
                    "\u0421\u043b\u043e\u0432\u043e '" + word + "' \u043d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d\u043e \u0432 \u0431\u0430\u0437\u0435");
            }
            outFile << resultLine << std::endl;
            results.push_back(std::move(resultLine));
        }
    );

    int failCount = static_cast<int>(inputWords.size()) - successCount;

    Logger::instance().log(LogLevel::INFO,
        "\u041e\u0431\u0440\u0430\u0431\u043e\u0442\u043a\u0430 \u0437\u0430\u0432\u0435\u0440\u0448\u0435\u043d\u0430: " +
        std::to_string(successCount) + " \u0443\u0441\u043f\u0435\u0448\u043d\u043e, " +
        std::to_string(failCount) + " \u043d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d\u043e. \u0420\u0435\u0437\u0443\u043b\u044c\u0442\u0430\u0442 \u0437\u0430\u043f\u0438\u0441\u0430\u043d \u0432: " + outputPath);

    return results;
}
