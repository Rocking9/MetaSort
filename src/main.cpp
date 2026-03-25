#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map> // UPGRADE 1: True O(1) Hash Map
#include <filesystem>
#include "TinyEXIF.h" 

namespace fs = std::filesystem;

// --- HELPER FUNCTION: O(1) Hash Map ---
std::string getMonthName(const std::string& monthNum) {
    static const std::unordered_map<std::string, std::string> monthMap = {
        {"01", "01_January"}, {"02", "02_February"}, {"03", "03_March"},
        {"04", "04_April"},   {"05", "05_May"},      {"06", "06_June"},
        {"07", "07_July"},    {"08", "08_August"},   {"09", "09_September"},
        {"10", "10_October"}, {"11", "11_November"}, {"12", "12_December"}
    };

    auto it = monthMap.find(monthNum);
    if (it != monthMap.end()) {
        return it->second;
    }
    return "Unknown_Month"; 
}

int main() {
    // Point to the entire FOLDER now, not a single file.
    std::string sourceDir = "D:\\Project_MetaSort\\test_data"; 
    std::string outputDirBase = "D:\\Project_MetaSort\\Sorted_Output";

    std::cout << "--- MetaSort Batch Engine Starting ---\n";
    std::cout << "Scanning Directory: " << sourceDir << "\n\n";

    // UPGRADE 3: The Recursive Directory Crawler
    for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) {
        
        // Skip folders, we only want to read actual files
        if (!entry.is_regular_file()) continue;

        std::string imagePath = entry.path().string();
        std::string ext = entry.path().extension().string();
        
        // Security Check: Only process JPEGs
        if (ext != ".jpg" && ext != ".jpeg" && ext != ".JPG" && ext != ".JPEG") {
            continue; 
        }

        std::cout << "Processing: " << entry.path().filename().string() << "\n";

        std::ifstream file(imagePath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "  [ERROR] Could not read file.\n";
            continue; // Skip to the next photo in the loop
        }

        TinyEXIF::EXIFInfo imageEXIF(file);

        // Ensure the EXIF data exists AND the date string is the full 19 characters (YYYY:MM:DD HH:MM:SS)
        if (imageEXIF.Fields && imageEXIF.DateTimeOriginal.length() >= 19) {
            std::string rawDate = imageEXIF.DateTimeOriginal; 
            
            // UPGRADE 2: DYNAMIC NAMING SLICES
            std::string year   = rawDate.substr(0, 4);   
            std::string month  = rawDate.substr(5, 2);
            std::string day    = rawDate.substr(8, 2);
            std::string hour   = rawDate.substr(11, 2);
            std::string minute = rawDate.substr(14, 2);
            std::string second = rawDate.substr(17, 2);
            
            std::string folderName = getMonthName(month);
            std::string targetDir = outputDirBase + "\\" + year + "\\" + folderName;
            
            // Construct the dynamic filename: YYYY-MM-DD_HH-MM-SS.jpg
            std::string newFileName = year + "-" + month + "-" + day + "_" + hour + "-" + minute + "-" + second + ext;
            std::string targetFile = targetDir + "\\" + newFileName;

            try {
                fs::create_directories(targetDir);

                if (!fs::exists(targetFile)) {
                    fs::create_hard_link(imagePath, targetFile);
                    std::cout << "  [SUCCESS] Routed to -> " << newFileName << "\n";
                } else {
                    std::cout << "  [SKIP] File already exists at destination.\n";
                }
            } 
            catch (const fs::filesystem_error& e) {
                std::cerr << "  [OS ERROR] " << e.what() << "\n";
            }
        } else {
            std::cerr << "  [WARNING] No valid EXIF date found. Skipping.\n";
        }
    }

    std::cout << "\n--- Batch Processing Complete ---\n";
    return 0;
}