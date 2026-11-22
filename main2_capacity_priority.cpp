#include <iostream>
#include <string>
#include <limits>

using namespace std;

// 1. ABSTRACT BASE CLASS
class Passenger {
public:
    string name;
    int age;
    int pnr;
    int seatNo;
    string status;
    int trainNo;
    string source;
    string destination;
    bool isDisabled;
    Passenger* next;
    Passenger* prev;

    Passenger(string n = "", int a = 0, int p = 0, int s = 0, int t = 0,
              string src = "", string dest = "", bool disabled = false)
        : name(n), age(a), pnr(p), seatNo(s), status("Confirmed"),
          trainNo(t), source(src), destination(dest), isDisabled(disabled),
          next(nullptr), prev(nullptr) {}

    virtual ~Passenger() {}

    virtual double calculateFare() const = 0;
    virtual string getTicketType() const = 0;

    void displayDetails() const {
        cout << "Name: " << name
             << " | Age: " << age
             << " | Type: " << getTicketType()
             << " | PNR: " << pnr
             << " | Seat: ";
        if (seatNo == 0) {
            cout << "Waiting";
        } else {
            cout << seatNo;
        }
        cout << " | Train: " << trainNo
             << " | Route: " << source << " -> " << destination
             << " | Priority: " << (isDisabled ? "Disabled" : "Standard")
             << " | Ticket Fare: Rs." << calculateFare()
             << " | Status: " << status << '\n';
    }
};

// 2. DERIVED CLASSES
class ChildPassenger : public Passenger {
public:
    ChildPassenger(string n, int a, int p, int s, int t, string src, string dest, bool disabled)
        : Passenger(n, a, p, s, t, src, dest, disabled) {}

    double calculateFare() const override {
        return 50.0;
    }

    string getTicketType() const override {
        return "Child";
    }
};

class SeniorPassenger : public Passenger {
public:
    SeniorPassenger(string n, int a, int p, int s, int t, string src, string dest, bool disabled)
        : Passenger(n, a, p, s, t, src, dest, disabled) {}

    double calculateFare() const override {
        return 60.0;
    }

    string getTicketType() const override {
        return "Senior Citizen";
    }
};

class GeneralPassenger : public Passenger {
public:
    GeneralPassenger(string n, int a, int p, int s, int t, string src, string dest, bool disabled)
        : Passenger(n, a, p, s, t, src, dest, disabled) {}

    double calculateFare() const override {
        return 100.0;
    }

    string getTicketType() const override {
        return "General";
    }
};

// 3. DOUBLY LINKED LIST (Confirmed Bookings + Merge Sort)
class ConfirmedList {
private:
    Passenger* head;
    Passenger* tail;

    Passenger* split(Passenger* src) {
        Passenger* fast = src;
        Passenger* slow = src;

        while (fast->next != nullptr && fast->next->next != nullptr) {
            fast = fast->next->next;
            slow = slow->next;
        }

        Passenger* secondHalf = slow->next;
        slow->next = nullptr;
        if (secondHalf != nullptr) {
            secondHalf->prev = nullptr;
        }
        return secondHalf;
    }

    Passenger* merge(Passenger* first, Passenger* second) {
        if (first == nullptr) return second;
        if (second == nullptr) return first;

        if (first->seatNo <= second->seatNo) {
            first->next = merge(first->next, second);
            if (first->next != nullptr) {
                first->next->prev = first;
            }
            first->prev = nullptr;
            return first;
        } else {
            second->next = merge(first, second->next);
            if (second->next != nullptr) {
                second->next->prev = second;
            }
            second->prev = nullptr;
            return second;
        }
    }

    Passenger* mergeSortRec(Passenger* node) {
        if (node == nullptr || node->next == nullptr) {
            return node;
        }

        Passenger* second = split(node);
        node = mergeSortRec(node);
        second = mergeSortRec(second);

        return merge(node, second);
    }

public:
    ConfirmedList() : head(nullptr), tail(nullptr) {}

