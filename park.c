#include "raylib.h"
#define RAYGUI_IMPLEMENTATION 
#include "raygui.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ----------------------------------------------------


////<<<<<-EMPLOYEE MANAGEMENT SYSTEM FUNCTION->>>>>

#define MAX_EMPLOYEES 15
#define MAX_VISITORS 50
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800
#define GUI_LINE_HEIGHT 30
#define GUI_SPACING 10

// Theme Park Colors
#define PARK_COLOR_DARK CLITERAL(Color){ 30, 40, 50, 255 }
#define PARK_COLOR_LIGHT CLITERAL(Color){ 200, 220, 240, 255 }
#define PARK_COLOR_ACCENT CLITERAL(Color){ 255, 100, 50, 255 }

// Global State Enum
typedef enum {
    SCREEN_HOME = 0,
    SCREEN_TICKET_COUNTER,
    SCREEN_TICKET_SCANNING,
    SCREEN_SECURITY,
    SCREEN_MAIN_MENU,
    SCREEN_RIDE_MANAGEMENT,
    SCREEN_FOOD_COURT,
    SCREEN_EMPLOYEE_VIEW,
    SCREEN_ADMIN_MENU,
    SCREEN_ADMIN_SECURITY,
    SCREEN_ADMIN_REPORT,
    SCREEN_ADMIN_VISITORS
} GameScreen;

// Global Game State Variables
GameScreen currentScreen = SCREEN_HOME;
int activeTextBox = 0;

//<<< ---TICKET MANAGEMENT FUNCTION--- >>>
struct TicketInput
{
    char ageText[10];
    char daysText[10];
    char wristBandText[10];
    char ticketTypeText[2]; // For scanning
    char passwordText[10];  // For scanning
    char metalDetectionText[10];
    char bagWeightText[10];
    char visitorName[50];
    char visitorGender[10];
    char visitorAge[10];
    char visitorTicketType[20];
    char visitorDays[10];
    char visitorPurchaseDate[20];
    char visitorExpiryDate[20];
    char visitorEntryTime[20];
    char visitorWristband[5];
    int age;
    int days;
    int wristBand;
    char ticketTypeChar;
    int password;
    int metalData;
    float bagWeight;
    char message[256];
};

struct TicketInput ticketInput = {0};

struct employee
{
    char name[50];
    int id;
    int age;
    float salary;
    char duty[50];
};
int password;

int toddlerCount = 0, childCount = 0;
int AdultCount = 0, seniorCount = 0;

int ticketRevenue = 0;
int totalRideRevenue = 0;
int totalFoodRevenue = 0;
int TotalticketSold = 0;

// Visitor data (Global Arrays)
char nameList[MAX_VISITORS][50];
char genderList[MAX_VISITORS][10];
int ageList[MAX_VISITORS];
char ticketTypeList[MAX_VISITORS][20]; 
int validityDays[MAX_VISITORS];
char purchaseDate[MAX_VISITORS][20];
char expiryDate[MAX_VISITORS][20];
int ticketNumber[MAX_VISITORS];
char wristbandList[MAX_VISITORS][5]; 
char entryTime[MAX_VISITORS][20];

// Counters
int totalVisitors = 0;

// Ride Management Variables
int rideChoice = 0;
float rideAge = 0;
float rideHeight = 0;
float rideWeight = 0;
int rideWristband = 0;
int rideFreeUsed[6] = {0};
int totalRides = 0, totalRidesBill = 0;
char rideOutput[256] = "Welcome to the Rides!";
char rideAgeText[10] = "";
char rideHeightText[10] = "";
char rideWeightText[10] = "";

// Food Court Variables
int foodChoice = 0;
int foodQuantity = 0;
int totalFoodBill = 0;
char foodQuantityText[10] = "";
char foodOutput[256] = "Welcome to the Food Court!";

// Emergency/Admin Variables
int emergencyChoice = 0;
int Level_OfSmoke = 0;
int medicalinjury = 0;
int emergencyExits[3] = {1, 1, 1};
char adminPassword[20] = "YES"; 
char adminInputText[20] = "";

char reportDisplay[2048] = "";

// AUDIO VARIABLES
// NOTE: You must load your own audio files (e.g., "sound/click.wav")
Music backgroundMusic;
Sound buttonClickSound;


// Function Prototypes (Adapted for GUI)
void DrawHomeScreen();
void DrawTicketCounterScreen();
void ProcessTicketCounterLogic();
void DrawTicketScanningScreen();
void ProcessTicketScanningLogic();
void DrawSecurityScreen();
void ProcessSecurityLogic();
void DrawMainMenu();
void DrawRideManagementScreen();
void ProcessRideManagementLogic();
void DrawFoodCourtScreen();
void ProcessFoodCourtLogic();
void DrawEmployeeViewScreen();
void DrawAdminMenuScreen();
void DrawAdminSecurityScreen();
void DrawAdminReportScreen();
void DrawAdminVisitorScreen();
void ProcessAdminVisitorLogic();
void UpdateReportDisplay();
void PlayButtonClickSound(); // New function prototype

// New function to play click sound


void PlayButtonClickSound()
{
    if (IsSoundValid(buttonClickSound))   // VALID IN RAYLIB
    {
        PlaySound(buttonClickSound);
    }
}


//<<< ---TICKET MANAGEMENT FUNCTION--- >>>
void TicketManagement(int age, int days, int wristBand, char *output_message)
{
    int wristBandPrice = 150, OriginalPrice = 0;
    char ticketType[20];
    int TicketPrice, Password = 0;

    if (age <= 0) // validation
    {
        strcpy(output_message, "Invalid Age");
        return;
    }
    else if (age <= 3) // check age for toddler and ticket type
    {
        TicketPrice = 0;
        strcpy(ticketType, "Toddler");
        Password = 1405;
        toddlerCount++;
    }
    else if (age > 3 && age <= 12) // check age for child and ticket type
    {
        TicketPrice = 150;
        strcpy(ticketType, "Child");
        Password = 9101;
        childCount++;
    }
    else if (age > 12 && age <= 59) // check age for Adult
    {
        TicketPrice = 300;
        strcpy(ticketType, "Adult");
        Password = 1234;
        AdultCount++;
    }
    else if (age > 59 && age <= 100) // check age for senior
    {
        TicketPrice = 100;
        strcpy(ticketType, "Senior");
        Password = 3491;
        seniorCount++;
    }
    else
    {
        strcpy(output_message, "InValid Age Range");
        return;
    }

    OriginalPrice = TicketPrice;
    //<<<======Check days====>>>>
    if (days == 2)
    {
        TicketPrice = (int)(TicketPrice * 1.5);
    }

    if (wristBand == 1)
    {
        TicketPrice += wristBandPrice; // Wristband extra fee
    }

    ticketRevenue += TicketPrice;
    TotalticketSold++;

    //<<<>>>Printing Ticket<<<>>>.
    // Compose output message for the GUI
    char temp_msg[512];
    sprintf(temp_msg, "<<<>>> TICKET <<<>>>\n"
                        "Ticket Price : %d\n"
                        "Tikcet Type : %s\n"
                        "Password: %d\n", OriginalPrice, ticketType, Password);

    if (days == 2)
    {
        int MultiDaypasPrice = OriginalPrice / 2; // 0.5 is 50% extra
        char extra_msg[100];
        sprintf(extra_msg, "Multi day Pass Extra charges : %d\n", MultiDaypasPrice); // multi day pass charges
        strcat(temp_msg, extra_msg);
    }

    if (wristBand == 1)
    {
        char extra_msg[100];
        sprintf(extra_msg, "Wrist Band Extra Charges : %d\n", wristBandPrice); // Wristband extra fee
        strcat(temp_msg, extra_msg);
    }
    
    char final_msg[100];
    sprintf(final_msg, "Total Charged: %d\nTotal Sold: %d", TicketPrice, TotalticketSold);
    strcat(temp_msg, final_msg);

    strcpy(output_message, temp_msg);
}

//<<<=================================Tikcet scanning======================================>>

