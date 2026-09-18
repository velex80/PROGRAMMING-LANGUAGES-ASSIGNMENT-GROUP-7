#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;


// Binding Time: Value bound at COMPILE-TIME[span_1](start_span)[span_1](end_span)
const double FIXED_CHARGE = 100.0;     // KES 100 fixed charge[span_2](start_span)[span_2](end_span)
const double RATE_BAND1 = 25.0;        // KES 25 per unit for first 10 units[span_3](start_span)[span_3](end_span)
const double RATE_BAND2 = 35.0;        // KES 35 per unit for next 20 units[span_4](start_span)[span_4](end_span)
const double RATE_BAND3 = 50.0;        // KES 50 per unit above 30 units[span_5](start_span)[span_5](end_span)

const int BAND1_LIMIT = 10;            // First 10 units[span_6](start_span)[span_6](end_span)
const int BAND2_LIMIT = 20;            // Next 20 units (units 11-30)[span_7](start_span)[span_7](end_span)

struct Customer {
    string id;
    string name;
    double previousReading; // Value bound at RUN-TIME via user/data input[span_8](start_span)[span_8](end_span)
    double currentReading;  // Value bound at RUN-TIME via user/data input[span_9](start_span)[span_9](end_span)
};

struct Bill {
    string customerID;
    string customerName;
    double unitsConsumed;
    double band1Charge;
    double band2Charge;
    double band3Charge;
    double fixedCharge;
    double totalBill;
    bool isValid;
    string errorMessage;
};

// Global summary tracking variables
double g_totalUnitsConsumed = 0.0;
double g_totalSimulationRevenue = 0.0;
double g_highestConsumption = -1.0;
string g_highestConsumerName = "";


Bill generateBill(const Customer& cust) {
    // TASK D: STATIC LOCAL VARIABLE EXPERIMENT[span_10](start_span)[span_10](end_span)
    // Lifetime: Program Lifetime (persists across all function calls)[span_11](start_span)[span_11](end_span)
    // Scope: Local to generateBill function[span_12](start_span)[span_12](end_span)
    static int billsGeneratedCount = 0;

    Bill bill;
    bill.customerID = cust.id;
    bill.customerName = cust.name;
    bill.band1Charge = 0.0;
    bill.band2Charge = 0.0;
    bill.band3Charge = 0.0;
    bill.fixedCharge = FIXED_CHARGE;
    bill.totalBill = 0.0;
    bill.unitsConsumed = 0.0;

    // Input Validation: currentReading >= previousReading[span_13](start_span)[span_13](end_span)
    if (cust.currentReading < cust.previousReading) {
        bill.isValid = false;
        bill.errorMessage = "Invalid Readings: Current reading is less than previous reading.";
        return bill;
    }

    bill.isValid = true;

    // TASK C: L-VALUE / R-VALUE ANALYSIS (Assignment 1)[span_14](start_span)[span_14](end_span)
    // L-value: bill.unitsConsumed (memory location being written to)[span_15](start_span)[span_15](end_span)
    // R-value: cust.currentReading - cust.previousReading (evaluated temporary value)[span_16](start_span)[span_16](end_span)
    bill.unitsConsumed = cust.currentReading - cust.previousReading;

    double remainingUnits = bill.unitsConsumed;

    // TASK D: BLOCK-LOCAL SCOPE EXPERIMENT[span_17](start_span)[span_17](end_span)
    // Declaring blockCharge inside each branch minimizes variable scope[span_18](start_span)[span_18](end_span)

    // Band 1 Calculation
    if (remainingUnits > 0) {
        double unitsInBand1 = min(remainingUnits, static_cast<double>(BAND1_LIMIT));

        // TASK D: block-local variable
        double blockCharge = unitsInBand1 * RATE_BAND1;
        bill.band1Charge = blockCharge;
        remainingUnits -= unitsInBand1;
    }

    // Band 2 Calculation
    if (remainingUnits > 0) {
        double unitsInBand2 = min(remainingUnits, static_cast<double>(BAND2_LIMIT));

        // TASK D: block-local variable (independent scope from Band 1 block)
        double blockCharge = unitsInBand2 * RATE_BAND2;
        bill.band2Charge = blockCharge;
        remainingUnits -= unitsInBand2;
    }

    // Band 3 Calculation
    if (remainingUnits > 0) {
        // TASK D: block-local variable
        double blockCharge = remainingUnits * RATE_BAND3;
        bill.band3Charge = blockCharge;
    }

    // TASK C: L-VALUE / R-VALUE ANALYSIS (Assignment 2)[span_19](start_span)[span_19](end_span)
    // L-value: bill.totalBill[span_20](start_span)[span_20](end_span)
    // R-value: bill.band1Charge + bill.band2Charge + bill.band3Charge + bill.fixedCharge[span_21](start_span)[span_21](end_span)
    bill.totalBill = bill.band1Charge + bill.band2Charge + bill.band3Charge + bill.fixedCharge;

    // Update static counter & global stats
    billsGeneratedCount++;
    g_totalUnitsConsumed += bill.unitsConsumed;
    g_totalSimulationRevenue += bill.totalBill;

    if (bill.unitsConsumed > g_highestConsumption) {
        g_highestConsumption = bill.unitsConsumed;
        g_highestConsumerName = bill.customerName;
    }

    return bill;
}


