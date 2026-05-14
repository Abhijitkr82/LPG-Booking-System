# LPG Booking Management System (LBMS v1.0)

A comprehensive console-based **LPG Booking Management System** developed in **C++ (OOP)** as a BCA Final Year Project.

---

## 📋 Project Info

| Field | Details |
|---|---|
| **Developer** | Abhijit Kumar Singh |
| **Roll No.** | 2420440080012 |
| **College** | B.S. College, Danapur |
| **University** | Patliputra University, Patna |
| **Batch** | BCA 3-Year (2023–2026) |
| **Guide** | Trivendram Kumar (M.Tech) |
| **Language** | C++ (ISO C++11) |
| **Lines of Code** | 2,100+ |
| **Classes** | 8 |
| **Functions** | 75+ |

---

## 🚀 Features

- ✅ **3-Role Authentication** — Admin, Operator, Delivery (with activity logging)
- ✅ **Subscriber Management** — Aadhaar verification, auto consumer number generation
- ✅ **Smart Booking System** — Duplicate booking detection, subsidy eligibility check
- ✅ **Auto Subsidy Tracking** — Government limit of 12 subsidised cylinders/year enforced
- ✅ **Real-Time Inventory** — 3 cylinder types (14.2 kg, 5 kg, 19 kg) with low-stock alerts
- ✅ **Payment Management** — Cash, UPI, Card, Online — with receipt generation
- ✅ **Distributor Management** — Oil company linkage (BPCL/HPCL/IOCL)
- ✅ **8 Report Types** — Daily bookings, subsidy utilisation, monthly revenue, customer rankings

---

## 🗂️ Project Structure

```
LPG-Booking-System/
├── src/
│   └── lbms.cpp          ← Complete C++ source code (single file)
├── data/                 ← Data files created at runtime (.dat files)
├── docs/                 ← Project report and documentation
└── README.md
```

---

## 🔧 How to Compile & Run

### Using GCC (Linux/Windows with MinGW)
```bash
g++ -std=c++11 -o lbms src/lbms.cpp
./lbms
```

### Using Dev-C++ / Code::Blocks
- Open `src/lbms.cpp`
- Build & Run (F9 in Dev-C++ / F8 in Code::Blocks)

---

## 🔐 Default Login Credentials

| Role | Username | Password |
|---|---|---|
| Admin | `admin` | `Admin@123` |
| Operator | `operator` | `Op@123` |
| Delivery | `delivery` | `Del@123` |

---

## 🗃️ Data Files (Auto-created on first run)

| File | Contents |
|---|---|
| `lbms_users.dat` | User accounts and roles |
| `lbms_customers.dat` | Subscriber records |
| `lbms_bookings.dat` | All booking records |
| `lbms_stock.dat` | Cylinder inventory |
| `lbms_distributors.dat` | Distributor data |
| `lbms_payments.dat` | Payment records |
| `lbms_activity.log` | System activity log |

---

## 🏗️ Classes Overview

| Class | Role |
|---|---|
| `User` | Authentication & RBAC |
| `Customer` | Subscriber records & subsidy logic |
| `Booking` | Booking lifecycle management |
| `CylinderStock` | Real-time inventory tracking |
| `Distributor` | Supplier management |
| `Payment` | Payment recording & receipts |
| `LPGSystem` | Master controller / main menu |

---

## 📚 OOP Concepts Demonstrated

- **Encapsulation** — All data members private, accessed via getters/setters
- **Abstraction** — Complex operations hidden behind simple interfaces
- **Inheritance-Ready** — Designed for easy extension (e.g., DomesticCustomer, CommercialCustomer)
- **Polymorphism** — Consistent `save()`/`load()` interface across all classes
- **STL** — `vector`, `map`, `sort()`, `remove_if()`, `all_of()`
- **File I/O** — Persistent pipe-delimited data storage

---

## 📄 License

This project is submitted as a BCA Final Year academic project.  
© 2024-25 Abhijit Kumar Singh, B.S. College Danapur, Patliputra University.
