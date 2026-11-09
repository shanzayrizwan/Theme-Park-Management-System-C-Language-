/*
 * =====================================================================
 *  THEME PARK MANAGEMENT SYSTEM  -  Raylib + raygui  (rewritten)
 * =====================================================================
 *  Build on Windows (MinGW):
 *      gcc park.c -o park.exe -lraylib -lopengl32 -lgdi32 -lwinmm
 *  Build on Linux:
 *      gcc park.c -o park -lraylib -lm -lpthread -ldl -lrt -lX11
 *
 *  Keep raygui.h in the same folder as this file.
 *  Optional: put bgsound.mp3 and click.wav next to the program for sound.
 * =====================================================================
 */

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/*  1. CONSTANTS AND THEME                                             */
/* ------------------------------------------------------------------ */

#define SCREEN_WIDTH       1280
#define SCREEN_HEIGHT      800

#define MAX_VISITORS       50
#define VISITORS_PER_PAGE  18
#define NAME_LEN           50
#define MSG_LEN            512
#define REPORT_LEN         1024

#define ADMIN_PASSWORD     "YES"
#define WRISTBAND_PRICE    150
#define BAG_LIMIT_KG       15.0f
#define SMOKE_LIMIT        50
#define EXIT_COUNT         3
#define RIDE_COUNT         5
#define FOOD_COUNT         11
#define FIELD_H            32

/* Theme: "Carnival at Night" - cream background, purple panels, pink/gold accents */
#define COL_BG      CLITERAL(Color){ 255, 243, 224, 255 }  /* warm cream background */
#define COL_PANEL   CLITERAL(Color){ 46, 26, 82, 255 }     /* night-sky purple panels */
#define COL_ACCENT  CLITERAL(Color){ 255, 88, 110, 255 }   /* carnival pink-red */
#define COL_GOLD    CLITERAL(Color){ 255, 200, 61, 255 }   /* ticket gold highlights */
#define COL_TEAL    CLITERAL(Color){ 36, 196, 180, 255 }   /* pressed buttons */
#define COL_TEXT    CLITERAL(Color){ 46, 26, 82, 255 }     /* dark text on cream */
#define COL_LIGHT   CLITERAL(Color){ 255, 248, 235, 255 }  /* light text on purple */

/* ------------------------------------------------------------------ */
/*  2. TYPES                                                           */
/* ------------------------------------------------------------------ */

/* Every screen in the program */
typedef enum {
    SCREEN_HOME,
    SCREEN_TICKET_COUNTER,
    SCREEN_GATE,
    SCREEN_VISITOR_MENU,
    SCREEN_FOOD_COURT,
    SCREEN_RIDES,
    SCREEN_EMPLOYEES,
    SCREEN_ADMIN_LOGIN,
    SCREEN_ADMIN_MENU,
    SCREEN_SAFETY,
    SCREEN_REPORT,
    SCREEN_VISITOR_RECORDS
} AppScreen;

/* Each text box gets an ID so only one box can be "active" (typing) at a time */
typedef enum {
    FIELD_NONE = 0,
    FIELD_AGE,
    FIELD_DAYS,
    FIELD_WRISTBAND,
    FIELD_GATE_CODE,
    FIELD_GATE_PASS,
    FIELD_METAL,
    FIELD_BAG,
    FIELD_RIDE_AGE,
    FIELD_RIDE_MONTHS,
    FIELD_RIDE_HEIGHT,
    FIELD_RIDE_WEIGHT,
    FIELD_FOOD_QTY,
    FIELD_SMOKE,
    FIELD_LOGIN,
    FIELD_V_NAME,
    FIELD_V_GENDER,
    FIELD_V_AGE,
    FIELD_V_DAYS,
    FIELD_V_WRIST
} InputField;

/* Ticket categories, decided by the visitor's age */
typedef enum {
    CAT_TODDLER,
    CAT_CHILD,
    CAT_ADULT,
    CAT_SENIOR,
    CAT_COUNT
} TicketCategory;

/* One saved visitor (from the admin "Visitor Records" screen) */
typedef struct {
    char name[NAME_LEN];
    char gender;        /* 'M' or 'F' */
    int  age;
    int  validityDays;
    bool wristband;
    int  ticketNumber;
} Visitor;

/* One staff member (read-only table) */
typedef struct {
    const char *name;
    int         id;
    int         age;
    int         salary;
    const char *duty;
} Employee;

/* ------------------------------------------------------------------ */
/*  3. FIXED DATA (prices, passwords, menus, staff)                    */
/* ------------------------------------------------------------------ */

static const char *categoryName[CAT_COUNT]     = { "Toddler", "Child", "Adult", "Senior" };
static const char  categoryCode[CAT_COUNT]     = { 'T', 'C', 'A', 'S' };
static const int   categoryPrice[CAT_COUNT]    = { 0, 150, 300, 100 };
static const int   categoryPassword[CAT_COUNT] = { 1405, 9101, 1234, 3491 };

static const char *rideName[RIDE_COUNT]  = { "Infant Ride", "Roller Coaster", "Sky Drop Tower", "Giant Wheel", "Dodgem Car" };
static const int   ridePrice[RIDE_COUNT] = { 150, 200, 200, 250, 250 };

/* Items 0-6 are food, items 7-10 are drinks */
static const char *foodName[FOOD_COUNT]  = {
    "Burger", "French Fries", "Pasta", "Small Pizza", "Large Pizza",
    "Veg Sandwich", "Club Sandwich", "Cold Drink", "Juice", "Tea", "Water"
};
static const int foodPrice[FOOD_COUNT] = { 250, 100, 300, 350, 550, 150, 200, 100, 70, 60, 100 };

