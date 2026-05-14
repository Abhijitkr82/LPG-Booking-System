// ====================================================================
//  LPG BOOKING MANAGEMENT SYSTEM (LBMS)
//  BCA Final Year Project
//  Language   : C++ (Object-Oriented Programming)
//  Compiler   : GCC / Dev-C++ / Code::Blocks
//  Platform   : Windows / Linux
//  Version    : 1.0
//  Developer  : [Student Name]
//  Roll No.   : [XXXX]
//  College    : [College Name]
//  Year       : 2024-25
// ====================================================================
//
//  MODULES:
//    1. Authentication & User Management
//    2. Customer (Subscriber) Management
//    3. Cylinder Booking System
//    4. Delivery Management
//    5. Distributor Management
//    6. Payment & Invoice Management
//    7. Stock / Inventory Management
//    8. Reports & Analytics
//
//  CLASSES:
//    User, Customer, Cylinder, Booking, Delivery,
//    Distributor, Payment, LPGSystem
//
// ====================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <limits>

using namespace std;

// ====================================================================
//  SECTION 1: GLOBAL CONSTANTS AND MACROS
// ====================================================================

// File names for persistent storage
const string FILE_USERS        = "lbms_users.dat";
const string FILE_CUSTOMERS    = "lbms_customers.dat";
const string FILE_BOOKINGS     = "lbms_bookings.dat";
const string FILE_DELIVERIES   = "lbms_deliveries.dat";
const string FILE_DISTRIBUTORS = "lbms_distributors.dat";
const string FILE_PAYMENTS     = "lbms_payments.dat";
const string FILE_STOCK        = "lbms_stock.dat";
const string FILE_LOG          = "lbms_activity.log";

// System constants
const int    MAX_LOGIN_ATTEMPTS    = 3;
const int    SUBSIDY_LIMIT         = 12;     // subsidised cylinders per year
const double SUBSIDISED_PRICE      = 903.00; // Rs per cylinder (subsidised)
const double MARKET_PRICE          = 1450.00;// Rs per cylinder (market rate)
const double COMMERCIAL_PRICE      = 1720.00;// Rs commercial cylinder
const double DELIVERY_CHARGE       = 50.00;  // Rs delivery charge
const double LATE_DELIVERY_PENALTY = 100.00; // Rs if delivery > 7 days
const int    BOOKING_WAIT_DAYS     = 21;     // min days between bookings
const int    LOW_STOCK_THRESHOLD   = 20;     // cylinders
const string SYSTEM_VERSION        = "1.0";
const string AGENCY_NAME           = "BHARATH GAS AGENCY";
const string AGENCY_ADDRESS        = "45, Gandhi Road, [City] - [PIN]";
const string AGENCY_PHONE          = "1800-XXX-YYYY";
const string AGENCY_LICENSE        = "DL-[XXXX]-BG-[YYYY]";

// ANSI colour codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

// ====================================================================
//  SECTION 2: UTILITY / HELPER FUNCTIONS
// ====================================================================

string getCurrentDateTime() {
time_t now = time(0);
char buf[80];
struct tm* t = localtime(&now);
strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", t);
return string(buf);
}

string getCurrentDate() {
time_t now = time(0);
char buf[20];
struct tm* t = localtime(&now);
strftime(buf, sizeof(buf), "%d/%m/%Y", t);
return string(buf);
}

string getCurrentMonthYear() {
time_t now = time(0);
char buf[20];
struct tm* t = localtime(&now);
strftime(buf, sizeof(buf), "%m/%Y", t);
return string(buf);
}

// Get current year as integer
int getCurrentYear() {
time_t now = time(0);
struct tm* t = localtime(&now);
return 1900 + t->tm_year;
}

void clearScreen() {
#ifdef _WIN32
system("cls");
#else
system("clear");
#endif
}

void printLine(char ch = '-', int len = 65) {
cout << string(len, ch) << endl;
}

void printHeader(const string& title) {
clearScreen();
cout << BOLD << BLUE;
printLine('=', 65);
int pad = (65 - (int)title.size()) / 2;
cout << string(pad > 0 ? pad : 0, ' ') << title << endl;
printLine('=', 65);
cout << RESET;
}

void printBanner() {
clearScreen();
cout << BOLD << GREEN;
printLine('*', 65);
cout << "*" << setw(63) << "*" << endl;
cout << "*" << string(8,' ') << AGENCY_NAME << string(8,' ') << "*" << endl;
cout << "*" << string(7,' ') << "LPG BOOKING MANAGEMENT SYSTEM" << string(7,' ') << "*" << endl;
cout << "*" << string(18,' ') << "Version " << SYSTEM_VERSION << string(18,' ') << "*" << endl;
cout << "*" << setw(63) << "*" << endl;
printLine('*', 65);
cout << RESET;
}

void pressEnter() {
cout << "\n" << CYAN << "  Press ENTER to continue..." << RESET;
cin.ignore(numeric_limits<streamsize>::max(), '\n');
cin.get();
}

int readInt(const string& prompt) {
int val;
while (true) {
cout << prompt;
if (cin >> val) { cin.ignore(); return val; }
cin.clear();
cin.ignore(numeric_limits<streamsize>::max(), '\n');
cout << RED << "  Invalid! Enter a number." << RESET << "\n";
}
}

double readDouble(const string& prompt) {
double val;
while (true) {
cout << prompt;
if (cin >> val && val >= 0) { cin.ignore(); return val; }
cin.clear();
cin.ignore(numeric_limits<streamsize>::max(), '\n');
cout << RED << "  Invalid! Enter a valid amount." << RESET << "\n";
}
}

string readString(const string& prompt) {
string val;
while (true) {
cout << prompt;
getline(cin, val);
if (!val.empty()) return val;
cout << RED << "  This field cannot be empty." << RESET << "\n";
}
}

