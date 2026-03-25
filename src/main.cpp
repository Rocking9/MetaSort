#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <unordered_set> // FOR O(1) DEDUPLICATION
#include <filesystem>
#include <chrono>        // FOR OS TIMESTAMP FALLBACK
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
    char buffer[4096] = {0}; // Read first 4KB
    f.read(buffer, sizeof(buffer));
    std::string data(buffer, f.gcount());
    
    // Signature = Hash(First 4KB) + File Size
    size_t dataHash = std::hash<std::string>{}(data);
    uintmax_t fileSize = fs::file_size(filepath);
    
    return std::to_string(dataHash) + "_" + std::to_string(fileSize);
}

int main() {
    std::string sourceDir = "D:\\Project_MetaSort\\test_data"; 
    std::string outputDirBase = "D:\\Project_MetaSort\\Sorted_Output";

    // THE GATEKEEPER: An O(1) Hash Set to memorize every photo we process
    std::unordered_set<std::string> processedSignatures;

    std::cout << "--- MetaSort V2 (Deduplication & Fallback Active) ---\n";
    std::cout << "Scanning Directory: " << sourceDir << "\n\n";

    for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) {
        if (!entry.is_regular_file()) continue;

        std::string imagePath = entry.path().string();
        std::string ext = entry.path().extension().string();
        
        if (ext != ".jpg" && ext != ".jpeg" && ext != ".JPG" && ext != ".JPEG") continue; 

        std::cout << "Processing: " << entry.path().filename().string() << "\n";

        // --- PHASE 1: DEDUPLICATION ---
        std::string fileSig = getFileSignature(imagePath);
        if (processedSignatures.find(fileSig) != processedSignatures.end()) {
            std::cout << "  [SKIPPED] Exact duplicate detected. Conserving CPU.\n";
            continue; // Move to the next photo instantly
        }
        processedSignatures.insert(fileSig); // Memorize this file signature

        // --- PHASE 2: METADATA EXTRACTION FUNNEL ---
        std::string rawDate = "";
        std::string originType = ""; // Just to print where we got the date from

        std::ifstream file(imagePath, std::ios::binary);
        TinyEXIF::EXIFInfo imageEXIF(file);

        if (imageEXIF.Fields && imageEXIF.DateTimeOriginal.length() >= 19) {
            // Path A: The standard EXIF route
            rawDate = imageEXIF.DateTimeOriginal; 
            originType = "EXIF";
        } else {
            // Path B: The WhatsApp Fallback (Ask the OS Kernel)
            auto ftime = fs::last_write_time(entry);
            // C++17 Chrono hack to convert OS File Time to Calendar Time
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
            std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
            std::tm* tm_info = std::localtime(&cftime);
            
            // Format it to match the EXIF standard perfectly (YYYY:MM:DD HH:MM:SS)
            char buffer[20];
            std::strftime(buffer, 20, "%Y:%m:%d %H:%M:%S", tm_info);
            rawDate = std::string(buffer);
            originType = "OS Fallback";
        }

        // --- PHASE 3: THE ROUTER (Both paths end up here) ---
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
                    fs::create_hard_link(imagePath, targetFile);
                    std::cout << "  [SUCCESS] " << originType << " Routed -> " << newFileName << "\n";
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