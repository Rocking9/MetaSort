# The MetaSort Engineering Journey

## The Core Problem
I had gigabytes of disorganized, unsorted photos. Standard Windows tools required manual drag-and-drop operations, and copying the files to organize them would have bloated my hard drive and wasted space. I needed a completely automated, zero-space data pipeline.

## Phase 1: Bypassing the OS to Read Raw Binary
My first challenge was that Windows file names are completely unreliable for sorting. I realized the true source of truth was the EXIF metadata written by the camera sensor.
* **The Action:** Instead of relying on text processing, I imported the file as raw binary (`std::ios::binary`) and used a specialized parser (`TinyEXIF`) to hunt for the `0x9003` hex signature. 
* **The SDE Lesson:** I learned how to extract guaranteed strings (`YYYY:MM:DD`) and slice them in $O(1)$ time rather than writing heavy, slow regular expressions.

## Phase 2: Manipulating the Physical File System
Once I had the exact date, I needed to route the files. 
* **The Pivot:** Originally, I considered standard file copying, but realized that organizing a 50GB photo library would instantly cost me another 50GB of disk space. 
* **The Architecture:** I dove into the `<filesystem>` library and utilized **OS-level Hard Links**. By commanding the Windows Kernel to point a new directory name to the exact same physical SSD payload, I achieved instantaneous file routing while consuming exactly 0 bytes of extra storage.

## Phase 3: Building the Pipeline & Handling Edge Cases
A script that processes one perfect file is useless. I had to build a pipeline for the real world.
* **Deduplication:** I realized users have duplicate files. I engineered an $O(1)$ Hash Set (`std::unordered_set`) that hashes the first 4KB of a file + its size to instantly detect and skip duplicates, saving massive CPU cycles.
* **The WhatsApp Edge Case:** I discovered social media platforms strip EXIF data, crashing standard organizers. I engineered a fallback using C++17 `<chrono>` that catches these exceptions, queries the Windows OS for the physical `last_write_time`, translates the timestamp, and routes the file anyway. Zero files left behind.

## The Final Result
I transformed a manual, 3-hour Windows chore into a lightning-fast C++ engine that crawls deeply nested directories, mathematically deduplicates files, and builds a chronological timeline architecture in milliseconds, all without using a single byte of extra disk space.