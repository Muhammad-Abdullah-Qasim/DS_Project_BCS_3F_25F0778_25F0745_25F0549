//#include <iostream>
//#include <string>
//#include <fstream>
//#include <iomanip>
//
//using namespace std;
//
//// ========================================================
//// 1. data records
//// ========================================================
//
//struct flight {
//    string callsign;
//    string aircrafttype;
//    string airline;
//    string origin;
//    string destination;
//    int passengercount;
//    int fuelremaining; // measured in ticks
//    string currentsector;
//    string assignedrunway;
//    int arrivaltick;
//    int prioritylevel; // 1=war ... 9=normal
//
//    flight* next;      // primary owner singly-linked list pointer
//
//    flight(string cs, string type, string al, string orig, string dest,
//        int pass, int fuel, string sec, string rwy, int arrtick, int prio)
//        : callsign(cs), aircrafttype(type), airline(al), origin(orig), destination(dest),
//        passengercount(pass), fuelremaining(fuel), currentsector(sec),
//        assignedrunway(rwy), arrivaltick(arrtick), prioritylevel(prio), next(nullptr) {
//    }
//};
//
//struct crew {
//    string employeeid;
//    string name;
//    string role;
//    string assignedcallsign;
//
//    crew* next;
//
//    crew(string id, string n, string r, string cs)
//        : employeeid(id), name(n), role(r), assignedcallsign(cs), next(nullptr) {
//    }
//};
//
//// ========================================================
//// 2. module 1: custom hash table (linear probing)
//// ==========================================
//struct hashentry {
//    string key;
//    flight* flightptr;
//    bool istombstone;
//
//    hashentry() : key(""), flightptr(nullptr), istombstone(false) {}
//};
//
//class flighthashtable {
//private:
//    static const int table_size = 101; // prime number required by manual
//    hashentry table[table_size];
//    int itemcount;
//    int collisioncount;
//    int maxprobechain;
//
//    int hashfunction(const string& key) const {
//        unsigned long hash = 0;
//        for (size_t i = 0; i < key.length(); i++) {
//            hash = (hash * 31 + static_cast<unsigned char>(key[i])) % table_size;
//        }
//        return static_cast<int>(hash);
//    }
//
//public:
//    flighthashtable() : itemcount(0), collisioncount(0), maxprobechain(0) {}
//
//    bool insert(const string& callsign, flight* flight) {
//        if (itemcount >= table_size) return false;
//
//        int index = hashfunction(callsign);
//        int probecount = 0;
//        int firsttombstone = -1;
//        int existingindex = -1;
//
//        while (table[index].flightptr != nullptr || table[index].istombstone) {
//            if (table[index].istombstone) {
//                if (firsttombstone == -1) firsttombstone = index;
//            }
//            else if (table[index].key == callsign) {
//                existingindex = index;
//                break;
//            }
//            else {
//                probecount++;
//            }
//            index = (index + 1) % table_size;
//            if (probecount >= table_size) break;
//        }
//
//        if (existingindex != -1) {
//            table[existingindex].flightptr = flight;
//            return true;
//        }
//
//        if (probecount > maxprobechain) maxprobechain = probecount;
//        if (probecount > 0) collisioncount++;
//
//        int targetindex = (firsttombstone != -1) ? firsttombstone : index;
//        if (targetindex < 0 || targetindex >= table_size ||
//            (table[targetindex].flightptr != nullptr && !table[targetindex].istombstone)) {
//            return false;
//        }
//
//        table[targetindex].key = callsign;
//        table[targetindex].flightptr = flight;
//        table[targetindex].istombstone = false;
//        itemcount++;
//        return true;
//    }
//
//    flight* find(const string& callsign) const {
//        int index = hashfunction(callsign);
//        int probes = 0;
//
//        while (table[index].flightptr != nullptr || table[index].istombstone) {
//            if (!table[index].istombstone && table[index].key == callsign) {
//                return table[index].flightptr;
//            }
//            index = (index + 1) % table_size;
//            probes++;
//            if (probes >= table_size) break;
//        }
//        return nullptr;
//    }
//
//    bool remove(const string& callsign) {
//        int index = hashfunction(callsign);
//        int probes = 0;
//
//        while (table[index].flightptr != nullptr || table[index].istombstone) {
//            if (!table[index].istombstone && table[index].key == callsign) {
//                table[index].flightptr = nullptr;
//                table[index].istombstone = true;
//                table[index].key = "";
//                itemcount--;
//                return true;
//            }
//            index = (index + 1) % table_size;
//            probes++;
//            if (probes >= table_size) break;
//        }
//        return false;
//    }
//
//    void displaystats() const {
//        float loadfactor = static_cast<float>(itemcount) / table_size;
//        cout << "\n--- hash table statistics ---\n";
//        cout << "table size:          " << table_size << "\n";
//        cout << "total active items:  " << itemcount << "\n";
//        cout << "load factor:         " << fixed << setprecision(3) << loadfactor << "\n";
//        cout << "total collisions:    " << collisioncount << "\n";
//        cout << "longest probe chain: " << maxprobechain << "\n";
//        cout << "-----------------------------\n";
//    }
//};
//
//// ========================================================
//// 3. module 1: class registration (registry manager)
//// ========================================================
//
//class registration {
//private:
//    flight* flighthead;
//    flight* flighttail;
//    crew* crewhead;
//    flighthashtable flightlookup;
//
//    // helper: split line by '|' delimiter without using std::stringstream
//    static int splitpipes(const string& line, string tokens[], int maxtokens) {
//        int count = 0;
//        string current = "";
//        for (size_t i = 0; i < line.length(); i++) {
//            if (line[i] == '|') {
//                if (count < maxtokens) tokens[count++] = current;
//                current = "";
//            }
//            else if (line[i] != '\r') {
//                current += line[i];
//            }
//        }
//        if (count < maxtokens) tokens[count++] = current;
//        return count;
//    }
//
//public:
//    registration() : flighthead(nullptr), flighttail(nullptr), crewhead(nullptr) {}
//
//    ~registration() {
//        flight* currflight = flighthead;
//        while (currflight != nullptr) {
//            flight* temp = currflight;
//            currflight = currflight->next;
//            delete temp;
//        }
//        flighthead = flighttail = nullptr;
//
//        crew* currcrew = crewhead;
//        while (currcrew != nullptr) {
//            crew* temp = currcrew;
//            currcrew = currcrew->next;
//            delete temp;
//        }
//        crewhead = nullptr;
//    }
//
//    void addflight(string cs, string type, string al, string orig, string dest,
//        int pass, int fuel, string sec, string rwy, int arrtick, int prio) {
//        flight* newflight = new flight(cs, type, al, orig, dest, pass, fuel, sec, rwy, arrtick, prio);
//
//        if (flighthead == nullptr) {
//            flighthead = flighttail = newflight;
//        }
//        else {
//            flighttail->next = newflight;
//            flighttail = newflight;
//        }
//
//        flightlookup.insert(cs, newflight);
//    }
//
//    bool removeflight(const string& callsign) {
//        flight* curr = flighthead;
//        flight* prev = nullptr;
//
//        while (curr != nullptr && curr->callsign != callsign) {
//            prev = curr;
//            curr = curr->next;
//        }
//
//        if (curr == nullptr) return false;
//
//        if (curr == flighthead) {
//            flighthead = flighthead->next;
//            if (flighthead == nullptr) flighttail = nullptr;
//        }
//        else {
//            prev->next = curr->next;
//            if (curr == flighttail) flighttail = prev;
//        }
//
//        delete curr;
//        flightlookup.remove(callsign);
//        return true;
//    }
//
//    void addcrew(string id, string name, string role, string assignedcallsign) {
//        crew* newcrew = new crew(id, name, role, assignedcallsign);
//        if (crewhead == nullptr) {
//            crewhead = newcrew;
//        }
//        else {
//            crew* curr = crewhead;
//            while (curr->next != nullptr) {
//                curr = curr->next;
//            }
//            curr->next = newcrew;
//        }
//    }
//
//    flight* findflight(const string& callsign) const {
//        return flightlookup.find(callsign);
//    }
//
//    flight* getflighthead() const {
//        return flighthead;
//    }
//
//    void displayhashstats() const {
//        flightlookup.displaystats();
//    }
//
//    bool loadflights(const string& filename) {
//        ifstream file(filename);
//        if (!file.is_open()) {
//            cout << "error: could not open " << filename << "\n";
//            return false;
//        }
//
//        string line;
//        bool isheader = true;
//        string tokens[11];
//
//        while (getline(file, line)) {
//            if (line.empty()) continue;
//            if (isheader) {
//                isheader = false;
//                continue;
//            }
//            int count = splitpipes(line, tokens, 11);
//            if (count == 11) {
//                addflight(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4],
//                    stoi(tokens[5]), stoi(tokens[6]), tokens[7], tokens[8],
//                    stoi(tokens[10]), stoi(tokens[9]));
//            }
//        }
//        file.close();
//        return true;
//    }
//
//    bool loadcrew(const string& filename) {
//        ifstream file(filename);
//        if (!file.is_open()) {
//            cout << "error: could not open " << filename << "\n";
//            return false;
//        }
//
//        string line;
//        bool isheader = true;
//        string tokens[4];
//
//        while (getline(file, line)) {
//            if (line.empty()) continue;
//            if (isheader) {
//                isheader = false;
//                continue;
//            }
//            int count = splitpipes(line, tokens, 4);
//            if (count == 4) {
//                addcrew(tokens[0], tokens[1], tokens[2], tokens[3]);
//            }
//        }
//        file.close();
//        return true;
//    }
//
//    void displayallflights() const {
//        cout << "\n============================== active flights registry ==============================\n";
//        cout << left << setw(9) << "callsign"
//            << setw(16) << "type"
//            << setw(10) << "airline"
//            << setw(10) << "from"
//            << setw(10) << "to"
//            << setw(6) << "pass"
//            << setw(6) << "fuel"
//            << setw(22) << "sector"
//            << setw(12) << "runway"
//            << setw(6) << "prio"
//            << "\n";
//        cout << "------------------------------------------------------------------------------------\n";
//
//        flight* curr = flighthead;
//        while (curr != nullptr) {
//            cout << left << setw(9) << curr->callsign
//                << setw(16) << curr->aircrafttype
//                << setw(10) << curr->airline
//                << setw(10) << curr->origin
//                << setw(10) << curr->destination
//                << setw(6) << curr->passengercount
//                << setw(6) << curr->fuelremaining
//                << setw(22) << curr->currentsector
//                << setw(12) << curr->assignedrunway
//                << setw(6) << curr->prioritylevel
//                << "\n";
//            curr = curr->next;
//        }
//        cout << "====================================================================================\n";
//    }
//
//    void displayallcrew() const {
//        cout << "\n======================== crew registry ========================\n";
//        cout << left << setw(10) << "empid"
//            << setw(24) << "name"
//            << setw(22) << "role"
//            << setw(10) << "flight"
//            << "\n";
//        cout << "---------------------------------------------------------------\n";
//
//        crew* curr = crewhead;
//        while (curr != nullptr) {
//            cout << left << setw(10) << curr->employeeid
//                << setw(24) << curr->name
//                << setw(22) << curr->role
//                << setw(10) << curr->assignedcallsign
//                << "\n";
//            curr = curr->next;
//        }
//        cout << "===============================================================\n";
//    }
//};
//
//// ========================================================
//// 4. module 2: landing & emergency dispatch
//// class: emergencylanding
//// ========================================================
//
//// 4.1 custom circular array queue for normal arrivals (fifo)
//class normalqueue {
//private:
//    static const int capacity = 100;
//    flight* queue[capacity];
//    int frontidx;
//    int rearidx;
//    int count;
//
//public:
//    normalqueue() : frontidx(0), rearidx(-1), count(0) {
//        for (int i = 0; i < capacity; i++) {
//            queue[i] = nullptr;
//        }
//    }
//
//    bool isempty() const {
//        return count == 0;
//    }
//
//    bool isfull() const {
//        return count == capacity;
//    }
//
//    int getcount() const {
//        return count;
//    }
//
//    bool enqueue(flight* flight) {
//        if (isfull()) {
//            cout << "warning: normal landing queue is full!\n";
//            return false;
//        }
//        rearidx = (rearidx + 1) % capacity;
//        queue[rearidx] = flight;
//        count++;
//        return true;
//    }
//
//    flight* dequeue() {
//        if (isempty()) return nullptr;
//        flight* flight = queue[frontidx];
//        queue[frontidx] = nullptr;
//        frontidx = (frontidx + 1) % capacity;
//        count--;
//        return flight;
//    }
//
//    flight* front() const {
//        if (isempty()) return nullptr;
//        return queue[frontidx];
//    }
//
//    // purge flight if escalated from normal queue to emergency
//    void removebycallsign(const string& callsign) {
//        if (isempty()) return;
//        int originalcount = count;
//        flight* tempbuffer[capacity];
//        int tempcount = 0;
//
//        for (int i = 0; i < originalcount; i++) {
//            flight* f = dequeue();
//            if (f != nullptr && f->callsign != callsign) {
//                tempbuffer[tempcount++] = f;
//            }
//        }
//        for (int i = 0; i < tempcount; i++) {
//            enqueue(tempbuffer[i]);
//        }
//    }
//
//    void display() const {
//        if (isempty()) {
//            cout << "normal queue: [empty]\n";
//            return;
//        }
//        cout << "\n--- normal landing queue (fifo) ---\n";
//        cout << left << setw(6) << "pos"
//            << setw(10) << "callsign"
//            << setw(10) << "fuel"
//            << setw(12) << "arrtick"
//            << setw(22) << "sector" << "\n";
//        cout << "----------------------------------------------------\n";
//
//        for (int i = 0; i < count; i++) {
//            int idx = (frontidx + i) % capacity;
//            flight* f = queue[idx];
//            cout << left << setw(6) << (i + 1)
//                << setw(10) << f->callsign
//                << setw(10) << f->fuelremaining
//                << setw(12) << f->arrivaltick
//                << setw(22) << f->currentsector << "\n";
//        }
//        cout << "----------------------------------------------------\n";
//    }
//};
//
//// 4.2 custom binary min-heap for emergency arrivals (priority + arrivaltie)
//class emergencyheap {
//private:
//    static const int capacity = 100;
//    flight* heap[capacity];
//    int heapsize;
//
//    // severity comparison: lower number = higher urgency.
//    // tie-breaker: earlier arrival tick = higher urgency.
//    static bool hashigherurgency(flight* a, flight* b) {
//        if (a->prioritylevel != b->prioritylevel) {
//            return a->prioritylevel < b->prioritylevel;
//        }
//        return a->arrivaltick < b->arrivaltick;
//    }
//
//    void swapelements(int i, int j) {
//        flight* temp = heap[i];
//        heap[i] = heap[j];
//        heap[j] = temp;
//    }
//
//    void siftup(int idx) {
//        while (idx > 0) {
//            int parent = (idx - 1) / 2;
//            if (hashigherurgency(heap[idx], heap[parent])) {
//                swapelements(idx, parent);
//                idx = parent;
//            }
//            else {
//                break;
//            }
//        }
//    }
//
//    void siftdown(int idx) {
//        while (true) {
//            int left = 2 * idx + 1;
//            int right = 2 * idx + 2;
//            int highesturgencyidx = idx;
//
//            if (left < heapsize && hashigherurgency(heap[left], heap[highesturgencyidx])) {
//                highesturgencyidx = left;
//            }
//            if (right < heapsize && hashigherurgency(heap[right], heap[highesturgencyidx])) {
//                highesturgencyidx = right;
//            }
//
//            if (highesturgencyidx != idx) {
//                swapelements(idx, highesturgencyidx);
//                idx = highesturgencyidx;
//            }
//            else {
//                break;
//            }
//        }
//    }
//
//public:
//    emergencyheap() : heapsize(0) {
//        for (int i = 0; i < capacity; i++) {
//            heap[i] = nullptr;
//        }
//    }
//
//    bool isempty() const {
//        return heapsize == 0;
//    }
//
//    bool isfull() const {
//        return heapsize == capacity;
//    }
//
//    int getsize() const {
//        return heapsize;
//    }
//
//    bool insert(flight* flight) {
//        if (isfull()) {
//            cout << "warning: emergency heap is full!\n";
//            return false;
//        }
//        heap[heapsize] = flight;
//        siftup(heapsize);
//        heapsize++;
//        return true;
//    }
//
//    flight* extractmin() {
//        if (isempty()) return nullptr;
//        flight* topflight = heap[0];
//        heap[0] = heap[heapsize - 1];
//        heap[heapsize - 1] = nullptr;
//        heapsize--;
//
//        if (heapsize > 0) {
//            siftdown(0);
//        }
//        return topflight;
//    }
//
//    flight* peekmin() const {
//        if (isempty()) return nullptr;
//        return heap[0];
//    }
//
//    void display() const {
//        if (isempty()) {
//            cout << "emergency heap: [empty]\n";
//            return;
//        }
//        cout << "\n--- emergency landing min-heap ---\n";
//        cout << left << setw(8) << "index"
//            << setw(10) << "callsign"
//            << setw(12) << "priolevel"
//            << setw(12) << "arrtick"
//            << setw(10) << "fuel" << "\n";
//        cout << "----------------------------------------------------\n";
//
//        for (int i = 0; i < heapsize; i++) {
//            flight* f = heap[i];
//            cout << left << setw(8) << i
//                << setw(10) << f->callsign
//                << setw(12) << f->prioritylevel
//                << setw(12) << f->arrivaltick
//                << setw(10) << f->fuelremaining << "\n";
//        }
//        cout << "----------------------------------------------------\n";
//    }
//};
//
//// 4.3 controller interface: emergencylanding
//class emergencylanding {
//private:
//    normalqueue normalqueue;
//    emergencyheap emergencyheap;
//
//public:
//    emergencylanding() {}
//
//    // queue operations
//    void enqueuenormal(flight* flight) {
//        normalqueue.enqueue(flight);
//    }
//
//    flight* dequeuenormal() {
//        return normalqueue.dequeue();
//    }
//
//    flight* frontnormal() const {
//        return normalqueue.front();
//    }
//
//    bool isemptynormal() const {
//        return normalqueue.isempty();
//    }
//
//    void displaynormalqueue() const {
//        normalqueue.display();
//    }
//
//    // heap operations
//    void insertemergency(flight* flight) {
//        normalqueue.removebycallsign(flight->callsign); // ensure it is not left behind in normal queue
//        emergencyheap.insert(flight);
//    }
//
//    flight* extractmin() {
//        return emergencyheap.extractmin();
//    }
//
//    flight* peekmin() const {
//        return emergencyheap.peekmin();
//    }
//
//    bool isemptyheap() const {
//        return emergencyheap.isempty();
//    }
//
//    void displayheap() const {
//        emergencyheap.display();
//    }
//
//    // priority landing rule: normal queue first, then emergency heap
//    flight* processnextlanding() {
//        if (!normalqueue.isempty()) {
//            return normalqueue.dequeue();
//        }
//        else if (!emergencyheap.isempty()) {
//            return emergencyheap.extractmin();
//        }
//        return nullptr;
//    }
//};
//
//// ========================================================
//// 5. scenario parser (event loader)
//// ========================================================
//
//struct scenarioevent {
//    int tick = 0;
//    string eventtype = "";
//    string target = "";
//};
//class scenarioloader {
//private:
//    static const int max_events = 50;
//    scenarioevent events[max_events];
//    int eventcount;
//
//    static int splitpipes(const string& line, string tokens[], int maxtokens) {
//        int count = 0;
//        string current = "";
//        for (size_t i = 0; i < line.length(); i++) {
//            if (line[i] == '|') {
//                if (count < maxtokens) tokens[count++] = current;
//                current = "";
//            }
//            else if (line[i] != '\r') {
//                current += line[i];
//            }
//        }
//        if (count < maxtokens) tokens[count++] = current;
//        return count;
//    }
//
//    static int mapeventtypetopriority(const string& type) {
//        if (type == "war") return 1;
//        if (type == "hijack") return 2;
//        if (type == "cyber") return 3;
//        if (type == "missing") return 4;
//        if (type == "fuel") return 5;
//        if (type == "medical") return 6;
//        if (type == "weather") return 7;
//        if (type == "mechanical") return 8;
//        return 9;
//    }
//
//public:
//    scenarioloader() : eventcount(0) {}
//
//    bool loadscenario(const string& filename) {
//        ifstream file(filename);
//        if (!file.is_open()) {
//            cout << "error: could not open " << filename << "\n";
//            return false;
//        }
//
//        string line;
//        bool isheader = true;
//        string tokens[3];
//
//        while (getline(file, line)) {
//            if (line.empty()) continue;
//            if (isheader) {
//                isheader = false;
//                continue;
//            }
//            int count = splitpipes(line, tokens, 3);
//            if (count == 3 && eventcount < max_events) {
//                events[eventcount].tick = stoi(tokens[0]);
//                events[eventcount].eventtype = tokens[1];
//                events[eventcount].target = tokens[2];
//                eventcount++;
//            }
//        }
//        file.close();
//        return true;
//    }
//
//    void applyeventsfortick(int currenttick, registration& reg, emergencylanding& dispatcher) {
//        for (int i = 0; i < eventcount; i++) {
//            if (events[i].tick == currenttick) {
//                cout << "\n>>> [tick event] " << events[i].eventtype << " on " << events[i].target << " <<<\n";
//                flight* f = reg.findflight(events[i].target);
//                if (f != nullptr) {
//                    f->prioritylevel = mapeventtypetopriority(events[i].eventtype);
//                    f->arrivaltick = currenttick; // updated trigger tick for tie-breaking
//                    dispatcher.insertemergency(f);
//                    cout << "flight " << f->callsign << " escalated to priority " << f->prioritylevel << " and routed to emergency heap.\n";
//                }
//                else {
//                    cout << "event target not found in active flights registry (or sector/missing): " << events[i].target << "\n";
//                }
//            }
//        }
//    }
//};
//
//
//
//// ========================================================
//// 6. module 4: flight manifest
//// class: bstflight
//// ========================================================
//
//// tree node for the binary search tree
//struct bstnode {
//    flight* flight;
//    bstnode* left;
//    bstnode* right;
//
//    bstnode(flight* f) : flight(f), left(nullptr), right(nullptr) {}
//};
//
//class bstflight {
//private:
//    bstnode* root;
//
//    // helper: recursive insertion by lexicographical callsign order
//    bstnode* inserthelper(bstnode* node, flight* flight) {
//        if (node == nullptr) {
//            return new bstnode(flight);
//        }
//
//        if (flight->callsign < node->flight->callsign) {
//            node->left = inserthelper(node->left, flight);
//        }
//        else if (flight->callsign > node->flight->callsign) {
//            node->right = inserthelper(node->right, flight);
//        }
//        else {
//            // duplicate callsign: update flight reference
//            node->flight = flight;
//        }
//        return node;
//    }
//
//    // helper: recursive binary search
//    flight* searchhelper(bstnode* node, const string& callsign) const {
//        if (node == nullptr) {
//            return nullptr;
//        }
//
//        if (callsign == node->flight->callsign) {
//            return node->flight;
//        }
//        else if (callsign < node->flight->callsign) {
//            return searchhelper(node->left, callsign);
//        }
//        else {
//            return searchhelper(node->right, callsign);
//        }
//    }
//
//    // helper: recursive in-order traversal (left -> node -> right)
//    void inorderhelper(bstnode* node) const {
//        if (node == nullptr) return;
//
//        inorderhelper(node->left);
//
//        cout << left << setw(10) << node->flight->callsign
//            << setw(16) << node->flight->aircrafttype
//            << setw(10) << node->flight->airline
//            << setw(10) << node->flight->origin
//            << setw(10) << node->flight->destination
//            << setw(6) << node->flight->passengercount
//            << setw(6) << node->flight->fuelremaining
//            << setw(22) << node->flight->currentsector
//            << setw(12) << node->flight->assignedrunway
//            << setw(6) << node->flight->prioritylevel
//            << "\n";
//
//        inorderhelper(node->right);
//    }
//
//    // helper: recursive height computation
//    int heighthelper(bstnode* node) const {
//        if (node == nullptr) {
//            return 0; // empty tree has height 0 (single root has height 1)
//        }
//        int lefth = heighthelper(node->left);
//        int righth = heighthelper(node->right);
//        return 1 + (lefth > righth ? lefth : righth);
//    }
//
//    // helper: post-order recursive cleanup (left -> right -> node)
//    void destroyhelper(bstnode* node) {
//        if (node == nullptr) return;
//
//        destroyhelper(node->left);
//        destroyhelper(node->right);
//        delete node; // frees tree node wrapper; does not delete node->flight
//    }
//
//public:
//    bstflight() : root(nullptr) {}
//
//    ~bstflight() {
//        destroytree();
//    }
//
//    // insert a flight pointer into the manifest bst
//    void insert(flight* flight) {
//        if (flight == nullptr) return;
//        root = inserthelper(root, flight);
//    }
//
//    // search by callsign (o(h) average o(log n), worst o(n))
//    flight* search(const string& callsign) const {
//        return searchhelper(root, callsign);
//    }
//
//    // print all flights alphabetically by callsign
//    void inordertraversal() const {
//        if (root == nullptr) {
//            cout << "flight manifest (bst): [empty]\n";
//            return;
//        }
//        cout << "\n======================== flight manifest (bst in-order) ========================\n";
//        cout << left << setw(10) << "callsign"
//            << setw(16) << "type"
//            << setw(10) << "airline"
//            << setw(10) << "from"
//            << setw(10) << "to"
//            << setw(6) << "pass"
//            << setw(6) << "fuel"
//            << setw(22) << "sector"
//            << setw(12) << "runway"
//            << setw(6) << "prio"
//            << "\n";
//        cout << "--------------------------------------------------------------------------------\n";
//        inorderhelper(root);
//        cout << "================================================================================\n";
//    }
//
//    // calculate maximum tree height
//    int height() const {
//        return heighthelper(root);
//    }
//
//    // deallocate all tree nodes
//    void destroytree() {
//        destroyhelper(root);
//        root = nullptr;
//    }
//
//    bool isempty() const {
//        return root == nullptr;
//    }
//};
//
//
//
//// ========================================================
//// module 5: balanced flight index
//// class: avlflight
//// ========================================================
//
//struct avlnode {
//    flight* flight;
//    avlnode* left;
//    avlnode* right;
//    int nodeheight;
//
//    avlnode(flight* f)
//        : flight(f), left(nullptr), right(nullptr), nodeheight(1) {
//    }
//};
//
//class avlflight {
//private:
//    avlnode* root;
//
//    int getnodeheight(avlnode* node) const {
//        return (node == nullptr) ? 0 : node->nodeheight;
//    }
//
//    int maxval(int a, int b) const {
//        return (a > b) ? a : b;
//    }
//
//    int getbalancefactor(avlnode* node) const {
//        if (node == nullptr) return 0;
//        return getnodeheight(node->left) - getnodeheight(node->right);
//    }
//
//    // single right rotation (ll case)
//    avlnode* rotateright(avlnode* y) {
//        avlnode* x = y->left;
//        avlnode* t2 = x->right;
//
//        // perform rotation
//        x->right = y;
//        y->left = t2;
//
//        // update heights (order matters: children first)
//        y->nodeheight = 1 + maxval(getnodeheight(y->left), getnodeheight(y->right));
//        x->nodeheight = 1 + maxval(getnodeheight(x->left), getnodeheight(x->right));
//
//        return x; // new root of subtree
//    }
//
//    // single left rotation (rr case)
//    avlnode* rotateleft(avlnode* x) {
//        avlnode* y = x->right;
//        avlnode* t2 = y->left;
//
//        // perform rotation
//        y->left = x;
//        x->right = t2;
//
//        // update heights
//        x->nodeheight = 1 + maxval(getnodeheight(x->left), getnodeheight(x->right));
//        y->nodeheight = 1 + maxval(getnodeheight(y->left), getnodeheight(y->right));
//
//        return y; // new root of subtree
//    }
//
//    // double rotation: left-right (lr case)
//    avlnode* rotateleftright(avlnode* node) {
//        node->left = rotateleft(node->left);
//        return rotateright(node);
//    }
//
//    // double rotation: right-left (rl case)
//    avlnode* rotaterightleft(avlnode* node) {
//        node->right = rotateright(node->right);
//        return rotateleft(node);
//    }
//
//    avlnode* inserthelper(avlnode* node, flight* flight) {
//        // 1. standard bst insertion
//        if (node == nullptr) {
//            return new avlnode(flight);
//        }
//
//        if (flight->callsign < node->flight->callsign) {
//            node->left = inserthelper(node->left, flight);
//        }
//        else if (flight->callsign > node->flight->callsign) {
//            node->right = inserthelper(node->right, flight);
//        }
//        else {
//            // duplicate callsigns not permitted
//            return node;
//        }
//
//        // 2. update height of current ancestor node
//        node->nodeheight = 1 + maxval(getnodeheight(node->left), getnodeheight(node->right));
//
//        // 3. determine balance factor
//        int balance = getbalancefactor(node);
//
//        // 4. check the 4 unbalance cases:
//        // left-left (ll)
//        if (balance > 1 && flight->callsign < node->left->flight->callsign) {
//            return rotateright(node);
//        }
//
//        // right-right (rr)
//        if (balance < -1 && flight->callsign > node->right->flight->callsign) {
//            return rotateleft(node);
//        }
//
//        // left-right (lr)
//        if (balance > 1 && flight->callsign > node->left->flight->callsign) {
//            return rotateleftright(node);
//        }
//
//        // right-left (rl)
//        if (balance < -1 && flight->callsign < node->right->flight->callsign) {
//            return rotaterightleft(node);
//        }
//
//        return node;
//    }
//
//    flight* searchhelper(avlnode* node, const string& callsign) const {
//        if (node == nullptr) return nullptr;
//        if (node->flight->callsign == callsign) return node->flight;
//
//        if (callsign < node->flight->callsign) {
//            return searchhelper(node->left, callsign);
//        }
//        return searchhelper(node->right, callsign);
//    }
//
//    void inorderhelper(avlnode* node) const {
//        if (node == nullptr) return;
//        inorderhelper(node->left);
//        cout << "  [" << node->flight->callsign << "] "
//            << node->flight->aircrafttype << " | "
//            << node->flight->airline << " | priority: "
//            << node->flight->prioritylevel << " | sector: "
//            << node->flight->currentsector << "\n";
//        inorderhelper(node->right);
//    }
//
//    void destroyhelper(avlnode* node) {
//        if (node == nullptr) return;
//        destroyhelper(node->left);
//        destroyhelper(node->right);
//        delete node; // deletes only the tree index node, not the flight
//    }
//
//public:
//    avlflight() : root(nullptr) {}
//
//    ~avlflight() {
//        destroytree();
//    }
//
//    void insert(flight* flight) {
//        if (flight != nullptr) {
//            root = inserthelper(root, flight);
//        }
//    }
//
//    flight* search(const string& callsign) const {
//        return searchhelper(root, callsign);
//    }
//
//    void inordertraversal() const {
//        if (root == nullptr) {
//            cout << "avl manifest is empty.\n";
//            return;
//        }
//        inorderhelper(root);
//    }
//
//    int height() const {
//        return getnodeheight(root);
//    }
//
//    void destroytree() {
//        destroyhelper(root);
//        root = nullptr;
//    }
//};
//
//
//void runheightcomparisondemo() {
//    bstflight bst;
//    avlflight avl;
//
//    // 15 strictly alphabetical callsigns
//    string samplecallsigns[15] = {
//        "aa-101", "aa-202", "bb-100", "bb-200", "bb-300",
//        "cc-401", "dd-501", "ee-601", "ff-701", "gg-801",
//        "hh-901", "ii-111", "jj-222", "kk-333", "ll-444"
//    };
//
//    // allocate dummy flight objects for verification
//    flight* dummyflights[15];
//    for (int i = 0; i < 15; i++) {
//
//        dummyflights[i] = new flight(samplecallsigns[i], "demojet", "blackbox",
//            "origin", "dest", 100, 20,
//            "sector 1", "runway 04", 0, 9);
//        dummyflights[i]->callsign = samplecallsigns[i];
//        dummyflights[i]->aircrafttype = "demojet";
//        dummyflights[i]->prioritylevel = 9;
//
//        bst.insert(dummyflights[i]);
//        avl.insert(dummyflights[i]);
//    }
//
//    cout << "\n============================================\n";
//    cout << "   module 5 verification: bst vs avl height   \n";
//    cout << "============================================\n";
//    cout << "inserted elements: 15 (alphabetically ordered)\n";
//    cout << "bst height (degenerated chain) : " << bst.height() << " (expected: 15)\n";
//    cout << "avl height (self-balanced o(log n)): " << avl.height() << " (expected: 4)\n";
//    cout << "============================================\n";
//
//    // clean up temporary tree structures and dummy flights
//    bst.destroytree();
//    avl.destroytree();
//    for (int i = 0; i < 15; i++) {
//        delete dummyflights[i];
//    }
//}
//
//
//
//
//
//// ========================================================
//// module 6: airspace network (space graph)
//// class: spacegraph
//// ========================================================
//
//// 1. linked node for adjacency list (sparse representation)
//struct edgenode {
//    int destindex;
//    int weight;
//    edgenode* next;
//
//    edgenode(int dest, int w) : destindex(dest), weight(w), next(nullptr) {}
//};
//
//class spacegraph {
//private:
//    static const int max_sectors = 30; // airspace has 11-15 sectors/runways
//    const int inf = 999999;
//
//    string* sectornames;      // 1d dynamic array: index -> sector name
//    edgenode** adjlist;       // array of linked-node head pointers
//    int** adjmatrix;          // 2d dynamic array: o(1) edge-weight lookup
//    int sectorcount;
//    int capacity;
//
//    // helper: split line by '|' delimiter without std::stringstream
//    static int splitpipes(const string& line, string tokens[], int maxtokens) {
//        int count = 0;
//        string current = "";
//        for (size_t i = 0; i < line.length(); i++) {
//            if (line[i] == '|') {
//                if (count < maxtokens) tokens[count++] = current;
//                current = "";
//            }
//            else if (line[i] != '\r') {
//                current += line[i];
//            }
//        }
//        if (count < maxtokens) tokens[count++] = current;
//        return count;
//    }
//
//public:
//    spacegraph(int cap = max_sectors) : sectorcount(0), capacity(cap) {
//        // allocate sector names mapping
//        sectornames = new string[capacity];
//
//        // allocate adjacency list headers
//        adjlist = new edgenode * [capacity];
//        for (int i = 0; i < capacity; i++) {
//            adjlist[i] = nullptr;
//        }
//
//        // allocate 2d adjacency matrix
//        adjmatrix = new int* [capacity];
//        for (int i = 0; i < capacity; i++) {
//            adjmatrix[i] = new int[capacity];
//            for (int j = 0; j < capacity; j++) {
//                adjmatrix[i][j] = (i == j) ? 0 : inf;
//            }
//        }
//    }
//
//    ~spacegraph() {
//        // clean up adjacency list
//        for (int i = 0; i < capacity; i++) {
//            edgenode* curr = adjlist[i];
//            while (curr != nullptr) {
//                edgenode* temp = curr;
//                curr = curr->next;
//                delete temp;
//            }
//        }
//        delete[] adjlist;
//
//        // clean up adjacency matrix
//        for (int i = 0; i < capacity; i++) {
//            delete[] adjmatrix[i];
//        }
//        delete[] adjmatrix;
//
//        // clean up names array
//        delete[] sectornames;
//    }
//
//    // register a sector/runway vertex; returns its index
//    int addsector(const string& name) {
//        for (int i = 0; i < sectorcount; i++) {
//            if (sectornames[i] == name) return i; // already registered
//        }
//        if (sectorcount >= capacity) {
//            cout << "error: spacegraph capacity exceeded!\n";
//            return -1;
//        }
//
//        int index = sectorcount;
//        sectornames[index] = name;
//        sectorcount++;
//        return index;
//    }
//
//    int getsectorindex(const string& name) const {
//        for (int i = 0; i < sectorcount; i++) {
//            if (sectornames[i] == name) return i;
//        }
//        return -1;
//    }
//
//    string getsectorname(int index) const {
//        if (index >= 0 && index < sectorcount) {
//            return sectornames[index];
//        }
//        return "unknown";
//    }
//
//    int getsectorcount() const {
//        return sectorcount;
//    }
//
//    // add bidirectional corridor edge to both list and matrix
//    void addcorridor(const string& from, const string& to, int weightseconds) {
//        int u = addsector(from);
//        int v = addsector(to);
//
//        if (u == -1 || v == -1) return;
//
//        // 1. update adjacency matrix (both directions)
//        adjmatrix[u][v] = weightseconds;
//        adjmatrix[v][u] = weightseconds;
//
//        // 2. update adjacency list: u -> v
//        edgenode* nodev = new edgenode(v, weightseconds);
//        nodev->next = adjlist[u];
//        adjlist[u] = nodev;
//
//        // update adjacency list: v -> u
//        edgenode* nodeu = new edgenode(u, weightseconds);
//        nodeu->next = adjlist[v];
//        adjlist[v] = nodeu;
//    }
//
//    // o(1) edge lookup using the matrix
//    int getweight(int fromindex, int toindex) const {
//        if (fromindex >= 0 && fromindex < sectorcount && toindex >= 0 && toindex < sectorcount) {
//            return adjmatrix[fromindex][toindex];
//        }
//        return inf;
//    }
//
//    int getweight(const string& from, const string& to) const {
//        int u = getsectorindex(from);
//        int v = getsectorindex(to);
//        if (u != -1 && v != -1) {
//            return adjmatrix[u][v];
//        }
//        return inf;
//    }
//
//    // provide head pointer of neighbor list for traversals (bfs, dfs, dijkstra)
//    edgenode* getneighbors(int sectorindex) const {
//        if (sectorindex >= 0 && sectorindex < sectorcount) {
//            return adjlist[sectorindex];
//        }
//        return nullptr;
//    }
//
//    // file i/o: construct network from airspace.txt
//    bool loadairspace(const string& filename) {
//        ifstream file(filename);
//        if (!file.is_open()) {
//            cout << "error: could not open " << filename << "\n";
//            return false;
//        }
//
//        string line;
//        bool isheader = true;
//        string tokens[3];
//
//        while (getline(file, line)) {
//            if (line.empty()) continue;
//            // detect header line
//            if (isheader && (line.find("from|to|weightticks") != string::npos || line.find("from") != string::npos)) {
//                isheader = false;
//                continue;
//            }
//            isheader = false;
//
//            int count = splitpipes(line, tokens, 3);
//            if (count == 3) {
//                addcorridor(tokens[0], tokens[1], stoi(tokens[2]));
//            }
//        }
//        file.close();
//        return true;
//    }
//
//    void displayadjacencylist() const {
//        cout << "\n======================== airspace adjacency list ========================\n";
//        for (int i = 0; i < sectorcount; i++) {
//            cout << "[" << left << setw(2) << i << "] " << setw(24) << sectornames[i] << " -> ";
//            edgenode* curr = adjlist[i];
//            if (curr == nullptr) {
//                cout << "[isolated]";
//            }
//            while (curr != nullptr) {
//                cout << "(" << sectornames[curr->destindex] << ", " << curr->weight << "s)";
//                if (curr->next != nullptr) cout << " -> ";
//                curr = curr->next;
//            }
//            cout << "\n";
//        }
//        cout << "=========================================================================\n";
//    }
//
//    void displayadjacencymatrix() const {
//        cout << "\n================================ adjacency matrix ================================\n";
//        cout << setw(4) << " ";
//        for (int i = 0; i < sectorcount; i++) {
//            cout << setw(4) << i;
//        }
//        cout << "\n";
//
//        for (int i = 0; i < sectorcount; i++) {
//            cout << left << setw(4) << i;
//            for (int j = 0; j < sectorcount; j++) {
//                if (adjmatrix[i][j] == inf) {
//                    cout << setw(4) << "-";
//                }
//                else {
//                    cout << setw(4) << adjmatrix[i][j];
//                }
//            }
//            cout << " | " << sectornames[i] << "\n";
//        }
//        cout << "==================================================================================\n";
//    }
//};
//
//
//int main() {
//    spacegraph airspace;
//
//    cout << "loading airspace network from airspace.txt...\n";
//    if (!airspace.loadairspace("airspace.txt")) {
//        cout << "failed to load airspace.txt\n";
//        return 1;
//    }
//
//    cout << "\ntotal vertices (sectors/runways) loaded: " << airspace.getsectorcount() << "\n";
//
//    // display representations
//    airspace.displayadjacencylist();
//    airspace.displayadjacencymatrix();
//
//    // verify key weights
//    cout << "\n[sanity checks]\n";
//    cout << "travel time (control tower <-> runway 12 (west)): "
//        << airspace.getweight("control tower", "runway 12 (west)") << "s (expected: 2)\n";
//    cout << "travel time (alpha tree (south) <-> restricted zone): "
//        << airspace.getweight("alpha tree (south)", "restricted zone") << "s (expected: 3)\n";
//    cout << "travel time (control tower <-> restricted zone): "
//        << (airspace.getweight("control tower", "restricted zone") >= 999999 ? "no direct edge (inf)" : "edge exists")
//        << " (expected: no direct edge)\n";
//
//    return 0;
//}