bool isNumeric(const string& s) {
return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

bool isValidPhone(const string& p) {
return p.size() == 10 && isNumeric(p);
}

bool isValidAadhaar(const string& a) {
return a.size() == 12 && isNumeric(a);
}

string toUpper(string s) {
transform(s.begin(), s.end(), s.begin(), ::toupper);
return s;
}

void writeLog(const string& user, const string& action) {
ofstream log(FILE_LOG, ios::app);
if (log.is_open()) {
log << "[" << getCurrentDateTime() << "] "
<< "User: " << setw(12) << left << user
<< " | " << action << "\n";
log.close();
}
}

string toCurrency(double amount) {
ostringstream oss;
oss << "Rs." << fixed << setprecision(2) << amount;
return oss.str();
}

// Generate unique booking reference number
string generateBookingRef(int id) {
ostringstream oss;
oss << "BK" << getCurrentYear() << setw(5) << setfill('0') << id;
return oss.str();
}

// ====================================================================
// ====================================================================
// ====================================================================
// ====================================================================
//  SECTION 3: CLASS - User (Authentication & Security)
// ====================================================================
class User {
private:
    string username;
    string password;
    string role;        
    string fullName;
    string email;
    string phone;
    bool   isActive;
    string lastLogin;
    int    loginCount;
    string createdDate;

public:
    User() : isActive(true), loginCount(0) {}

    User(string u, string p, string r, string fn = "", string ph = "")
        : username(u), password(p), role(r), fullName(fn),
          phone(ph), isActive(true), loginCount(0) {
        createdDate = getCurrentDate(); 
    }

    // --- Getters ---
    string getUsername()   const { return username;    }
    string getPassword()   const { return password;    }
    string getRole()       const { return role;        }
    string getFullName()   const { return fullName;    }
    bool   getIsActive()   const { return isActive;    }

    // --- Setters (FIXED: Added missing functions) ---
    void setPassword(const string& p) { 
        password = p; 
    }

    void setIsActive(bool a) { 
        isActive = a; 
    }

    void incrementLogin() {
        loginCount++;
        lastLogin = getCurrentDate(); 
    }

    // --- Core Methods ---
    static User createDefaultAdmin() {
        return User("admin", "admin123", "admin", "System Administrator", "9999999999");
    }

    void displayRow() const {
        cout << left 
             << setw(15) << username 
             << setw(20) << (fullName.empty() ? "-" : fullName)
             << setw(12) << role 
             << setw(10) << (isActive ? GREEN "Active" RESET : RED "Inactive" RESET)
             << setw(6)  << loginCount << endl;
    }

    void save(ofstream& f) const {
        f << username << "|" << password << "|" << role << "|" 
          << fullName << "|" << email << "|" << phone << "|" 
          << (isActive ? "1" : "0") << "|" << lastLogin << "|" << loginCount << "|" 
          << createdDate << "\n";
    }

    bool load(ifstream& f) {
        string line;
        if (!getline(f, line) || line.empty()) return false;
        stringstream ss(line);
        string tok;

        getline(ss, username,     '|');
        getline(ss, password,     '|');
        getline(ss, role,         '|');
        getline(ss, fullName,     '|');
        getline(ss, email,        '|');
        getline(ss, phone,        '|');
        getline(ss, tok,          '|'); isActive = (tok == "1");
        getline(ss, lastLogin,    '|');
        getline(ss, tok,          '|'); loginCount = tok.empty() ? 0 : stoi(tok);
        getline(ss, createdDate,  '|');
        
        return !username.empty();
    }
};
// ====================================================================
//  SECTION 4: CLASS - Customer (LPG Subscriber)
// ====================================================================
class Customer {
private:
int    customerID;
string consumerNumber;   // unique LPG consumer number
string name;
string phone;
string alternatePhone;
string email;
string address;
string city;
string pincode;
string aadhaarNumber;
string connectionType;   // "Domestic", "Commercial"
string cylinderType;     // "14.2 kg", "5 kg", "19 kg"
int    subsidisedCount;  // cylinders taken this year (subsidised)
int    totalCylinders;   // total cylinders taken lifetime
string regDate;
bool   isActive;
bool   hasEmergencyContact;
string emergencyContact;
double totalAmountPaid;
int    distributorID;

public:
Customer()
: customerID(0), subsidisedCount(0), totalCylinders(0),
isActive(true), hasEmergencyContact(false),
totalAmountPaid(0), distributorID(1) {}

// Setters
void setCustomerID(int id)             { customerID       = id;   }
void setConsumerNumber(const string& c){ consumerNumber   = c;   }
void setName(const string& n)          { name             = n;   }
void setPhone(const string& p)         { phone            = p;   }
void setAlternatePhone(const string& p){ alternatePhone   = p;   }
void setEmail(const string& e)         { email            = e;   }
void setAddress(const string& a)       { address          = a;   }
void setCity(const string& c)          { city             = c;   }
void setPincode(const string& p)       { pincode          = p;   }
void setAadhaar(const string& a)       { aadhaarNumber    = a;   }
void setConnectionType(const string& t){ connectionType   = t;   }
void setCylinderType(const string& t)  { cylinderType     = t;   }
void setRegDate(const string& d)       { regDate          = d;   }
void setIsActive(bool a)               { isActive         = a;   }
void setEmergencyContact(const string& e){ emergencyContact=e; hasEmergencyContact=true; }
void setDistributorID(int id)          { distributorID    = id;  }

// Getters
int    getCustomerID()      const { return customerID;      }
string getConsumerNumber()  const { return consumerNumber;  }
string getName()            const { return name;            }
string getPhone()           const { return phone;           }
string getAddress()         const { return address;         }
string getCity()            const { return city;            }
string getAadhaar()         const { return aadhaarNumber;   }
string getConnectionType()  const { return connectionType;  }
string getCylinderType()    const { return cylinderType;    }
int    getSubsidisedCount() const { return subsidisedCount; }
int    getTotalCylinders()  const { return totalCylinders;  }
double getTotalAmountPaid() const { return totalAmountPaid; }
bool   getIsActive()        const { return isActive;        }
int    getDistributorID()   const { return distributorID;   }
string getRegDate()         const { return regDate;         }

// Check if customer can get subsidised cylinder
bool canGetSubsidy() const {
return connectionType == "Domestic" && subsidisedCount < SUBSIDY_LIMIT;
}

// Get applicable price for next cylinder
double getApplicablePrice() const {
if (connectionType == "Commercial") return COMMERCIAL_PRICE;
if (canGetSubsidy()) return SUBSIDISED_PRICE;
return MARKET_PRICE;
}

// Record a cylinder delivery
void recordDelivery(double amount) {
totalCylinders++;
totalAmountPaid += amount;
if (connectionType == "Domestic" && subsidisedCount < SUBSIDY_LIMIT)
subsidisedCount++;
}

// Reset subsidised count for new year
void resetSubsidyCount() { subsidisedCount = 0; }

void displayRow() const {
cout << left
<< setw(8)  << customerID
<< setw(14) << consumerNumber
<< setw(20) << name
<< setw(13) << phone
<< setw(12) << connectionType
<< setw(8)  << subsidisedCount << "/" << SUBSIDY_LIMIT
<< (isActive ? GREEN "Active" RESET : RED "Inactive" RESET)
<< endl;
}

void displayDetail() const {
printLine('=', 65);
cout << BOLD << "  CUSTOMER PROFILE — LPG SUBSCRIBER" << RESET << endl;
printLine();
cout << "Customer ID      : " << customerID                         << endl;
cout << "Consumer No.     : " << consumerNumber                     << endl;
cout << "Name             : " << name                               << endl;
cout << "Phone            : " << phone                              << endl;
cout << "Alt. Phone       : " << (alternatePhone.empty()?"N/A":alternatePhone) << endl;
cout << "Email            : " << (email.empty()?"N/A":email)        << endl;
cout << "Address          : " << address                            << endl;
cout << "City / Pincode   : " << city << " - " << pincode           << endl;
cout << "Aadhaar No.      : " << aadhaarNumber                      << endl;
cout << "Connection Type  : " << connectionType                     << endl;
cout << "Cylinder Type    : " << cylinderType                       << endl;
cout << "Subsidised Used  : " << subsidisedCount<<"/"<<SUBSIDY_LIMIT<< " this year" << endl;
cout << "Total Cylinders  : " << totalCylinders                     << endl;
cout << "Total Amount Paid: " << toCurrency(totalAmountPaid)        << endl;
cout << "Registered On    : " << regDate                            << endl;
cout << "Next Cylinder    : "
<< (canGetSubsidy() ? GREEN "Subsidised @ " RESET + toCurrency(SUBSIDISED_PRICE)
: YELLOW "Market Rate @ " RESET + toCurrency(MARKET_PRICE))   << endl;
cout << "Status           : "
     << (isActive ? GREEN "Active" RESET
                  : RED "Inactive" RESET)
     << endl;
printLine('=', 65);
}

void save(ofstream& f) const {
f << customerID << "|" << consumerNumber << "|" << name << "|"
<< phone << "|" << alternatePhone << "|" << email << "|"
<< address << "|" << city << "|" << pincode << "|"
<< aadhaarNumber << "|" << connectionType << "|" << cylinderType << "|"
<< subsidisedCount << "|" << totalCylinders << "|" << regDate << "|"
<< isActive << "|" << emergencyContact << "|" << totalAmountPaid << "|"
<< distributorID << "\n";
}

bool load(ifstream& f) {
string line;
if (!getline(f, line) || line.empty()) return false;
stringstream ss(line);
string tok;
getline(ss, tok,             '|'); customerID       = stoi(tok);
getline(ss, consumerNumber,  '|');
getline(ss, name,            '|');
getline(ss, phone,           '|');
getline(ss, alternatePhone,  '|');
getline(ss, email,           '|');
getline(ss, address,         '|');
getline(ss, city,            '|');
getline(ss, pincode,         '|');
getline(ss, aadhaarNumber,   '|');
getline(ss, connectionType,  '|');
getline(ss, cylinderType,    '|');
getline(ss, tok,             '|'); subsidisedCount  = tok.empty()?0:stoi(tok);
getline(ss, tok,             '|'); totalCylinders   = tok.empty()?0:stoi(tok);
getline(ss, regDate,         '|');
getline(ss, tok,             '|'); isActive         = (tok=="1");
getline(ss, emergencyContact,'|');
getline(ss, tok,             '|'); totalAmountPaid  = tok.empty()?0:stod(tok);
getline(ss, tok,             '|'); distributorID    = tok.empty()?1:stoi(tok);
return customerID > 0;
}
};

// ====================================================================
//  SECTION 5: CLASS - Booking
// ====================================================================
class Booking {
private:
int    bookingID;
string bookingRef;
int    customerID;
string customerName;
string customerPhone;
string consumerNumber;
string bookingDate;
string expectedDeliveryDate;
string actualDeliveryDate;
string cylinderType;    // "14.2 kg", "5 kg", "19 kg"
int    quantity;
string connectionType;  // "Domestic", "Commercial"
string bookingStatus;   // "Pending", "Confirmed", "Out for Delivery", "Delivered", "Cancelled"
double cylinderPrice;
double deliveryCharge;
double totalAmount;
bool   isPaid;
string paymentMode;     // "Cash", "Online", "UPI", "Card"
string bookedBy;        // operator username
int    deliveryBoyID;
string remarks;
bool   isSubsidised;

public:
Booking()
: bookingID(0), customerID(0), quantity(1),
deliveryCharge(DELIVERY_CHARGE), totalAmount(0),
isPaid(false), deliveryBoyID(0), isSubsidised(false) {}

// Setters
void setBookingID(int id)               { bookingID          = id;  }
void setBookingRef(const string& r)     { bookingRef         = r;   }
void setCustomerID(int id)              { customerID         = id;  }
void setCustomerName(const string& n)   { customerName       = n;   }
void setCustomerPhone(const string& p)  { customerPhone      = p;   }
void setConsumerNumber(const string& c) { consumerNumber     = c;   }
void setBookingDate(const string& d)    { bookingDate        = d;   }
void setExpectedDelivery(const string& d){ expectedDeliveryDate=d; }
void setActualDelivery(const string& d) { actualDeliveryDate = d;   }
void setCylinderType(const string& t)   { cylinderType       = t;   }
void setQuantity(int q)                 { quantity           = q;   }
void setConnectionType(const string& t) { connectionType     = t;   }
void setBookingStatus(const string& s)  { bookingStatus      = s;   }
void setCylinderPrice(double p)         { cylinderPrice      = p;   }
void setDeliveryCharge(double d)        { deliveryCharge     = d;   }
void setTotalAmount(double t)           { totalAmount        = t;   }
void setIsPaid(bool p)                  { isPaid             = p;   }
void setPaymentMode(const string& m)    { paymentMode        = m;   }
void setBookedBy(const string& u)       { bookedBy           = u;   }
void setDeliveryBoyID(int id)           { deliveryBoyID      = id;  }
void setRemarks(const string& r)        { remarks            = r;   }
void setIsSubsidised(bool s)            { isSubsidised       = s;   }

// Calculate total amount
void calculateTotal() {
totalAmount = (cylinderPrice * quantity) + deliveryCharge;
}

// Getters
int    getBookingID()       const { return bookingID;          }
string getBookingRef()      const { return bookingRef;         }
int    getCustomerID()      const { return customerID;         }
string getCustomerName()    const { return customerName;       }
string getCustomerPhone()   const { return customerPhone;      }
string getConsumerNumber()  const { return consumerNumber;     }
string getBookingDate()     const { return bookingDate;        }
string getExpectedDelivery()const { return expectedDeliveryDate;}
string getActualDelivery()  const { return actualDeliveryDate; }
string getCylinderType()    const { return cylinderType;       }
int    getQuantity()        const { return quantity;           }
string getConnectionType()  const { return connectionType;     }
string getBookingStatus()   const { return bookingStatus;      }
double getCylinderPrice()   const { return cylinderPrice;      }
double getDeliveryCharge()  const { return deliveryCharge;     }
double getTotalAmount()     const { return totalAmount;        }
bool   getIsPaid()          const { return isPaid;             }
string getPaymentMode()     const { return paymentMode;        }
string getBookedBy()        const { return bookedBy;           }
bool   getIsSubsidised()    const { return isSubsidised;       }

// Print formatted booking confirmation
void printConfirmation() const {
printLine('=', 65);
cout << BOLD << GREEN;
cout << "              " << AGENCY_NAME << "\n";
cout << "         " << AGENCY_ADDRESS << "\n";
cout << "              Ph: " << AGENCY_PHONE << "\n";
cout << "        ** LPG CYLINDER BOOKING CONFIRMATION **" << RESET << BOLD << GREEN << "\n";
printLine('=', 65);
cout << RESET;
cout << "Booking Ref.  : " << bookingRef << "\n";
cout << "Booking Date  : " << bookingDate << "\n";
cout << "Booked By     : " << bookedBy << "\n";
printLine('-', 65);
cout << "Consumer No.  : " << consumerNumber << "\n";
cout << "Customer Name : " << customerName << "\n";
cout << "Phone         : " << customerPhone << "\n";
printLine('-', 65);
cout << "Cylinder Type : " << cylinderType << "\n";
cout << "Connection    : " << connectionType << "\n";
cout << "Quantity      : " << quantity << " cylinder(s)\n";
cout << "Type          : "
     << (isSubsidised ? YELLOW "Subsidised" RESET : "Market Rate")
     << "\n";
printLine('-', 65);
cout << right << setw(40) << "Cylinder Price : " << toCurrency(cylinderPrice) << " x " << quantity << "\n";
cout << right << setw(40) << "Delivery Charge: " << toCurrency(deliveryCharge) << "\n";
printLine('-', 65);
cout << BOLD << right << setw(40) << "TOTAL AMOUNT   : " << toCurrency(totalAmount) << RESET << "\n";
printLine('-', 65);
cout << "Expected Delivery: " << expectedDeliveryDate << "\n";
cout << "Status           : " << CYAN << bookingStatus << RESET << "\n";
printLine('=', 65);
cout << GREEN << "  Thank you for booking with " << AGENCY_NAME << "!\n";
cout << "  Please keep this slip for future reference." << RESET << "\n";
printLine('=', 65);
}

void displayRow() const {
string statusColor = "";
if (bookingStatus=="Delivered")        statusColor = GREEN;
else if (bookingStatus=="Pending")     statusColor = YELLOW;
else if (bookingStatus=="Cancelled")   statusColor = RED;
else                                   statusColor = CYAN;
cout << left
<< setw(6)  << bookingID
<< setw(14) << bookingRef
<< setw(11) << bookingDate
<< setw(20) << customerName
<< setw(10) << cylinderType
<< setw(12) << toCurrency(totalAmount)
<< statusColor << setw(18) << bookingStatus << RESET
<< endl;
}

void save(ofstream& f) const {
f << bookingID << "|" << bookingRef << "|" << customerID << "|"
<< customerName << "|" << customerPhone << "|" << consumerNumber << "|"
<< bookingDate << "|" << expectedDeliveryDate << "|" << actualDeliveryDate << "|"
<< cylinderType << "|" << quantity << "|" << connectionType << "|"
<< bookingStatus << "|" << cylinderPrice << "|" << deliveryCharge << "|"
<< totalAmount << "|" << isPaid << "|" << paymentMode << "|"
<< bookedBy << "|" << deliveryBoyID << "|" << remarks << "|"
<< isSubsidised << "\n";
}

bool load(ifstream& f) {
string line;
if (!getline(f, line) || line.empty()) return false;
stringstream ss(line);
string tok;
getline(ss, tok,              '|'); bookingID          = stoi(tok);
getline(ss, bookingRef,       '|');
getline(ss, tok,              '|'); customerID         = stoi(tok);
getline(ss, customerName,     '|');
getline(ss, customerPhone,    '|');
getline(ss, consumerNumber,   '|');
getline(ss, bookingDate,      '|');
getline(ss, expectedDeliveryDate,'|');
getline(ss, actualDeliveryDate,'|');
getline(ss, cylinderType,     '|');
getline(ss, tok,              '|'); quantity           = tok.empty()?1:stoi(tok);
getline(ss, connectionType,   '|');
getline(ss, bookingStatus,    '|');
getline(ss, tok,              '|'); cylinderPrice      = stod(tok);
getline(ss, tok,              '|'); deliveryCharge     = stod(tok);
getline(ss, tok,              '|'); totalAmount        = stod(tok);
getline(ss, tok,              '|'); isPaid             = (tok=="1");
getline(ss, paymentMode,      '|');
getline(ss, bookedBy,         '|');
getline(ss, tok,              '|'); deliveryBoyID      = tok.empty()?0:stoi(tok);
getline(ss, remarks,          '|');
getline(ss, tok,              '|'); isSubsidised       = (tok=="1");
return bookingID > 0;
}
};

// ====================================================================
//  SECTION 6: CLASS - Distributor
// ====================================================================
class Distributor {
private:
int    distributorID;
string name;
string contactPerson;
string phone;
string email;
string address;
string city;
string licenseNumber;
string oilCompany;     // "BPCL", "IOCL", "HPCL"
int    cylindersAllocated;
int    cylindersSupplied;
bool   isActive;
string regDate;
double outstandingAmount;

public:
Distributor()
: distributorID(0), cylindersAllocated(0),
cylindersSupplied(0), isActive(true), outstandingAmount(0) {}

// Setters
void setDistributorID(int id)          { distributorID      = id;  }
void setName(const string& n)          { name               = n;   }
void setContactPerson(const string& c) { contactPerson      = c;   }
void setPhone(const string& p)         { phone              = p;   }
void setEmail(const string& e)         { email              = e;   }
void setAddress(const string& a)       { address            = a;   }
void setCity(const string& c)          { city               = c;   }
void setLicenseNumber(const string& l) { licenseNumber      = l;   }
void setOilCompany(const string& o)    { oilCompany         = o;   }
void setAllocated(int a)               { cylindersAllocated = a;   }
void setIsActive(bool b)               { isActive           = b;   }
void setRegDate(const string& d)       { regDate            = d;   }
void addSupply(int qty)                { cylindersSupplied  += qty;}

// Getters
int    getDistributorID()   const { return distributorID;    }
string getName()            const { return name;             }
string getPhone()           const { return phone;            }
string getOilCompany()      const { return oilCompany;       }
int    getAllocated()        const { return cylindersAllocated;}
int    getSupplied()        const { return cylindersSupplied; }
bool   getIsActive()        const { return isActive;         }
double getOutstanding()     const { return outstandingAmount; }

void displayRow() const {
cout << left
<< setw(6)  << distributorID
<< setw(25) << name
<< setw(13) << phone
<< setw(8)  << oilCompany
<< setw(12) << cylindersAllocated
<< (isActive ? GREEN " Active" RESET
              : RED " Inactive" RESET)
<< endl;
}

void displayDetail() const {
printLine('=', 65);
cout << BOLD << "  DISTRIBUTOR PROFILE" << RESET << endl;
printLine();
cout << "Distributor ID    : " << distributorID      << endl;
cout << "Agency Name       : " << name               << endl;
cout << "Contact Person    : " << contactPerson      << endl;
cout << "Phone             : " << phone              << endl;
cout << "Email             : " << email              << endl;
cout << "Address           : " << address            << endl;
cout << "City              : " << city               << endl;
cout << "License Number    : " << licenseNumber      << endl;
cout << "Oil Company       : " << oilCompany         << endl;
cout << "Cylinders Alloc.  : " << cylindersAllocated << endl;
cout << "Cylinders Supplied: " << cylindersSupplied  << endl;
cout << "Registered On     : " << regDate            << endl;
cout << "Status            : "
     << (isActive ? GREEN "Active" RESET
                  : RED "Inactive" RESET)
     << endl;
printLine('=', 65);
}

void save(ofstream& f) const {
f << distributorID << "|" << name << "|" << contactPerson << "|"
<< phone << "|" << email << "|" << address << "|" << city << "|"
<< licenseNumber << "|" << oilCompany << "|" << cylindersAllocated << "|"
<< cylindersSupplied << "|" << isActive << "|" << regDate << "|"
<< outstandingAmount << "\n";
}

bool load(ifstream& f) {
string line;
if (!getline(f, line) || line.empty()) return false;
stringstream ss(line);
string tok;
getline(ss, tok,            '|'); distributorID      = stoi(tok);
getline(ss, name,           '|');
getline(ss, contactPerson,  '|');
getline(ss, phone,          '|');
getline(ss, email,          '|');
getline(ss, address,        '|');
getline(ss, city,           '|');
getline(ss, licenseNumber,  '|');
getline(ss, oilCompany,     '|');
getline(ss, tok,            '|'); cylindersAllocated = tok.empty()?0:stoi(tok);
getline(ss, tok,            '|'); cylindersSupplied  = tok.empty()?0:stoi(tok);
getline(ss, tok,            '|'); isActive           = (tok=="1");
getline(ss, regDate,        '|');
getline(ss, tok,            '|'); outstandingAmount  = tok.empty()?0:stod(tok);
return distributorID > 0;
}
};

// ====================================================================
// ====================================================================
//  SECTION 7: CLASS - CylinderStock (Inventory)
// ====================================================================

// Note: Agar LOW_STOCK_THRESHOLD define nahi hai, toh ise use karein
#ifndef LOW_STOCK_THRESHOLD
#define LOW_STOCK_THRESHOLD 20
#endif

class CylinderStock {
private:
    int    stockID;
    string cylinderType;   // "14.2 kg", "5 kg", "19 kg"
    int    totalStock;
    int    availableStock;
    int    deliveredToday;
    int    receivedToday;
    double priceSubsidised;
    double priceMarket;
    double priceCommercial;
    string lastUpdated;
    int    minStockLevel;
    int    totalReceived;
    int    totalDelivered;

public:
    CylinderStock()
        : stockID(0), totalStock(0), availableStock(0),
          deliveredToday(0), receivedToday(0),
          minStockLevel(LOW_STOCK_THRESHOLD),
          totalReceived(0), totalDelivered(0) {}

    CylinderStock(int id, string type, int stock,
                  double sub, double mkt, double com)
        : stockID(id), cylinderType(type),
          totalStock(stock), availableStock(stock),
          deliveredToday(0), receivedToday(0),
          priceSubsidised(sub), priceMarket(mkt),
          priceCommercial(com),
          minStockLevel(LOW_STOCK_THRESHOLD),
          totalReceived(stock), totalDelivered(0),
          lastUpdated(getCurrentDate()) {}

    // Getters
    int    getStockID()         const { return stockID;         }
    string getCylinderType()    const { return cylinderType;    }
    int    getAvailableStock()  const { return availableStock;  }
    double getSubsidisedPrice() const { return priceSubsidised; }
    double getMarketPrice()     const { return priceMarket;     }
    double getCommercialPrice() const { return priceCommercial; }
    bool   isLowStock()         const { return availableStock < minStockLevel; }

    // Setters
    void setSubsidisedPrice(double p) { priceSubsidised = p; }
    void setMarketPrice(double p)      { priceMarket     = p; }
    void setCommercialPrice(double p) { priceCommercial = p; }

    void receiveStock(int qty) {
        availableStock += qty;
        totalStock     += qty;
        totalReceived  += qty;
        receivedToday  += qty;
        lastUpdated     = getCurrentDate();
    }

    bool deductStock(int qty) {
        if (availableStock < qty) return false;
        availableStock -= qty;
        deliveredToday += qty;
        totalDelivered += qty;
        lastUpdated     = getCurrentDate();
        return true;
    }

    void displayRow() const {
        cout << left
             << setw(5)  << stockID
             << setw(12) << cylinderType
             << setw(10) << availableStock
             << setw(12) << toCurrency(priceSubsidised)
             << setw(12) << toCurrency(priceMarket)
             << setw(14) << toCurrency(priceCommercial);
        // FIXED: Added space in literal to prevent C++11 warning
        if (isLowStock()) cout << RED " *** LOW STOCK ***" RESET;
        cout << endl;
    }

    void save(ofstream& f) const {
        f << stockID << "|" << cylinderType << "|" << totalStock << "|"
          << availableStock << "|" << deliveredToday << "|" << receivedToday << "|"
          << priceSubsidised << "|" << priceMarket << "|" << priceCommercial << "|"
          << lastUpdated << "|" << minStockLevel << "|"
          << totalReceived << "|" << totalDelivered << "\n";
    }

    bool load(ifstream& f) {
        string line;
        if (!getline(f, line) || line.empty()) return false;
        stringstream ss(line);
        string tok;
        getline(ss, tok, '|'); stockID = stoi(tok);
        getline(ss, cylinderType, '|');
        getline(ss, tok, '|'); totalStock = stoi(tok);
        getline(ss, tok, '|'); availableStock = stoi(tok);
        getline(ss, tok, '|'); deliveredToday = tok.empty()?0:stoi(tok);
        getline(ss, tok, '|'); receivedToday = tok.empty()?0:stoi(tok);
        getline(ss, tok, '|'); priceSubsidised = stod(tok);
        getline(ss, tok, '|'); priceMarket = stod(tok);
        getline(ss, tok, '|'); priceCommercial = stod(tok);
        getline(ss, lastUpdated, '|');
        getline(ss, tok, '|'); minStockLevel = tok.empty()?20:stoi(tok);
        getline(ss, tok, '|'); totalReceived = tok.empty()?0:stoi(tok);
        getline(ss, tok, '|'); totalDelivered = tok.empty()?0:stoi(tok);
        return stockID > 0;
    }
};

// ====================================================================
// ====================================================================
//  SECTION 8: CLASS - Payment (Updated with missing Getters)
// ====================================================================
class Payment {
private:
    int    paymentID;
    int    bookingID;
    string bookingRef;
    int    customerID;
    string customerName;
    double amount;
    string paymentMode;
    string paymentDate;
    string transactionID;
    bool   isVerified;
    string verifiedBy;
    string remarks;

public:
    Payment() : paymentID(0), bookingID(0), customerID(0), amount(0), isVerified(false) {}

    // --- Setters ---
    void setPaymentID(int id)    { paymentID = id; }
    void setBookingID(int id)    { bookingID = id; }
    void setBookingRef(const string& r) { bookingRef = r; }
    void setCustomerID(int id)   { customerID = id; }
    void setCustomerName(const string& n){ customerName = n; }
    void setAmount(double a)     { amount = a; }
    void setPaymentMode(const string& m){ paymentMode = m; }
    void setPaymentDate(const string& d){ paymentDate = d; }
    void setTransactionID(const string& t){ transactionID = t; }
    void setIsVerified(bool v)   { isVerified = v; }
    void setVerifiedBy(const string& v) { verifiedBy = v; }
    void setRemarks(const string& r)    { remarks = r; }

    // --- Getters (FIXED: Added missing member functions) ---
    int    getPaymentID()     const { return paymentID;    }
    int    getBookingID()     const { return bookingID;    }
    string getBookingRef()    const { return bookingRef;   }
    int    getCustomerID()    const { return customerID;   }
    string getCustomerName()  const { return customerName; }
    double getAmount()        const { return amount;       }
    string getPaymentMode()   const { return paymentMode;  }
    string getPaymentDate()   const { return paymentDate;  }
    bool   getIsVerified()    const { return isVerified;   }

    // --- Display Methods ---
    void displayRow() const {
        cout << left
             << setw(7)  << paymentID
             << setw(11) << paymentDate
             << setw(14) << bookingRef
             << setw(20) << customerName
             << setw(12) << toCurrency(amount)
             << setw(10) << paymentMode
             << (isVerified ? GREEN " Verified" RESET : YELLOW " Pending" RESET)
             << endl;
    }

    // --- File Handling ---
    void save(ofstream& f) const {
        f << paymentID << "|" << bookingID << "|" << bookingRef << "|"
          << customerID << "|" << customerName << "|" << amount << "|"
          << paymentMode << "|" << paymentDate << "|" << transactionID << "|"
          << isVerified << "|" << verifiedBy << "|" << remarks << "\n";
    }

    bool load(ifstream& f) {
        string line;
        if (!getline(f, line) || line.empty()) return false;
        stringstream ss(line);
        string tok;
        try {
            getline(ss, tok, '|'); paymentID = stoi(tok);
            getline(ss, tok, '|'); bookingID = stoi(tok);
            getline(ss, bookingRef, '|');
            getline(ss, tok, '|'); customerID = stoi(tok);
            getline(ss, customerName, '|');
            getline(ss, tok, '|'); amount = stod(tok);
            getline(ss, paymentMode, '|');
            getline(ss, paymentDate, '|');
            getline(ss, transactionID, '|');
            getline(ss, tok, '|'); isVerified = (tok == "1");
            getline(ss, verifiedBy, '|');
            getline(ss, remarks, '|');
        } catch (...) { return false; }
        return paymentID > 0;
    }
};
// ====================================================================
//  SECTION 9: CLASS - LPGSystem (Main Controller)
// ====================================================================
class LPGSystem {
private:
    vector<User>          users;
    vector<Customer>      customers;
    vector<Booking>       bookings;
    vector<Distributor>   distributors;
    vector<CylinderStock> stocks;
    vector<Payment>       payments;

    string currentUser;
    string currentRole;
    int    nextBookingID;
    int    nextCustomerID;
    int    nextPaymentID;

    // --- File I/O ---
    void loadUsers() {
        users.clear();
        ifstream f(FILE_USERS);
        if (!f.is_open()) {
            // Default users agar file na ho
            users.push_back(User("admin",    "admin123", "admin",    "System Admin",    "9900000001"));
            users.push_back(User("operator", "operator123", "operator", "Booking Operator","9900000002"));
            saveUsers(); 
            return;
        }
        User u;
        while (u.load(f)) { users.push_back(u); u = User(); }
        f.close();
        
        // Safety check: Agar file khali ho toh admin add karein
        if(users.empty()) {
            users.push_back(User("admin", "admin123", "admin", "System Admin", "9900000001"));
            saveUsers();
        }
    }

    void saveUsers() { 
        ofstream f(FILE_USERS); 
        for (auto& u : users) u.save(f); 
        f.close();
    }

    void loadCustomers() {
        customers.clear(); nextCustomerID = 1001;
        ifstream f(FILE_CUSTOMERS);
        if (!f.is_open()) return;
        Customer c;
        while (c.load(f)) {
            customers.push_back(c);
            if (c.getCustomerID() >= nextCustomerID) nextCustomerID = c.getCustomerID() + 1;
            c = Customer();
        }
    }
    void saveCustomers() { ofstream f(FILE_CUSTOMERS); for(auto&c:customers)c.save(f); }

    void loadBookings() {
        bookings.clear(); nextBookingID = 5001;
        ifstream f(FILE_BOOKINGS);
        if (!f.is_open()) return;
        Booking b;
        while (b.load(f)) {
            bookings.push_back(b);
            if (b.getBookingID() >= nextBookingID) nextBookingID = b.getBookingID() + 1;
            b = Booking();
        }
    }
    void saveBookings() { ofstream f(FILE_BOOKINGS); for(auto&b:bookings)b.save(f); }

    void loadStocks() {
        stocks.clear();
        ifstream f(FILE_STOCK);
        if (!f.is_open()) {
            stocks.push_back(CylinderStock(1,"14.2 kg",150, 850.0, 1100.0, 0));
            stocks.push_back(CylinderStock(2,"5 kg", 80, 480.0, 700.0, 0));
            stocks.push_back(CylinderStock(3,"19 kg", 50, 0, 0, 1800.0));
            saveStocks(); return;
        }
        CylinderStock s;
        while (s.load(f)) { stocks.push_back(s); s = CylinderStock(); }
    }
    void saveStocks() { ofstream f(FILE_STOCK); for(auto&s:stocks)s.save(f); }

    void loadPayments() {
        payments.clear(); nextPaymentID = 9001;
        ifstream f(FILE_PAYMENTS);
        if (!f.is_open()) return;
        Payment p;
        while (p.load(f)) {
            payments.push_back(p);
            if (p.getPaymentID() >= nextPaymentID) nextPaymentID = p.getPaymentID() + 1;
            p = Payment();
        }
    }
    void savePayments() { ofstream f(FILE_PAYMENTS); for(auto&p:payments)p.save(f); }

    void loadDistributors() {
        distributors.clear();
        ifstream f(FILE_DISTRIBUTORS);
        if (!f.is_open()) {
            Distributor d;
            d.setDistributorID(1); d.setName("Bharath Gas Agency - Main");
            d.setPhone("9988776655"); d.setOilCompany("BPCL");
            d.setAllocated(500); d.setRegDate(getCurrentDate());
            distributors.push_back(d);
            saveDistributors(); return;
        }
        Distributor d;
        while (d.load(f)) { distributors.push_back(d); d = Distributor(); }
    }
    void saveDistributors() { ofstream f(FILE_DISTRIBUTORS); for(auto&d:distributors)d.save(f); }

    void loadAllData() {
        loadUsers(); loadCustomers(); loadBookings();
        loadDistributors(); loadStocks(); loadPayments();
    }

    // --- Lookup Helpers ---
    Customer* findCustomerByID(int id) {
        for (auto& c:customers) if(c.getCustomerID()==id) return &c;
        return nullptr;
    }
    Customer* findCustomerByPhone(const string& ph) {
        for (auto& c:customers) if(c.getPhone()==ph) return &c;
        return nullptr;
    }
    Customer* findCustomerByConsumerNo(const string& cn) {
        for (auto& c:customers) if(toUpper(c.getConsumerNumber())==toUpper(cn)) return &c;
        return nullptr;
    }
    Booking* findBookingByRef(const string& ref) {
        for (auto& b:bookings) if(toUpper(b.getBookingRef())==toUpper(ref)) return &b;
        return nullptr;
    }
    CylinderStock* findStockByType(const string& type) {
        for (auto& s:stocks) if(s.getCylinderType()==type) return &s;
        return nullptr;
    }

    bool hasPendingBooking(int custID) {
        for (auto& b:bookings)
            if (b.getCustomerID()==custID && (b.getBookingStatus()=="Pending"||b.getBookingStatus()=="Confirmed"))
                return true;
        return false;
    }

    // --- MODULES ---
    bool login() {
        int attempts = 0;
        while (attempts < 3) {
            clearScreen();
            printBanner();
            cout << BOLD << "\n  LOGIN TO LPG MANAGEMENT SYSTEM" << RESET << "\n";
            printLine();
            string user, pass;
            cout << "  Username : "; cin >> user;
            cout << "  Password : "; cin >> pass;
            cin.ignore();

            // Check hardcoded admin for first-time use
            if(user == "admin" && pass == "admin123") {
                currentUser = "admin";
                currentRole = "admin";
                return true;
            }

            for (auto& u : users) {
                if (u.getUsername() == user && u.getPassword() == pass && u.getIsActive()) {
                    currentUser = user;
                    currentRole = u.getRole();
                    u.incrementLogin();
                    saveUsers();
                    cout << GREEN << "\n  Welcome, " << u.getFullName() << "!" << RESET << "\n";
                    pressEnter();
                    return true;
                }
            }
            attempts++;
            cout << RED << "\n  Invalid credentials! Attempts left: " << (3 - attempts) << RESET << "\n";
            if (attempts < 3) pressEnter();
        }
        return false;
    }

    // (Add all other menu functions like addCustomer, newBooking here as per your previous code)
    // ... [Add customerMenu, bookingMenu, stockMenu, reportsMenu here] ...

public:
    LPGSystem() {
        loadAllData();
    }

    void run() {
        if (!login()) {
            cout << RED << "\n  System access denied. Program exiting." << RESET << "\n";
            return;
        }

        int ch;
        do {
            printHeader("LPG BOOKING MANAGEMENT SYSTEM - MAIN MENU");
            cout << "\n  Logged in: " << CYAN << currentUser << " [" << toUpper(currentRole) << "]" << RESET << "\n";
            cout << "  1. Subscriber Management\n"
                 << "  2. Cylinder Booking\n"
                 << "  3. Cylinder Stock Management\n"
                 << "  4. Payment Management\n"
                 << "  5. Reports & Analytics\n"
                 << "  6. User Management (Admin Only)\n"
                 << "  0. Logout & Exit\n\n  Choice: ";
            ch = readInt("");

            switch(ch) {
                case 1: /* customerMenu(); */ break; 
                case 2: /* bookingMenu(); */ break;
                case 3: /* stockMenu(); */ break;
                case 4: /* paymentMenu(); */ break;
                case 5: /* reportsMenu(); */ break;
                case 6: userManagement(); break;
                case 0: cout << "\n  Logging out...\n"; break;
                default: cout << RED << "  Invalid Choice!" << RESET << "\n"; pressEnter();
            }
        } while(ch != 0);
    }

    void userManagement() {
        if(currentRole != "admin") {
            cout << RED << "  Access Denied: Admin only function." << RESET << "\n";
            pressEnter();
            return;
        }
        printHeader("USER MANAGEMENT");
        for(auto& u : users) u.displayRow();
        pressEnter();
    }
};

// ====================================================================
//  MAIN FUNCTION
// ====================================================================
int main() {
    LPGSystem lbms;
    lbms.run();
    return 0;
}
