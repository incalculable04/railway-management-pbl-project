#include <iostream>
#include <string>

using namespace std;

// Passenger Node structure
class Passenger {
public:
    string name;
    int age;
    int pnr;
    int seatNo;
    double fare;
    string status;
    int trainNo;
    bool isDisabled;
    Passenger* next;

    Passenger(string n = "", int a = 0, int p = 0, int s = 0, int t = 0, bool disabled = false) {
        name = n;
        age = a;
        pnr = p;
        seatNo = s;
        trainNo = t;
        isDisabled = disabled;
        status = "Confirmed";
        next = nullptr;

        // Fare calculation rules
        if (age < 10) {
            fare = 50.0; // Child discount
        } else if (age >= 60) {
            fare = 60.0; // Senior citizen discount
        } else {
            fare = 100.0; // Standard price
        }
    }

    // 1. OPERATOR OVERLOADING: Stream Insertion (<<) for elegant printing
    friend ostream& operator<<(ostream& os, const Passenger& p) {
        os << "Name: " << p.name 
           << " | Age: " << p.age 
           << " | PNR: " << p.pnr 
           << " | Seat: " << (p.seatNo == 0 ? "Waiting" : to_string(p.seatNo))
           << " | Train: " << p.trainNo 
           << " | Category: " << (p.isDisabled ? "Disabled (Priority)" : "General")
           << " | Fare: Rs." << p.fare 
           << " | Status: " << p.status << '\n';
        return os;
    }

    // 2. OPERATOR OVERLOADING: Less Than (<) to determine priority ranking
    // Returns true if 'this' passenger has lower priority than 'other'
    bool operator<(const Passenger& other) const {
        // Disabled passengers have strictly higher priority than General passengers
        if (this->isDisabled != other.isDisabled) {
            return !this->isDisabled; // If 'this' is not disabled but 'other' is, 'this' has less priority
        }
        // If both have the same priority status, we maintain FIFO order (handled in queue traversal)
        return false; 
    }
};

// 1. LINKED LIST for Confirmed Bookings
class ConfirmedList {
private:
    Passenger* head;

public:
    ConfirmedList() : head(nullptr) {}

    void addPassenger(Passenger* newPassenger) {
        if (!head) {
            head = newPassenger;
        } else {
            Passenger* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newPassenger;
        }
        newPassenger->next = nullptr;
    }

    Passenger* searchAndRemove(int pnr) {
        if (!head) return nullptr;
        if (head->pnr == pnr) {
            Passenger* target = head;
            head = head->next;
            target->next = nullptr;
            return target;
        }
        Passenger* prev = head;
        Passenger* curr = head->next;
        while (curr) {
            if (curr->pnr == pnr) {
                prev->next = curr->next;
                curr->next = nullptr;
                return curr;
            }
            prev = curr;
            curr = curr->next;
        }
        return nullptr;
    }