static const Employee staff[] = {
    { "Qasim",     123, 30, 20000, "Manager" },
    { "Muhammad",  124, 31, 18000, "Supervisor" },
    { "Atif",      321, 35,  5000, "Infant ride" },
    { "Asim",      322, 37,  5000, "Roller coaster" },
    { "Zain",      323, 32,  5000, "Sky drop tower" },
    { "Zakir",     324, 40,  5000, "Giant wheel" },
    { "Shahzaib",  325, 35,  5000, "Dodgem car" },
    { "Abdullah",  227, 45,  5000, "Cleaner" },
    { "Umer",      228, 48,  5000, "Cleaner" },
    { "Zaid",      300, 32, 15000, "Chef" },
    { "Aleena",    301, 29, 15000, "Chef" },
    { "Shayan",    250, 30,  5000, "Waiter" },
    { "Izhaan",    252, 40,  5000, "Waiter" },
    { "Usman",     100, 45,  4000, "Security Guard" },
    { "Musfirah",  201, 27, 15000, "Doctor" },
};
#define STAFF_COUNT ((int)(sizeof(staff) / sizeof(staff[0])))

/* ------------------------------------------------------------------ */
/*  4. GLOBAL STATE                                                    */
/* ------------------------------------------------------------------ */

static AppScreen  currentScreen = SCREEN_HOME;
static InputField focusedField  = FIELD_NONE;   /* which text box is being typed in */
static bool       exitRequested = false;
static bool       adminLoggedIn = false;

/* Audio (loaded only if the files exist) */
static Sound clickSound;
static Music bgMusic;
static bool  hasSound = false;
static bool  hasMusic = false;

/* Ticket sales */
static int ticketsSold           = 0;
static int ticketsByCategory[CAT_COUNT] = { 0 };
static int ticketRevenue         = 0;

/* Rides */
static int  selectedRide    = -1;                 /* 0..4, -1 = nothing picked */
static bool wristbandOn     = true;
static bool freeRideUsed[RIDE_COUNT] = { false }; /* wristband perk: one free ride per type */
static int  ridesTaken      = 0;
static int  rideRevenue     = 0;

/* Food court */
static int selectedFood = -1;                     /* 0..10, -1 = nothing picked */
static int foodRevenue  = 0;

/* Safety */
static int  safetyMode        = 1;                /* 1..5 (which safety tool is open) */
static bool evacuationActive  = false;
static bool exitOpen[EXIT_COUNT] = { true, true, true };

/* Visitor records (added by admin) */
static Visitor visitors[MAX_VISITORS];
static int     visitorCount = 0;
static int     visitorPage  = 0;

/* Text box buffers */
static char ageText[8]        = "30";
static char daysText[4]       = "1";
static char wristText[4]      = "1";
static char gateCodeText[4]   = "";
static char gatePassText[8]   = "";
static char metalText[4]      = "0";
static char bagText[8]        = "10";
static char rideAgeText[6]    = "";
static char rideMonthsText[6] = "";
static char rideHeightText[6] = "";
static char rideWeightText[6] = "";
static char foodQtyText[6]    = "1";
static char smokeText[8]      = "";
static char loginText[20]     = "";
static char vNameText[NAME_LEN] = "";
static char vGenderText[4]    = "";
static char vAgeText[6]       = "";
static char vDaysText[6]      = "1";
static char vWristText[4]     = "N";

/* Output messages shown on each screen */
static char ticketMsg[MSG_LEN]   = "Enter the visitor's details and press Generate Ticket.";
static char gateMsg[MSG_LEN]     = "Scan a ticket at the gate.";
static char securityMsg[MSG_LEN] = "";
static char rideMsg[MSG_LEN]     = "Pick a ride and fill in the visitor's details.";
static char foodMsg[MSG_LEN]     = "Pick an item, set the quantity, then add to order.";
static char safetyMsg[MSG_LEN]   = "";
static char visitorMsg[MSG_LEN]  = "";
static char loginMsg[MSG_LEN]    = "";
static char reportText[REPORT_LEN] = "";

/* ------------------------------------------------------------------ */
/*  5. SMALL HELPERS (drawing, buttons, text boxes)                    */
/* ------------------------------------------------------------------ */

static void GoTo(AppScreen screen)
{
    currentScreen = screen;
    focusedField = FIELD_NONE;       /* leaving a screen closes any open text box */
}

static void PlayClick(void)
{
    if (hasSound) PlaySound(clickSound);
}

/* Draws a button and returns true on the frame it is clicked */
static bool Button(Rectangle r, const char *text)
{
    if (GuiButton(r, text))
    {
        PlayClick();
        return true;
    }
    return false;
}

/* Draws a text box; clicking it makes it the active field */
static void TextField(Rectangle r, char *buffer, size_t size, InputField id)
{
    if (GuiTextBox(r, buffer, (int)size, focusedField == id))
    {
        focusedField = id;
    }
}

/* Draws text inside a dark panel (light colored) */
static void PanelText(int x, int y, const char *text)
{
    DrawText(text, x, y, 20, COL_LIGHT);
}

/* Draws a rounded dark panel with an optional gold title */
static void DrawPanel(Rectangle r, const char *title)
{
    DrawRectangleRounded(r, 0.05f, 8, COL_PANEL);
    if (title != NULL)
    {
        DrawText(title, (int)r.x + 20, (int)r.y + 14, 24, COL_GOLD);
    }
}

/* Draws a big title at the top-left of a screen */
static void DrawScreenTitle(const char *title)
{
    DrawText(title, 40, 92, 34, COL_ACCENT);
}

/* Draws text that may contain '\n' (DrawText alone does not understand newlines) */
static void DrawMultiline(const char *text, int x, int y, int size, Color color)
{
    char line[256];
    const char *p = text;
    int row = 0;

    while (*p != '\0')
    {
        int n = 0;
        while (*p != '\0' && *p != '\n' && n < (int)sizeof(line) - 1)
        {
            line[n++] = *p++;
        }
        line[n] = '\0';

        DrawText(line, x, y + row * (size + 8), size, color);
        row++;

        if (*p == '\n') p++;     /* skip the newline character */
    }
}

/* Top bar drawn on every screen */
static void DrawHeader(void)
{
    const char *title = "THEME PARK MANAGEMENT";
    int width = MeasureText(title, 34);

    DrawRectangle(0, 0, SCREEN_WIDTH, 70, COL_PANEL);
    DrawText(title, SCREEN_WIDTH / 2 - width / 2, 18, 34, COL_GOLD);
}

