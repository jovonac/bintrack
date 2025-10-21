/*
 CMPS 367
 Final Presentation - BinTrack
 Purpose: Manage storage bins using a simple Command-Line Interface (CLI)
 Group Name: Power PC
 Group Members: Vivianna Razon, Jovon Chiguina, Aditya Singh
 Date: 19 May 2025
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <stdexcept>

using namespace std;


//Removes the white space from the string

string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t");
    if (start == string::npos) return "";
    size_t end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}

//Turn every letter in the string to Lowercase
//This makes it case insensitive

string toLower(const string& str) {
    string s = str;
    for (size_t i = 0; i < s.size(); ++i) {
        s[i] = static_cast<char>(tolower(s[i]));
    }
    return s;
}


//This holds the information of each bin

class Bin {
public:
    string id;
    string name;
    bool isFragile;
    vector<string> items;
    string dateCreated;

    string toString() const {
        stringstream ss;
        ss << "[ " << id << " ] " << name
           << "\nFragile: " << (isFragile ? "Yes" : "No")
           << "\nItems: ";
        for (size_t i = 0; i < items.size(); ++i) {
            ss << items[i];
            if (i + 1 < items.size()) ss << ", ";
        }
        ss << "\nDate Created: " << dateCreated << "\n";
        return ss.str();
    }
};

//This manages all the bins with loading them,saving ,and menu actions


class BinManager {
private:
    vector<Bin> bins;
    const string fileName = "bins.txt";

    string generateID() {
        return "BIN" + to_string(static_cast<int>(bins.size()) + 1);
    }

    string currentDate() {
        time_t t = time(nullptr);
        tm* now = localtime(&t);
        stringstream ss;
        ss << (now->tm_year + 1900) << "-"
           << setw(2) << setfill('0') << (now->tm_mon + 1) << "-"
           << setw(2) << setfill('0') << now->tm_mday;
        return ss.str();
    }

    //write every bin to the files
    void saveBins() {
        ofstream file(fileName.c_str());
        if (!file) throw runtime_error("Error opening file for writing.");
        for (size_t i = 0; i < bins.size(); ++i) {
            const Bin& bin = bins[i];
            file << bin.id << "|" << bin.name << "|"
                 << (bin.isFragile ? "fragile" : "not_fragile") << "|";
            for (size_t j = 0; j < bin.items.size(); ++j) {
                file << bin.items[j];
                if (j + 1 < bin.items.size()) file << ",";
            }
            file << "|" << bin.dateCreated << "\n";
        }
    }


//Read the file and rebuild the bins vector

public:
    void loadBins() {
        bins.clear();
        ifstream file(fileName.c_str());
        if (!file) return;

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            Bin b;
            string frag, itemsStr;
            getline(ss, b.id, '|');
            getline(ss, b.name, '|');
            getline(ss, frag, '|');
            b.isFragile = (frag == "fragile");
            getline(ss, itemsStr, '|');
            getline(ss, b.dateCreated, '|');

            stringstream is(itemsStr);
            string item;
            while (getline(is, item, ',')) {
                item = trim(item);
                if (!item.empty()) b.items.push_back(item);
            }
            bins.push_back(b);
        }
    }
//Asks the user to enter the bin name and checks if there are no duplications
//Checks if the bin is name is empty,then saves the bin to the file

    void createBin() {
        string name;
        while (true) {
            cout << "Enter Bin Name: ";
            getline(cin, name);
            name = trim(name);
            if (name.empty()) {
                cout << "Name cannot be empty. Please try again.\n";
                continue;
            }
            bool duplicate = false;
            for (size_t i = 0; i < bins.size(); ++i) {
                if (toLower(bins[i].name) == toLower(name)) {
                    cout << "This BIN already exists. Choose another name.\n";
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate) break;
        }

        Bin bin;
        bin.id = generateID();
        bin.name = name;

        cout << "Is the bin fragile? (yes/no): ";
        string frag; getline(cin, frag);
        bin.isFragile = (toLower(trim(frag)) == "yes");

        cout << "Enter items (comma-separated, or leave blank): ";
        string itemsLine; getline(cin, itemsLine);
        stringstream ss(itemsLine);
        string itm;
        while (getline(ss, itm, ',')) {
            itm = trim(itm);
            if (!itm.empty()) bin.items.push_back(itm);
        }

        bin.dateCreated = currentDate();
        bins.push_back(bin);

        try {
            saveBins();
            cout << "Bin created successfully!\n";
        } catch (const exception& e) {
            cout << "Error saving bin: " << e.what() << "\n";
        }
    }

//Lets the user search for a bin by its ID

    // Step 2: Find a bin by number (1 → BIN1) or by name
void findBin() {
    cout << "Enter bin number (e.g. '1' for BIN1) or full bin name: ";
    string input;
    getline(cin, input);
    input = trim(input);
    string lowerInput = toLower(input);

    bool found = false;
    // look for matching ID or exact name
    for (size_t i = 0; i < bins.size(); ++i) {
        // build ID from number
        string tryID = "BIN" + input;
        if (bins[i].id == tryID
            || toLower(bins[i].name) == lowerInput)
        {
            cout << bins[i].toString();
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "No matching bin found.\n";
    }
}

    // Lets the user search for an item in the bins
    void findByItem() {
        cout << "Enter item to search for: ";
        string term; getline(cin, term);
        term = toLower(trim(term));
        for (size_t i = 0; i < bins.size(); ++i) {
            for (size_t j = 0; j < bins[i].items.size(); ++j) {
                if (toLower(bins[i].items[j]).find(term) != string::npos) {
                    cout << bins[i].toString();
                    return;
                }
            }
        }
        cout << "No bins contain that item. Please go to option 2.\n";
    }

    //Lets the user edit a bin
    void editBin() {
        cout << "Enter Bin ID to edit (e.g. BIN1): ";
        string id;
        getline(cin, id);

        int idx = -1;
        for (size_t i = 0; i < bins.size(); ++i) {
            if (bins[i].id == id) {
                idx = static_cast<int>(i);
                break;
            }
        }
        if (idx < 0) {
            cout << "Bin ID not found.\n";
            return;
        }

        Bin& bin = bins[idx];
        cout << "Editing Bin: " << bin.name << "\n";

        string opt;
        string itm;
        while (true) {
            cout << "1) Change Name  2) Add Item  3) Remove Item  4) Toggle Fragile  5) Delete Bin  x) Cancel\n"
                 << "Select an option: ";
            getline(cin, opt);
            if (opt == "x") return;
            if (opt >= "1" && opt <= "5") break;
            cout << "Mis-spelling, please repeat.\n";
        }

        if (opt == "1") {
            cout << "Enter new name: ";
            getline(cin, bin.name);
        }
        else if (opt == "2") {
            cout << "Enter item to add: ";
            getline(cin, itm);
            itm = trim(itm);
            if (!itm.empty()) bin.items.push_back(itm);
        }
        else if (opt == "3") {
            cout << "Enter item to remove: ";
            getline(cin, itm);
            itm = trim(itm);
            vector<string>::iterator itRem =
                remove(bin.items.begin(), bin.items.end(), itm);
            if (itRem == bin.items.end()) {
                cout << "Item not found, please retry.\n";
                return;
            }
            bin.items.erase(itRem, bin.items.end());
        }
        else if (opt == "4") {
            bin.isFragile = !bin.isFragile;
        }
        else {
            bins.erase(bins.begin() + idx);
            cout << "Bin deleted.\n";
            saveBins();
            return;
        }

        try {
            saveBins();
            cout << "Bin updated.\n";
        } catch (const exception& e) {
            cout << "Error saving changes: " << e.what() << "\n";
        }
    }

    //Shows all the lists of IDS and names only
    void listAllBins() {
        if (bins.empty()) {
            cout << "No bins available.\n";
            return;
        }
        for (size_t i = 0; i < bins.size(); ++i) {
            cout << bins[i].id << " - " << bins[i].name << "\n";
        }
    }
//Wipe out all the bins
//it also asks for confirmation just incase the user makes a mistake

    void deleteAllBins() {
        cout << "Delete all bins? (y/n): ";
        string c; getline(cin, c);
        if (toLower(trim(c)) == "y") {
            bins.clear();
            try {
                saveBins();
                cout << "All bins deleted.\n";
            } catch (const exception& e) {
                cout << "Error deleting bins: " << e.what() << "\n";
            }
        } else {
            cout << "Deletion canceled.\n";
        }
    }
//this code prints the stats how many bins and items are in the bins
    void showStats() {
        int totalBins = static_cast<int>(bins.size());
        int totalItems = 0;
        int fragileCount = 0;
        for (size_t i = 0; i < bins.size(); ++i) {
            totalItems += static_cast<int>(bins[i].items.size());
            if (bins[i].isFragile) ++fragileCount;
        }
        cout << "Bins: " << totalBins
             << "   Items: " << totalItems
             << "   Fragile: " << fragileCount << "\n";
    }
};


//MAIN PROGRAM

int main() {
    BinManager manager;
    try {
        manager.loadBins();
    } catch (const exception& e) {
        cout << "Error loading bins: " << e.what() << "\n";
    }

    while (true) {
        cout << "\n===== BinTrack =====\n"
             << "1. Create New Bin\n"
             << "2. Find Bin by Number or Name\n"
             << "3. Find Bins by Item\n"
             << "4. Edit Bin\n"
             << "5. List All Bins\n"
             << "6. Delete All Bins\n"
             << "7. Show Stats\n"
             << "x. Exit\n"
             << "Select an option: ";

        string choice;
        getline(cin, choice);

        if      (choice == "1") manager.createBin();
        else if (choice == "2") manager.findBin();
        else if (choice == "3") manager.findByItem();
        else if (choice == "4") manager.editBin();
        else if (choice == "5") manager.listAllBins();
        else if (choice == "6") manager.deleteAllBins();
        else if (choice == "7") manager.showStats();
        else if (toLower(trim(choice)) == "x") {
            cout << "Goodbye!\n";
            break;
        } else {
            cout << "Invalid option. Try again.\n";
        }
    }

    return 0;
}
