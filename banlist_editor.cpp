#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <algorithm> // Para remover espaços em branco

using Byte = uint8_t;
using Int = int32_t;
using Short = uint16_t;

struct Card {
    Int id;
    std::string name;
    std::string category; // "Forbidden", "Limited", "Semi-Limited"
};

Short ReadShort(std::ifstream &file) {
    Short value;
    file.read(reinterpret_cast<char*>(&value), sizeof(Short));
    if (!file) throw std::runtime_error("Erro ao ler Short");
    return value;
}

void LoadCards(std::unordered_map<Short, std::string> &ID_to_name, std::unordered_map<std::string, Short> &name_to_ID, const std::string &cardFilePath) {
    std::ifstream cardFile(cardFilePath);
    if (!cardFile.is_open()) throw std::runtime_error("Erro ao abrir o arquivo cards.cpp.");

    std::string line;
    while (std::getline(cardFile, line)) {
        try {
            if (line.find("name_to_ID") != std::string::npos) {
                size_t nameStart = line.find("[\"") + 2;
                size_t nameEnd = line.find("\"]");
                size_t idStart = line.find("=") + 1;
                size_t idEnd = line.find(";");

                if (nameStart < nameEnd && idStart < idEnd) {
                    std::string name = line.substr(nameStart, nameEnd - nameStart);
                    std::string idString = line.substr(idStart, idEnd - idStart);

                    idString.erase(std::remove_if(idString.begin(), idString.end(), ::isspace), idString.end());
                    
                    if (!idString.empty() && std::all_of(idString.begin(), idString.end(), ::isdigit)) {
                        Short id = static_cast<Short>(std::stoi(idString));
                        name_to_ID[name] = id;
                        ID_to_name[id] = name;
                    }
                }
            }
        } catch (const std::exception &e) {
            std::cerr << "Erro ao processar linha: " << line << " - " << e.what() << std::endl;
        }
    }

    if (ID_to_name.empty() || name_to_ID.empty()) throw std::runtime_error("Nenhuma carta válida encontrada no arquivo cards.cpp.");
}

void ExtractBanlist(const std::string& banlistFilePath, const std::unordered_map<Short, std::string>& ID_to_name, const std::string& outputTxtFilePath) {
    std::ifstream inputFile(banlistFilePath, std::ios::binary);
    if (!inputFile.is_open()) throw std::runtime_error("Erro ao abrir o arquivo da banlist.");

    inputFile.seekg(0xA8, std::ios::beg); // Offset inicial corrigido para a lista de cartas

    std::vector<Card> banlist;
    while (inputFile.peek() != EOF) {
        try {
            Short category = ReadShort(inputFile);
            Short cardID = ReadShort(inputFile);

            Card card;
            card.id = cardID;
            card.name = ID_to_name.count(cardID) ? ID_to_name.at(cardID) : "Unknown";

            if (category == 0x0000) card.category = "Forbidden";
            else if (category == 0x0001) card.category = "Limited";
            else if (category == 0x0002) card.category = "Semi-Limited";

            banlist.push_back(card);
        } catch (...) {
            break;
        }
    }

    std::ofstream outputFile(outputTxtFilePath);
    if (!outputFile.is_open()) throw std::runtime_error("Erro ao criar o arquivo de saída.");

    outputFile << "[Forbidden]\n";
    for (const auto& card : banlist) {
        if (card.category == "Forbidden") {
            outputFile << card.name << " (" << card.id << ")\n";
        }
    }

    outputFile << "\n[Limited]\n";
    for (const auto& card : banlist) {
        if (card.category == "Limited") {
            outputFile << card.name << " (" << card.id << ")\n";
        }
    }

    outputFile << "\n[Semi-Limited]\n";
    for (const auto& card : banlist) {
        if (card.category == "Semi-Limited") {
            outputFile << card.name << " (" << card.id << ")\n";
        }
    }

    std::cout << "Banlist exportada com sucesso para " << outputTxtFilePath << "!\n";
}

void IncludeBanlistInFile(const std::string& banlistFilePath, const std::string& inputTxtFilePath, const std::unordered_map<std::string, Short>& name_to_ID) {
    std::ifstream inputFile(inputTxtFilePath);
    if (!inputFile.is_open()) throw std::runtime_error("Erro ao abrir o arquivo de entrada da banlist.");

    std::map<std::string, std::vector<Short>> categories = {
        {"Forbidden", {}},
        {"Limited", {}},
        {"Semi-Limited", {}}
    };

    std::string line;
    std::string currentCategory;

    while (std::getline(inputFile, line)) {
        if (line == "[Forbidden]" || line == "[Limited]" || line == "[Semi-Limited]") {
            currentCategory = line.substr(1, line.length() - 2);
        } else {
            size_t nameEnd = line.find(" (");
            if (nameEnd != std::string::npos) {
                std::string cardName = line.substr(0, nameEnd);
                if (name_to_ID.count(cardName)) {
                    categories[currentCategory].push_back(name_to_ID.at(cardName));
                }
            }
        }
    }

    std::ofstream outputFile(banlistFilePath, std::ios::binary | std::ios::in);
    if (!outputFile.is_open()) throw std::runtime_error("Erro ao abrir o arquivo para incluir a banlist.");

    outputFile.seekp(0xA8, std::ios::beg);
    for (const auto& category : categories) {
        for (const auto& cardID : category.second) {
            Short categoryCode = (category.first == "Forbidden") ? 0x0000 :
                                 (category.first == "Limited") ? 0x0001 : 0x0002;

            outputFile.write(reinterpret_cast<const char*>(&categoryCode), sizeof(Short));
            outputFile.write(reinterpret_cast<const char*>(&cardID), sizeof(Short));
        }
    }

    std::cout << "Banlist incluída com sucesso no arquivo " << banlistFilePath << "!\n";
}

int main() {
    const std::string banlistFilePath = "SampleBanList.YGL";
    const std::string cardFilePath = "cards.cpp";
    const std::string outputTxtFilePath = "banlist.txt";
    const std::string inputTxtFilePath = "banlist_input.txt";

    try {
        std::unordered_map<Short, std::string> ID_to_name;
        std::unordered_map<std::string, Short> name_to_ID;
        LoadCards(ID_to_name, name_to_ID, cardFilePath);

        std::cout << "Escolha uma opção: [1] Extrair Banlist [2] Incluir Banlist\n";
        int opcao;
        std::cin >> opcao;

        if (opcao == 1) {
            ExtractBanlist(banlistFilePath, ID_to_name, outputTxtFilePath);
        } else if (opcao == 2) {
            IncludeBanlistInFile(banlistFilePath, inputTxtFilePath, name_to_ID);
        } else {
            std::cout << "Opção inválida!\n";
        }
    } catch (const std::exception &e) {
        std::cerr << "Erro: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