/* The "< Back" button at the bottom-left. Returns true when clicked. */
static bool BackButton(void)
{
    return Button((Rectangle){ 30, SCREEN_HEIGHT - 70, 200, 45 }, "< Back");
}

/* Sets raygui colors once, so every button and text box matches the theme */
static void ApplyTheme(void)
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL,    ColorToInt(COL_ACCENT));
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL,  ColorToInt(COL_PANEL));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL,    ColorToInt(COL_LIGHT));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED,   ColorToInt(COL_GOLD));
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED,   ColorToInt(COL_TEXT));
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED,   ColorToInt(COL_TEAL));
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED,   ColorToInt(COL_TEXT));

    GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL,    ColorToInt(COL_LIGHT));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL,    ColorToInt(COL_TEXT));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL,  ColorToInt(COL_PANEL));
    GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED,   ColorToInt(RAYWHITE));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED,   ColorToInt(COL_TEXT));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, ColorToInt(COL_GOLD));
}

/* ------------------------------------------------------------------ */
/*  6. BUSINESS LOGIC (no drawing in these functions)                  */
/* ------------------------------------------------------------------ */

/* Maps an age to a ticket category. Returns -1 if the age is not valid. */
static int CategoryFromAge(int age)
{
    if (age < 1 || age > 100) return -1;
    if (age <= 3)  return CAT_TODDLER;
    if (age <= 12) return CAT_CHILD;
    if (age <= 59) return CAT_ADULT;
    return CAT_SENIOR;
}

/* Creates a ticket, adds it to the sales totals, and writes the ticket text into out. */
static void SellTicket(int age, bool multiDay, bool wristband, char *out, size_t size)
{
    int cat = CategoryFromAge(age);
    if (cat < 0)
    {
        snprintf(out, size, "Age must be between 1 and 100.");
        return;
    }

    int base      = categoryPrice[cat];
    int dayExtra  = multiDay ? base / 2 : 0;          /* multi-day = +50% */
    int bandFee   = wristband ? WRISTBAND_PRICE : 0;
    int total     = base + dayExtra + bandFee;

    ticketRevenue += total;
    ticketsSold++;
    ticketsByCategory[cat]++;

    snprintf(out, size,
             "Category        : %s\n"
             "Base price      : Rs.%d\n"
             "Multi-day extra : Rs.%d\n"
             "Wristband fee   : Rs.%d\n"
             "TOTAL           : Rs.%d\n\n"
             "Gate code: %c     Password: %d\n"
             "Tickets sold so far: %d",
             categoryName[cat], base, dayExtra, bandFee, total,
             categoryCode[cat], categoryPassword[cat], ticketsSold);
}

/* Checks a ticket at the gate using its code (T/C/A/S) and password. */
static void CheckGate(char code, int password)
{
    for (int i = 0; i < CAT_COUNT; i++)
    {
        if (categoryCode[i] == code)
        {
            if (password == categoryPassword[i])
            {
                snprintf(gateMsg, MSG_LEN, "ACCEPTED - %s ticket.\nWelcome to the park, enjoy!", categoryName[i]);
            }
            else
            {
                snprintf(gateMsg, MSG_LEN, "DENIED - wrong password for the %s ticket.", categoryName[i]);
            }
            return;
        }
    }
    snprintf(gateMsg, MSG_LEN, "Unknown ticket code. Use T, C, A or S.");
}

/* Metal detector + bag weight check. */
static void CheckSecurity(bool metalFound, float bagKg)
{
    snprintf(securityMsg, MSG_LEN, "%s\n%s",
             metalFound ? "Metal detected: hold the bag and the person for a manual check."
                        : "Metal detector: clear.",
             (bagKg <= BAG_LIMIT_KG) ? "Bag: 15 kg or less, you may enter with it."
                                     : "Bag: over 15 kg, manual check required.");
}

/* Rules for each ride. Returns true if allowed, otherwise fills reason. */
static bool RideAllowed(int ride, int ageYears, int ageMonths, int heightCm,
                        float weightKg, char *reason, size_t size)
{
    switch (ride)
    {
        case 0:  /* Infant ride */
            if (ageMonths <= 12 && weightKg <= 15.0f) return true;
            snprintf(reason, size, "Needs age 12 months or less and weight 15 kg or less.");
            return false;

        case 1:  /* Roller coaster */
        case 2:  /* Sky drop tower */
            if (ageYears >= 12 && heightCm >= 140) return true;
            snprintf(reason, size, "Needs age 12 or more and height 140 cm or more.");
            return false;

        case 3:  /* Giant wheel - open to everyone */
            return true;

        case 4:  /* Dodgem car */
            if (ageYears >= 5) return true;
            snprintf(reason, size, "Needs age 5 or more.");
            return false;

        default:
            snprintf(reason, size, "Unknown ride.");
            return false;
    }
}

/* Runs the ride check for the selected ride and updates the totals. */
static void TakeRide(void)
{
    if (selectedRide < 0)
    {
        snprintf(rideMsg, MSG_LEN, "Please choose a ride first.");
        return;
    }

    int  ageYears  = atoi(rideAgeText);
    int  ageMonths = atoi(rideMonthsText);
    int  heightCm  = atoi(rideHeightText);
    float weightKg = (float)atof(rideWeightText);

    if (selectedRide == 0 && rideMonthsText[0] == '\0')
    {
        snprintf(rideMsg, MSG_LEN, "Infant ride: please enter the age in months.");
        return;
    }

    char reason[160];
    if (!RideAllowed(selectedRide, ageYears, ageMonths, heightCm, weightKg, reason, sizeof reason))
    {
        snprintf(rideMsg, MSG_LEN, "NOT ALLOWED on the %s.\n%s", rideName[selectedRide], reason);
        return;
    }

    /* Wristband holders get the first ride of each type free */
    bool useFree = wristbandOn && !freeRideUsed[selectedRide];
    int  price   = useFree ? 0 : ridePrice[selectedRide];
    if (useFree) freeRideUsed[selectedRide] = true;

    rideRevenue += price;
    ridesTaken++;

    snprintf(rideMsg, MSG_LEN, "Enjoy the %s!\nPrice: Rs.%d%s",
             rideName[selectedRide], price,
             useFree ? "\n(Free: wristband perk for this ride type)" : "");
}