void TicketScanning(char ticketType, int password, char *output_message)
{

    int toddlerPassword = 1405;
    int childPassword = 9101;
    int AdultPassword = 1234;
    int seniorPassword = 3491;
    
    int typeMatched = 0;
    char typeString[20] = "";

    //<<<============================Check Ticket Type==================================>>>

    if (ticketType == 'T')
    {
        typeMatched = 1;
        strcpy(typeString, "Toddler");
    }
    else if (ticketType == 'C')
    {
        typeMatched = 1;
        strcpy(typeString, "Child");
    }
    else if (ticketType == 'A')
    {
        typeMatched = 1;
        strcpy(typeString, "Adult");
    }
    else if (ticketType == 'S')
    {
        typeMatched = 1;
        strcpy(typeString, "Senior");
    }
    
    char temp_msg[256];
    
    if (typeMatched)
    {
        sprintf(temp_msg, "TicketType Matched Successfully!! (%s)\n", typeString);
    }
    else
    {
        strcpy(temp_msg, "Invalid Ticket Type\n");
    }

    //<<<============================Check Password A/C To Ticket Type===============================>>>
    int passMatched = 0;

    if (password == toddlerPassword && ticketType == 'T')
    {
        passMatched = 1;
    }
    else if (password == childPassword && ticketType == 'C')
    {
        passMatched = 1;
    }
    else if (password == AdultPassword && ticketType == 'A')
    {
        passMatched = 1;
    }
    else if (password == seniorPassword && ticketType == 'S')
    {
        passMatched = 1;
    }
    
    if (passMatched)
    {
        strcat(temp_msg, "Password Matched.\nYou Can Enter!\n");
    }
    else
    {
        strcat(temp_msg, "Invalid Password!!!\n");
    }
    
    strcpy(output_message, temp_msg);
}

//<<<<<<====================Metal Detector =========================>>

void MetalDetector(int data, char *output_message)
{
    if (data == 1)
    {
        strcpy(output_message, "Hold The Bag And The Person!!\nWARNING: Metal Detected.");
    }
    else
    {
        strcpy(output_message, "You Can Go!\nSecurity Check: Metal Free.");
    }
}

//<<<<<<====================Bag Scanner============================>>>

void BagScanner(float BagWeight, char *output_message)
{
    if (BagWeight <= 15.0)
    {
        strcpy(output_message, "You Can Entered With Your Bag (Weight: <15kg)\n");
    }
    else
    {
        strcpy(output_message, "Manual Checking required!!! (Weight: >15kg)\n");
    }
}

//<<<<====================Functions for Emergency System===============>>>
void CheckingFireAlarm(int smoke, char *output_message)
{
    int Limit_OfSmoke = 50;
    char temp_msg[256];

    if (smoke > Limit_OfSmoke)
    {
        sprintf(temp_msg, "\n Fire Detected!!! (Smoke Level: %d)\nAlert!!!\nEmergency Team Notified!\n", smoke);
    }
    else
    {
        sprintf(temp_msg, "\n No fire Detected (Smoke Level: %d)\nEverything is safe\n", smoke);
    }
    strcpy(output_message, temp_msg);
}

void evacuationPlan(int fire_Alarm, char *output_message)
{
    if (fire_Alarm == 1)
    {
        strcpy(output_message, "\nEmergency Alert!!! Evacuation Plan Activated\nFollow The Nearest Exit Signs\nStaff Guide The Visitors For Exits\n");
    }
    else
    {
        strcpy(output_message, "\nEvacuation Plan Is On Standby\n");
    }
}

void checkIngEmergencyExits(int emergencyExits[], int size, char *output_message)
{
    char temp_msg[512];
    strcpy(temp_msg, "\nChecking exits...\n");

    for (int i = 0; i < size; i++)
    {
        char exit_status[100];
        if (emergencyExits[i] == 1)
        {
            sprintf(exit_status, "Exit %d : OPEN & Lights ON.\n", i + 1);
        }
        else
        {
            sprintf(exit_status, "Exit %d : BLOCKED\n", i + 1);
        }
        strcat(temp_msg, exit_status);
    }
    strcpy(output_message, temp_msg);
}

void publicAnnouncement(int emergency, char *output_message)
{
    if (emergency == 1)
    {
        strcpy(output_message, "\n Public Announcement:\nDonot Panic... Relax Yorrself And Move Towards The Exiting Points \n");
    }
    else
    {
        strcpy(output_message, "\nAnnouncemmet System Is On Standby.\n");
    }
}

void medicalHelp(int reportedInjury, char *output_message)
{
    if (reportedInjury == 1)
    {
        strcpy(output_message, "\nEmergency Case Registered\nAssigned Nearest Medical Helper\n");
    }
    else
    {
        strcpy(output_message, "\nEveryOne is Fine \n");
    }
}

// Function to add a visitor (GUI Adaptation)
void addVisitor(char *output_message)
{
    if (totalVisitors >= MAX_VISITORS)
    {
        strcpy(output_message, "Visitor list full!");
        return;
    }

    // Parse input from GUI TextBoxes (simplified for demo/line count)
    int age = atoi(ticketInput.visitorAge);
    int validity = atoi(ticketInput.visitorDays);

    if (age <= 0 || validity <= 0 || ticketInput.visitorName[0] == '\0')
    {
        strcpy(output_message, "Invalid/Missing Input for Visitor Details.");
        return;
    }

    // Copy data to global arrays
    strcpy(nameList[totalVisitors], ticketInput.visitorName);
    strcpy(genderList[totalVisitors], ticketInput.visitorGender);
    ageList[totalVisitors] = age;
    strcpy(ticketTypeList[totalVisitors], ticketInput.visitorTicketType);
    validityDays[totalVisitors] = validity;
    strcpy(purchaseDate[totalVisitors], ticketInput.visitorPurchaseDate);
    strcpy(expiryDate[totalVisitors], ticketInput.visitorExpiryDate);
    strcpy(entryTime[totalVisitors], ticketInput.visitorEntryTime);
    strcpy(wristbandList[totalVisitors], ticketInput.visitorWristband);

    // Count adults and children
    if (ageList[totalVisitors] <= 3)
        toddlerCount++;
    else if (ageList[totalVisitors] > 3 && ageList[totalVisitors] <= 12)
        childCount++;
    else if (ageList[totalVisitors] > 12 && ageList[totalVisitors] <= 59)
        AdultCount++;
    else if (ageList[totalVisitors] > 59 && ageList[totalVisitors] <= 100)
        seniorCount++;

    // Auto Generate ticket number
    ticketNumber[totalVisitors] = 1000 + totalVisitors;

    char temp_msg[128];
    sprintf(temp_msg, "Visitor Added Successfully!\nGenerated Ticket Number: %d", ticketNumber[totalVisitors]);
    strcpy(output_message, temp_msg);

    totalVisitors++;
}

// Function to display all visitors (GUI Adaptation)
void displayVisitors(char *output_message)
{
    if (totalVisitors == 0)
    {
        strcpy(output_message, "\nNo visitors recorded yet.\n");
        return;
    }

    char temp_msg[4096];
    strcpy(temp_msg, "--- Visitor Records ---\n");
    for (int i = 0; i < totalVisitors; i++)
    {
        char visitor_info[512];
        sprintf(visitor_info, "V#%d: %s | Age: %d | Ticket: %s | Wristband: %s | T#: %d\n",
                i + 1, nameList[i], ageList[i], ticketTypeList[i], wristbandList[i], ticketNumber[i]);
        strcat(temp_msg, visitor_info);
    }

    char summary[128];
    sprintf(summary, "\nTotal Visitors: %d\nAdults: %d | Children: %d\n", totalVisitors, AdultCount, childCount);
    strcat(temp_msg, summary);

    strcpy(output_message, temp_msg);
}


//<<<<<-RIDE MANAGEMENT SYSTEM FUNCTION->>>>>