void printBill(const Bill& bill) {
    cout << "--------------------------------------------------\n";
    cout << "CUSTOMER BILL STATEMENT\n";
    cout << "--------------------------------------------------\n";
    cout << "Customer ID    : " << bill.customerID << "\n";
    cout << "Customer Name  : " << bill.customerName << "\n";

    if (!bill.isValid) {
        cout << "STATUS         : REJECTED\n";
        cout << "Reason         : " << bill.errorMessage << "\n";
        cout << "--------------------------------------------------\n\n";
        return;
    }

    cout << "Units Consumed : " << bill.unitsConsumed << " units\n";
    cout << fixed << setprecision(2);
    cout << "Fixed Charge   : KES " << bill.fixedCharge << "\n";
    cout << "Band 1 Charge  : KES " << bill.band1Charge << "\n";
    cout << "Band 2 Charge  : KES " << bill.band2Charge << "\n";
    cout << "Band 3 Charge  : KES " << bill.band3Charge << "\n";
    cout << "TOTAL AMOUNT   : KES " << bill.totalBill << "\n";
    cout << "--------------------------------------------------\n\n";
}

int main() {
    // TASK A: Dataset containing at least 6 customers (including boundary & error cases)[span_22](start_span)[span_22](end_span)
    vector<Customer> dataset = {
        {"C001", "Alice Wambui", 120.0, 128.0},  // 8 units (Band 1 only)
        {"C002", "Brian Ochieng", 300.0, 325.0}, // 25 units (Band 1 + Band 2)
        {"C003", "Catherine Nduta", 50.0, 95.0},  // 45 units (All Bands)
        {"C004", "David Kiprop", 210.0, 210.0},  // 0 units (Boundary Case - Minimum consumption)
        {"C005", "Eunice Hassan", 450.0, 430.0}, // Invalid Case (Current < Previous)[span_23](start_span)[span_23](end_span)
        {"C006", "Frank Mutua", 100.0, 130.0}    // 30 units (Band 1 + Band 2 exact limit)
    };

    int rejectedCount = 0;

    cout << "==================================================\n";
    cout << "  COUNTY WATER CONSUMPTION AND BILLING SYSTEM\n";
    cout << "==================================================\n\n";

    for (const auto& customer : dataset) {
        Bill bill = generateBill(customer);
        if (!bill.isValid) {
            rejectedCount++;
        }
        printBill(bill);
    }

    // TASK E: SYSTEM SUMMARY REPORT[span_24](start_span)[span_24](end_span)
    cout << "==================================================\n";
    cout << "                SIMULATION SUMMARY\n";
    cout << "==================================================\n";
    cout << fixed << setprecision(2);
    cout << "Total Water Consumed    : " << g_totalUnitsConsumed << " units\n";
    cout << "Total Revenue Generated : KES " << g_totalSimulationRevenue << "\n";
    cout << "Highest Consumption     : " << g_highestConsumption << " units (" << g_highestConsumerName << ")\n";
    cout << "Rejected Meter Records  : " << rejectedCount << "\n";
    cout << "==================================================\n";

    return 0;
}