    ~ConfirmedList() {
        Passenger* curr = head;
        while (curr != nullptr) {
            Passenger* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
    }

    void addPassenger(Passenger* newPassenger) {
        newPassenger->status = "Confirmed";
        newPassenger->next = nullptr;
        newPassenger->prev = nullptr;

        if (head == nullptr) {
            head = tail = newPassenger;
        } else {
            tail->next = newPassenger;
            newPassenger->prev = tail;
            tail = newPassenger;
        }
    }

    void sortBySeat() {
        if (head == nullptr || head->next == nullptr) {
            return;
        }

        head = mergeSortRec(head);

        Passenger* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        tail = temp;
    }

    Passenger* searchAndRemove(int pnr) {
        Passenger* curr = head;
        while (curr != nullptr) {
            if (curr->pnr == pnr) {
                if (curr == head) head = curr->next;
                if (curr == tail) tail = curr->prev;
                if (curr->prev != nullptr) curr->prev->next = curr->next;
                if (curr->next != nullptr) curr->next->prev = curr->prev;

                curr->next = nullptr;
                curr->prev = nullptr;
                return curr;
            }
            curr = curr->next;
        }
        return nullptr;
    }

    Passenger* search(int pnr) const {
        Passenger* temp = head;
        while (temp != nullptr) {
            if (temp->pnr == pnr) return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    void display() const {
        if (head == nullptr) {
            cout << "No confirmed bookings.\n";
            return;
        }
        Passenger* temp = head;
        int i = 1;
        while (temp != nullptr) {
            cout << i++ << ". ";
            temp->displayDetails();
            temp = temp->next;
        }
    }

    void displayReverse() const {
        if (tail == nullptr) {
            cout << "No confirmed bookings.\n";
            return;
        }
        Passenger* temp = tail;
        int i = 1;
        while (temp != nullptr) {
            cout << i++ << ". ";
            temp->displayDetails();
            temp = temp->prev;
        }
    }

    bool isEmpty() const { return head == nullptr; }
};

// 4. PRIORITY QUEUE (Waiting List)
class PriorityWaitingQueue {
private:
    Passenger* frontNode;

public:
    PriorityWaitingQueue() : frontNode(nullptr) {}

    ~PriorityWaitingQueue() {
        Passenger* curr = frontNode;
        while (curr != nullptr) {
            Passenger* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
    }

    void enqueue(Passenger* p) {
        p->status = "Waiting (Priority)";
        p->seatNo = 0;
        p->prev = nullptr;
        p->next = nullptr;

        // Disabled-priority passengers are placed before standard passengers.
        // FIFO is preserved within each priority group.
        if (frontNode == nullptr) {
            frontNode = p;
            return;
        }

        if (p->isDisabled && !frontNode->isDisabled) {
            p->next = frontNode;
            frontNode->prev = p;
            frontNode = p;
            return;
        }

        Passenger* temp = frontNode;
        if (p->isDisabled) {
            // Move to the end of the existing priority group.
            while (temp->next != nullptr && temp->next->isDisabled) {
                temp = temp->next;
            }
        } else {
            // Standard passengers are appended, preserving FIFO.
            while (temp->next != nullptr) {
                temp = temp->next;
            }
        }

        p->next = temp->next;
        p->prev = temp;
        if (temp->next != nullptr) {
            temp->next->prev = p;
        }
        temp->next = p;
    }

    Passenger* dequeue() {
        if (frontNode == nullptr) return nullptr;
        Passenger* temp = frontNode;
        frontNode = frontNode->next;
        if (frontNode != nullptr) frontNode->prev = nullptr;
        temp->next = nullptr;
        temp->prev = nullptr;
        return temp;
    }

    bool isEmpty() const { return frontNode == nullptr; }

    void display() const {
        if (frontNode == nullptr) {
            cout << "Priority waiting list is empty.\n";
            return;
        }
        Passenger* temp = frontNode;
        int i = 1;
        while (temp != nullptr) {
            cout << i++ << ". ";
            temp->displayDetails();
            temp = temp->next;
        }
    }
};

// 5. STACK (Cancelled Tickets - LIFO)
class CancelledStack {
private:
    Passenger* topNode;

public:
    CancelledStack() : topNode(nullptr) {}

    ~CancelledStack() {
        Passenger* curr = topNode;
        while (curr != nullptr) {
            Passenger* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
    }

    void push(Passenger* p) {
        p->status = "Cancelled";
        p->prev = nullptr;
        p->next = topNode;
        topNode = p;
    }

    void display() const {
        if (topNode == nullptr) {
            cout << "No cancellation history.\n";
            return;
        }
        Passenger* temp = topNode;
        int i = 1;
        while (temp != nullptr) {
            cout << i++ << ". ";
            temp->displayDetails();
            temp = temp->next;
        }
    }
};

// 6. MAIN RAILWAY SYSTEM CONTROLLER
class RailwaySystem {
private:
    ConfirmedList confirmedList;
    PriorityWaitingQueue waitingQueue;
    CancelledStack cancelledStack;
    int nextPNR;
    int nextSeat;
    int maxConfirmedSeats;
    int currentConfirmedCount;

    Passenger* createPassenger(string name, int age, int pnr, int seat, int trainNo,
                             string source, string destination, bool isDisabled) {
        if (age < 10)
            return new ChildPassenger(name, age, pnr, seat, trainNo, source, destination, isDisabled);
        if (age >= 60)
            return new SeniorPassenger(name, age, pnr, seat, trainNo, source, destination, isDisabled);
        return new GeneralPassenger(name, age, pnr, seat, trainNo, source, destination, isDisabled);
    }


public:
    RailwaySystem(int capacity)
        : nextPNR(1001), nextSeat(1), maxConfirmedSeats(capacity), currentConfirmedCount(0) {}



    void bookTicket() {
        string name, source, destination;
        int age, trainNo;
        char priority;
        cout << "Passenger name: "; getline(cin, name);
        if (name.empty()) { cout << "Name cannot be empty.\n"; return; }
        cout << "Age: ";
        if (!(cin >> age) || age < 0) { cout << "Invalid age.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); return; }
        cout << "Train number: ";
        if (!(cin >> trainNo) || trainNo <= 0) { cout << "Invalid train number.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); return; }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Source: "; getline(cin, source);
        cout << "Destination: "; getline(cin, destination);
        cout << "Disability priority (y/n): "; cin >> priority;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        bool disabled = (priority == 'y' || priority == 'Y');
        int pnr = nextPNR++;
        Passenger* passenger = createPassenger(name, age, pnr, 0, trainNo, source, destination, disabled);
        if (currentConfirmedCount < maxConfirmedSeats) {
            passenger->seatNo = nextSeat++;
            confirmedList.addPassenger(passenger);
            confirmedList.sortBySeat(); // Keep confirmed tickets ordered using Merge Sort
            currentConfirmedCount++;
            cout << "Booking confirmed. PNR: " << pnr << ", Seat: " << passenger->seatNo << "\n";
        } else {
            waitingQueue.enqueue(passenger);
            cout << "Full capacity. Added to waiting list. PNR: " << pnr << "\n";
        }
    }

    void cancelTicket() {
        int pnr;
        cout << "PNR to cancel: ";
        if (!(cin >> pnr)) { cout << "Invalid PNR.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); return; }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        Passenger* passenger = confirmedList.searchAndRemove(pnr);
        if (!passenger) { cout << "Confirmed PNR not found.\n"; return; }
        currentConfirmedCount--;
        cancelledStack.push(passenger);
        cout << "Ticket cancelled.\n";
        if (!waitingQueue.isEmpty()) {
            Passenger* promoted = waitingQueue.dequeue();
            promoted->seatNo = nextSeat++;
            confirmedList.addPassenger(promoted);
            confirmedList.sortBySeat(); // Re-sort after promotion
            currentConfirmedCount++;
            cout << "A waiting passenger was promoted to confirmed.\n";
        }
    }

    void searchTicket() const {
        int pnr;
        cout << "PNR to search: ";
        if (!(cin >> pnr)) { cout << "Invalid PNR.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); return; }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        Passenger* p = confirmedList.search(pnr);
        if (p) p->displayDetails(); else cout << "PNR not found in confirmed tickets.\n";
    }

    void showConfirmed() const { cout << "\n-- Confirmed Tickets --\n"; confirmedList.display(); }
    void showWaiting() const { cout << "\n-- Waiting List --\n"; waitingQueue.display(); }
    void showCancelled() const { cout << "\n-- Cancelled Tickets --\n"; cancelledStack.display(); }
};

int main() {
    int capacity;
    cout << "Enter maximum train seat capacity: ";
    while (!(cin >> capacity) || capacity <= 0) {
        cout << "Capacity must be a positive whole number. Enter again: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    RailwaySystem railway(capacity);
    int choice;
    do {
        cout << "\n===== RAILWAY RESERVATION SYSTEM =====\n"
             << "1. Book/Confirm ticket\n2. Cancel confirmed ticket\n3. Search by PNR\n"
             << "4. Show confirmed tickets\n5. Show waiting list\n6. Show cancelled tickets\n0. Exit\n"
             << "Choice: ";
        if (!(cin >> choice)) { cout << "Please enter a valid number.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        switch (choice) {
            case 1: railway.bookTicket(); break;
            case 2: railway.cancelTicket(); break;
            case 3: railway.searchTicket(); break;
            case 4: railway.showConfirmed(); break;
            case 5: railway.showWaiting(); break;
            case 6: railway.showCancelled(); break;
            case 0: cout << "Goodbye.\n"; break;
            default: cout << "Invalid option. Try again.\n";
        }
    } while (choice != 0);
    return 0;
}
