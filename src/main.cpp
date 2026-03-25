#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>
#include "TinyEXIF.h" 
#include <map> //for HashMap

namespace fs = std::filesystem;


// --- HELPER FUNCTION: Maps "02" to "02_February" ---
std::string getMonthName(const std::string& monthNum) {
    // We use a static map so it's only created once in memory, not every time a photo is processed.
    static const std::map<std::string, std::string> monthMap = {
        {"01", "01_January"}, {"02", "02_February"}, {"03", "03_March"},
        {"04", "04_April"},   {"05", "05_May"},      {"06", "06_June"},
        {"07", "07_July"},    {"08", "08_August"},   {"09", "09_September"},
        {"10", "10_October"}, {"11", "11_November"}, {"12", "12_December"}
    };

    auto it = monthMap.find(monthNum);
    if (it != monthMap.end()) {
        return it->second;
    }
    return "Unknown_Month"; // Fallback for corrupted data
}



int main() {
    // 1. Point to our safe test sandbox
    std::string imagePath = "D:\\Project_MetaSort\\test_data\\test_photo.jpg"; 

    std::cout << "--- MetaSort EXIF Extractor Starting ---\n";
    std::cout << "Target File: " << imagePath << "\n\n";

    // 2. Open the file in raw BINARY mode
    std::ifstream file(imagePath, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "[CRITICAL ERROR] Could not open file. Check the path!\n";
        return 1;
    }


    

    // 3. Pass the raw bytes to our EXIF engine
    TinyEXIF::EXIFInfo imageEXIF(file);




    // 4. Verify extraction and Process Data
    if (imageEXIF.Fields) {
        std::string rawDate = imageEXIF.DateTimeOriginal;
        
        std::cout << "[SUCCESS] EXIF Extracted: " << rawDate << "\n";
        
        // --- THE SDE SLICE ---
        // Ensure the string is long enough to prevent out_of_range crashes
        if (rawDate.length() >= 7) { 
            std::string year = rawDate.substr(0, 4);   
            std::string month = rawDate.substr(5, 2);  
            
            std::string folderName = getMonthName(month);
            
            // 1. Build the target directory path
            std::string targetDir = "D:\\Project_MetaSort\\Sorted_Output\\" + year + "\\" + folderName;
            
            // 2. Build the final file path (Directory + Filename)
            // For now, we hardcode the output name. ***We will make this dynamic later.***
            std::string targetFile = targetDir + "\\test_photo_sorted.jpg";

            std::cout << "Routing to : " << targetFile << "\n";

            try {
                // 3. Command the OS to create the nested folders (if they don't exist)
                fs::create_directories(targetDir);

                // 4. Command the OS to create the zero-space Hard Link
                // Check if it already exists to prevent crash on re-run
                if (!fs::exists(targetFile)) {
                    fs::create_hard_link(imagePath, targetFile);
                    std::cout << "[SUCCESS] Hard Link created! Zero space consumed.\n";
                } else {
                    std::cout << "[SKIP] File already exists at destination.\n";
                }
            } 
            catch (const fs::filesystem_error& e) {
                std::cerr << "[CRITICAL OS ERROR] " << e.what() << "\n";
            }

        } else {
            std::cerr << "[WARNING] Date format is corrupted: " << rawDate << "\n";
        }

        std::cout << "----------------------------------\n";
    } else {
        std::cerr << "[FAILED] No EXIF data found. Is it a stripped image?\n";
    }

    return 0;
}