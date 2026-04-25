#ifndef FILE_PROCESSOR_H
#define FILE_PROCESSOR_H

#include "DeclensionDatabase.h"
#include <string>
#include <vector>

class FileProcessor {
public:
    explicit FileProcessor(const DeclensionDatabase& db);

    std::vector<std::string> processFile(
        const std::string& inputPath,
        const std::string& outputPath
    );

private:
    const DeclensionDatabase& database_;
};

#endif