/* Adds the selected food item to the order totals. */
static void AddFoodToOrder(void)
{
    if (selectedFood < 0)
    {
        snprintf(foodMsg, MSG_LEN, "Please select an item first.");
        return;
    }

    int qty = atoi(foodQtyText);
    if (qty < 1 || qty > 50)
    {
        snprintf(foodMsg, MSG_LEN, "Quantity must be between 1 and 50.");
        return;
    }

    int cost = qty * foodPrice[selectedFood];
    foodRevenue += cost;
    snprintf(foodMsg, MSG_LEN, "Added %d x %s\nSubtotal: Rs.%d",
             qty, foodName[selectedFood], cost);
}

/* Builds the daily revenue report text. */
static void BuildReport(char *buf, size_t size)
{
    int total = ticketRevenue + rideRevenue + foodRevenue;

    snprintf(buf, size,
             "=========== DAILY REVENUE REPORT ===========\n\n"
             "Tickets sold         : %d\n"
             "   Toddler: %d   Child: %d   Adult: %d   Senior: %d\n"
             "Ticket revenue       : Rs.%d\n\n"
             "Rides taken          : %d\n"
             "Ride revenue         : Rs.%d\n\n"
             "Food revenue         : Rs.%d\n"
             "Visitors registered  : %d\n\n"
             "TOTAL REVENUE        : Rs.%d",
             ticketsSold,
             ticketsByCategory[CAT_TODDLER], ticketsByCategory[CAT_CHILD],
             ticketsByCategory[CAT_ADULT], ticketsByCategory[CAT_SENIOR],
             ticketRevenue,
             ridesTaken, rideRevenue,
             foodRevenue, visitorCount, total);
}

/* Fire check: smoke above the limit activates the evacuation plan. */
static void CheckFire(void)
{
    int smoke = atoi(smokeText);

    if (smoke > SMOKE_LIMIT)
    {
        evacuationActive = true;
        snprintf(safetyMsg, MSG_LEN,
                 "FIRE DETECTED (smoke level %d)!\nEvacuation plan ACTIVATED.\nEmergency team notified.",
                 smoke);
    }
    else
    {
        snprintf(safetyMsg, MSG_LEN, "No fire detected (smoke level %d).\nEverything is safe.", smoke);
    }
}

/* Lists the state of all emergency exits into safetyMsg. */
static void CheckAllExits(void)
{
    char line[64];
    safetyMsg[0] = '\0';

    for (int i = 0; i < EXIT_COUNT; i++)
    {
        snprintf(line, sizeof line, "Exit %d: %s\n", i + 1, exitOpen[i] ? "OPEN, lights ON" : "BLOCKED");
        strncat(safetyMsg, line, MSG_LEN - strlen(safetyMsg) - 1);
    }
}

/* Validates and saves a new visitor from the form. */
static void SaveVisitor(void)
{
    if (visitorCount >= MAX_VISITORS)
    {
        snprintf(visitorMsg, MSG_LEN, "Visitor list is full (max %d).", MAX_VISITORS);
        return;
    }

    char gender = (char)toupper((unsigned char)vGenderText[0]);
    char wrist  = (char)toupper((unsigned char)vWristText[0]);
    int  age    = atoi(vAgeText);
    int  days   = atoi(vDaysText);

    if (vNameText[0] == '\0')
    {
        snprintf(visitorMsg, MSG_LEN, "Please enter the visitor's name.");
        return;
    }
    if (gender != 'M' && gender != 'F')
    {
        snprintf(visitorMsg, MSG_LEN, "Gender must be M or F.");
        return;
    }
    if (age < 1 || age > 100)
    {
        snprintf(visitorMsg, MSG_LEN, "Age must be between 1 and 100.");
        return;
    }
    if (days < 1 || days > 365)
    {
        snprintf(visitorMsg, MSG_LEN, "Validity must be between 1 and 365 days.");
        return;
    }
    if (wrist != 'Y' && wrist != 'N')
    {
        snprintf(visitorMsg, MSG_LEN, "Wristband must be Y or N.");
        return;
    }

    Visitor *v = &visitors[visitorCount];
    snprintf(v->name, NAME_LEN, "%s", vNameText);
    v->gender       = gender;
    v->age          = age;
    v->validityDays = days;
    v->wristband    = (wrist == 'Y');
    v->ticketNumber = 1000 + visitorCount + 1;
    visitorCount++;

    snprintf(visitorMsg, MSG_LEN, "Visitor saved!\nTicket number: %d", v->ticketNumber);

    /* Clear the form for the next visitor */
    vNameText[0]   = '\0';
    vGenderText[0] = '\0';
    vAgeText[0]    = '\0';
    snprintf(vDaysText, sizeof vDaysText, "1");
    snprintf(vWristText, sizeof vWristText, "N");
}

/* ------------------------------------------------------------------ */
/*  7. SCREENS (each function draws one screen and handles its buttons) */
/* ------------------------------------------------------------------ */

/* ---------- HOME ---------- */
static void DrawHome(void)
{
    const char *welcome = "Welcome to the Theme Park!";
    int width = MeasureText(welcome, 40);
    DrawText(welcome, SCREEN_WIDTH / 2 - width / 2, 120, 40, COL_ACCENT);

    int x = SCREEN_WIDTH / 2 - 160;

    if (Button((Rectangle){ x, 230, 320, 55 }, "Buy Ticket"))
        GoTo(SCREEN_TICKET_COUNTER);

    if (Button((Rectangle){ x, 300, 320, 55 }, "Enter Park (Gate Scan)"))
        GoTo(SCREEN_GATE);

    if (Button((Rectangle){ x, 370, 320, 55 }, "Visitor Menu"))
        GoTo(SCREEN_VISITOR_MENU);

    if (Button((Rectangle){ x, 440, 320, 55 }, "Staff & Admin"))
        GoTo(adminLoggedIn ? SCREEN_ADMIN_MENU : SCREEN_ADMIN_LOGIN);

    if (Button((Rectangle){ x, 510, 320, 55 }, "Exit Park"))
        exitRequested = true;
}

