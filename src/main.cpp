#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <unordered_set> 
#include <filesystem>
#include <chrono>        
#include <iomanip>
#include "TinyEXIF.h" 

namespace fs = std::filesystem;

// --- HELPER 1: Month Mapper ---
std::string getMonthName(const std::string& monthNum) {
    static const std::unordered_map<std::string, std::string> monthMap = {
        {"01", "01_January"}, {"02", "02_February"}, {"03", "03_March"},
        {"04", "04_April"},   {"05", "05_May"},      {"06", "06_June"},
        {"07", "07_July"},    {"08", "08_August"},   {"09", "09_September"},
        {"10", "10_October"}, {"11", "11_November"}, {"12", "12_December"}
    };
    auto it = monthMap.find(monthNum);
    return (it != monthMap.end()) ? it->second : "Unknown_Month"; 
}

// --- HELPER 2: File Signature Generator ---
std::string getFileSignature(const std::string& filepath) {
    std::ifstream f(filepath, std::ios::binary);
    char buffer[4096] = {0}; 
    f.read(buffer, sizeof(buffer));
    std::string data(buffer, f.gcount());
    
    size_t dataHash = std::hash<std::string>{}(data);
    uintmax_t fileSize = fs::file_size(filepath);
    
    return std::to_string(dataHash) + "_" + std::to_string(fileSize);
}

// --- PHASE 1: CLI ARGUMENTS ---
int main(int argc, char* argv[]) {
    // 1. Validate terminal inputs
    if (argc != 3) {
        std::cerr << "\n[ERROR] Invalid Arguments!\n";
        std::cerr << "Usage:   metasort <Source_Directory> <Output_Directory>\n";
        std::cerr << "Example: metasort \"D:\\Project_MetaSort\\test_data\" \"D:\\Project_MetaSort\\Sorted_Output\"\n\n";
        return 1; 
    }

    std::string sourceDir = argv[1]; 
    std::string outputDirBase = argv[2];

    if (!fs::exists(sourceDir) || !fs::is_directory(sourceDir)) {
        std::cerr << "\n[CRITICAL ERROR] Source folder does not exist: " << sourceDir << "\n";
        return 1;
    }

    std::unordered_set<std::string> processedSignatures;

    std::cout << "--- MetaSort V3 (Zero-Space Hard Links & CLI) ---\n";
    std::cout << "Scanning Directory: " << sourceDir << "\n";
    std::cout << "Targeting Output:   " << outputDirBase << "\n\n";

    for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) {
        if (!entry.is_regular_file()) continue;

        std::string imagePath = entry.path().string();
        std::string ext = entry.path().extension().string();
        
        if (ext != ".jpg" && ext != ".jpeg" && ext != ".JPG" && ext != ".JPEG") continue; 

        std::cout << "Processing: " << entry.path().filename().string() << "\n";

        // --- DEDUPLICATION ---
        std::string fileSig = getFileSignature(imagePath);
        if (processedSignatures.find(fileSig) != processedSignatures.end()) {
            std::cout << "  [SKIPPED] Exact duplicate detected. Conserving CPU.\n";
            continue; 
        }
        processedSignatures.insert(fileSig); 

        // --- METADATA FUNNEL ---
        std::string rawDate = "";
        std::string originType = ""; 

        std::ifstream file(imagePath, std::ios::binary);
        TinyEXIF::EXIFInfo imageEXIF(file);

        if (imageEXIF.Fields && imageEXIF.DateTimeOriginal.length() >= 19) {
            rawDate = imageEXIF.DateTimeOriginal; 
            originType = "EXIF";
        } else {
            auto ftime = fs::last_write_time(entry);
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
            std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
            std::tm* tm_info = std::localtime(&cftime);
            
            char buffer[20];
            std::strftime(buffer, 20, "%Y:%m:%d %H:%M:%S", tm_info);
            rawDate = std::string(buffer);
            originType = "OS Fallback";
        }

        // --- THE ROUTER ---
        if (rawDate.length() >= 19) {
            std::string year   = rawDate.substr(0, 4);   
            std::string month  = rawDate.substr(5, 2);
            std::string day    = rawDate.substr(8, 2);
            std::string hour   = rawDate.substr(11, 2);
            std::string minute = rawDate.substr(14, 2);
            std::string second = rawDate.substr(17, 2);
            
            std::string folderName = getMonthName(month);
            std::string targetDir = outputDirBase + "\\" + year + "\\" + folderName;
            
            std::string newFileName = year + "-" + month + "-" + day + "_" + hour + "-" + minute + "-" + second + ext;
            std::string targetFile = targetDir + "\\" + newFileName;

            try {
                fs::create_directories(targetDir);

                if (!fs::exists(targetFile)) {
                    // THE MAIN OBJECTIVE: Hard Link
                    fs::create_hard_link(imagePath, targetFile);

                    // THE SAFETY LOCK: Strip write permissions to force "Save As..." behavior
                    fs::permissions(targetFile, fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write, fs::perm_options::remove);

                    std::cout << "  [SUCCESS] " << originType << " Hard Linked & Locked -> " << newFileName << "\n";
                } else {
                    std::cout << "  [SKIP] Filename already exists at destination.\n";
                }
            } 
            catch (const fs::filesystem_error& e) {
                std::cerr << "  [OS ERROR] " << e.what() << "\n";
            }
        }
    }

    std::cout << "\n--- Batch Processing Complete ---\n";
    return 0;
}