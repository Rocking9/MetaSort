# Project MetaSort 🚀

A high-performance, local, C++ based media ingest and curation engine. MetaSort automatically traverses messy directories, extracts hidden EXIF metadata, deduplicates files using cryptographic hashing, and routes them into chronological physical folders using zero-space OS Hard Links.

## 🛠️ Tech Stack (Current)
* **Language:** C++17
* **Compiler:** MinGW (g++)
* **Build System:** Directory-flagged manual compilation

## 📚 Acknowledgements & References
* **[TinyEXIF](https://github.com/cdcseacave/TinyEXIF):** A lightweight, header-only C++ library used for extracting `DateTimeOriginal` and camera model data directly from the binary headers of JPEG files.

* **[tinyxml2](https://github.com/leethomason/tinyxml2):** An efficient C++ XML parser required as a dependency by TinyEXIF to handle modern Adobe XMP metadata tags.



## 🚧 Status
* [x] Project Architecture & Folder Setup
* [ ] Core EXIF Extraction Engine
* [ ] Hash-Based Deduplication (std::unordered_set)
* [ ] OS-Level Hard Link Routing
* [ ] Local HTML/CSS GUI (via Webview)


# Project MetaSort 🚀

A high-performance, local, C++ based media ingest and curation engine. MetaSort automatically traverses messy directories, extracts hidden EXIF metadata, deduplicates files using cryptographic hashing, and routes them into chronological physical folders using zero-space OS Hard Links.

## 🛠️ Tech Stack (Current)
* **Language:** C++17
* **Compiler:** MinGW (g++)
* **Build System:** Directory-flagged manual compilation

## 📚 Acknowledgements & References
* **[TinyEXIF](https://github.com/cdcseacave/TinyEXIF):** A lightweight, header-only C++ library used for extracting `DateTimeOriginal` and camera model data directly from the binary headers of JPEG files.

* **[tinyxml2](https://github.com/leethomason/tinyxml2):** An efficient C++ XML parser required as a dependency by TinyEXIF to handle modern Adobe XMP metadata tags.



## 🚧 Status
* [x] Project Architecture & Folder Setup
* [ ] Core EXIF Extraction Engine
* [ ] Hash-Based Deduplication (std::unordered_set)
* [ ] OS-Level Hard Link Routing
* [ ] Local HTML/CSS GUI (via Webview)