/* ---------- TICKET COUNTER ---------- */
static void OnGenerateTicket(void)
{
    int age   = atoi(ageText);
    int days  = atoi(daysText);
    int wrist = atoi(wristText);

    if (age < 1 || age > 100)
    {
        snprintf(ticketMsg, MSG_LEN, "Age must be between 1 and 100.");
        return;
    }
    if (days != 1 && days != 2)
    {
        snprintf(ticketMsg, MSG_LEN, "Days must be 1 (day pass) or 2 (multi-day).");
        return;
    }
    if (wrist != 0 && wrist != 1)
    {
        snprintf(ticketMsg, MSG_LEN, "Wristband must be 1 (yes) or 0 (no).");
        return;
    }

    SellTicket(age, days == 2, wrist == 1, ticketMsg, MSG_LEN);
}

static void DrawTicketCounter(void)
{
    DrawScreenTitle("TICKET COUNTER");

    /* Input panel */
    DrawPanel((Rectangle){ 40, 150, 520, 380 }, "Visitor Details");
    PanelText(70, 205, "Age (1-100):");
    TextField((Rectangle){ 330, 200, 150, FIELD_H }, ageText, sizeof ageText, FIELD_AGE);

    PanelText(70, 255, "Days (1 or 2):");
    TextField((Rectangle){ 330, 250, 150, FIELD_H }, daysText, sizeof daysText, FIELD_DAYS);

    PanelText(70, 305, "Wristband (1=yes, 0=no):");
    TextField((Rectangle){ 330, 300, 150, FIELD_H }, wristText, sizeof wristText, FIELD_WRISTBAND);

    if (Button((Rectangle){ 330, 360, 220, 50 }, "Generate Ticket"))
        OnGenerateTicket();

    /* Ticket output panel */
    DrawPanel((Rectangle){ 590, 150, 650, 380 }, "Your Ticket");
    DrawMultiline(ticketMsg, 615, 200, 20, COL_LIGHT);

    if (BackButton()) GoTo(SCREEN_HOME);
}

/* ---------- GATE (ticket scan + security) ---------- */
static void DrawGate(void)
{
    DrawScreenTitle("GATE: TICKET & SECURITY CHECK");

    /* Ticket scan section */
    DrawPanel((Rectangle){ 40, 150, 540, 220 }, "Ticket Scan");
    PanelText(70, 205, "Ticket code (T/C/A/S):");
    TextField((Rectangle){ 400, 200, 120, FIELD_H }, gateCodeText, sizeof gateCodeText, FIELD_GATE_CODE);

    PanelText(70, 255, "Ticket password:");
    TextField((Rectangle){ 400, 250, 180, FIELD_H }, gatePassText, sizeof gatePassText, FIELD_GATE_PASS);

    if (Button((Rectangle){ 400, 300, 200, 45 }, "Scan Ticket"))
    {
        char code = (char)toupper((unsigned char)gateCodeText[0]);
        CheckGate(code, atoi(gatePassText));
    }

    /* Security section */
    DrawPanel((Rectangle){ 40, 390, 540, 320 }, "Security Check");
    PanelText(70, 445, "Metal detected (1=yes, 0=no):");
    TextField((Rectangle){ 400, 440, 120, FIELD_H }, metalText, sizeof metalText, FIELD_METAL);

    PanelText(70, 495, "Bag weight (kg):");
    TextField((Rectangle){ 400, 490, 120, FIELD_H }, bagText, sizeof bagText, FIELD_BAG);

    if (Button((Rectangle){ 400, 540, 200, 45 }, "Check Security"))
    {
        CheckSecurity(atoi(metalText) == 1, (float)atof(bagText));
    }

    /* Output panel */
    DrawPanel((Rectangle){ 610, 150, 630, 560 }, "Gate Output");
    DrawText("Gate result", 640, 200, 22, COL_GOLD);
    DrawMultiline(gateMsg, 640, 235, 20, COL_LIGHT);

    DrawText("Security result", 640, 420, 22, COL_GOLD);
    DrawMultiline(securityMsg, 640, 455, 20, COL_LIGHT);

    if (BackButton()) GoTo(SCREEN_HOME);
}

/* ---------- VISITOR MENU ---------- */
static void DrawVisitorMenu(void)
{
    DrawScreenTitle("VISITOR MENU");

    int x = SCREEN_WIDTH / 2 - 160;
    if (Button((Rectangle){ x, 200, 320, 55 }, "Food Court"))           GoTo(SCREEN_FOOD_COURT);
    if (Button((Rectangle){ x, 270, 320, 55 }, "Rides"))                GoTo(SCREEN_RIDES);
    if (Button((Rectangle){ x, 340, 320, 55 }, "Employee Information")) GoTo(SCREEN_EMPLOYEES);

    if (BackButton()) GoTo(SCREEN_HOME);
}

