#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "filehandling.h"
#include "cards.h"

struct BanlistEntry {
    unsigned short cardId;
    unsigned short category;
};

std::unordered_map<unsigned short, std::string> ID_to_name;
std::unordered_map<std::string, unsigned short> name_to_ID;

std::string GetCardName(unsigned short cardId) {
    auto it = ID_to_name.find(cardId);
    if (it != ID_to_name.end()) {
        return it->second;
    }
    return "Unknown";
}

void ExtractYGL(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream file(inputFile, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open " << inputFile << std::endl;
        return;
    }

    file.seekg(168, std::ios::beg);
    unsigned short cardCount = ReadShort(file);

    std::vector<BanlistEntry> entries;
    for (int i = 0; i < cardCount; ++i) {
        BanlistEntry entry;
        entry.cardId = ReadShort(file);
        entry.category = ReadShort(file);
        if (ID_to_name.find(entry.cardId) != ID_to_name.end()) {
            entries.push_back(entry);
        } else {
            std::cerr << "Warning: Unknown CardID " << entry.cardId << " encountered." << std::endl;
        }
    }

    file.close();

    std::ofstream outFile(outputFile);
    if (!outFile) {
        std::cerr << "Failed to open " << outputFile << std::endl;
        return;
    }

    outFile << "CardCount: " << entries.size() << std::endl;

    outFile << "\nForbidden:\n";
    for (const auto& entry : entries) {
        if (entry.category == 0) {
            outFile << "CardID: " << entry.cardId << " (" << GetCardName(entry.cardId) << ")\n";
        }
    }

    outFile << "\nLimited:\n";
    for (const auto& entry : entries) {
        if (entry.category == 1) {
            outFile << "CardID: " << entry.cardId << " (" << GetCardName(entry.cardId) << ")\n";
        }
    }

    outFile << "\nSemi-Limited:\n";
    for (const auto& entry : entries) {
        if (entry.category == 2) {
            outFile << "CardID: " << entry.cardId << " (" << GetCardName(entry.cardId) << ")\n";
        }
    }

    outFile.close();
    std::cout << "Extracted " << entries.size() << " entries to " << outputFile << std::endl;
}

void CompressYGL(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile);
    if (!inFile) {
        std::cerr << "Failed to open " << inputFile << std::endl;
        return;
    }

    std::vector<BanlistEntry> forbiddenEntries, limitedEntries, semiLimitedEntries;
    std::string line;
    unsigned short currentCategory = -1;

    while (std::getline(inFile, line)) {
        if (line.find("Forbidden") != std::string::npos) {
            currentCategory = 0;
        } else if (line.find("Limited") != std::string::npos && line.find("Semi-Limited") == std::string::npos) {
            currentCategory = 1;
        } else if (line.find("Semi-Limited") != std::string::npos) {
            currentCategory = 2;
        } else if (line.rfind("CardID:", 0) == 0) {
            BanlistEntry entry;
            size_t idPos = line.find("CardID:") + 7;
            size_t namePos = line.find("(");
            entry.cardId = std::stoi(line.substr(idPos, namePos - idPos));
            entry.category = currentCategory;
            if (ID_to_name.find(entry.cardId) != ID_to_name.end()) {
                if (currentCategory == 0) {
                    forbiddenEntries.push_back(entry);
                } else if (currentCategory == 1) {
                    limitedEntries.push_back(entry);
                } else if (currentCategory == 2) {
                    semiLimitedEntries.push_back(entry);
                }
            } else {
                std::cerr << "Warning: Unknown CardID " << entry.cardId << " in input file. Skipping." << std::endl;
            }
        }
    }
    inFile.close();

    std::ofstream file(outputFile, std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open " << outputFile << std::endl;
        return;
    }

    file.seekp(168, std::ios::beg);
    unsigned short cardCount = forbiddenEntries.size() + limitedEntries.size() + semiLimitedEntries.size();
    WriteShort(file, cardCount);

    auto writeEntries = [&file](const std::vector<BanlistEntry>& entries) {
        for (const auto& entry : entries) {
            WriteShort(file, entry.cardId);
            WriteShort(file, entry.category);
        }
    };

    writeEntries(forbiddenEntries);
    writeEntries(limitedEntries);
    writeEntries(semiLimitedEntries);

    file.close();
    std::cout << "Compressed " << cardCount << " entries to " << outputFile << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " extract|compress input_file output_file" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string inputFile = argv[2];
    std::string outputFile = argv[3];

    LoadCards(ID_to_name, name_to_ID);

    if (mode == "extract") {
        ExtractYGL(inputFile, outputFile);
    } else if (mode == "compress") {
        CompressYGL(inputFile, outputFile);
    } else {
        std::cerr << "Unknown mode: " << mode << std::endl;
        return 1;
    }

    return 0;
}