int rideManagement(int choice, float age, float height, float weight, int wristband, int *freeUsed, char *output_message)
{
    int price = 0;
    int rideIndex = choice;
    char temp_msg[256];
    
    if (choice < 1 || choice > 5)
    {
        strcpy(output_message, "Invalid ride choice.");
        return 0;
    }

    switch (choice)
    {
    // INFANT RIDE
    case 1:
    {
        if (age <= 12.0 && weight <= 15.0)
        {
            strcpy(temp_msg, "Valid for the ride, enjoy! :)\n");
            price = (wristband && freeUsed[rideIndex] == 0) ? 0 : 150;
            if (price == 0) freeUsed[rideIndex] = 1;
            totalRides++;
        }
        else
        {
            strcpy(temp_msg, "Not valid for the ride (Age<=12 months, Weight<=15kg).\n");
            price = 0;
        }
        break;
    }

    // ROLLER COASTER
    case 2:
    {
        if (age >= 12.0 && height >= 140.0)
        {
            strcpy(temp_msg, "Valid for the ride!\n");
            price = (wristband && freeUsed[rideIndex] == 0) ? 0 : 200;
            if (price == 0) freeUsed[rideIndex] = 1;
            totalRides++;
        }
        else
        {
            strcpy(temp_msg, "Not allowed (Age>=12 & Height>=140 required)\n");
            price = 0;
        }
        break;
    }

    // SKY DROP
    case 3:
    {
        if (age >= 12.0 && height >= 140.0)
        {
            strcpy(temp_msg, "Valid for the ride!\n");
            price = (wristband && freeUsed[rideIndex] == 0) ? 0 : 200;
            if (price == 0) freeUsed[rideIndex] = 1;
            totalRides++;
        }
        else
        {
            strcpy(temp_msg, "Not allowed (Age>=12 & Height>=140 required)\n");
            price = 0;
        }
        break;
    }

    // GIANT WHEEL
    case 4:
    {
        strcpy(temp_msg, "Valid for the ride!\n");
        price = (wristband && freeUsed[rideIndex] == 0) ? 0 : 250;
        if (price == 0) freeUsed[rideIndex] = 1;
        totalRides++;
        break;
    }

    // DODGEM CAR
    case 5:
    {
        if (age >= 5.0)
        {
            strcpy(temp_msg, "Valid for the ride!\n");
            price = (wristband && freeUsed[rideIndex] == 0) ? 0 : 250;
            if (price == 0) freeUsed[rideIndex] = 1;
            totalRides++;
        }
        else
        {
            strcpy(temp_msg, "Not allowed (age must be >=5)\n");
            price = 0;
        }
        break;
    }

    default:
        strcpy(output_message, "Invalid choice!");
        return 0;
    }
    
    char price_msg[50];
    sprintf(price_msg, "Ride Price: Rs.%d (Free=%s)", price, (price == 0) ? "YES" : "NO");
    strcat(temp_msg, price_msg);
    strcpy(output_message, temp_msg);

    return price;
}

//<<<<<-FOOD COURT FUNCTION->>>>>

int foodCourt(int choice, int quantity, char *output_message)
{
    int price = 0;
    char item_name[50];

    if (quantity <= 0)
    {
        strcpy(output_message, "Quantity must be greater than zero.");
        return 0;
    }

    switch (choice)
    {
    case 1: strcpy(item_name, "Burger"); price = 250; break;
    case 2: strcpy(item_name, "French Fries"); price = 100; break;
    case 3: strcpy(item_name, "Pasta"); price = 300; break;
    case 4: strcpy(item_name, "Small Pizza"); price = 350; break;
    case 5: strcpy(item_name, "Large Pizza"); price = 550; break;
    case 6: strcpy(item_name, "Veg Sandwitch"); price = 150; break;
    case 7: strcpy(item_name, "Club Sandwitch"); price = 200; break;
    case 8: strcpy(item_name, "Cold Drink"); price = 100; break;
    case 9: strcpy(item_name, "Juice"); price = 70; break;
    case 10: strcpy(item_name, "Tea"); price = 60; break;
    case 11: strcpy(item_name, "Water"); price = 100; break;
    default:
        strcpy(output_message, "Invalid Food Choice!");
        return 0;
    }
    
    int item_cost = quantity * price;
    char temp_msg[256];
    sprintf(temp_msg, "Added %d x %s (Rs.%d each).\nSubtotal: Rs.%d", quantity, item_name, price, item_cost);
    strcpy(output_message, temp_msg);

    return item_cost;
}

void generateReport(int totalTicketRevenue, int totalRideRevenue, int totalFoodRevenue, char *report_buffer)
{
    char temp_msg[4096];
    strcpy(temp_msg, "======================= THEME PARK REPORT =======================\n\n");

    // Visitors
    strcat(temp_msg, "--- Visitor Details ---\n");
    for (int i = 0; i < totalVisitors; i++)
    {
        char visitor_info[512];
        sprintf(visitor_info, "V#%d: %s | Age: %d | Ticket: %s | Wristband: %s | T#: %d\n",
               i + 1, nameList[i], ageList[i], ticketTypeList[i], wristbandList[i], ticketNumber[i]);
        strcat(temp_msg, visitor_info);
    }
    char visitor_summary[128];
    sprintf(visitor_summary, "Total Visitors: %d\nToddlers: %d | Children: %d | Adults: %d | Seniors: %d\n\n",
            totalVisitors, toddlerCount, childCount, AdultCount, seniorCount);
    strcat(temp_msg, visitor_summary);

    // Revenue Summary
    strcat(temp_msg, "--- Revenue Summary ---\n");
    char revenue_summary[256];
    sprintf(revenue_summary, "Ticket Revenue: Rs.%d\nRide Revenue: Rs.%d\nFood Revenue: Rs.%d\n",
            totalTicketRevenue, totalRideRevenue, totalFoodRevenue);
    strcat(temp_msg, revenue_summary);

    char total_revenue_msg[100];
    sprintf(total_revenue_msg, "TOTAL Revenue: Rs.%d\n\n", totalTicketRevenue + totalRideRevenue + totalFoodRevenue);
    strcat(temp_msg, total_revenue_msg);

    strcat(temp_msg, "================================================================\n");
    strcpy(report_buffer, temp_msg);
}