/* ---------- FOOD COURT ---------- */
static void DrawFoodCourt(void)
{
    DrawScreenTitle("FOOD COURT");

    /* Food column (items 0-6) */
    DrawText("FOOD", 60, 150, 26, COL_ACCENT);
    for (int i = 0; i < 7; i++)
    {
        char label[80];
        snprintf(label, sizeof label, "%s%s  (Rs.%d)", selectedFood == i ? "> " : "", foodName[i], foodPrice[i]);
        if (Button((Rectangle){ 60, 190 + i * 48, 330, 40 }, label))
            selectedFood = i;
    }

    /* Drinks column (items 7-10) */
    DrawText("DRINKS", 420, 150, 26, COL_ACCENT);
    for (int i = 7; i < FOOD_COUNT; i++)
    {
        char label[80];
        snprintf(label, sizeof label, "%s%s  (Rs.%d)", selectedFood == i ? "> " : "", foodName[i], foodPrice[i]);
        if (Button((Rectangle){ 420, 190 + (i - 7) * 48, 330, 40 }, label))
            selectedFood = i;
    }

    /* Order panel */
    DrawPanel((Rectangle){ 800, 150, 440, 260 }, "Your Order");
    PanelText(820, 205, "Selected:");
    DrawText(selectedFood >= 0 ? foodName[selectedFood] : "nothing yet", 940, 205, 20, COL_GOLD);

    PanelText(820, 255, "Quantity:");
    TextField((Rectangle){ 960, 250, 120, FIELD_H }, foodQtyText, sizeof foodQtyText, FIELD_FOOD_QTY);

    if (Button((Rectangle){ 820, 310, 260, 45 }, "Add to Order"))
        AddFoodToOrder();

    /* Message + total panel */
    DrawPanel((Rectangle){ 800, 430, 440, 280 }, "Order Summary");
    DrawMultiline(foodMsg, 820, 480, 20, COL_LIGHT);

    char total[80];
    snprintf(total, sizeof total, "Total food sales: Rs.%d", foodRevenue);
    DrawText(total, 820, 655, 22, COL_GOLD);

    if (BackButton()) GoTo(SCREEN_VISITOR_MENU);
}

/* ---------- RIDES ---------- */
static void DrawRides(void)
{
    DrawScreenTitle("RIDES");

    DrawText("Choose a ride:", 60, 150, 24, COL_ACCENT);
    for (int i = 0; i < RIDE_COUNT; i++)
    {
        char label[80];
        snprintf(label, sizeof label, "%s%s  (Rs.%d)", selectedRide == i ? "> " : "", rideName[i], ridePrice[i]);
        if (Button((Rectangle){ 60, 190 + i * 50, 340, 42 }, label))
            selectedRide = i;
    }

    if (Button((Rectangle){ 60, 460, 340, 42 }, wristbandOn ? "Wristband: YES" : "Wristband: NO"))
        wristbandOn = !wristbandOn;
    DrawText("(Wristband = first ride of each type is free)", 60, 510, 16, COL_TEXT);

    /* Visitor details panel */
    DrawPanel((Rectangle){ 460, 150, 780, 230 }, "Visitor Details");
    PanelText(490, 195, "Age (years):");
    TextField((Rectangle){ 820, 190, 140, FIELD_H }, rideAgeText, sizeof rideAgeText, FIELD_RIDE_AGE);

    PanelText(490, 240, "Age in months (infant ride only):");
    TextField((Rectangle){ 820, 235, 140, FIELD_H }, rideMonthsText, sizeof rideMonthsText, FIELD_RIDE_MONTHS);

    PanelText(490, 285, "Height (cm):");
    TextField((Rectangle){ 820, 280, 140, FIELD_H }, rideHeightText, sizeof rideHeightText, FIELD_RIDE_HEIGHT);

    PanelText(490, 330, "Weight (kg):");
    TextField((Rectangle){ 820, 325, 140, FIELD_H }, rideWeightText, sizeof rideWeightText, FIELD_RIDE_WEIGHT);

    if (Button((Rectangle){ 490, 400, 240, 45 }, "Take Selected Ride"))
        TakeRide();

    /* Result panel */
    DrawPanel((Rectangle){ 460, 470, 780, 240 }, "Ride Result");
    DrawMultiline(rideMsg, 490, 520, 20, COL_LIGHT);

    char summary[120];
    snprintf(summary, sizeof summary, "Rides taken: %d   |   Ride revenue: Rs.%d", ridesTaken, rideRevenue);
    DrawText(summary, 490, 660, 22, COL_GOLD);

    if (BackButton()) GoTo(SCREEN_VISITOR_MENU);
}

/* ---------- EMPLOYEES ---------- */
static void DrawEmployees(void)
{
    DrawScreenTitle("EMPLOYEE INFORMATION");

    DrawPanel((Rectangle){ 40, 150, 1200, 480 }, NULL);

    /* Table header */
    DrawText("ID",          70,  165, 22, COL_GOLD);
    DrawText("Name",        170, 165, 22, COL_GOLD);
    DrawText("Age",         360, 165, 22, COL_GOLD);
    DrawText("Salary (Rs)", 440, 165, 22, COL_GOLD);
    DrawText("Duty",        640, 165, 22, COL_GOLD);
    DrawLine(60, 200, 1220, 200, COL_GOLD);

    /* Table rows */
    for (int i = 0; i < STAFF_COUNT; i++)
    {
        int y = 212 + i * 26;
        DrawText(TextFormat("%d", staff[i].id),     70,  y, 19, COL_LIGHT);
        DrawText(staff[i].name,                     170, y, 19, COL_LIGHT);
        DrawText(TextFormat("%d", staff[i].age),    360, y, 19, COL_LIGHT);
        DrawText(TextFormat("%d", staff[i].salary), 440, y, 19, COL_LIGHT);
        DrawText(staff[i].duty,                     640, y, 19, COL_LIGHT);
    }

    DrawText("Job applications: sorry, all vacancies are currently filled.", 40, 655, 22, COL_ACCENT);

    if (BackButton()) GoTo(SCREEN_VISITOR_MENU);
}

/* ---------- ADMIN LOGIN ---------- */
static void OnLogin(void)
{
    if (strcmp(loginText, ADMIN_PASSWORD) == 0)
    {
        adminLoggedIn = true;
        loginText[0] = '\0';
        loginMsg[0]  = '\0';
        GoTo(SCREEN_ADMIN_MENU);
    }
    else
    {
        snprintf(loginMsg, MSG_LEN, "Wrong password. Try again.");
    }
}

