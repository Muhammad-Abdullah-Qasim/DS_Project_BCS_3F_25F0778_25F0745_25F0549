//#include <iostream>
//#include <string>
//#include <fstream>
//#include <iomanip>
//
//using namespace std;
//
//// ==========================================
//// 1. DATA RECORDS
//// ==========================================
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
//// ==========================================
//// 2. CUSTOM HASH TABLE (LINEAR PROBING)
//// ==========================================
//
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
//
//    // Polynomial rolling hash function using Horner's rule
//    int hashFunction(const string& key) const {
//        unsigned long hash = 0;
//        for (size_t i = 0; i < key.length(); i++) {
//            hash = (hash * 31 + static_cast<unsigned char>(key[i])) % TABLE_SIZE;
//        }
//        return static_cast<int>(hash);
//    }
//
//public:
//    FlightHashTable() : itemCount(0), collisionCount(0) {}
//
//    bool insert(const string& callsign, Flight* flight) {
//        if (itemCount >= TABLE_SIZE) return false;
//
//        int index = hashFunction(callsign);
//        int probeCount = 0;
//        int firstTombstone = -1;
//        int existingIndex = -1;
//
//        // Probe until an entirely empty slot is found or entire table checked
//        while (table[index].flightPtr != nullptr || table[index].isTombstone) {
//            if (table[index].isTombstone) {
//                if (firstTombstone == -1) {
//                    firstTombstone = index;
//                }
//            }
//            else if (table[index].key == callsign) {
//                existingIndex = index;
//                break;
//            }
//            else {
//                probeCount++;
//            }
//
//            index = (index + 1) % TABLE_SIZE;
//            if (probeCount >= TABLE_SIZE) break;
//        }
//
//        // If key already exists, update reference in place
//        if (existingIndex != -1) {
//            table[existingIndex].flightPtr = flight;
//            return true;
//        }
//
//        // Key doesn't exist; pick slot
//        int targetIndex = (firstTombstone != -1) ? firstTombstone : index;
//        if (table[targetIndex].flightPtr != nullptr && !table[targetIndex].isTombstone) {
//            return false; // Table completely full
//        }
//
//        if (probeCount > 0) collisionCount++;
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
//        int longestChain = 0;
//        for (int i = 0; i < TABLE_SIZE; i++) {
//            if (table[i].flightPtr != nullptr && !table[i].isTombstone) {
//                int expectedIdx = hashFunction(table[i].key);
//                int chainLen = (i >= expectedIdx) ? (i - expectedIdx) : (TABLE_SIZE - expectedIdx + i);
//                if (chainLen > longestChain) longestChain = chainLen;
//            }
//        }
//        float loadFactor = static_cast<float>(itemCount) / TABLE_SIZE;
//        cout << "\n--- Hash Table Statistics ---\n";
//        cout << "Table Size:          " << TABLE_SIZE << "\n";
//        cout << "Total Active Items:  " << itemCount << "\n";
//        cout << "Load Factor:         " << fixed << setprecision(3) << loadFactor << "\n";
//        cout << "Total Collisions:    " << collisionCount << "\n";
//        cout << "Longest Probe Chain: " << longestChain << "\n";
//        cout << "-----------------------------\n";
//    }
//};
//
//// ==========================================
//// 3. CLASS REGISTRATION (REGISTRY MANAGER)
//// ==========================================
//
//class Registration {
//private:
//    Flight* flightHead;
//    Flight* flightTail;
//    Crew* crewHead;
//    FlightHashTable flightLookup;
//
//    // Helper: Split line by '|' delimiter without using std::stringstream or vectors
//    static int splitPipes(const string& line, string tokens[], int maxTokens) {
//        int count = 0;
//        string current = "";
//        for (size_t i = 0; i < line.length(); i++) {
//            if (line[i] == '|') {
//                if (count < maxTokens) tokens[count++] = current;
//                current = "";
//            }
//            else if (line[i] != '\r') { // Ignore carriage returns
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
//    void updateAllFlights() {
//        Flight* curr = flightHead;
//        while (curr != nullptr) {
//            if (curr->fuelRemaining > 0) {
//                curr->fuelRemaining--;
//            }
//            curr = curr->next;
//        }
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
//    void displayHashStats() const {
//        flightLookup.displayStats();
//    }
//
//    // Module 11/Integration File Loading: flights.txt
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
//                isHeader = false; // Skip header line: Callsign|Type|Airline|...
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
//    // Module 11/Integration File Loading: crew.txt
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
//                isHeader = false; // Skip header: EmployeeID|Name|Role|AssignedCallsign
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
//// ==========================================
//// 4. VERIFICATION / TESTING
//// ==========================================
//
//int main() {
//    Registration reg;
//
//    // Test 1: Manual Insert / Unit check
//    cout << "Loading flights...\n";
//    if (reg.loadFlights("flights.txt")) {
//        cout << "flights.txt loaded successfully.\n";
//    }
//
//    cout << "\nLoading crew...\n";
//    if (reg.loadCrew("crew.txt")) {
//        cout << "crew.txt loaded successfully.\n";
//    }
//
//    // Display loaded data
//    reg.displayAllFlights();
//    reg.displayAllCrew();
//
//    // Test 2: Search Verification
//    cout << "\n[Test] Searching for BB-437 via Hash Table:\n";
//    Flight* f = reg.findFlight("BB-437");
//    if (f) {
//        cout << "Found: " << f->callsign << " | Airline: " << f->airline << " | Fuel: " << f->fuelRemaining << "\n";
//    }
//    else {
//        cout << "Flight not found!\n";
//    }
//
//    // Test 3: Fuel Tick Advance
//    cout << "\n[Test] Advancing tick (decrementing fuel)...";
//    reg.updateAllFlights();
//    cout << "\nBB-437 Fuel after tick: " << f->fuelRemaining << " (Expected 11)\n";
//
//    // Test 4: Delete and Tombstone Probe
//    cout << "\n[Test] Removing BB-437...\n";
//    reg.removeFlight("BB-437");
//    if (reg.findFlight("BB-437") == nullptr) {
//        cout << "BB-437 successfully removed.\n";
//    }
//
//    reg.displayHashStats();
//    return 0;
//}