    Passenger* search(int pnr) const {
        Passenger* temp = head;
        while (temp) {
            if (temp->pnr == pnr) return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    void display() const {
        if (!head) {
            cout << "No confirmed bookings.\n";
            return;
        }
        Passenger* temp = head;
        int i = 1;
        while (temp) {
            cout << i++ << ". " << *temp; // Using overloaded << operator
            temp = temp->next;
        }
    }
    
    bool isEmpty() const { return head == nullptr; }
};

// 2. PRIORITY QUEUE for Disabled / Special Passengers
class PriorityWaitingQueue {
private:
    Passenger* frontNode;

public:
    PriorityWaitingQueue() : frontNode(nullptr) {}

    void enqueue(Passenger* p) {
        p->status = "Waiting (Priority)";
        p->seatNo = 0;

        // Using overloaded < operator to check if head node has lower priority than new passenger
        if (!frontNode || (*frontNode < *p)) {
            p->next = frontNode;
            frontNode = p;
            return;
        }

        Passenger* temp = frontNode;
        // Traverse as long as the next passenger does NOT have less priority than the new passenger
        // This ensures disabled passengers bundle together at the front in FIFO sequence
        if (p->isDisabled) {
            while (temp->next && temp->next->isDisabled) {
                temp = temp->next;
            }
        } else {
            while (temp->next) {
                temp = temp->next;
            }
        }

        p->next = temp->next;
        temp->next = p;
    }

    Passenger* dequeue() {
        if (!frontNode) return nullptr;
        Passenger* temp = frontNode;
        frontNode = frontNode->next;
        temp->next = nullptr;
        return temp;
    }

    bool isEmpty() const { return frontNode == nullptr; }

    void display() const {
        if (!frontNode) {
            cout << "Priority waiting list is empty.\n";
            return;
        }
        Passenger* temp = frontNode;
        int i = 1;
        while (temp) {
            cout << i++ << ". " << *temp; // Using overloaded << operator
            temp = temp->next;
        }
    }
};

// 3. STACK for Cancelled Tickets (LIFO)
class CancelledStack {
private:
    Passenger* topNode;

public:
    CancelledStack() : topNode(nullptr) {}

    void push(Passenger* p) {
        p->status = "Cancelled";
        p->next = topNode;
        topNode = p;
    }

    void display() const {
        if (!topNode) {
            cout << "No cancellation history.\n";
            return;
        }
        Passenger* temp = topNode;
        int i = 1;
        while (temp) {
            cout << i++ << ". " << *temp; // Using overloaded << operator
            temp = temp->next;
        }
    }
};

// Main Railway System Controller
class RailwaySystem {
private:
    ConfirmedList confirmedList;
    PriorityWaitingQueue waitingQueue;
    CancelledStack cancelledStack;
    int nextPNR;
    int nextSeat;
    int maxConfirmedSeats;
    int currentConfirmedCount;

public:
    RailwaySystem() : nextPNR(1001), nextSeat(1), maxConfirmedSeats(3), currentConfirmedCount(0) {}

    void bookTicket() {
        string name;
        int age, trainNo, choice;
        bool isDisabled = false;

        cout << "\n--- BOOK TICKET ---\n";

cout << "Enter Passenger Name: ";
cin.ignore(10000, '\n');
getline(cin, name);

if (name.empty()) {
    cout << "Name cannot be empty.\n";
    return;
}

cout << "Enter Age: ";
if (!(cin >> age) || age < 0 || age > 120) {
    cout << "Invalid age. Enter an age between 0 and 120.\n";
    cin.clear();
    cin.ignore(10000, '\n');
    return;
}

cout << "Enter Train Number: ";
if (!(cin >> trainNo) || trainNo <= 0) {
    cout << "Invalid train number.\n";
    cin.clear();
    cin.ignore(10000, '\n');
    return;
}

cout << "Is passenger disabled? (1 for Yes, 0 for No): ";
if (!(cin >> choice) || (choice != 0 && choice != 1)) {
    cout << "Enter only 1 for Yes or 0 for No.\n";
    cin.clear();
    cin.ignore(10000, '\n');
    return;
}

isDisabled = (choice == 1);

        if (currentConfirmedCount < maxConfirmedSeats) {
            Passenger* newPassenger = new Passenger(name, age, nextPNR, nextSeat, trainNo, isDisabled);
            confirmedList.addPassenger(newPassenger);
            currentConfirmedCount++;
            cout << "\nTicket Confirmed Successfully!\n";
            cout << *newPassenger; // Using overloaded << operator
            nextPNR++;
            nextSeat++;
        } else {
            Passenger* newPassenger = new Passenger(name, age, nextPNR, 0, trainNo, isDisabled);
            waitingQueue.enqueue(newPassenger);
            cout << "\nSeats full! Added to Priority Waiting Queue.\n";
            cout << *newPassenger; // Using overloaded << operator
            nextPNR++;
        }
    }

    void cancelTicket() {
        if (confirmedList.isEmpty()) {
            cout << "\nNo confirmed bookings to cancel.\n";
            return;
        }

        int pnr;
        cout << "\nEnter PNR to cancel: ";
        cin >> pnr;

        Passenger* cancelledPassenger = confirmedList.searchAndRemove(pnr);
        if (cancelledPassenger) {
            int freedSeat = cancelledPassenger->seatNo;
            currentConfirmedCount--;
            cout << "\nTicket with PNR " << pnr << " cancelled successfully.\n";
            
            cancelledStack.push(cancelledPassenger);

            // Auto-promote highest priority waiting passenger
            if (!waitingQueue.isEmpty()) {
                Passenger* promotedPassenger = waitingQueue.dequeue();
                promotedPassenger->seatNo = freedSeat;
                promotedPassenger->status = "Confirmed";
                confirmedList.addPassenger(promotedPassenger);
                currentConfirmedCount++;
                cout << "\n[Auto-Promotion] Priority waiting passenger promoted to confirmed seat!\n";
                cout << *promotedPassenger; // Using overloaded << operator
            }
        } else {
            cout << "Booking not found in confirmed list for PNR: " << pnr << "\n";
        }
    }

    void searchBooking() const {
        Passenger* search(int pnr) const {
    Passenger* temp = frontNode;

    while (temp) {
        if (temp->pnr == pnr) {
            return temp;
        }
        temp = temp->next;
    }

    return nullptr;
}
    }

    void displayWaitingList() const {
        cout << "\n--- PRIORITY WAITING LIST ---\n";
        waitingQueue.display();
    }

    void displayCancelledHistory() const {
        cout << "\n--- CANCELLATION HISTORY (Stack) ---\n";
        cancelledStack.display();
    }

    void displayAllConfirmed() const {
        cout << "\n--- CONFIRMED BOOKINGS (Linked List) ---\n";
        confirmedList.display();
    }
};

int main() {
    RailwaySystem system;
    int choice;

    do {
        cout << "\n   RAILWAY RESERVATION & PRIORITY SYSTEM    \n";    
        cout << "1. Book Ticket\n";
        cout << "2. Cancel Ticket\n";
        cout << "3. View / Search Confirmed Booking\n";
        cout << "4. Display Priority Waiting List\n";
        cout << "5. Display Cancelled Tickets (Stack)\n";
        cout << "6. Display All Confirmed Bookings\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000);
            cout << "Invalid input. Enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1: system.bookTicket(); break;
            case 2: system.cancelTicket(); break;
            case 3: system.searchBooking(); break;
            case 4: system.displayWaitingList(); break;
            case 5: system.displayCancelledHistory(); break;
            case 6: system.displayAllConfirmed(); break;
            case 0: cout << "Exiting system. Goodbye!\n"; break;
            default: cout << "Invalid choice. Select 0-6.\n";
        }
    } while (choice != 0);

    return 0;
}