static void DrawAdminLogin(void)
{
    DrawScreenTitle("STAFF & ADMIN LOGIN");

    DrawPanel((Rectangle){ 40, 150, 520, 300 }, "Admin Login");
    PanelText(70, 205, "Admin password:");
    TextField((Rectangle){ 70, 240, 300, FIELD_H }, loginText, sizeof loginText, FIELD_LOGIN);

    if (Button((Rectangle){ 70, 300, 300, 45 }, "Login"))
        OnLogin();

    DrawMultiline(loginMsg, 70, 370, 20, COL_GOLD);

    if (BackButton()) GoTo(SCREEN_HOME);
}

/* ---------- ADMIN DASHBOARD ---------- */
static void DrawAdminMenu(void)
{
    DrawScreenTitle("ADMIN DASHBOARD");

    int x = SCREEN_WIDTH / 2 - 180;
    if (Button((Rectangle){ x, 200, 360, 55 }, "Safety & Emergency Systems"))
        GoTo(SCREEN_SAFETY);

    if (Button((Rectangle){ x, 270, 360, 55 }, "Daily Revenue Report"))
    {
        BuildReport(reportText, sizeof reportText);
        GoTo(SCREEN_REPORT);
    }

    if (Button((Rectangle){ x, 340, 360, 55 }, "Visitor Records"))
        GoTo(SCREEN_VISITOR_RECORDS);

    if (Button((Rectangle){ x, 410, 360, 55 }, "Log Out"))
    {
        adminLoggedIn = false;
        GoTo(SCREEN_HOME);
    }

    if (BackButton()) GoTo(SCREEN_HOME);
}

/* ---------- SAFETY & EMERGENCY ---------- */
static void DrawSafety(void)
{
    DrawScreenTitle("SAFETY & EMERGENCY");

    static const char *modeLabel[] = {
        "", "1. Fire alarm check", "2. Evacuation plan",
        "3. Emergency exits", "4. Public announcement", "5. Medical help"
    };

    /* Left menu: choose which safety tool to use */
    for (int i = 1; i <= 5; i++)
    {
        char label[64];
        snprintf(label, sizeof label, "%s%s", safetyMode == i ? "> " : "", modeLabel[i]);
        if (Button((Rectangle){ 40, 150 + (i - 1) * 56, 300, 45 }, label))
        {
            safetyMode = i;
            safetyMsg[0] = '\0';
        }
    }

    /* Right control panel */
    DrawPanel((Rectangle){ 370, 150, 870, 520 }, "Control Panel");

    switch (safetyMode)
    {
        case 1: /* Fire alarm */
            PanelText(400, 205, "Smoke level (0-100):");
            TextField((Rectangle){ 720, 200, 150, FIELD_H }, smokeText, sizeof smokeText, FIELD_SMOKE);
            if (Button((Rectangle){ 400, 255, 220, 45 }, "Check Fire"))
                CheckFire();
            break;

        case 2: /* Evacuation */
            PanelText(400, 205, evacuationActive ? "Status: EVACUATION ACTIVE" : "Status: on standby");
            if (Button((Rectangle){ 400, 255, 320, 45 }, "Activate evacuation plan"))
            {
                evacuationActive = true;
                snprintf(safetyMsg, MSG_LEN, "Evacuation plan ACTIVATED.\nFollow the nearest exit signs.\nStaff guide visitors to the exits.");
            }
            if (Button((Rectangle){ 400, 315, 320, 45 }, "Put plan on standby"))
            {
                evacuationActive = false;
                snprintf(safetyMsg, MSG_LEN, "Evacuation plan is on standby.");
            }
            break;

        case 3: /* Exits */
            for (int i = 0; i < EXIT_COUNT; i++)
            {
                char label[40];
                snprintf(label, sizeof label, "Exit %d: %s", i + 1, exitOpen[i] ? "OPEN" : "BLOCKED");
                if (Button((Rectangle){ 400, 210 + i * 55, 320, 45 }, label))
                    exitOpen[i] = !exitOpen[i];
            }
            if (Button((Rectangle){ 400, 390, 320, 45 }, "Check all exits"))
                CheckAllExits();
            break;

        case 4: /* Public announcement */
            if (Button((Rectangle){ 400, 210, 340, 45 }, "Emergency announcement"))
                snprintf(safetyMsg, MSG_LEN, "Please do not panic. Stay calm and walk calmly to the nearest exit.");
            if (Button((Rectangle){ 400, 265, 340, 45 }, "Normal announcement"))
                snprintf(safetyMsg, MSG_LEN, "Welcome to the park! Enjoy your day.");
            break;

        case 5: /* Medical */
            if (Button((Rectangle){ 400, 210, 340, 45 }, "Report an injury"))
                snprintf(safetyMsg, MSG_LEN, "Emergency case registered.\nNearest medical helper has been assigned.");
            if (Button((Rectangle){ 400, 265, 340, 45 }, "No injuries - everyone is fine"))
                snprintf(safetyMsg, MSG_LEN, "Everyone is fine. Medical team is on standby.");
            break;

        default:
            break;
    }

    DrawText("System output:", 400, 460, 22, COL_GOLD);
    DrawMultiline(safetyMsg, 400, 495, 20, COL_LIGHT);

    if (BackButton()) GoTo(SCREEN_ADMIN_MENU);
}

/* ---------- DAILY REPORT ---------- */
static void DrawReport(void)
{
    DrawScreenTitle("DAILY REVENUE REPORT");

    DrawPanel((Rectangle){ 40, 150, 1200, 520 }, NULL);
    DrawMultiline(reportText, 70, 185, 22, COL_LIGHT);

    if (BackButton()) GoTo(SCREEN_ADMIN_MENU);
}

