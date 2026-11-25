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
//        if (table[targetIndex].flightPtr != nullptr && !table[targetIndex].isTombstone) {
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
//    int tick;
//    string eventType;
//    string target;
//};
//
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
//// ========================================================
//// 6. MAIN SIMULATION TEST DRIVER
//// ========================================================
//
//int main() {
//    Registration reg;
//    EmergencyLanding dispatcher;
//    ScenarioLoader scenario;
//
//    cout << "Loading flights.txt, crew.txt, and scenario.txt...\n";
//    if (!reg.loadFlights("flights.txt") || !reg.loadCrew("crew.txt") || !scenario.loadScenario("scenario.txt")) {
//        cout << "Initialization failed. Check file paths.\n";
//        return 1;
//    }
//
//    // Step 1: Initial routing for flights loaded from flights.txt
//    Flight* curr = reg.getFlightHead();
//    while (curr != nullptr) {
//        if (curr->priorityLevel == 9) {
//            dispatcher.enqueueNormal(curr);
//        }
//        else {
//            dispatcher.insertEmergency(curr);
//        }
//        curr = curr->next;
//    }
//
//    cout << "\nInitial system state:";
//    dispatcher.displayNormalQueue();
//    dispatcher.displayHeap();
//
//    // Step 2: Simulate Tick 5 (Hijack on BB-876)
//    cout << "\n--- Advancing to Tick 5 ---";
//    scenario.applyEventsForTick(5, reg, dispatcher);
//    dispatcher.displayNormalQueue();
//    dispatcher.displayHeap();
//
//    // Step 3: Simulate Tick 12 (Low Fuel on BB-437)
//    cout << "\n--- Advancing to Tick 12 ---";
//    scenario.applyEventsForTick(12, reg, dispatcher);
//    dispatcher.displayNormalQueue();
//    dispatcher.displayHeap();
//
//    // Step 4: Test Dispatching
//    cout << "\n--- Clearing Runways via processNextLanding() ---\n";
//    for (int i = 1; i <= 4; i++) {
//        Flight* landed = dispatcher.processNextLanding();
//        if (landed) {
//            cout << "Landing #" << i << ": " << landed->callsign
//                << " | Priority: " << landed->priorityLevel
//                << " | ArrivalTick: " << landed->arrivalTick
//                << " | Sector: " << landed->currentSector << "\n";
//        }
//    }
//
//    cout << "\nRemaining states after dispatches:";
//    dispatcher.displayNormalQueue();
//    dispatcher.displayHeap();
//
//    return 0;
//}