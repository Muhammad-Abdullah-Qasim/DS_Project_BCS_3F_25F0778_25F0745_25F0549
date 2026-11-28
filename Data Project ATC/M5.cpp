//#include <iostream>
//#include <string>
//#include <fstream>
//#include <iomanip>
//
//using namespace std;
//
//// ========================================================
//// 1. DATA RECORDS
//// ========================================================
//
//struct Flight {
//    string callsign;
//    string aircraftType;
//    string airline;
//    string origin;
//    string destination;
//    int passengerCount;
//    int fuelRemaining; // Measured in ticks
//    string currentSector;
//    string assignedRunway;
//    int arrivalTick;
//    int priorityLevel; // 1=War ... 9=Normal
//
//    Flight* next;      // Primary owner singly-linked list pointer
//
//    Flight(string cs, string type, string al, string orig, string dest,
//        int pass, int fuel, string sec, string rwy, int arrTick, int prio)
//        : callsign(cs), aircraftType(type), airline(al), origin(orig), destination(dest),
//        passengerCount(pass), fuelRemaining(fuel), currentSector(sec),
//        assignedRunway(rwy), arrivalTick(arrTick), priorityLevel(prio), next(nullptr) {
//    }
//};
//
//struct Crew {
//    string employeeID;
//    string name;
//    string role;
//    string assignedCallsign;
//
//    Crew* next;
//
//    Crew(string id, string n, string r, string cs)
//        : employeeID(id), name(n), role(r), assignedCallsign(cs), next(nullptr) {
//    }
//};
//
//// ========================================================
//// 2. MODULE 1: CUSTOM HASH TABLE (LINEAR PROBING)
//// ==========================================
//struct HashEntry {
//    string key;
//    Flight* flightPtr;
//    bool isTombstone;
//
//    HashEntry() : key(""), flightPtr(nullptr), isTombstone(false) {}
//};
//
//class FlightHashTable {
//private:
//    static const int TABLE_SIZE = 101; // Prime number required by manual
//    HashEntry table[TABLE_SIZE];
//    int itemCount;
//    int collisionCount;
//    int maxProbeChain;
//
//    int hashFunction(const string& key) const {
//        unsigned long hash = 0;
//        for (size_t i = 0; i < key.length(); i++) {
//            hash = (hash * 31 + static_cast<unsigned char>(key[i])) % TABLE_SIZE;
//        }
//        return static_cast<int>(hash);
//    }
//
//public:
//    FlightHashTable() : itemCount(0), collisionCount(0), maxProbeChain(0) {}
//
//    bool insert(const string& callsign, Flight* flight) {
//        if (itemCount >= TABLE_SIZE) return false;
//
//        int index = hashFunction(callsign);
//        int probeCount = 0;
//        int firstTombstone = -1;
//        int existingIndex = -1;
//
//        while (table[index].flightPtr != nullptr || table[index].isTombstone) {
//            if (table[index].isTombstone) {
//                if (firstTombstone == -1) firstTombstone = index;
//            }
//            else if (table[index].key == callsign) {
//                existingIndex = index;
//                break;
//            }
//            else {
//                probeCount++;
//            }
//            index = (index + 1) % TABLE_SIZE;
//            if (probeCount >= TABLE_SIZE) break;
//        }
//
//        if (existingIndex != -1) {
//            table[existingIndex].flightPtr = flight;
//            return true;
//        }
//
//        if (probeCount > maxProbeChain) maxProbeChain = probeCount;
//        if (probeCount > 0) collisionCount++;
//
//        int targetIndex = (firstTombstone != -1) ? firstTombstone : index;
//        if (targetIndex < 0 || targetIndex >= TABLE_SIZE ||
//            (table[targetIndex].flightPtr != nullptr && !table[targetIndex].isTombstone)) {
//            return false;
//        }
//
//        table[targetIndex].key = callsign;
//        table[targetIndex].flightPtr = flight;
//        table[targetIndex].isTombstone = false;
//        itemCount++;
//        return true;
//    }
//
//    Flight* find(const string& callsign) const {
//        int index = hashFunction(callsign);
//        int probes = 0;
//
//        while (table[index].flightPtr != nullptr || table[index].isTombstone) {
//            if (!table[index].isTombstone && table[index].key == callsign) {
//                return table[index].flightPtr;
//            }
//            index = (index + 1) % TABLE_SIZE;
//            probes++;
//            if (probes >= TABLE_SIZE) break;
//        }
//        return nullptr;
//    }
//
//    bool remove(const string& callsign) {
//        int index = hashFunction(callsign);
//        int probes = 0;
//
//        while (table[index].flightPtr != nullptr || table[index].isTombstone) {
//            if (!table[index].isTombstone && table[index].key == callsign) {
//                table[index].flightPtr = nullptr;
//                table[index].isTombstone = true;
//                table[index].key = "";
//                itemCount--;
//                return true;
//            }
//            index = (index + 1) % TABLE_SIZE;
//            probes++;
//            if (probes >= TABLE_SIZE) break;
//        }
//        return false;
//    }
//
//    void displayStats() const {
//        float loadFactor = static_cast<float>(itemCount) / TABLE_SIZE;
//        cout << "\n--- Hash Table Statistics ---\n";
//        cout << "Table Size:          " << TABLE_SIZE << "\n";
//        cout << "Total Active Items:  " << itemCount << "\n";
//        cout << "Load Factor:         " << fixed << setprecision(3) << loadFactor << "\n";
//        cout << "Total Collisions:    " << collisionCount << "\n";
//        cout << "Longest Probe Chain: " << maxProbeChain << "\n";
//        cout << "-----------------------------\n";
//    }
//};
//
//// ========================================================
//// 3. MODULE 1: CLASS REGISTRATION (REGISTRY MANAGER)
//// ========================================================
//
//class Registration {
//private:
//    Flight* flightHead;
//    Flight* flightTail;
//    Crew* crewHead;
//    FlightHashTable flightLookup;
//
//    // Helper: Split line by '|' delimiter without using std::stringstream
//    static int splitPipes(const string& line, string tokens[], int maxTokens) {
//        int count = 0;
//        string current = "";
//        for (size_t i = 0; i < line.length(); i++) {
//            if (line[i] == '|') {
//                if (count < maxTokens) tokens[count++] = current;
//                current = "";
//            }
//            else if (line[i] != '\r') {
//                current += line[i];
//            }
//        }
//        if (count < maxTokens) tokens[count++] = current;
//        return count;
//    }
//
//public:
//    Registration() : flightHead(nullptr), flightTail(nullptr), crewHead(nullptr) {}
//
//    ~Registration() {
//        Flight* currFlight = flightHead;
//        while (currFlight != nullptr) {
//            Flight* temp = currFlight;
//            currFlight = currFlight->next;
//            delete temp;
//        }
//        flightHead = flightTail = nullptr;
//
//        Crew* currCrew = crewHead;
//        while (currCrew != nullptr) {
//            Crew* temp = currCrew;
//            currCrew = currCrew->next;
//            delete temp;
//        }
//        crewHead = nullptr;
//    }
//
//    void addFlight(string cs, string type, string al, string orig, string dest,
//        int pass, int fuel, string sec, string rwy, int arrTick, int prio) {
//        Flight* newFlight = new Flight(cs, type, al, orig, dest, pass, fuel, sec, rwy, arrTick, prio);
//
//        if (flightHead == nullptr) {
//            flightHead = flightTail = newFlight;
//        }
//        else {
//            flightTail->next = newFlight;
//            flightTail = newFlight;
//        }
//
//        flightLookup.insert(cs, newFlight);
//    }
//
//    bool removeFlight(const string& callsign) {
//        Flight* curr = flightHead;
//        Flight* prev = nullptr;
//
//        while (curr != nullptr && curr->callsign != callsign) {
//            prev = curr;
//            curr = curr->next;
//        }
//
//        if (curr == nullptr) return false;
//
//        if (curr == flightHead) {
//            flightHead = flightHead->next;
//            if (flightHead == nullptr) flightTail = nullptr;
//        }
//        else {
//            prev->next = curr->next;
//            if (curr == flightTail) flightTail = prev;
//        }
//
//        delete curr;
//        flightLookup.remove(callsign);
//        return true;
//    }
//
//    void addCrew(string id, string name, string role, string assignedCallsign) {
//        Crew* newCrew = new Crew(id, name, role, assignedCallsign);
//        if (crewHead == nullptr) {
//            crewHead = newCrew;
//        }
//        else {
//            Crew* curr = crewHead;
//            while (curr->next != nullptr) {
//                curr = curr->next;
//            }
//            curr->next = newCrew;
//        }
//    }
//
//    Flight* findFlight(const string& callsign) const {
//        return flightLookup.find(callsign);
//    }
//
//    Flight* getFlightHead() const {
//        return flightHead;
//    }
//
//    void displayHashStats() const {
//        flightLookup.displayStats();
//    }
//
//    bool loadFlights(const string& filename) {
//        ifstream file(filename);
//        if (!file.is_open()) {
//            cout << "Error: Could not open " << filename << "\n";
//            return false;
//        }
//
//        string line;
//        bool isHeader = true;
//        string tokens[11];
//
//        while (getline(file, line)) {
//            if (line.empty()) continue;
//            if (isHeader) {
//                isHeader = false;
//                continue;
//            }
//            int count = splitPipes(line, tokens, 11);
//            if (count == 11) {
//                addFlight(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4],
//                    stoi(tokens[5]), stoi(tokens[6]), tokens[7], tokens[8],
//                    stoi(tokens[10]), stoi(tokens[9]));
//            }
//        }
//        file.close();
//        return true;
//    }
//
//    bool loadCrew(const string& filename) {
//        ifstream file(filename);
//        if (!file.is_open()) {
//            cout << "Error: Could not open " << filename << "\n";
//            return false;
//        }
//
//        string line;
//        bool isHeader = true;
//        string tokens[4];
//
//        while (getline(file, line)) {
//            if (line.empty()) continue;
//            if (isHeader) {
//                isHeader = false;
//                continue;
//            }
//            int count = splitPipes(line, tokens, 4);
//            if (count == 4) {
//                addCrew(tokens[0], tokens[1], tokens[2], tokens[3]);
//            }
//        }
//        file.close();
//        return true;
//    }
//
//    void displayAllFlights() const {
//        cout << "\n============================== ACTIVE FLIGHTS REGISTRY ==============================\n";
//        cout << left << setw(9) << "Callsign"
//            << setw(16) << "Type"
//            << setw(10) << "Airline"
//            << setw(10) << "From"
//            << setw(10) << "To"
//            << setw(6) << "Pass"
//            << setw(6) << "Fuel"
//            << setw(22) << "Sector"
//            << setw(12) << "Runway"
//            << setw(6) << "Prio"
//            << "\n";
//        cout << "------------------------------------------------------------------------------------\n";
//
//        Flight* curr = flightHead;
//        while (curr != nullptr) {
//            cout << left << setw(9) << curr->callsign
//                << setw(16) << curr->aircraftType
//                << setw(10) << curr->airline
//                << setw(10) << curr->origin
//                << setw(10) << curr->destination
//                << setw(6) << curr->passengerCount
//                << setw(6) << curr->fuelRemaining
//                << setw(22) << curr->currentSector
//                << setw(12) << curr->assignedRunway
//                << setw(6) << curr->priorityLevel
//                << "\n";
//            curr = curr->next;
//        }
//        cout << "====================================================================================\n";
//    }
//
//    void displayAllCrew() const {
//        cout << "\n======================== CREW REGISTRY ========================\n";
//        cout << left << setw(10) << "EmpID"
//            << setw(24) << "Name"
//            << setw(22) << "Role"
//            << setw(10) << "Flight"
//            << "\n";
//        cout << "---------------------------------------------------------------\n";
//
//        Crew* curr = crewHead;
//        while (curr != nullptr) {
//            cout << left << setw(10) << curr->employeeID
//                << setw(24) << curr->name
//                << setw(22) << curr->role
//                << setw(10) << curr->assignedCallsign
//                << "\n";
//            curr = curr->next;
//        }
//        cout << "===============================================================\n";
//    }
//};
//
//// ========================================================
//// 4. MODULE 2: LANDING & EMERGENCY DISPATCH
//// Class: EmergencyLanding
//// ========================================================
//
//// 4.1 Custom Circular Array Queue for Normal Arrivals (FIFO)
//class NormalQueue {
//private:
//    static const int CAPACITY = 100;
//    Flight* queue[CAPACITY];
//    int frontIdx;
//    int rearIdx;
//    int count;
//
//public:
//    NormalQueue() : frontIdx(0), rearIdx(-1), count(0) {
//        for (int i = 0; i < CAPACITY; i++) {
//            queue[i] = nullptr;
//        }
//    }
//
//    bool isEmpty() const {
//        return count == 0;
//    }
//
//    bool isFull() const {
//        return count == CAPACITY;
//    }
//
//    int getCount() const {
//        return count;
//    }
//
//    bool enqueue(Flight* flight) {
//        if (isFull()) {
//            cout << "Warning: Normal landing queue is full!\n";
//            return false;
//        }
//        rearIdx = (rearIdx + 1) % CAPACITY;
//        queue[rearIdx] = flight;
//        count++;
//        return true;
//    }
//
//    Flight* dequeue() {
//        if (isEmpty()) return nullptr;
//        Flight* flight = queue[frontIdx];
//        queue[frontIdx] = nullptr;
//        frontIdx = (frontIdx + 1) % CAPACITY;
//        count--;
//        return flight;
//    }
//
//    Flight* front() const {
//        if (isEmpty()) return nullptr;
//        return queue[frontIdx];
//    }
//
//    // Purge flight if escalated from normal queue to emergency
//    void removeByCallsign(const string& callsign) {
//        if (isEmpty()) return;
//        int originalCount = count;
//        Flight* tempBuffer[CAPACITY];
//        int tempCount = 0;
//
//        for (int i = 0; i < originalCount; i++) {
//            Flight* f = dequeue();
//            if (f != nullptr && f->callsign != callsign) {
//                tempBuffer[tempCount++] = f;
//            }
//        }
//        for (int i = 0; i < tempCount; i++) {
//            enqueue(tempBuffer[i]);
//        }
//    }
//
//    void display() const {
//        if (isEmpty()) {
//            cout << "Normal Queue: [Empty]\n";
//            return;
//        }
//        cout << "\n--- Normal Landing Queue (FIFO) ---\n";
//        cout << left << setw(6) << "Pos"
//            << setw(10) << "Callsign"
//            << setw(10) << "Fuel"
//            << setw(12) << "ArrTick"
//            << setw(22) << "Sector" << "\n";
//        cout << "----------------------------------------------------\n";
//
//        for (int i = 0; i < count; i++) {
//            int idx = (frontIdx + i) % CAPACITY;
//            Flight* f = queue[idx];
//            cout << left << setw(6) << (i + 1)
//                << setw(10) << f->callsign
//                << setw(10) << f->fuelRemaining
//                << setw(12) << f->arrivalTick
//                << setw(22) << f->currentSector << "\n";
//        }
//        cout << "----------------------------------------------------\n";
//    }
//};
//
//// 4.2 Custom Binary Min-Heap for Emergency Arrivals (Priority + ArrivalTie)
//class EmergencyHeap {
//private:
//    static const int CAPACITY = 100;
//    Flight* heap[CAPACITY];
//    int heapSize;
//
//    // Severity comparison: lower number = higher urgency.
//    // Tie-breaker: earlier arrival tick = higher urgency.
//    static bool hasHigherUrgency(Flight* a, Flight* b) {
//        if (a->priorityLevel != b->priorityLevel) {
//            return a->priorityLevel < b->priorityLevel;
//        }
//        return a->arrivalTick < b->arrivalTick;
//    }
//
//    void swapElements(int i, int j) {
//        Flight* temp = heap[i];
//        heap[i] = heap[j];
//        heap[j] = temp;
//    }
//
//    void siftUp(int idx) {
//        while (idx > 0) {
//            int parent = (idx - 1) / 2;
//            if (hasHigherUrgency(heap[idx], heap[parent])) {
//                swapElements(idx, parent);
//                idx = parent;
//            }
//            else {
//                break;
//            }
//        }
//    }
//
//    void siftDown(int idx) {
//        while (true) {
//            int left = 2 * idx + 1;
//            int right = 2 * idx + 2;
//            int highestUrgencyIdx = idx;
//
//            if (left < heapSize && hasHigherUrgency(heap[left], heap[highestUrgencyIdx])) {
//                highestUrgencyIdx = left;
//            }
//            if (right < heapSize && hasHigherUrgency(heap[right], heap[highestUrgencyIdx])) {
//                highestUrgencyIdx = right;
//            }
//
//            if (highestUrgencyIdx != idx) {
//                swapElements(idx, highestUrgencyIdx);
//                idx = highestUrgencyIdx;
//            }
//            else {
//                break;
//            }
//        }
//    }
//
//public:
//    EmergencyHeap() : heapSize(0) {
//        for (int i = 0; i < CAPACITY; i++) {
//            heap[i] = nullptr;
//        }
//    }
//
//    bool isEmpty() const {
//        return heapSize == 0;
//    }
//
//    bool isFull() const {
//        return heapSize == CAPACITY;
//    }
//
//    int getSize() const {
//        return heapSize;
//    }
//
//    bool insert(Flight* flight) {
//        if (isFull()) {
//            cout << "Warning: Emergency heap is full!\n";
//            return false;
//        }
//        heap[heapSize] = flight;
//        siftUp(heapSize);
//        heapSize++;
//        return true;
//    }
//
//    Flight* extractMin() {
//        if (isEmpty()) return nullptr;
//        Flight* topFlight = heap[0];
//        heap[0] = heap[heapSize - 1];
//        heap[heapSize - 1] = nullptr;
//        heapSize--;
//
//        if (heapSize > 0) {
//            siftDown(0);
//        }
//        return topFlight;
//    }
//
//    Flight* peekMin() const {
//        if (isEmpty()) return nullptr;
//        return heap[0];
//    }
//
//    void display() const {
//        if (isEmpty()) {
//            cout << "Emergency Heap: [Empty]\n";
//            return;
//        }
//        cout << "\n--- Emergency Landing Min-Heap ---\n";
//        cout << left << setw(8) << "Index"
//            << setw(10) << "Callsign"
//            << setw(12) << "PrioLevel"
//            << setw(12) << "ArrTick"
//            << setw(10) << "Fuel" << "\n";
//        cout << "----------------------------------------------------\n";
//
//        for (int i = 0; i < heapSize; i++) {
//            Flight* f = heap[i];
//            cout << left << setw(8) << i
//                << setw(10) << f->callsign
//                << setw(12) << f->priorityLevel
//                << setw(12) << f->arrivalTick
//                << setw(10) << f->fuelRemaining << "\n";
//        }
//        cout << "----------------------------------------------------\n";
//    }
//};
//
//// 4.3 Controller Interface: EmergencyLanding
//class EmergencyLanding {
//private:
//    NormalQueue normalQueue;
//    EmergencyHeap emergencyHeap;
//
//public:
//    EmergencyLanding() {}
//
//    // Queue operations
//    void enqueueNormal(Flight* flight) {
//        normalQueue.enqueue(flight);
//    }
//
//    Flight* dequeueNormal() {
//        return normalQueue.dequeue();
//    }
//
//    Flight* frontNormal() const {
//        return normalQueue.front();
//    }
//
//    bool isEmptyNormal() const {
//        return normalQueue.isEmpty();
//    }
//
//    void displayNormalQueue() const {
//        normalQueue.display();
//    }
//
//    // Heap operations
//    void insertEmergency(Flight* flight) {
//        normalQueue.removeByCallsign(flight->callsign); // Ensure it is not left behind in normal queue
//        emergencyHeap.insert(flight);
//    }
//
//    Flight* extractMin() {
//        return emergencyHeap.extractMin();
//    }
//
//    Flight* peekMin() const {
//        return emergencyHeap.peekMin();
//    }
//
//    bool isEmptyHeap() const {
//        return emergencyHeap.isEmpty();
//    }
//
//    void displayHeap() const {
//        emergencyHeap.display();
//    }
//
//    // Priority landing rule: Normal queue first, then emergency heap
//    Flight* processNextLanding() {
//        if (!normalQueue.isEmpty()) {
//            return normalQueue.dequeue();
//        }
//        else if (!emergencyHeap.isEmpty()) {
//            return emergencyHeap.extractMin();
//        }
//        return nullptr;
//    }
//};
//
//// ========================================================
//// 5. SCENARIO PARSER (EVENT LOADER)
//// ========================================================
//
//struct ScenarioEvent {
//    int tick = 0;
//    string eventType = "";
//    string target = "";
//};
//class ScenarioLoader {
//private:
//    static const int MAX_EVENTS = 50;
//    ScenarioEvent events[MAX_EVENTS];
//    int eventCount;
//
//    static int splitPipes(const string& line, string tokens[], int maxTokens) {
//        int count = 0;
//        string current = "";
//        for (size_t i = 0; i < line.length(); i++) {
//            if (line[i] == '|') {
//                if (count < maxTokens) tokens[count++] = current;
//                current = "";
//            }
//            else if (line[i] != '\r') {
//                current += line[i];
//            }
//        }
//        if (count < maxTokens) tokens[count++] = current;
//        return count;
//    }
//
//    static int mapEventTypeToPriority(const string& type) {
//        if (type == "War") return 1;
//        if (type == "Hijack") return 2;
//        if (type == "Cyber") return 3;
//        if (type == "Missing") return 4;
//        if (type == "Fuel") return 5;
//        if (type == "Medical") return 6;
//        if (type == "Weather") return 7;
//        if (type == "Mechanical") return 8;
//        return 9;
//    }
//
//public:
//    ScenarioLoader() : eventCount(0) {}
//
//    bool loadScenario(const string& filename) {
//        ifstream file(filename);
//        if (!file.is_open()) {
//            cout << "Error: Could not open " << filename << "\n";
//            return false;
//        }
//
//        string line;
//        bool isHeader = true;
//        string tokens[3];
//
//        while (getline(file, line)) {
//            if (line.empty()) continue;
//            if (isHeader) {
//                isHeader = false;
//                continue;
//            }
//            int count = splitPipes(line, tokens, 3);
//            if (count == 3 && eventCount < MAX_EVENTS) {
//                events[eventCount].tick = stoi(tokens[0]);
//                events[eventCount].eventType = tokens[1];
//                events[eventCount].target = tokens[2];
//                eventCount++;
//            }
//        }
//        file.close();
//        return true;
//    }
//
//    void applyEventsForTick(int currentTick, Registration& reg, EmergencyLanding& dispatcher) {
//        for (int i = 0; i < eventCount; i++) {
//            if (events[i].tick == currentTick) {
//                cout << "\n>>> [TICK EVENT] " << events[i].eventType << " on " << events[i].target << " <<<\n";
//                Flight* f = reg.findFlight(events[i].target);
//                if (f != nullptr) {
//                    f->priorityLevel = mapEventTypeToPriority(events[i].eventType);
//                    f->arrivalTick = currentTick; // Updated trigger tick for tie-breaking
//                    dispatcher.insertEmergency(f);
//                    cout << "Flight " << f->callsign << " escalated to Priority " << f->priorityLevel << " and routed to Emergency Heap.\n";
//                }
//                else {
//                    cout << "Event target not found in active flights registry (or sector/missing): " << events[i].target << "\n";
//                }
//            }
//        }
//    }
//};
//
//
//
//// ========================================================
//// 6. MODULE 4: FLIGHT MANIFEST
//// Class: BSTflight
//// ========================================================
//
//// Tree node for the Binary Search Tree
//struct BSTNode {
//    Flight* flight;
//    BSTNode* left;
//    BSTNode* right;
//
//    BSTNode(Flight* f) : flight(f), left(nullptr), right(nullptr) {}
//};
//
//class BSTflight {
//private:
//    BSTNode* root;
//
//    // Helper: Recursive insertion by lexicographical callsign order
//    BSTNode* insertHelper(BSTNode* node, Flight* flight) {
//        if (node == nullptr) {
//            return new BSTNode(flight);
//        }
//
//        if (flight->callsign < node->flight->callsign) {
//            node->left = insertHelper(node->left, flight);
//        }
//        else if (flight->callsign > node->flight->callsign) {
//            node->right = insertHelper(node->right, flight);
//        }
//        else {
//            // Duplicate callsign: update flight reference
//            node->flight = flight;
//        }
//        return node;
//    }
//
//    // Helper: Recursive binary search
//    Flight* searchHelper(BSTNode* node, const string& callsign) const {
//        if (node == nullptr) {
//            return nullptr;
//        }
//
//        if (callsign == node->flight->callsign) {
//            return node->flight;
//        }
//        else if (callsign < node->flight->callsign) {
//            return searchHelper(node->left, callsign);
//        }
//        else {
//            return searchHelper(node->right, callsign);
//        }
//    }
//
//    // Helper: Recursive in-order traversal (Left -> Node -> Right)
//    void inorderHelper(BSTNode* node) const {
//        if (node == nullptr) return;
//
//        inorderHelper(node->left);
//
//        cout << left << setw(10) << node->flight->callsign
//            << setw(16) << node->flight->aircraftType
//            << setw(10) << node->flight->airline
//            << setw(10) << node->flight->origin
//            << setw(10) << node->flight->destination
//            << setw(6) << node->flight->passengerCount
//            << setw(6) << node->flight->fuelRemaining
//            << setw(22) << node->flight->currentSector
//            << setw(12) << node->flight->assignedRunway
//            << setw(6) << node->flight->priorityLevel
//            << "\n";
//
//        inorderHelper(node->right);
//    }
//
//    // Helper: Recursive height computation
//    int heightHelper(BSTNode* node) const {
//        if (node == nullptr) {
//            return 0; // Empty tree has height 0 (single root has height 1)
//        }
//        int leftH = heightHelper(node->left);
//        int rightH = heightHelper(node->right);
//        return 1 + (leftH > rightH ? leftH : rightH);
//    }
//
//    // Helper: Post-order recursive cleanup (Left -> Right -> Node)
//    void destroyHelper(BSTNode* node) {
//        if (node == nullptr) return;
//
//        destroyHelper(node->left);
//        destroyHelper(node->right);
//        delete node; // Frees tree node wrapper; does NOT delete node->flight
//    }
//
//public:
//    BSTflight() : root(nullptr) {}
//
//    ~BSTflight() {
//        destroyTree();
//    }
//
//    // Insert a flight pointer into the manifest BST
//    void insert(Flight* flight) {
//        if (flight == nullptr) return;
//        root = insertHelper(root, flight);
//    }
//
//    // Search by callsign (O(h) average O(log N), worst O(N))
//    Flight* search(const string& callsign) const {
//        return searchHelper(root, callsign);
//    }
//
//    // Print all flights alphabetically by callsign
//    void inorderTraversal() const {
//        if (root == nullptr) {
//            cout << "Flight Manifest (BST): [Empty]\n";
//            return;
//        }
//        cout << "\n======================== FLIGHT MANIFEST (BST IN-ORDER) ========================\n";
//        cout << left << setw(10) << "Callsign"
//            << setw(16) << "Type"
//            << setw(10) << "Airline"
//            << setw(10) << "From"
//            << setw(10) << "To"
//            << setw(6) << "Pass"
//            << setw(6) << "Fuel"
//            << setw(22) << "Sector"
//            << setw(12) << "Runway"
//            << setw(6) << "Prio"
//            << "\n";
//        cout << "--------------------------------------------------------------------------------\n";
//        inorderHelper(root);
//        cout << "================================================================================\n";
//    }
//
//    // Calculate maximum tree height
//    int height() const {
//        return heightHelper(root);
//    }
//
//    // Deallocate all tree nodes
//    void destroyTree() {
//        destroyHelper(root);
//        root = nullptr;
//    }
//
//    bool isEmpty() const {
//        return root == nullptr;
//    }
//};
//
//
//
//// ========================================================
//// MODULE 5: BALANCED FLIGHT INDEX
//// Class: AVLflight
//// ========================================================
//
//struct AVLNode {
//    Flight* flight;
//    AVLNode* left;
//    AVLNode* right;
//    int nodeHeight;
//
//    AVLNode(Flight* f)
//        : flight(f), left(nullptr), right(nullptr), nodeHeight(1) {
//    }
//};
//
//class AVLflight {
//private:
//    AVLNode* root;
//
//    int getNodeHeight(AVLNode* node) const {
//        return (node == nullptr) ? 0 : node->nodeHeight;
//    }
//
//    int maxVal(int a, int b) const {
//        return (a > b) ? a : b;
//    }
//
//    int getBalanceFactor(AVLNode* node) const {
//        if (node == nullptr) return 0;
//        return getNodeHeight(node->left) - getNodeHeight(node->right);
//    }
//
//    // Single Right Rotation (LL Case)
//    AVLNode* rotateRight(AVLNode* y) {
//        AVLNode* x = y->left;
//        AVLNode* T2 = x->right;
//
//        // Perform rotation
//        x->right = y;
//        y->left = T2;
//
//        // Update heights (order matters: children first)
//        y->nodeHeight = 1 + maxVal(getNodeHeight(y->left), getNodeHeight(y->right));
//        x->nodeHeight = 1 + maxVal(getNodeHeight(x->left), getNodeHeight(x->right));
//
//        return x; // New root of subtree
//    }
//
//    // Single Left Rotation (RR Case)
//    AVLNode* rotateLeft(AVLNode* x) {
//        AVLNode* y = x->right;
//        AVLNode* T2 = y->left;
//
//        // Perform rotation
//        y->left = x;
//        x->right = T2;
//
//        // Update heights
//        x->nodeHeight = 1 + maxVal(getNodeHeight(x->left), getNodeHeight(x->right));
//        y->nodeHeight = 1 + maxVal(getNodeHeight(y->left), getNodeHeight(y->right));
//
//        return y; // New root of subtree
//    }
//
//    // Double Rotation: Left-Right (LR Case)
//    AVLNode* rotateLeftRight(AVLNode* node) {
//        node->left = rotateLeft(node->left);
//        return rotateRight(node);
//    }
//
//    // Double Rotation: Right-Left (RL Case)
//    AVLNode* rotateRightLeft(AVLNode* node) {
//        node->right = rotateRight(node->right);
//        return rotateLeft(node);
//    }
//
//    AVLNode* insertHelper(AVLNode* node, Flight* flight) {
//        // 1. Standard BST insertion
//        if (node == nullptr) {
//            return new AVLNode(flight);
//        }
//
//        if (flight->callsign < node->flight->callsign) {
//            node->left = insertHelper(node->left, flight);
//        }
//        else if (flight->callsign > node->flight->callsign) {
//            node->right = insertHelper(node->right, flight);
//        }
//        else {
//            // Duplicate callsigns not permitted
//            return node;
//        }
//
//        // 2. Update height of current ancestor node
//        node->nodeHeight = 1 + maxVal(getNodeHeight(node->left), getNodeHeight(node->right));
//
//        // 3. Determine balance factor
//        int balance = getBalanceFactor(node);
//
//        // 4. Check the 4 unbalance cases:
//        // Left-Left (LL)
//        if (balance > 1 && flight->callsign < node->left->flight->callsign) {
//            return rotateRight(node);
//        }
//
//        // Right-Right (RR)
//        if (balance < -1 && flight->callsign > node->right->flight->callsign) {
//            return rotateLeft(node);
//        }
//
//        // Left-Right (LR)
//        if (balance > 1 && flight->callsign > node->left->flight->callsign) {
//            return rotateLeftRight(node);
//        }
//
//        // Right-Left (RL)
//        if (balance < -1 && flight->callsign < node->right->flight->callsign) {
//            return rotateRightLeft(node);
//        }
//
//        return node;
//    }
//
//    Flight* searchHelper(AVLNode* node, const string& callsign) const {
//        if (node == nullptr) return nullptr;
//        if (node->flight->callsign == callsign) return node->flight;
//
//        if (callsign < node->flight->callsign) {
//            return searchHelper(node->left, callsign);
//        }
//        return searchHelper(node->right, callsign);
//    }
//
//    void inorderHelper(AVLNode* node) const {
//        if (node == nullptr) return;
//        inorderHelper(node->left);
//        cout << "  [" << node->flight->callsign << "] "
//            << node->flight->aircraftType << " | "
//            << node->flight->airline << " | Priority: "
//            << node->flight->priorityLevel << " | Sector: "
//            << node->flight->currentSector << "\n";
//        inorderHelper(node->right);
//    }
//
//    void destroyHelper(AVLNode* node) {
//        if (node == nullptr) return;
//        destroyHelper(node->left);
//        destroyHelper(node->right);
//        delete node; // Deletes only the tree index node, not the Flight
//    }
//
//public:
//    AVLflight() : root(nullptr) {}
//
//    ~AVLflight() {
//        destroyTree();
//    }
//
//    void insert(Flight* flight) {
//        if (flight != nullptr) {
//            root = insertHelper(root, flight);
//        }
//    }
//
//    Flight* search(const string& callsign) const {
//        return searchHelper(root, callsign);
//    }
//
//    void inorderTraversal() const {
//        if (root == nullptr) {
//            cout << "AVL Manifest is empty.\n";
//            return;
//        }
//        inorderHelper(root);
//    }
//
//    int height() const {
//        return getNodeHeight(root);
//    }
//
//    void destroyTree() {
//        destroyHelper(root);
//        root = nullptr;
//    }
//};
//
//
//void runHeightComparisonDemo() {
//    BSTflight bst;
//    AVLflight avl;
//
//    // 15 strictly alphabetical callsigns
//    string sampleCallsigns[15] = {
//        "AA-101", "AA-202", "BB-100", "BB-200", "BB-300",
//        "CC-401", "DD-501", "EE-601", "FF-701", "GG-801",
//        "HH-901", "II-111", "JJ-222", "KK-333", "LL-444"
//    };
//
//    // Allocate dummy flight objects for verification
//    Flight* dummyFlights[15];
//    for (int i = 0; i < 15; i++) {
//       
//        dummyFlights[i] = new Flight(sampleCallsigns[i], "DemoJet", "BlackBox",
//            "Origin", "Dest", 100, 20,
//            "Sector 1", "Runway 04", 0, 9);
//        dummyFlights[i]->callsign = sampleCallsigns[i];
//        dummyFlights[i]->aircraftType = "DemoJet";
//        dummyFlights[i]->priorityLevel = 9;
//
//        bst.insert(dummyFlights[i]);
//        avl.insert(dummyFlights[i]);
//    }
//
//    cout << "\n============================================\n";
//    cout << "   MODULE 5 VERIFICATION: BST vs AVL HEIGHT   \n";
//    cout << "============================================\n";
//    cout << "Inserted elements: 15 (Alphabetically Ordered)\n";
//    cout << "BST Height (Degenerated Chain) : " << bst.height() << " (Expected: 15)\n";
//    cout << "AVL Height (Self-Balanced O(log N)): " << avl.height() << " (Expected: 4)\n";
//    cout << "============================================\n";
//
//    // Clean up temporary tree structures and dummy flights
//    bst.destroyTree();
//    avl.destroyTree();
//    for (int i = 0; i < 15; i++) {
//        delete dummyFlights[i];
//    }
//}
//
//int main() {
//    // -------------------------------------------------------------
//    // MODULE 5 STANDALONE VERIFICATION DRIVER
//    // -------------------------------------------------------------
//    cout << "========================================================\n";
//    cout << "          MAYDAY-404: MODULE 5 (AVLflight) TEST         \n";
//    cout << "========================================================\n\n";
//
//    AVLflight avl;
//
//    // 1. Basic Insertion and Auto-Balancing Test
//    // Insert dummy flights to verify pointer linking and in-order display
//    Flight f1("BB-876", "Airbus A320neo", "BlackBox", "A", "B", 165, 15, "Sector 1", "Runway 04", 0, 9);
//    Flight f2("BB-437", "Viking DHC-8", "BlackBox", "A", "B", 60, 12, "Sector 2", "Runway 09", 0, 9);
//    Flight f3("BB-211", "ATR 72", "BlackBox", "A", "B", 70, 10, "Sector 3", "Runway 17", 0, 9);
//    Flight f4("BB-940", "MedEvac-1", "BlackBox", "A", "B", 4, 8, "Sector 4", "Runway 12", 0, 9);
//
//    avl.insert(&f1);
//    avl.insert(&f2);
//    avl.insert(&f3);
//    avl.insert(&f4);
//
//    cout << "--- AVL In-Order Traversal (Alphabetical by Callsign) ---\n";
//    avl.inorderTraversal();
//
//    // 2. Search Test (O(log N))
//    cout << "\n--- AVL Search Verification ---\n";
//    Flight* res = avl.search("BB-437");
//    if (res != nullptr) {
//        cout << "Search SUCCESS: Found " << res->callsign
//            << " | Type: " << res->aircraftType
//            << " | Runway: " << res->assignedRunway << "\n";
//    }
//    else {
//        cout << "Search FAILED: Flight not found.\n";
//    }
//
//    // 3. Mandatory Requirement Test: BST vs. AVL Height Comparison
//    // (Simulates Menu Option [22] using 15 strictly sequential callsigns)
//    cout << "\n--- Running Menu Option [22]: BST vs AVL Height Check ---\n";
//    runHeightComparisonDemo();
//
//    return 0;
//}