// GUI DRAWING FUNCTIONS (Main Loop)
// --- HOME SCREEN ---
void DrawHomeScreen()
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
    DrawText("WELCOME TO THE THEME PARK", SCREEN_WIDTH/2 - 250, 100, 35, PARK_COLOR_ACCENT);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    if (GuiButton((Rectangle){ SCREEN_WIDTH/2 - 100, 250, 200, 50 }, "Buy Ticket"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_TICKET_COUNTER;
    }
    if (GuiButton((Rectangle){ SCREEN_WIDTH/2 - 100, 320, 200, 50 }, "Enter Park (Scan)"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_TICKET_SCANNING;
    }
    if (GuiButton((Rectangle){ SCREEN_WIDTH/2 - 100, 390, 200, 50 }, "Visitor Menu"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_MAIN_MENU;
    }
    if (GuiButton((Rectangle){ SCREEN_WIDTH/2 - 100, 510, 200, 50 }, "Admin/Management"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_ADMIN_MENU;
    }
    if (GuiButton((Rectangle){ SCREEN_WIDTH/2 - 100, 580, 200, 50 }, "Exit Park"))
    {
        PlayButtonClickSound(); // Play sound
        // Exit the loop in the main function
        CloseWindow();
    }
}
// --- TICKET COUNTER SCREEN ---
void DrawTicketCounterScreen()
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
    DrawText("TICKET COUNTER", SCREEN_WIDTH/2 - 100, 50, 30, PARK_COLOR_ACCENT);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    
    int x_start = 100;
    int y_start = 150;
    
    DrawText("Enter Age:", x_start, y_start, 20, PARK_COLOR_DARK);
    if (GuiTextBox((Rectangle){ x_start + 150, y_start, 200, GUI_LINE_HEIGHT }, ticketInput.ageText, 10, activeTextBox == 1)) activeTextBox = 1;

    DrawText("Select Days (1/2):", x_start, y_start + GUI_LINE_HEIGHT + GUI_SPACING, 20, PARK_COLOR_DARK);
    if (GuiTextBox((Rectangle){ x_start + 150, y_start + GUI_LINE_HEIGHT + GUI_SPACING, 200, GUI_LINE_HEIGHT }, ticketInput.daysText, 10, activeTextBox == 2)) activeTextBox = 2;

    DrawText("Wrist Band (1=Yes, 0=No):", x_start, y_start + 2*(GUI_LINE_HEIGHT + GUI_SPACING), 20, PARK_COLOR_DARK);
    if (GuiTextBox((Rectangle){ x_start + 150, y_start + 2*(GUI_LINE_HEIGHT + GUI_SPACING), 200, GUI_LINE_HEIGHT }, ticketInput.wristBandText, 10, activeTextBox == 3)) activeTextBox = 3;

    if (GuiButton((Rectangle){ x_start + 150, y_start + 4*(GUI_LINE_HEIGHT + GUI_SPACING), 200, 50 }, "Generate Ticket"))
    {
        PlayButtonClickSound(); // Play sound
        ProcessTicketCounterLogic();
    }
    
    // Output/Message Area
    DrawRectangle(SCREEN_WIDTH/2, y_start, SCREEN_WIDTH/2 - 100, 400, PARK_COLOR_DARK); // Changed background to DARK for contrast
    DrawText("Ticket Output:", SCREEN_WIDTH/2 + 20, y_start + 20, 25, PARK_COLOR_ACCENT);
    DrawText(ticketInput.message, SCREEN_WIDTH/2 + 20, y_start + 60, 18, PARK_COLOR_LIGHT);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_ACCENT)); // Set theme for Back Button
    if (GuiButton((Rectangle){ 20, SCREEN_HEIGHT - 60, 150, 40 }, "Back to Home"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_HOME;
        memset(&ticketInput, 0, sizeof(struct TicketInput)); // Clear inputs
    }
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_LIGHT)); // Reset theme
}
void ProcessTicketCounterLogic()
{
    // Try to parse inputs safely
    ticketInput.age = atoi(ticketInput.ageText);
    ticketInput.days = atoi(ticketInput.daysText);
    ticketInput.wristBand = atoi(ticketInput.wristBandText);

    // Basic validation
    if (ticketInput.age <= 0 || (ticketInput.days != 1 && ticketInput.days != 2) || (ticketInput.wristBand != 0 && ticketInput.wristBand != 1))
    {
        strcpy(ticketInput.message, "Invalid Input: Check Age, Days (1/2), or Wristband (0/1).");
        return;
    }

    TicketManagement(ticketInput.age, ticketInput.days, ticketInput.wristBand, ticketInput.message);
}
// --- TICKET SCANNING SCREEN ---
void DrawTicketScanningScreen()
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
    DrawText("TICKET & SECURITY CHECK", SCREEN_WIDTH/2 - 180, 50, 30, PARK_COLOR_ACCENT);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    int x_start = 100;
    int y_start = 150;
    
    // Ticket Scanning Inputs
    DrawText("Ticket Type (T/C/A/S):", x_start, y_start, 20, PARK_COLOR_DARK);
    if (GuiTextBox((Rectangle){ x_start + 200, y_start, 150, GUI_LINE_HEIGHT }, ticketInput.ticketTypeText, 2, activeTextBox == 4)) activeTextBox = 4;
    
    DrawText("Ticket Password:", x_start, y_start + GUI_LINE_HEIGHT + GUI_SPACING, 20, PARK_COLOR_DARK);
    if (GuiTextBox((Rectangle){ x_start + 200, y_start + GUI_LINE_HEIGHT + GUI_SPACING, 150, GUI_LINE_HEIGHT }, ticketInput.passwordText, 10, activeTextBox == 5)) activeTextBox = 5;

    if (GuiButton((Rectangle){ x_start + 200, y_start + 3*(GUI_LINE_HEIGHT + GUI_SPACING), 150, 40 }, "Scan Ticket"))
    {
        PlayButtonClickSound(); // Play sound
        // Process Ticket Scanning
        ticketInput.ticketTypeChar = ticketInput.ticketTypeText[0];
        ticketInput.password = atoi(ticketInput.passwordText);
        TicketScanning(ticketInput.ticketTypeChar, ticketInput.password, ticketInput.message);
    }
    
    // Security Scanning Inputs (Metal/Bag)
    DrawText("Metal Detected (1=Yes, 0=No):", x_start, y_start + 6*(GUI_LINE_HEIGHT + GUI_SPACING), 20, PARK_COLOR_DARK);
    if (GuiTextBox((Rectangle){ x_start + 200, y_start + 6*(GUI_LINE_HEIGHT + GUI_SPACING), 150, GUI_LINE_HEIGHT }, ticketInput.metalDetectionText, 10, activeTextBox == 6)) activeTextBox = 6;
    
    DrawText("Bag Weight (kg):", x_start, y_start + 7*(GUI_LINE_HEIGHT + GUI_SPACING), 20, PARK_COLOR_DARK);
    if (GuiTextBox((Rectangle){ x_start + 200, y_start + 7*(GUI_LINE_HEIGHT + GUI_SPACING), 150, GUI_LINE_HEIGHT }, ticketInput.bagWeightText, 10, activeTextBox == 7)) activeTextBox = 7;

    if (GuiButton((Rectangle){ x_start + 200, y_start + 9*(GUI_LINE_HEIGHT + GUI_SPACING), 150, 40 }, "Check Security"))
    {
        PlayButtonClickSound(); // Play sound
        // Process Security Scanning
        ticketInput.metalData = atoi(ticketInput.metalDetectionText);
        ticketInput.bagWeight = atof(ticketInput.bagWeightText);
        
        char sec_msg[512];
        char metal_msg[256];
        char bag_msg[256];
        
        MetalDetector(ticketInput.metalData, metal_msg);
        BagScanner(ticketInput.bagWeight, bag_msg);
        
        sprintf(sec_msg, "--- SECURITY CHECK ---\n%s\n%s", metal_msg, bag_msg);
        strcat(ticketInput.message, sec_msg); // Append to previous ticket scan message
    }

    // Output/Message Area
    DrawRectangle(SCREEN_WIDTH/2, y_start, SCREEN_WIDTH/2 - 100, 400, PARK_COLOR_DARK); // Changed background to DARK for contrast
    DrawText("Gate/Security Output:", SCREEN_WIDTH/2 + 20, y_start + 20, 25, PARK_COLOR_ACCENT);
    DrawText(ticketInput.message, SCREEN_WIDTH/2 + 20, y_start + 60, 18, PARK_COLOR_LIGHT);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_ACCENT)); // Set theme for Back Button
    if (GuiButton((Rectangle){ 20, SCREEN_HEIGHT - 60, 150, 40 }, "Back to Home"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_HOME;
        memset(&ticketInput, 0, sizeof(struct TicketInput)); // Clear inputs
    }
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_LIGHT)); // Reset theme
}
// --- VISITOR MAIN MENU ---
void DrawMainMenu()
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
    DrawText("VISITOR MAIN MENU", SCREEN_WIDTH/2 - 150, 100, 35, PARK_COLOR_ACCENT);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    if (GuiButton((Rectangle){ SCREEN_WIDTH/2 - 150, 250, 300, 50 }, "Go to Food Court"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_FOOD_COURT;
    }
    if (GuiButton((Rectangle){ SCREEN_WIDTH/2 - 150, 320, 300, 50 }, "Go to Rides"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_RIDE_MANAGEMENT;
    }
    if (GuiButton((Rectangle){ SCREEN_WIDTH/2 - 150, 390, 300, 50 }, "View Employee Information"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_EMPLOYEE_VIEW;
    }
    
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_ACCENT)); // Set theme for Back Button
    if (GuiButton((Rectangle){ SCREEN_WIDTH/2 - 150, 510, 300, 50 }, "Back to Entry Gate"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_HOME;
    }
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_LIGHT)); // Reset theme
}
// --- RIDE MANAGEMENT SCREEN ---
void DrawRideManagementScreen()
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
    DrawText("RIDE MANAGEMENT SYSTEM", 50, 50, 30, PARK_COLOR_ACCENT);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    
    int x_start = 50;
    int y_start = 120;
    int menu_width = 300;

    DrawText("Do you have wristband? (1=YES, 0=NO):", x_start, y_start, 20, PARK_COLOR_DARK);
    if (GuiTextBox((Rectangle){ x_start + menu_width, y_start, 50, GUI_LINE_HEIGHT }, rideWristband == 1 ? "1" : "0", 2, false))
    {
        // Simple toggle for demo
        if (rideWristband == 0) rideWristband = 1; else rideWristband = 0;
    }

    // Ride Menu
    DrawText("RIDES MENU:", x_start, y_start + 2*(GUI_LINE_HEIGHT + GUI_SPACING), 25, PARK_COLOR_DARK);
    if (GuiButton((Rectangle){ x_start, y_start + 4*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "1. Infant Ride (Rs.150)")) rideChoice = 1;
    if (GuiButton((Rectangle){ x_start, y_start + 5*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "2. Roller Coaster (Rs.200)")) rideChoice = 2;
    if (GuiButton((Rectangle){ x_start, y_start + 6*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "3. Sky Drop Tower (Rs.200)")) rideChoice = 3;
    if (GuiButton((Rectangle){ x_start, y_start + 7*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "4. Giant Wheel (Rs.250)")) rideChoice = 4;
    if (GuiButton((Rectangle){ x_start, y_start + 8*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "5. Dodgem Car (Rs.250)")) rideChoice = 5;
    
    // Inputs for Ride Checks (Always visible for ease of demo)
    int x_input = SCREEN_WIDTH/2;
    int y_input = 120;
    
    DrawText("Age (Years/Months):", x_input, y_input, 20, PARK_COLOR_DARK);
    if (GuiTextBox((Rectangle){ x_input + 180, y_input, 150, GUI_LINE_HEIGHT }, rideAgeText, 10, activeTextBox == 8)) activeTextBox = 8;

    DrawText("Height (cm):", x_input, y_input + GUI_LINE_HEIGHT + GUI_SPACING, 20, PARK_COLOR_DARK);
    if (GuiTextBox((Rectangle){ x_input + 180, y_input + GUI_LINE_HEIGHT + GUI_SPACING, 150, GUI_LINE_HEIGHT }, rideHeightText, 10, activeTextBox == 9)) activeTextBox = 9;
    
    DrawText("Weight (kg):", x_input, y_input + 2*(GUI_LINE_HEIGHT + GUI_SPACING), 20, PARK_COLOR_DARK);
    if (GuiTextBox((Rectangle){ x_input + 180, y_input + 2*(GUI_LINE_HEIGHT + GUI_SPACING), 150, GUI_LINE_HEIGHT }, rideWeightText, 10, activeTextBox == 10)) activeTextBox = 10;
    
    if (GuiButton((Rectangle){ x_input + 180, y_input + 4*(GUI_LINE_HEIGHT + GUI_SPACING), 150, 40 }, "Take Selected Ride"))
    {
        PlayButtonClickSound(); // Play sound
        ProcessRideManagementLogic();
    }
    
    // Output/Bill Area
    DrawRectangle(x_input, y_input + 6*(GUI_LINE_HEIGHT + GUI_SPACING), SCREEN_WIDTH/2 - 100, 250, PARK_COLOR_DARK); // Changed background to DARK for contrast
    DrawText("Ride Output:", x_input + 10, y_input + 6*(GUI_LINE_HEIGHT + GUI_SPACING) + 10, 25, PARK_COLOR_ACCENT);
    DrawText(rideOutput, x_input + 10, y_input + 6*(GUI_LINE_HEIGHT + GUI_SPACING) + 50, 18, PARK_COLOR_LIGHT);
    
    char bill_msg[100];
    sprintf(bill_msg, "Total Rides Taken: %d | Total Ride Bill: Rs.%d", totalRides, totalRidesBill);
    DrawText(bill_msg, x_input + 10, y_input + 6*(GUI_LINE_HEIGHT + GUI_SPACING) + 200, 20, PARK_COLOR_ACCENT);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_ACCENT)); // Set theme for Back Button
    if (GuiButton((Rectangle){ 20, SCREEN_HEIGHT - 60, 150, 40 }, "Back to Main Menu"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_MAIN_MENU;
    }
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_LIGHT)); // Reset theme
}
void ProcessRideManagementLogic()
{
    if (rideChoice == 0)
    {
        strcpy(rideOutput, "Please select a ride first.");
        return;
    }
    
    // Parse inputs
    rideAge = atof(rideAgeText);
    rideHeight = atof(rideHeightText);
    rideWeight = atof(rideWeightText);
    
    int cost = rideManagement(rideChoice, rideAge, rideHeight, rideWeight, rideWristband, rideFreeUsed, rideOutput);
    totalRidesBill += cost;
    totalRideRevenue += cost;
    rideChoice = 0; // Reset selection
}
// --- FOOD COURT SCREEN ---
void DrawFoodCourtScreen()
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
    DrawText("WELCOME TO FOOD COURT", 50, 50, 30, PARK_COLOR_ACCENT);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    
    int x_start = 50;
    int y_start = 120;
    int menu_width = 300;
    
    // Food Menu
    DrawText("FOODS MENU:", x_start, y_start, 25, PARK_COLOR_DARK);
    if (GuiButton((Rectangle){ x_start, y_start + 1*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "1.Burger (Rs.250)")) foodChoice = 1;
    if (GuiButton((Rectangle){ x_start, y_start + 2*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "2.French Fries (Rs.100)")) foodChoice = 2;
    if (GuiButton((Rectangle){ x_start, y_start + 3*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "3.Pasta (Rs.300)")) foodChoice = 3;
    if (GuiButton((Rectangle){ x_start, y_start + 4*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "4.Small Pizza (Rs.350)")) foodChoice = 4;
    if (GuiButton((Rectangle){ x_start, y_start + 5*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "5.Large Pizza (Rs.550)")) foodChoice = 5;
    if (GuiButton((Rectangle){ x_start, y_start + 6*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "6.Veg Sandwitch (Rs.150)")) foodChoice = 6;
    if (GuiButton((Rectangle){ x_start, y_start + 7*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "7.Club Sandwitch (Rs.200)")) foodChoice = 7;
    
    // Drinks Menu
    int x_drinks = 400;
    DrawText("DRINKS MENU:", x_drinks, y_start, 25, PARK_COLOR_DARK);
    if (GuiButton((Rectangle){ x_drinks, y_start + 1*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "8.Cold Drink (Rs.100)")) foodChoice = 8;
    if (GuiButton((Rectangle){ x_drinks, y_start + 2*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "9.Juice (Rs.70)")) foodChoice = 9;
    if (GuiButton((Rectangle){ x_drinks, y_start + 3*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "10.Tea (Rs.60)")) foodChoice = 10;
    if (GuiButton((Rectangle){ x_drinks, y_start + 4*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "11.Water (Rs.100)")) foodChoice = 11;
    
    // Order Inputs
    int x_input = SCREEN_WIDTH - 300;
    DrawText("Selected Item:", x_input, 120, 20, PARK_COLOR_DARK);
    DrawText(TextFormat("%d (Selected)", foodChoice), x_input, 150, 25, PARK_COLOR_ACCENT);

    DrawText("Quantity:", x_input, 200, 20, PARK_COLOR_DARK);
    if (GuiTextBox((Rectangle){ x_input, 230, 200, GUI_LINE_HEIGHT }, foodQuantityText, 10, activeTextBox == 11)) activeTextBox = 11;
    
    if (GuiButton((Rectangle){ x_input, 300, 200, 40 }, "Add to Order"))
    {
        PlayButtonClickSound(); // Play sound
        ProcessFoodCourtLogic();
    }
    
    // Output/Bill Area
    DrawRectangle(x_start, 500, SCREEN_WIDTH - 100, 250, PARK_COLOR_DARK); // Changed background to DARK for contrast
    DrawText("Order Output:", x_start + 10, 510, 25, PARK_COLOR_ACCENT);
    DrawText(foodOutput, x_start + 10, 550, 18, PARK_COLOR_LIGHT);
    
    char bill_msg[100];
    sprintf(bill_msg, "Total Food Bill: Rs.%d", totalFoodBill);
    DrawText(bill_msg, x_start + 10, 700, 25, PARK_COLOR_ACCENT);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_ACCENT)); // Set theme for Back Button
    if (GuiButton((Rectangle){ 20, SCREEN_HEIGHT - 60, 150, 40 }, "Back to Main Menu"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_MAIN_MENU;
    }
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_LIGHT)); // Reset theme
}
void ProcessFoodCourtLogic()
{
    if (foodChoice == 0)
    {
        strcpy(foodOutput, "Please select a food item first.");
        return;
    }
    
    foodQuantity = atoi(foodQuantityText);
    
    if (foodQuantity <= 0)
    {
        strcpy(foodOutput, "Invalid quantity. Please enter a positive number.");
        return;
    }
    
    int cost = foodCourt(foodChoice, foodQuantity, foodOutput);
    totalFoodBill += cost;
    totalFoodRevenue += cost;
    foodChoice = 0; // Reset selection
    strcpy(foodQuantityText, ""); // Clear quantity
}
// --- EMPLOYEE VIEW SCREEN ---
void DrawEmployeeViewScreen()
{
    struct employee e[MAX_EMPLOYEES] = {
        {"Qasim", 123, 30, 20000, "Manager"},
        {"Muhammad", 124, 31, 18000, "Supervisor"},
        {"Atif", 321, 35, 5000, "Infant ride"},
        {"Asim", 322, 37, 5000, "Roller coaster"},
        {"Zain", 323, 32, 5000, "Sky drop tower"},
        {"Zakir", 324, 40, 5000, "Giant wheel"},
        {"Shahzaib", 325, 35, 5000, "Dodgem car"},
        {"Abdullah", 227, 45, 5000, "Cleaner"},
        {"Umer", 228, 48, 5000, "Cleaner"},
        {"Zaid", 300, 32, 15000, "Chef"},
        {"Aleena", 301, 29, 15000, "Chef"},
        {"Shayan", 250, 30, 5000, "Waiter"},
        {"Izhaan", 252, 40, 5000, "Waiter"},
        {"Usman", 100, 45, 4000, "Security Guard"},
        {"Musfirah", 201, 27, 15000, "Doctor"},
    };

    GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
    DrawText("EMPLOYEE INFORMATION", SCREEN_WIDTH/2 - 150, 50, 30, PARK_COLOR_ACCENT);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);

    int y_offset = 120;
    int line_h = 25;
    
    // Draw Header
    DrawText("ID", 50, y_offset, 20, PARK_COLOR_ACCENT);
    DrawText("Name", 150, y_offset, 20, PARK_COLOR_ACCENT);
    DrawText("Age", 300, y_offset, 20, PARK_COLOR_ACCENT);
    DrawText("Salary", 400, y_offset, 20, PARK_COLOR_ACCENT);
    DrawText("Duty", 550, y_offset, 20, PARK_COLOR_ACCENT);
    DrawLine(40, y_offset + 25, SCREEN_WIDTH - 40, y_offset + 25, PARK_COLOR_ACCENT);

    // Draw Employee List
    for (int i = 0; i < MAX_EMPLOYEES; i++)
    {
        DrawText(TextFormat("%d", e[i].id), 50, y_offset + (i+1)*line_h + 10, 18, PARK_COLOR_DARK);
        DrawText(e[i].name, 150, y_offset + (i+1)*line_h + 10, 18, PARK_COLOR_DARK);
        DrawText(TextFormat("%d", e[i].age), 300, y_offset + (i+1)*line_h + 10, 18, PARK_COLOR_DARK);
        DrawText(TextFormat("%.0f", e[i].salary), 400, y_offset + (i+1)*line_h + 10, 18, PARK_COLOR_DARK);
        DrawText(e[i].duty, 550, y_offset + (i+1)*line_h + 10, 18, PARK_COLOR_DARK);
    }
    
    DrawText("Job Application: Sorry, The vacancy of jobs are full", 50, y_offset + (MAX_EMPLOYEES+2)*line_h + 50, 20, RED);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_ACCENT)); // Set theme for Back Button
    if (GuiButton((Rectangle){ 20, SCREEN_HEIGHT - 60, 150, 40 }, "Back to Main Menu"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_MAIN_MENU;
    }
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_LIGHT)); // Reset theme
}
// --- ADMIN MENU SCREEN ---
void DrawAdminMenuScreen()
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
    DrawText("ADMIN / MANAGEMENT LOGIN", SCREEN_WIDTH/2 - 250, 100, 35, PARK_COLOR_ACCENT);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    // Simplistic admin login for demo
    DrawText("Enter Password (YES):", SCREEN_WIDTH/2 - 150, 200, 20, PARK_COLOR_DARK);
    if (GuiTextBox((Rectangle){ SCREEN_WIDTH/2 - 150, 230, 300, GUI_LINE_HEIGHT }, adminInputText, 20, activeTextBox == 12)) activeTextBox = 12;

    if (GuiButton((Rectangle){ SCREEN_WIDTH/2 - 150, 300, 300, 50 }, "Login"))
    {
        PlayButtonClickSound(); // Play sound
        if (strcmp(adminInputText, adminPassword) == 0)
        {
            currentScreen = SCREEN_ADMIN_SECURITY; // Default landing after login
        }
        else
        {
            strcpy(ticketInput.message, "Invalid Admin Password.");
        }
    }
    DrawText(ticketInput.message, SCREEN_WIDTH/2 - 150, 370, 20, RED);
    
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_ACCENT)); // Set theme for Back Button
    if (GuiButton((Rectangle){ 20, SCREEN_HEIGHT - 60, 150, 40 }, "Back to Home"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_HOME;
        strcpy(adminInputText, "");
        strcpy(ticketInput.message, "");
    }
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_LIGHT)); // Reset theme
    
    // Sub-menu after hypothetical login (always visible for easier testing)
    if (strcmp(adminInputText, adminPassword) == 0 || IsKeyDown(KEY_ENTER))
    {
        DrawText("ADMIN DASHBOARD", 800, 200, 25, PARK_COLOR_ACCENT);
        if (GuiButton((Rectangle){ 800, 250, 250, 40 }, "1. Security System")) 
        {
            PlayButtonClickSound(); // Play sound
            currentScreen = SCREEN_ADMIN_SECURITY;
        }
        if (GuiButton((Rectangle){ 800, 300, 250, 40 }, "2. View Daily Report"))
        {
            PlayButtonClickSound(); // Play sound
            UpdateReportDisplay();
            currentScreen = SCREEN_ADMIN_REPORT;
        }
        if (GuiButton((Rectangle){ 800, 350, 250, 40 }, "3. Visitor Management"))
        {
            PlayButtonClickSound(); // Play sound
            currentScreen = SCREEN_ADMIN_VISITORS;
        }
        if (GuiButton((Rectangle){ 800, 400, 250, 40 }, "4. Exit Admin"))
        {
            PlayButtonClickSound(); // Play sound
            currentScreen = SCREEN_HOME;
        }
    }
}
// --- ADMIN SECURITY SCREEN ---
void DrawAdminSecurityScreen()
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
    DrawText("THEME PARK SAFETY SYSTEM", 50, 50, 30, PARK_COLOR_ACCENT);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    
    int x_start = 50;
    int y_start = 120;
    int menu_width = 300;
    
    // Security Menu
    if (GuiButton((Rectangle){ x_start, y_start, menu_width, 30 }, "1. Check Fire Alarm")) 
    {
        PlayButtonClickSound(); // Play sound
        emergencyChoice = 1;
    }
    if (GuiButton((Rectangle){ x_start, y_start + (GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "2. Emergency Evacuation Plan"))
    {
        PlayButtonClickSound(); // Play sound
        emergencyChoice = 2;
    }
    if (GuiButton((Rectangle){ x_start, y_start + 2*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "3. Check Emergency Exits")) 
    {
        PlayButtonClickSound(); // Play sound
        emergencyChoice = 3;
    }
    if (GuiButton((Rectangle){ x_start, y_start + 3*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "4. Public Announcement System")) 
    {
        PlayButtonClickSound(); // Play sound
        emergencyChoice = 4;
    }
    if (GuiButton((Rectangle){ x_start, y_start + 4*(GUI_LINE_HEIGHT + GUI_SPACING), menu_width, 30 }, "5. Medical Emergency")) 
    {
        PlayButtonClickSound(); // Play sound
        emergencyChoice = 5;
    }

    // Input/Logic Area
    int x_input = SCREEN_WIDTH/2;
    DrawRectangle(x_input, y_start, SCREEN_WIDTH/2 - 100, 400, PARK_COLOR_DARK); // Changed background to DARK for contrast
    DrawText("Security Control Panel:", x_input + 10, y_start + 10, 25, PARK_COLOR_ACCENT);
    
    static char controlInputText[10] = "";
    
    int y_control = y_start + 50;

    switch (emergencyChoice)
    {
        case 1: // Fire Alarm
            DrawText("Enter smoke level:", x_input + 20, y_control, 20, PARK_COLOR_LIGHT);
            if (GuiTextBox((Rectangle){ x_input + 20, y_control + GUI_LINE_HEIGHT, 150, GUI_LINE_HEIGHT }, controlInputText, 10, activeTextBox == 13)) activeTextBox = 13;
            if (GuiButton((Rectangle){ x_input + 200, y_control + GUI_LINE_HEIGHT, 150, GUI_LINE_HEIGHT }, "Check"))
            {
                PlayButtonClickSound(); // Play sound
                Level_OfSmoke = atoi(controlInputText);
                CheckingFireAlarm(Level_OfSmoke, ticketInput.message);
            }
            break;
        case 2: // Evacuation Plan
            DrawText("Enter 1 if fire alarm is ON, 0 if OFF:", x_input + 20, y_control, 20, PARK_COLOR_LIGHT);
            if (GuiTextBox((Rectangle){ x_input + 20, y_control + GUI_LINE_HEIGHT, 150, GUI_LINE_HEIGHT }, controlInputText, 10, activeTextBox == 13)) activeTextBox = 13;
            if (GuiButton((Rectangle){ x_input + 200, y_control + GUI_LINE_HEIGHT, 150, GUI_LINE_HEIGHT }, "Activate/Deactivate"))
            {
                PlayButtonClickSound(); // Play sound
                Level_OfSmoke = atoi(controlInputText);
                evacuationPlan(Level_OfSmoke, ticketInput.message);
            }
            break;
        case 3: // Emergency Exits
            DrawText("Set Exit Status (1=Open, 0=Blocked):", x_input + 20, y_control, 20, PARK_COLOR_LIGHT);
            for(int i = 0; i < 3; i++)
            {
                DrawText(TextFormat("Exit %d:", i+1), x_input + 20, y_control + (i+1)*(GUI_LINE_HEIGHT + GUI_SPACING), 18, PARK_COLOR_LIGHT);
                if (GuiButton((Rectangle){ x_input + 100, y_control + (i+1)*(GUI_LINE_HEIGHT + GUI_SPACING), 80, GUI_LINE_HEIGHT }, emergencyExits[i] == 1 ? "OPEN" : "BLOCKED"))
                {
                    PlayButtonClickSound(); // Play sound
                    emergencyExits[i] = 1 - emergencyExits[i];
                }
            }
            if (GuiButton((Rectangle){ x_input + 200, y_control + 5*(GUI_LINE_HEIGHT + GUI_SPACING), 150, GUI_LINE_HEIGHT }, "Check All Exits"))
            {
                PlayButtonClickSound(); // Play sound
                checkIngEmergencyExits(emergencyExits, 3, ticketInput.message);
            }
            break;
        case 4: // Public Announcement
            DrawText("Enter 1 for emergency announcement, 0 for normal:", x_input + 20, y_control, 20, PARK_COLOR_LIGHT);
            if (GuiTextBox((Rectangle){ x_input + 20, y_control + GUI_LINE_HEIGHT, 150, GUI_LINE_HEIGHT }, controlInputText, 10, activeTextBox == 13)) activeTextBox = 13;
            if (GuiButton((Rectangle){ x_input + 200, y_control + GUI_LINE_HEIGHT, 150, GUI_LINE_HEIGHT }, "Announce"))
            {
                PlayButtonClickSound(); // Play sound
                Level_OfSmoke = atoi(controlInputText);
                publicAnnouncement(Level_OfSmoke, ticketInput.message);
            }
            break;
        case 5: // Medical Help
            DrawText("Any injury? (1 Yes / 0 No):", x_input + 20, y_control, 20, PARK_COLOR_LIGHT);
            if (GuiTextBox((Rectangle){ x_input + 20, y_control + GUI_LINE_HEIGHT, 150, GUI_LINE_HEIGHT }, controlInputText, 10, activeTextBox == 13)) activeTextBox = 13;
            if (GuiButton((Rectangle){ x_input + 200, y_control + GUI_LINE_HEIGHT, 150, GUI_LINE_HEIGHT }, "Report Injury"))
            {
                PlayButtonClickSound(); // Play sound
                medicalinjury = atoi(controlInputText);
                medicalHelp(medicalinjury, ticketInput.message);
            }
            break;
        default:
            DrawText("Select a system to manage.", x_input + 20, y_control, 20, PARK_COLOR_LIGHT);
            break;
    }

    // Output Message
    DrawText("System Message:", x_input + 10, y_start + 420, 25, PARK_COLOR_ACCENT);
    DrawText(ticketInput.message, x_input + 10, y_start + 460, 18, PARK_COLOR_LIGHT);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_ACCENT)); // Set theme for Back Button
    if (GuiButton((Rectangle){ 20, SCREEN_HEIGHT - 60, 150, 40 }, "Back to Admin Menu"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_ADMIN_MENU;
        strcpy(ticketInput.message, "");
    }
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_LIGHT)); // Reset theme
}
// --- ADMIN REPORT SCREEN ---
void UpdateReportDisplay()
{
    generateReport(ticketRevenue, totalRideRevenue, totalFoodRevenue, reportDisplay);
}
void DrawAdminReportScreen()
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
    DrawText("DAILY REVENUE AND VISITOR REPORT", SCREEN_WIDTH/2 - 250, 50, 30, PARK_COLOR_ACCENT);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
    
    // Report Display
    DrawRectangle(50, 100, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 200, PARK_COLOR_DARK); // Changed background to DARK for contrast
    DrawText(reportDisplay, 60, 110, 18, PARK_COLOR_LIGHT);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_ACCENT)); // Set theme for Back Button
    if (GuiButton((Rectangle){ 20, SCREEN_HEIGHT - 60, 150, 40 }, "Back to Admin Menu"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_ADMIN_MENU;
    }
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_LIGHT)); // Reset theme
}
// --- ADMIN VISITOR MANAGEMENT SCREEN ---
void DrawAdminVisitorScreen()
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
    DrawText("VISITOR MANAGEMENT (ADMIN)", 50, 50, 30, PARK_COLOR_ACCENT);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    int x_start = 50;
    int y_start = 120;
    int menu_width = 300;

    if (GuiButton((Rectangle){ x_start, y_start, menu_width, 40 }, "1. Add New Visitor")) 
    {
        PlayButtonClickSound(); // Play sound
        emergencyChoice = 1;
    }
    if (GuiButton((Rectangle){ x_start, y_start + 50, menu_width, 40 }, "2. Display All Visitors")) 
    {
        PlayButtonClickSound(); // Play sound
        emergencyChoice = 2;
    }

    int x_input = SCREEN_WIDTH/2;
    DrawRectangle(x_input, y_start, SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT - 200, PARK_COLOR_DARK); // Changed background to DARK for contrast
    DrawText("Visitor Data Entry/Output:", x_input + 10, y_start + 10, 25, PARK_COLOR_ACCENT);
    
    int y_control = y_start + 50;
    
    if (emergencyChoice == 1) // Add Visitor
    {
        DrawText("Name:", x_input + 20, y_control, 20, PARK_COLOR_LIGHT);
        if (GuiTextBox((Rectangle){ x_input + 150, y_control, 250, GUI_LINE_HEIGHT }, ticketInput.visitorName, 50, activeTextBox == 14)) activeTextBox = 14;

        DrawText("Gender (M/F):", x_input + 20, y_control + 1*(GUI_LINE_HEIGHT + GUI_SPACING), 20, PARK_COLOR_LIGHT);
        if (GuiTextBox((Rectangle){ x_input + 150, y_control + 1*(GUI_LINE_HEIGHT + GUI_SPACING), 50, GUI_LINE_HEIGHT }, ticketInput.visitorGender, 10, activeTextBox == 15)) activeTextBox = 15;
        
        DrawText("Age:", x_input + 20, y_control + 2*(GUI_LINE_HEIGHT + GUI_SPACING), 20, PARK_COLOR_LIGHT);
        if (GuiTextBox((Rectangle){ x_input + 150, y_control + 2*(GUI_LINE_HEIGHT + GUI_SPACING), 50, GUI_LINE_HEIGHT }, ticketInput.visitorAge, 10, activeTextBox == 16)) activeTextBox = 16;
        
        DrawText("Ticket Type (Day/MultiDay):", x_input + 20, y_control + 3*(GUI_LINE_HEIGHT + GUI_SPACING), 20, PARK_COLOR_LIGHT);
        if (GuiTextBox((Rectangle){ x_input + 250, y_control + 3*(GUI_LINE_HEIGHT + GUI_SPACING), 150, GUI_LINE_HEIGHT }, ticketInput.visitorTicketType, 20, activeTextBox == 17)) activeTextBox = 17;
        
        DrawText("Validity (days):", x_input + 20, y_control + 4*(GUI_LINE_HEIGHT + GUI_SPACING), 20, PARK_COLOR_LIGHT);
        if (GuiTextBox((Rectangle){ x_input + 150, y_control + 4*(GUI_LINE_HEIGHT + GUI_SPACING), 50, GUI_LINE_HEIGHT }, ticketInput.visitorDays, 10, activeTextBox == 18)) activeTextBox = 18;
        
        DrawText("Wristband (Yes/No):", x_input + 20, y_control + 5*(GUI_LINE_HEIGHT + GUI_SPACING), 20, PARK_COLOR_LIGHT);
        if (GuiTextBox((Rectangle){ x_input + 200, y_control + 5*(GUI_LINE_HEIGHT + GUI_SPACING), 80, GUI_LINE_HEIGHT }, ticketInput.visitorWristband, 5, activeTextBox == 19)) activeTextBox = 19;
        
        if (GuiButton((Rectangle){ x_input + 20, y_control + 7*(GUI_LINE_HEIGHT + GUI_SPACING), 200, 40 }, "Save Visitor"))
        {
            PlayButtonClickSound(); // Play sound
            addVisitor(ticketInput.message);
        }
    }
    else if (emergencyChoice == 2) // Display Visitors
    {
        displayVisitors(ticketInput.message);
    }
    
    // Output Message
    DrawText(ticketInput.message, x_input + 20, SCREEN_HEIGHT - 150, 18, PARK_COLOR_LIGHT);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_ACCENT)); // Set theme for Back Button
    if (GuiButton((Rectangle){ 20, SCREEN_HEIGHT - 60, 150, 40 }, "Back to Admin Menu"))
    {
        PlayButtonClickSound(); // Play sound
        currentScreen = SCREEN_ADMIN_MENU;
        emergencyChoice = 0;
        strcpy(ticketInput.message, "");
    }
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(PARK_COLOR_LIGHT)); // Reset theme
}