/* ---------- VISITOR RECORDS (admin) ---------- */
static void DrawVisitorRecords(void)
{
    DrawScreenTitle("VISITOR RECORDS");

    /* Add-visitor form */
    DrawPanel((Rectangle){ 40, 150, 440, 520 }, "Add New Visitor");

    PanelText(60, 205, "Name:");
    TextField((Rectangle){ 270, 200, 190, FIELD_H }, vNameText, sizeof vNameText, FIELD_V_NAME);

    PanelText(60, 255, "Gender (M/F):");
    TextField((Rectangle){ 270, 250, 80, FIELD_H }, vGenderText, sizeof vGenderText, FIELD_V_GENDER);

    PanelText(60, 305, "Age (1-100):");
    TextField((Rectangle){ 270, 300, 80, FIELD_H }, vAgeText, sizeof vAgeText, FIELD_V_AGE);

    PanelText(60, 355, "Validity (days):");
    TextField((Rectangle){ 270, 350, 80, FIELD_H }, vDaysText, sizeof vDaysText, FIELD_V_DAYS);

    PanelText(60, 405, "Wristband (Y/N):");
    TextField((Rectangle){ 270, 400, 80, FIELD_H }, vWristText, sizeof vWristText, FIELD_V_WRIST);

    if (Button((Rectangle){ 60, 460, 200, 45 }, "Save Visitor"))
        SaveVisitor();

    DrawMultiline(visitorMsg, 60, 530, 20, COL_GOLD);

    /* Visitor list panel (paged) */
    DrawPanel((Rectangle){ 500, 150, 740, 520 }, "All Visitors");

    int pages = (visitorCount == 0) ? 1 : (visitorCount + VISITORS_PER_PAGE - 1) / VISITORS_PER_PAGE;
    if (visitorPage >= pages) visitorPage = pages - 1;

    DrawText("#",      520,  195, 20, COL_GOLD);
    DrawText("Ticket", 565,  195, 20, COL_GOLD);
    DrawText("Name",   655,  195, 20, COL_GOLD);
    DrawText("G",      880,  195, 20, COL_GOLD);
    DrawText("Age",    925,  195, 20, COL_GOLD);
    DrawText("Days",   990,  195, 20, COL_GOLD);
    DrawText("Band",   1080, 195, 20, COL_GOLD);
    DrawLine(515, 225, 1220, 225, COL_GOLD);

    if (visitorCount == 0)
    {
        DrawText("No visitors saved yet.", 520, 240, 20, COL_LIGHT);
    }

    int start = visitorPage * VISITORS_PER_PAGE;
    for (int i = 0; i < VISITORS_PER_PAGE && start + i < visitorCount; i++)
    {
        Visitor *v = &visitors[start + i];
        int y = 235 + i * 22;

        char shortName[24];
        snprintf(shortName, sizeof shortName, "%.22s", v->name);
        char gender[2] = { v->gender, '\0' };

        DrawText(TextFormat("%d", start + i + 1),   520,  y, 18, COL_LIGHT);
        DrawText(TextFormat("%d", v->ticketNumber), 565,  y, 18, COL_LIGHT);
        DrawText(shortName,                         655,  y, 18, COL_LIGHT);
        DrawText(gender,                            880,  y, 18, COL_LIGHT);
        DrawText(TextFormat("%d", v->age),          925,  y, 18, COL_LIGHT);
        DrawText(TextFormat("%d", v->validityDays), 990,  y, 18, COL_LIGHT);
        DrawText(v->wristband ? "Yes" : "No",       1080, y, 18, COL_LIGHT);
    }

    /* Page buttons */
    if (Button((Rectangle){ 520, 628, 120, 36 }, "< Prev") && visitorPage > 0)
        visitorPage--;

    if (Button((Rectangle){ 1080, 628, 120, 36 }, "Next >") && visitorPage < pages - 1)
        visitorPage++;

    char pageText[32];
    snprintf(pageText, sizeof pageText, "Page %d of %d", visitorPage + 1, pages);
    DrawText(pageText, 720, 638, 20, COL_LIGHT);

    if (BackButton()) GoTo(SCREEN_ADMIN_MENU);
}

/* ------------------------------------------------------------------ */
/*  8. ASSETS AND MAIN LOOP                                            */
/* ------------------------------------------------------------------ */

static void LoadAssets(void)
{
    if (FileExists("bgsound.mp3"))
    {
        bgMusic  = LoadMusicStream("bgsound.mp3");
        hasMusic = IsMusicValid(bgMusic);
        if (hasMusic)
        {
            SetMusicVolume(bgMusic, 0.4f);
            PlayMusicStream(bgMusic);
        }
    }

    if (FileExists("click.wav"))
    {
        clickSound = LoadSound("click.wav");
        hasSound   = IsSoundValid(clickSound);
    }
}

static void UnloadAssets(void)
{
    if (hasMusic) UnloadMusicStream(bgMusic);
    if (hasSound) UnloadSound(clickSound);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Theme Park Management System");
    InitAudioDevice();
    SetTargetFPS(60);

    LoadAssets();
    ApplyTheme();

    while (!WindowShouldClose() && !exitRequested)
    {
        if (hasMusic) UpdateMusicStream(bgMusic);

        /* Clicking anywhere first un-focuses text boxes.
           A click on a text box then focuses it again while drawing. */
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            focusedField = FIELD_NONE;
        }

        BeginDrawing();
        ClearBackground(COL_BG);
        DrawHeader();

        switch (currentScreen)
        {
            case SCREEN_HOME:            DrawHome();            break;
            case SCREEN_TICKET_COUNTER:  DrawTicketCounter();   break;
            case SCREEN_GATE:            DrawGate();            break;
            case SCREEN_VISITOR_MENU:    DrawVisitorMenu();     break;
            case SCREEN_FOOD_COURT:      DrawFoodCourt();       break;
            case SCREEN_RIDES:           DrawRides();           break;
            case SCREEN_EMPLOYEES:       DrawEmployees();       break;
            case SCREEN_ADMIN_LOGIN:     DrawAdminLogin();      break;
            case SCREEN_ADMIN_MENU:      DrawAdminMenu();       break;
            case SCREEN_SAFETY:          DrawSafety();          break;
            case SCREEN_REPORT:          DrawReport();          break;
            case SCREEN_VISITOR_RECORDS: DrawVisitorRecords();  break;
        }

        EndDrawing();
    }

    UnloadAssets();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
