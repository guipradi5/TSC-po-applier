#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <filesystem>
#include <regex>
#include <map>
#include <algorithm>

// Convert string to bytes (UTF-8)
std::vector<unsigned char> stringToBytes(const std::string &str) {
    return std::vector<unsigned char>(str.begin(), str.end());
}

// Replace all occurrences of original with replacement, adjusting the length byte
int replaceAllWithLength(std::vector<unsigned char> &buffer, const std::string &original, const std::string &replacement) {
    auto origBytes = stringToBytes(original);
    auto replBytes = stringToBytes(replacement);
    if (origBytes.empty()) return 0;

    int replacements = 0;
    size_t pos = 0;

    while (pos + 4 <= buffer.size()) {
        // Encontrar fin del texto (antes de los 0x00) empezando en pos
        size_t textEnd = pos;
        while (textEnd < buffer.size() && buffer[textEnd] != 0x00) textEnd++;

        // Tamaño del texto
        size_t textSize = textEnd - pos;

        // Tamaño bloque: múltiplo de 4 bytes que cubre el texto
        size_t blockSize = ((textSize + 3) / 4) * 4;

        // Si no hay suficiente tamaño para el bloque, salir para evitar overflow
        if (pos + blockSize > buffer.size()) break;

        // Extraer texto del bloque para comparación exacta
        std::string blockText(buffer.begin() + pos, buffer.begin() + pos + textSize);

        if (blockText == original) {
            // Reemplazo

            size_t newBlockSize = ((replBytes.size() + 3) / 4) * 4;

            if (newBlockSize > blockSize) {
                size_t extra = newBlockSize - blockSize;
                buffer.insert(buffer.begin() + pos + blockSize, extra, 0x00);
                blockSize = newBlockSize;
            }

            std::copy(replBytes.begin(), replBytes.end(), buffer.begin() + pos);

            if (replBytes.size() < blockSize) {
                std::fill(buffer.begin() + pos + replBytes.size(), buffer.begin() + pos + blockSize, 0x00);
            }

            if (pos >= 4) {
                buffer[pos - 4] = static_cast<unsigned char>(replBytes.size());
            }

            pos += blockSize;
            replacements++;
        } else {
            // Avanzar pos al siguiente bloque (mínimo 4 bytes, no menos)
            pos += (blockSize > 0) ? blockSize : 4;
        }
    }

    return replacements;
}


// Load translations from PO file
std::map<std::string, std::string> loadTranslations(const std::string &poPath) {
    std::map<std::string, std::string> translations;
    std::ifstream file(poPath);
    if (!file) return translations;

    std::string line, msgid, msgstr;
    while (std::getline(file, line)) {
        if (line.rfind("msgid ", 0) == 0) {
            msgid = std::regex_replace(line.substr(6), std::regex("^\"|\"$"), "");
        } else if (line.rfind("msgstr ", 0) == 0) {
            msgstr = std::regex_replace(line.substr(7), std::regex("^\"|\"$"), "");
            if (!msgid.empty() && !msgstr.empty() && msgid != msgstr) {
                translations[msgid] = msgstr;
            }
        }
    }
    return translations;
}

int main() {
    std::string poDir = "po";
    std::string datDir = "dat";
    std::string outDir = "output";

    std::filesystem::create_directory(outDir);

    int totalFiles = 0;
    int totalReplacements = 0;

    for (const auto &entry : std::filesystem::directory_iterator(poDir)) {
        if (entry.path().extension() == ".po") {
            std::string poFile = entry.path().filename().string();
            std::string baseName = entry.path().stem().string();
            std::string datPath = datDir + "/" + baseName;

            std::cout << "\n--- Processing PO file: " << poFile << " ---\n";

            if (!std::filesystem::exists(datPath)) {
                std::cout << "[SKIP] No binary file found for: " << baseName << "\n";
                continue;
            }

            // Load translations
            auto translations = loadTranslations(entry.path().string());
            if (translations.empty()) {
                std::cout << "[INFO] No translations found in " << poFile << "\n";
                continue;
            }

            // Read .dat file
            std::ifstream inFile(datPath, std::ios::binary);
            std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
            inFile.close();

            int fileReplacements = 0;

            // Apply translations
            for (const auto &[orig, repl] : translations) {
                int count = replaceAllWithLength(buffer, orig, repl);
                if (count > 0) {
                    std::cout << "Replaced \"" << orig << "\" => \"" << repl << "\" (" << count << " times)\n";
                    fileReplacements += count;
                }
            }

            // Save modified .dat
            std::string outPath = outDir + "/" + baseName;
            std::ofstream outFile(outPath, std::ios::binary);
            outFile.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());
            outFile.close();

            std::cout << "[DONE] Translated file saved as: " << outPath << "\n";
            std::cout << "[SUMMARY] Total replacements in this file: " << fileReplacements << "\n";

            totalFiles++;
            totalReplacements += fileReplacements;
        }
    }

    std::cout << "\n========================================\n";
    std::cout << "All files processed successfully.\n";
    std::cout << "Processed files: " << totalFiles << "\n";
    std::cout << "Total replacements made: " << totalReplacements << "\n";
    std::cout << "Output folder: " << outDir << "\n";
    std::cout << "========================================\n";
    std::cout << "Press ENTER to exit...";
    std::cin.get();
    return 0;
}