int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Theme Park Management System - Raylib GUI");

    // --- Audio Initialization ---
    InitAudioDevice();
    
    // NOTE: Replace "audio/bgsound.mp3" and "audio/click.wav" with your actual file paths
     backgroundMusic = LoadMusicStream("bgsound.mp3"); // Uncomment this line and add your music file
     buttonClickSound = LoadSound("click.wav");        // Uncomment this line and add your click sound file

     PlayMusicStream(backgroundMusic);  // Raylib will handle it


    
    // --- Fullscreen Toggle ---
    ToggleFullscreen();

    // Initialize all input buffers to empty strings for proper text box display
    memset(&ticketInput, 0, sizeof(struct TicketInput));
    rideWristband = 1; // Default to having a wristband for demo ease
    
    // Initialize temporary input buffers
    strcpy(ticketInput.ageText, "30");
    strcpy(ticketInput.daysText, "1");
    strcpy(ticketInput.wristBandText, "1");
    strcpy(ticketInput.ticketTypeText, "A");
    strcpy(ticketInput.passwordText, "1234");
    strcpy(ticketInput.metalDetectionText, "0");
    strcpy(ticketInput.bagWeightText, "10.0");
    
    strcpy(ticketInput.visitorName, "New Visitor");
    strcpy(ticketInput.visitorGender, "M");
    strcpy(ticketInput.visitorAge, "25");
    strcpy(ticketInput.visitorTicketType, "Day");
    strcpy(ticketInput.visitorDays, "1");
    strcpy(ticketInput.visitorPurchaseDate, "24-11-2025");
    strcpy(ticketInput.visitorExpiryDate, "25-11-2025");
    strcpy(ticketInput.visitorEntryTime, "09:00");
    strcpy(ticketInput.visitorWristband, "Yes");

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
       // --- Update ---
        // The line below is the correct Raylib function call.
        UpdateMusicStream(backgroundMusic); 
        
        // Handle input focus for TextBoxes
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            // Reset focus if clicked outside of any active textbox area
            activeTextBox = 0; 
        }

        // --- Draw ---
        BeginDrawing();
        ClearBackground(PARK_COLOR_LIGHT);
        
        // Background elements (optional, for padding/visuals)
        DrawRectangle(0, 0, SCREEN_WIDTH, 80, PARK_COLOR_DARK);
        DrawRectangle(0, SCREEN_HEIGHT - 80, SCREEN_WIDTH, 80, PARK_COLOR_DARK);
        DrawText("THEME PARK MANAGEMENT", SCREEN_WIDTH/2 - 150, 20, 30, PARK_COLOR_ACCENT);

        switch (currentScreen)
        {
            case SCREEN_HOME:
                DrawHomeScreen();
                break;
            case SCREEN_TICKET_COUNTER:
                DrawTicketCounterScreen();
                break;
            case SCREEN_TICKET_SCANNING:
                DrawTicketScanningScreen();
                break;
            case SCREEN_MAIN_MENU:
                DrawMainMenu();
                break;
            case SCREEN_RIDE_MANAGEMENT:
                DrawRideManagementScreen();
                break;
            case SCREEN_FOOD_COURT:
                DrawFoodCourtScreen();
                break;
            case SCREEN_EMPLOYEE_VIEW:
                DrawEmployeeViewScreen();
                break;
            case SCREEN_ADMIN_MENU:
                DrawAdminMenuScreen();
                break;
            case SCREEN_ADMIN_SECURITY:
                DrawAdminSecurityScreen();
                break;
            case SCREEN_ADMIN_REPORT:
                DrawAdminReportScreen();
                break;
            case SCREEN_ADMIN_VISITORS:
                DrawAdminVisitorScreen();
                break;
            default:
                DrawText("UNKNOWN SCREEN", 500, 400, 30, RED);
                break;
        }

        // Draw current screen title/name for debugging/clarity
        DrawText(TextFormat("Screen: %d", currentScreen), SCREEN_WIDTH - 200, SCREEN_HEIGHT - 30, 15, WHITE);

        EndDrawing();
    }
    
    // --- Unload Resources ---
     UnloadMusicStream(backgroundMusic); 
 UnloadSound(buttonClickSound);      
    CloseAudioDevice();

    CloseWindow();
    return 0;
}