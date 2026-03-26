# MetaSort: Architecture Decision Record (ADR)

This document tracks the "Why This, Not That" engineering decisions made during the development of the MetaSort engine.

### 1. Project Directory Structure
* **Decision:** SDE-standard nested tree (`src/`, `include/`, `bin/`) instead of a flat directory.
* **Why:** A flat directory is easier to compile initially, but scales poorly. The nested tree isolates source code from compiled binaries, making version control (Git) cleaner and the codebase instantly readable to other engineers.

### 2. EXIF Parsing Engine
* **Decision:** Included `TinyEXIF` (Header-only library) via direct source compilation instead of writing a binary parser from scratch.
* **Why:** Writing a raw JPEG binary parser from scratch would take weeks due to inconsistencies in manufacturer hex tags. `TinyEXIF` abstracts this reliably. We compile it directly alongside `main.cpp` rather than using a Dynamic Link Library (`.dll`) to ensure maximum portability of our final `.exe` without risking "DLL Hell".

### 3. File Routing & Deduplication Architecture (Pending)
* **Decision:** We will use OS-level Hard Links (`std::filesystem::create_hard_link`) instead of Windows Shortcuts (`.lnk`) or proprietary databases.
* **Why:** Hard Links allow a single physical photo to exist in multiple logical folders (e.g., "Sony_Camera" and "Lucknow_Trip") without consuming additional hard drive space. Unlike standard shortcuts, Hard Links do not break if the original file is renamed, and they are natively readable by heavy software like Adobe Premiere.

### 4. Dependency Management (The XML Trap)
* **Decision:** Manually pulled in `tinyxml2.h` and `tinyxml2.cpp` alongside `TinyEXIF`.
* **Why:** Modern images often contain Adobe XMP metadata (written in XML) alongside standard EXIF. `TinyEXIF` has a hidden dependency on `tinyxml2` to parse this. Instead of hacking the library to strip out XML support, we fulfilled the dependency to maintain robust, production-ready code.

### 5. Compiler Execution Strategy
* **Decision:** We use a wildcard compilation command with an explicit include path and console flag: `g++ src/*.cpp -I include -o bin/metasort.exe`
* **Why:** The `*.cpp` wildcard automatically scoops up new source files (like `tinyxml2`) without requiring command-line updates. The `-I include` flag satisfies VS Code IntelliSense and the compiler simultaneously.
# MetaSort: Architecture Decision Record (ADR)

This document tracks the "Why This, Not That" engineering decisions made during the development of the MetaSort engine.

### 1. Project Directory Structure
* **Decision:** SDE-standard nested tree (`src/`, `include/`, `bin/`) instead of a flat directory.
* **Why:** A flat directory is easier to compile initially, but scales poorly. The nested tree isolates source code from compiled binaries, making version control (Git) cleaner and the codebase instantly readable to other engineers.

### 2. EXIF Parsing Engine
* **Decision:** Included `TinyEXIF` (Header-only library) via direct source compilation instead of writing a binary parser from scratch.
* **Why:** Writing a raw JPEG binary parser from scratch would take weeks due to inconsistencies in manufacturer hex tags. `TinyEXIF` abstracts this reliably. We compile it directly alongside `main.cpp` rather than using a Dynamic Link Library (`.dll`) to ensure maximum portability of our final `.exe` without risking "DLL Hell".

### 3. File Routing & Deduplication Architecture (Pending)
* **Decision:** We will use OS-level Hard Links (`std::filesystem::create_hard_link`) instead of Windows Shortcuts (`.lnk`) or proprietary databases.
* **Why:** Hard Links allow a single physical photo to exist in multiple logical folders (e.g., "Sony_Camera" and "Lucknow_Trip") without consuming additional hard drive space. Unlike standard shortcuts, Hard Links do not break if the original file is renamed, and they are natively readable by heavy software like Adobe Premiere.

### 4. Dependency Management (The XML Trap)
* **Decision:** Manually pulled in `tinyxml2.h` and `tinyxml2.cpp` alongside `TinyEXIF`.
* **Why:** Modern images often contain Adobe XMP metadata (written in XML) alongside standard EXIF. `TinyEXIF` has a hidden dependency on `tinyxml2` to parse this. Instead of hacking the library to strip out XML support, we fulfilled the dependency to maintain robust, production-ready code.

### 5. Compiler Execution Strategy
* **Decision:** We use a wildcard compilation command with an explicit include path and console flag: `g++ src/*.cpp -I include -o bin/metasort.exe`
* **Why:** The `*.cpp` wildcard automatically scoops up new source files (like `tinyxml2`) without requiring command-line updates. The `-I include` flag satisfies VS Code IntelliSense and the compiler simultaneously.





### 7. Directory Traversal
* **Decision:** Used `std::filesystem::recursive_directory_iterator` instead of a standard surface-level iterator.
* **Why:** Users frequently nest folders (e.g., `Good Photos \ Vacations \ 2025`). A surface iterator would miss nested data.
* **Pros:** Absolute data thoroughness. Zero hidden files left behind.
* **Cons:** Marginally higher execution time on massive, deeply nested drives, and susceptible to OS-level infinite loops if a system has corrupted shortcut cycles.

### 8. The Deduplication Engine
* **Decision:** Implemented an $O(1)$ Hash Set (`std::unordered_set`) combined with a Partial File Signature (SHA of the first 4KB + Total File Size).
* **Why:** Processing identical files wastes CPU cycles and clutters the final sorted directories.
* **Pros:** Blazingly fast $O(1)$ lookup. By only hashing the first 4,096 bytes instead of the entire 5MB file, we bypass the severe I/O bottleneck of full-file hashing while maintaining 99.99% accuracy.
* **Cons:** Minor RAM footprint to store string signatures during runtime. 

### 9. Edge Case Fallback (The WhatsApp Problem)
* **Decision:** Fallback to OS-level file creation timestamps (`fs::last_write_time`) when EXIF metadata is stripped.
* **Why:** Social media apps strip EXIF data for privacy. The engine must be robust enough to handle broken DNA without leaving files in an "Unsorted" dump folder.
* **Pros:** Guarantees 100% file routing. Maintains general timeline integrity based on when the user received or downloaded the file.
* **Cons:** Accuracy drift. The OS timestamp represents the file's entry onto the hard drive, not the exact millisecond the camera shutter fired.






## Future Roadmap & Feature Expansion

### 1. Hardware-Specific Sub-Routing
* **The Concept:** Utilize the `Make` and `Model` EXIF tags to create sub-directories within the chronological timeline (e.g., `2026 \ 02_February \ Sony_A7IV`).
* **The Value:** Allows professional photographers to instantly separate DSLR RAW files from casual smartphone snapshots.

### 2. "Gallery View" Desktop GUI
* **The Concept:** Move beyond a simple execution UI and build a React/Electron desktop application that acts as a visual gallery.
* **The Value:** Users can visually browse their newly sorted `Sorted_Output` folder with a modern, responsive UI directly inside the app, complete with the SDE warnings about Read-Only modification limits.