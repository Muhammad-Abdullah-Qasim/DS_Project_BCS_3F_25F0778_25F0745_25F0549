# MayDay-404 — Real-Time Air Traffic Control Emergency Simulator

**Course:** CL2001 – Data Structures (BCS-3F, Fall 2026)  
**Institution:** National University of Computer & Emerging Sciences (FAST-NUCES), Faisalabad-Chiniot Campus  
**Repository:** [DS_Project_BCS_3F_25F0778_25F0745_25F0549](https://github.com/Muhammad-Abdullah-Qasim/DS_Project_BCS_3F_25F0778_25F0745_25F0549)

---

## 1. Project Overview
**MayDay-404** is a menu-driven, turn-based console Air Traffic Control (ATC) emergency simulation game developed in C++ without using the Standard Template Library (STL). Acting as the **Chief Radar Officer (CRO)**, the player manages active airspace traffic, fuel consumption, runway clearances, and escalating emergencies (hijacks, cyber-attacks, missing aircraft, and hostile incursions) across 30 simulation ticks using custom-built data structures and graph algorithms.

---

## 2. Team Members (Section BCS-3F)

| Name | Roll Number | Modules & Work Description |
| :--- | :--- | :--- |
| **Muhammad Abdullah** | `25F-0745` | Linear Structures & Core Integration (Registry Linked List, Linear Probing Hash Table, GameEngine Loop) |
| **Abdul Basit** | `25F-0778` | Priority Dispatch, History & Trees (Circular Queue, Two-Field Emergency Min-Heap, Command History/Undo Stack, BST & AVL Trees) |
| **Muhammad Qasim** | `25F-0549` | Airspace Network & Persistence (Dual Adjacency List/Matrix Graph, BFS/DFS Radar Sweep, Dijkstra Shortest Path, Kruskal/Prim MST, Leaderboard Persistence) |

---

## 3. Data Structures & Algorithms Implemented (Zero STL)
Every core mechanic is driven by a custom data structure built from scratch using dynamic memory allocation (`new` / `delete`):
* **Singly-Linked List & Linear Probing Hash Table (`Registration`):** Primary ownership of active flight/crew records and $O(1)$ average callsign lookup.
* **Circular Queue & Two-Field Min-Heap (`EmergencyLanding`):** FIFO normal arrival sequencing and priority-ordered emergency dispatch (`priorityLevel`, tie-broken by `arrivalTick`).
* **Doubly-Linked List & Linked Stack (`CommandHistory`):** Reverse-chronological command audit trail and LIFO tactical command rollback (`Undo`)[cite: 15].
* **Binary Search Tree & AVL Tree (`BSTflight`, `AVLflight`):** Ordered alphabetical flight manifests and self-balancing height comparisons[cite: 15].
* **Weighted Graph — Adjacency List & Matrix (`SpaceGraph`):** Bidirectional airspace corridors and runways[cite: 15].
* **Graph Algorithms (`RadarSweep`, `RouteFinder`, `RadarMST`):** BFS/DFS sector sweeps and missing-flight search, Dijkstra's fastest runway intercept route, and Minimum Spanning Tree radar coverage[cite: 15].
* **Custom File Persistence & Sorting (`LeaderBoard`):** Pipe-delimited `.txt` data parsing and ranked session leaderboard storage[cite: 15].

---

## 4. Build & Run Instructions

### Prerequisites
* **Compiler / IDE:** Microsoft Visual Studio (C++17 or C++20 standard)[cite: 15]
* **Libraries:** Standard I/O and string headers only (`<iostream>`, `<fstream>`, `<string>`, `<iomanip>`, `<ctime>`)[cite: 18]

### Steps to Build & Execute
1. Clone the repository:
   ```bash
   git clone [https://github.com/Muhammad-Abdullah-Qasim/DS_Project_BCS_3F_25F0778_25F0745_25F0549.git](https://github.com/Muhammad-Abdullah-Qasim/DS_Project_BCS_3F_25F0778_25F0745_25F0549.git)