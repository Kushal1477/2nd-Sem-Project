#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <limits>
#include <sstream>
#include <windows.h>

using namespace std;

// ============================================================
// COLORS
// ============================================================

#define RESET     "\033[0m"
#define RED       "\033[31m"
#define GREEN     "\033[32m"
#define YELLOW    "\033[33m"
#define BLUE      "\033[34m"
#define MAGENTA   "\033[35m"
#define CYAN      "\033[36m"
#define WHITE     "\033[37m"
#define BOLD      "\033[1m"
#define BRIGHTRED "\033[91m"
#define BRIGHTGREEN "\033[92m"
#define BRIGHTYELLOW "\033[93m"
#define BRIGHTCYAN "\033[96m"

void showClock();

void enableAnsiColors()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;

    if(GetConsoleMode(hOut, &mode))
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

void hideCursor()
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;

    GetConsoleCursorInfo(console, &info);
    info.bVisible = FALSE;
    SetConsoleCursorInfo(console, &info);
}

void showCursor()
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;

    GetConsoleCursorInfo(console, &info);
    info.bVisible = TRUE;
    SetConsoleCursorInfo(console, &info);
}

// ============================================================
// CONSTANTS
// ============================================================

const int SCREEN_WIDTH = 80;

// Left margin used for all input prompts (label on the left, cursor right after it)
const int LEFT_MARGIN = 10;

// Left space for detail lines and slot grids (every line of one block starts at the same column)
const int DETAIL_INDENT = 20;

// Left space of the parking slot grid (10 slots per row are centered)
const int GRID_INDENT = 15;

const int MAX_BIKES = 30;
const int MAX_CARS = 20;
const int MAX_HEAVY = 5;

const string ADMIN_FILE = "admin.txt";
const string DATA_FILE = "parking_data.txt";
const string LOG_FILE = "parking_log.txt";

// ============================================================
// CENTERED OUTPUT FUNCTIONS
// ============================================================

void clearScreen()
{
    system("cls");
}

// ------------------------------------------------------------
// Print centered normal text
// ------------------------------------------------------------

void printCentered(string text)
{
    int spaces = (SCREEN_WIDTH - (int)text.length()) / 2;

    if(spaces < 0)
        spaces = 0;

    cout << string(spaces, ' ') << WHITE << text << RESET << endl;
}

// ------------------------------------------------------------
// Print centered colored text
// ------------------------------------------------------------

void printCenteredColor(string text, string color)
{
    int spaces = (SCREEN_WIDTH - (int)text.length()) / 2;

    if(spaces < 0)
        spaces = 0;

    cout << string(spaces, ' ')
         << color
         << text
         << RESET
         << endl;
}

// ------------------------------------------------------------
// Print centered bold colored text
// ------------------------------------------------------------

void printCenteredBold(string text, string color)
{
    int spaces = (SCREEN_WIDTH - (int)text.length()) / 2;

    if(spaces < 0)
        spaces = 0;

    cout << string(spaces, ' ')
         << color
         << BOLD
         << text
         << RESET
         << endl;
}

// ------------------------------------------------------------
// Print colored prompt on the left side (no new line, so the
// cursor stays right after the text)
// ------------------------------------------------------------

void printLeftPrompt(string text, string color)
{
    cout << string(LEFT_MARGIN, ' ')
         << color
         << text
         << RESET;
}

// ------------------------------------------------------------
// Print detail lines (Label : Value) on the left side.
// All lines start at the same column, so the colons line up.
// ------------------------------------------------------------

void printDetail(string text)
{
    cout << string(DETAIL_INDENT, ' ')
         << WHITE
         << text
         << RESET
         << endl;
}

void printDetailColor(string text, string color)
{
    cout << string(DETAIL_INDENT, ' ')
         << color
         << text
         << RESET
         << endl;
}

void printDetailBold(string text, string color)
{
    cout << string(DETAIL_INDENT, ' ')
         << color
         << BOLD
         << text
         << RESET
         << endl;
}

// ------------------------------------------------------------
// Centered line
// ------------------------------------------------------------

void line()
{
    string text = "============================================================";
    printCenteredColor(text, CYAN);
}

// ------------------------------------------------------------
// Centered small line
// ------------------------------------------------------------

void smallLine()
{
    string text = "------------------------------------------------------------";
    printCenteredColor(text, CYAN);
}

// ============================================================
// CENTERED HEADER
// ============================================================

void header(string title)
{
    clearScreen();

    line();

    printCenteredBold(
        "SMART PARKING LOT SYSTEM",
        YELLOW
    );

    line();

    printCenteredBold(
        title,
        GREEN
    );

    showClock();
    line();
}

// ============================================================
// WAIT FOR KEY
// ============================================================

void waitForKey()
{
    cout << endl;

    printCenteredColor(
        "Press any key to continue...",
        CYAN
    );

    _getch();
}

// ============================================================
// CONVERT STRING TO UPPERCASE
// ============================================================

string toUpper(string text)
{
    for(int i = 0; i < (int)text.length(); i++)
    {
        if(text[i] >= 'a' && text[i] <= 'z')
        {
            text[i] = text[i] - 32;
        }
    }

    return text;
}

// ============================================================
// CLEAR INVALID INPUT
// ============================================================

void clearInput()
{
    cin.clear();

    cin.ignore(
        numeric_limits<streamsize>::max(),
        '\n'
    );
}

// ============================================================
// GET INTEGER INPUT SAFELY
// ============================================================

int getInteger(string message)
{
    int value;

    while(true)
    {
        printLeftPrompt(message + " ", YELLOW);

        showCursor();
        cin >> value;
        hideCursor();

        if(cin.fail())
        {
            clearInput();

            printCenteredColor(
                "Invalid input! Please enter a number.",
                RED
            );

            cout << endl;
        }
        else
        {
            clearInput();
            return value;
        }
    }
}

// ============================================================
// YES / NO FUNCTION
// ============================================================

bool confirm(string message)//bool is used  to check whether it is true or false
{
    char choice;

    while(true)
    {
        printLeftPrompt(
            message + " (Y/N) : ",
            YELLOW
        );

        showCursor();
        cin >> choice;
        hideCursor();

        clearInput();

        if(choice == 'Y' || choice == 'y')
        {
            return true;
        }

        if(choice == 'N' || choice == 'n')
        {
            return false;
        }

        printCenteredColor(
            "Please enter Y or N.",
            RED
        );
    }
}

// ============================================================
// VEHICLE NUMBER VALIDATION
// Required format : BA-X-PA-XXXX   (example: BA-1-PA-1234)
//
//   Part 1 : 2 letters          (zone code,     example BA)
//   Part 2 : 1 or 2 digits      (lot number,    example 1)
//   Part 3 : 2 or 3 letters     (vehicle class, example PA)
//   Part 4 : exactly 4 digits   (number,        example 1234)
//
// Parts are separated by hyphen (-).
// The number can also be typed without hyphens (BA2PA3983).
// It is converted to the standard form BA-2-PA-3983, so both
// ways of typing give the same vehicle number.
// Spaces, @, #, $ and all other symbols are NOT allowed.
// ============================================================

// ------------------------------------------------------------
// Check that text contains only letters A-Z
// ------------------------------------------------------------

bool onlyLetters(string text)
{
    if(text.length() == 0)
    {
        return false;
    }

    for(int i = 0; i < (int)text.length(); i++)
    {
        if(text[i] < 'A' || text[i] > 'Z')
        {
            return false;
        }
    }

    return true;
}

// ------------------------------------------------------------
// Check that text contains only digits 0-9
// ------------------------------------------------------------

bool onlyDigits(string text)
{
    if(text.length() == 0)
    {
        return false;
    }

    for(int i = 0; i < (int)text.length(); i++)
    {
        if(text[i] < '0' || text[i] > '9')
        {
            return false;
        }
    }

    return true;
}

// ------------------------------------------------------------
// Convert BA2PA3983 into BA-2-PA-3983 (text must be UPPERCASE)
// A number that already has hyphens is returned unchanged.
// Anything that does not fit is returned unchanged too,
// so isValidVehicleNumber() can reject it.
// ------------------------------------------------------------

string makeStandardNumber(string number)
{
    // Already has hyphens : nothing to change

    for(int i = 0; i < (int)number.length(); i++)
    {
        if(number[i] == '-')
        {
            return number;
        }
    }

    string result = "";
    int groupCount = 0;

    // L = letter group, D = digit group

    char lastType = ' ';

    for(int i = 0; i < (int)number.length(); i++)
    {
        char type;

        if(number[i] >= 'A' && number[i] <= 'Z')
        {
            type = 'L';
        }
        else if(number[i] >= '0' && number[i] <= '9')
        {
            type = 'D';
        }
        else
        {
            return number;
        }

        // A new group starts when letters change to digits
        // (or digits change to letters)

        if(type != lastType)
        {
            groupCount++;

            if(groupCount > 4)
            {
                return number;
            }

            if(groupCount > 1)
            {
                result += '-';
            }

            lastType = type;
        }

        result += number[i];
    }

    return result;
}

// ------------------------------------------------------------
// Check complete vehicle number (text must be UPPERCASE)
// ------------------------------------------------------------

bool isValidVehicleNumber(string number)
{
    string parts[4];
    int hyphenCount = 0;

    // Split the number into 4 parts using the hyphen

    for(int i = 0; i < (int)number.length(); i++)
    {
        if(number[i] == '-')
        {
            hyphenCount++;

            if(hyphenCount > 3)
            {
                return false;
            }
        }
        else
        {
            parts[hyphenCount] += number[i];
        }
    }

    // Must have exactly 3 hyphens (4 parts)

    if(hyphenCount != 3)
    {
        return false;
    }

    // Part 1 : 2 letters

    if(parts[0].length() != 2 || !onlyLetters(parts[0]))
    {
        return false;
    }

    // Part 2 : 1 or 2 digits

    if(parts[1].length() > 2 || !onlyDigits(parts[1]))
    {
        return false;
    }

    // Part 3 : 2 or 3 letters

    if(parts[2].length() < 2 || parts[2].length() > 3 || !onlyLetters(parts[2]))
    {
        return false;
    }

    // Part 4 : exactly 4 digits

    if(parts[3].length() != 4 || !onlyDigits(parts[3]))
    {
        return false;
    }

    return true;
}

// ------------------------------------------------------------
// Ask the user for a vehicle number until it is valid.
// Returns an empty text if the user types 0 to cancel.
// ------------------------------------------------------------

string getVehicleNumber()
{
    string number;

    while(true)
    {
        cout << endl;

        printLeftPrompt(
            "Format : BA-2-PA-3983  or  BA2PA3983   (type 0 to cancel)",
            CYAN
        );

        cout << endl;

        printLeftPrompt(
            "Enter Vehicle Registration Number : ",
            YELLOW
        );

        showCursor();
        getline(cin, number);
        hideCursor();

        number = toUpper(number);

        number = makeStandardNumber(number);

        if(number == "0")
        {
            return "";
        }

        if(isValidVehicleNumber(number))
        {
            return number;
        }

        cout << endl;

        printCenteredColor(
            "Invalid vehicle number!",
            RED
        );

        printCenteredColor(
            "Use letters and digits only. Example: BA-2-PA-3983 or BA2PA3983",
            RED
        );
    }
}

// ============================================================
// TIME CLASS
// ============================================================

class Time
{
private:

    int hour;
    int minute;

public:

    Time()//constructor
    {
        hour = 0;
        minute = 0;
    }

    Time(int h, int m)
    {
        hour = h;
        minute = m;
    }

    int getHour()
    {
        return hour;
    }

    int getMinute()
    {
        return minute;
    }

    int totalMinutes()
    {
        return hour * 60 + minute;
    }

    // --------------------------------------------------------
    // Calculate difference between two times
    // --------------------------------------------------------

    int operator-(Time other)
    {
        int current = totalMinutes();
        int previous = other.totalMinutes();

        int difference = current - previous;

        // Overnight parking
        if(difference < 0)
        {
            difference += 24 * 60;
        }

        return difference;
    }

    // --------------------------------------------------------
    // Display time
    // --------------------------------------------------------

    friend ostream& operator<<(ostream& out, Time t)//
    {
        out << setfill('0')
            << setw(2)
            << t.hour
            << ":"
            << setw(2)
            << t.minute
            << setfill(' ');

        return out;//
    }
};

// ============================================================
// GET CURRENT TIME
// ============================================================

Time getCurrentTime()
{
    time_t currentTime = time(0);

    tm *localTime = localtime(&currentTime);

    return Time(
        localTime->tm_hour,
        localTime->tm_min
    );
}

void showClock()
{
    Time current = getCurrentTime();
    stringstream clockStream;
    clockStream << current;

    printCenteredColor(
        "CURRENT TIME: " + clockStream.str(),
        BRIGHTCYAN
    );
}

// ============================================================
// PARKING FULL EXCEPTION
// ============================================================

class ParkingFullException
{
private:

    string message;

public:

    ParkingFullException(string msg)
    {
        message = msg;
    }

    string getMessage()
    {
        return message;
    }
};

// ============================================================
// ABSTRACT VEHICLE CLASS
// ============================================================

class Vehicle
{
protected:

    string vehicleNumber;
    int slotNumber;
    Time entryTime;

public:

    Vehicle()
    {
        vehicleNumber = "";
        slotNumber = 0;
        entryTime = Time();
    }

    Vehicle(
        string number,
        int slot,
        Time entry
    )
    {
        vehicleNumber = number;
        slotNumber = slot;
        entryTime = entry;
    }

    virtual ~Vehicle()
    {
    }

    // Pure virtual functions

    virtual string getVehicleType() = 0;

    virtual double getRate() = 0;

    // --------------------------------------------------------
    // Calculate parking fee
    // --------------------------------------------------------

    double calculateFee(Time exitTime)
    {
        int minutes = exitTime - entryTime;

        if(minutes <= 0)
        {
            minutes = 1;
        }

        int hours = (minutes + 59) / 60;

        return hours * getRate();
    }

    // --------------------------------------------------------
    // Get vehicle number
    // --------------------------------------------------------

    string getVehicleNumber()
    {
        return vehicleNumber;
    }

    // --------------------------------------------------------
    // Get slot
    // --------------------------------------------------------

    int getSlotNumber()
    {
        return slotNumber;
    }

    // --------------------------------------------------------
    // Get entry time
    // --------------------------------------------------------

    Time getEntryTime()
    {
        return entryTime;
    }

    // --------------------------------------------------------
    // Check occupied
    // --------------------------------------------------------

    bool isOccupied()
    {
        if(slotNumber != 0)
        {
            return true;
        }

        return false;
    }
};

// ============================================================
// TWO WHEELER CLASS
// ============================================================

class TwoWheeler : public Vehicle
{
public:

    TwoWheeler()
        : Vehicle()
    {
    }

    TwoWheeler(
        string number,
        int slot,
        Time entry
    )
        : Vehicle(number, slot, entry)
    {
    }

    string getVehicleType()
    {
        return "Two-Wheeler";
    }

    double getRate()
    {
        return 20;
    }
};

// ============================================================
// FOUR WHEELER CLASS
// ============================================================

class FourWheeler : public Vehicle
{
public:

    FourWheeler()
        : Vehicle()
    {
    }

    FourWheeler(
        string number,
        int slot,
        Time entry
    )
        : Vehicle(number, slot, entry)
    {
    }

    string getVehicleType()
    {
        return "Four-Wheeler";
    }

    double getRate()
    {
        return 50;
    }
};

// ============================================================
// HEAVY VEHICLE CLASS
// ============================================================

class HeavyVehicle : public Vehicle
{
public:

    HeavyVehicle()
        : Vehicle()
    {
    }

    HeavyVehicle(
        string number,
        int slot,
        Time entry
    )
        : Vehicle(number, slot, entry)
    {
    }

    string getVehicleType()
    {
        return "Heavy Vehicle";
    }

    double getRate()
    {
        return 100;
    }

    double calculateHeavyFee(Time exitTime)
    {
        double fee = calculateFee(exitTime);

        if(fee < 300)
        {
            fee = 300;
        }

        return fee;
    }
};

// ============================================================
// TABLE ROW
// Every column has a fixed width, so all rows line up
// ============================================================

string makeVehicleRow(
    string number,
    string type,
    string slot,
    string entry,
    string fee
)
{
    stringstream row;

    row << left
        << setw(16) << number
        << setw(16) << type
        << setw(6)  << slot
        << setw(8)  << entry
        << setw(12) << fee;

    return row.str();
}

// ============================================================
// PARKING LOT CLASS
// ============================================================

class ParkingLot
{
private:

    TwoWheeler bikes[MAX_BIKES];

    FourWheeler cars[MAX_CARS];

    HeavyVehicle heavy[MAX_HEAVY];

    int bikeCount;
    int carCount;
    int heavyCount;

    double totalRevenue;

    // ========================================================
    // FIND BIKE
    // ========================================================

    int findBike(string number)
    {
        number = toUpper(number);

        for(int i = 0; i < MAX_BIKES; i++)
        {
            if(bikes[i].isOccupied())
            {
                if(bikes[i].getVehicleNumber() == number)
                {
                    return i;
                }
            }
        }

        return -1;
    }

    // ========================================================
    // FIND CAR
    // ========================================================

    int findCar(string number)
    {
        number = toUpper(number);

        for(int i = 0; i < MAX_CARS; i++)
        {
            if(cars[i].isOccupied())
            {
                if(cars[i].getVehicleNumber() == number)
                {
                    return i;
                }
            }
        }

        return -1;
    }

    // ========================================================
    // FIND HEAVY VEHICLE
    // ========================================================

    int findHeavy(string number)
    {
        number = toUpper(number);

        for(int i = 0; i < MAX_HEAVY; i++)
        {
            if(heavy[i].isOccupied())
            {
                if(heavy[i].getVehicleNumber() == number)
                {
                    return i;
                }
            }
        }

        return -1;
    }

    // ========================================================
    // WRITE ACTIVITY LOG
    // ========================================================

    void writeLog(string message)
    {
        ofstream file(
            LOG_FILE.c_str(),
            ios::app
        );

        if(file)
        {
            time_t now = time(0);

            tm *current = localtime(&now);

            file << current->tm_year + 1900
                 << "-"
                 << setw(2)
                 << setfill('0')
                 << current->tm_mon + 1
                 << "-"
                 << setw(2)
                 << current->tm_mday
                 << " "
                 << setw(2)
                 << current->tm_hour
                 << ":"
                 << setw(2)
                 << current->tm_min
                 << setfill(' ')
                 << " | "
                 << message
                 << endl;

            file.close();
        }
    }

public:

    // ========================================================
    // CONSTRUCTOR
    // ========================================================

    ParkingLot()
    {
        bikeCount = 0;
        carCount = 0;
        heavyCount = 0;

        totalRevenue = 0;
    }

    // ========================================================
    // VEHICLE ENTRY
    // ========================================================

    void vehicleEntry()
    {
        header("VEHICLE ENTRY");

        // ----------------------------------------------------
        // Step 1 : Select vehicle type
        // ----------------------------------------------------

        cout << endl;

        printCenteredColor(
            "1. Two-Wheeler   - Rs.20/hour",
            GREEN
        );

        printCenteredColor(
            "2. Four-Wheeler  - Rs.50/hour",
            YELLOW
        );

        printCenteredColor(
            "3. Heavy Vehicle - Rs.100/hour",
            MAGENTA
        );

        cout << endl;

        int choice = getInteger(
            "Select Vehicle Type :"
        );

        if(choice < 1 || choice > 3)
        {
            cout << endl;

            printCenteredColor(
                "Invalid vehicle type!",
                RED
            );

            waitForKey();

            return;
        }

        // ----------------------------------------------------
        // Check free space before asking for the vehicle number
        // ----------------------------------------------------

        try
        {
            if(choice == 1 && bikeCount >= MAX_BIKES)
            {
                throw ParkingFullException(
                    "Two-Wheeler parking is full!"
                );
            }

            if(choice == 2 && carCount >= MAX_CARS)
            {
                throw ParkingFullException(
                    "Four-Wheeler parking is full!"
                );
            }

            if(choice == 3 && heavyCount >= MAX_HEAVY)
            {
                throw ParkingFullException(
                    "Heavy Vehicle parking is full!"
                );
            }
        }
        catch(ParkingFullException &e)
        {
            cout << endl;

            printCenteredBold(
                "PARKING FULL!",
                RED
            );

            printCentered(
                e.getMessage()
            );

            waitForKey();

            return;
        }

        // ----------------------------------------------------
        // Step 2 : Get a valid vehicle number
        // (BA-2-PA-3983 or BA2PA3983)
        // ----------------------------------------------------

        string number = getVehicleNumber();

        if(number == "")
        {
            cout << endl;

            printCenteredColor(
                "Vehicle entry cancelled.",
                YELLOW
            );

            waitForKey();

            return;
        }

        // ----------------------------------------------------
        // Check duplicate
        // ----------------------------------------------------

        if(
            findBike(number) != -1 ||
            findCar(number) != -1 ||
            findHeavy(number) != -1
        )
        {
            cout << endl;

            printCenteredColor(
                "Vehicle is already parked!",
                RED
            );

            waitForKey();

            return;
        }

        Time entry = getCurrentTime();

        try
        {
            // =================================================
            // TWO WHEELER
            // =================================================

            if(choice == 1)
            {
                if(bikeCount >= MAX_BIKES)
                {
                    throw ParkingFullException(
                        "Two-Wheeler parking is full!"
                    );
                }

                int slot = -1;

                for(int i = 0; i < MAX_BIKES; i++)
                {
                    if(!bikes[i].isOccupied())
                    {
                        slot = i;

                        break;
                    }
                }

                if(
                    !confirm(
                        "Confirm vehicle entry?"
                    )
                )
                {
                    printCenteredColor(
                        "Vehicle entry cancelled.",
                        YELLOW
                    );

                    waitForKey();

                    return;
                }

                bikes[slot] =
                    TwoWheeler(
                        number,
                        slot + 1,
                        entry
                    );

                bikeCount++;

                cout << endl;

                printCenteredBold(
                    "VEHICLE ENTRY SUCCESSFUL!",
                    GREEN
                );

                smallLine();

                printDetail(
                    "Vehicle Number : " + number
                );

                printDetail(
                    "Vehicle Type   : Two-Wheeler"
                );

                printDetail(
                    "Slot Number    : " +
                    to_string(slot + 1)
                );

                stringstream ss;
                ss << entry;

                printDetail(
                    "Entry Time     : " +
                    ss.str()
                );

                smallLine();

                writeLog(
                    "ENTRY | Two-Wheeler | " +
                    number +
                    " | Slot: " +
                    to_string(slot + 1)
                );
            }

            // =================================================
            // FOUR WHEELER
            // =================================================

            else if(choice == 2)
            {
                if(carCount >= MAX_CARS)
                {
                    throw ParkingFullException(
                        "Four-Wheeler parking is full!"
                    );
                }

                int slot = -1;

                for(int i = 0; i < MAX_CARS; i++)
                {
                    if(!cars[i].isOccupied())
                    {
                        slot = i;

                        break;
                    }
                }

                if(
                    !confirm(
                        "Confirm vehicle entry?"
                    )
                )
                {
                    printCenteredColor(
                        "Vehicle entry cancelled.",
                        YELLOW
                    );

                    waitForKey();

                    return;
                }

                cars[slot] =
                    FourWheeler(
                        number,
                        slot + 1,
                        entry
                    );

                carCount++;

                cout << endl;

                printCenteredBold(
                    "VEHICLE ENTRY SUCCESSFUL!",
                    GREEN
                );

                smallLine();

                printDetail(
                    "Vehicle Number : " + number
                );

                printDetail(
                    "Vehicle Type   : Four-Wheeler"
                );

                printDetail(
                    "Slot Number    : " +
                    to_string(slot + 1)
                );

                stringstream ss;
                ss << entry;

                printDetail(
                    "Entry Time     : " +
                    ss.str()
                );

                smallLine();

                writeLog(
                    "ENTRY | Four-Wheeler | " +
                    number +
                    " | Slot: " +
                    to_string(slot + 1)
                );
            }

            // =================================================
            // HEAVY VEHICLE
            // =================================================

            else if(choice == 3)
            {
                if(heavyCount >= MAX_HEAVY)
                {
                    throw ParkingFullException(
                        "Heavy Vehicle parking is full!"
                    );
                }

                int slot = -1;

                for(int i = 0; i < MAX_HEAVY; i++)
                {
                    if(!heavy[i].isOccupied())
                    {
                        slot = i;

                        break;
                    }
                }

                if(
                    !confirm(
                        "Confirm vehicle entry?"
                    )
                )
                {
                    printCenteredColor(
                        "Vehicle entry cancelled.",
                        YELLOW
                    );

                    waitForKey();

                    return;
                }

                heavy[slot] =
                    HeavyVehicle(
                        number,
                        slot + 1,
                        entry
                    );

                heavyCount++;

                cout << endl;

                printCenteredBold(
                    "VEHICLE ENTRY SUCCESSFUL!",
                    GREEN
                );

                smallLine();

                printDetail(
                    "Vehicle Number : " + number
                );

                printDetail(
                    "Vehicle Type   : Heavy Vehicle"
                );

                printDetail(
                    "Slot Number    : " +
                    to_string(slot + 1)
                );

                stringstream ss;
                ss << entry;

                printDetail(
                    "Entry Time     : " +
                    ss.str()
                );

                smallLine();

                writeLog(
                    "ENTRY | Heavy Vehicle | " +
                    number +
                    " | Slot: " +
                    to_string(slot + 1)
                );
            }

            else
            {
                printCenteredColor(
                    "Invalid vehicle type!",
                    RED
                );
            }
        }

        catch(ParkingFullException &e)
        {
            cout << endl;

            printCenteredBold(
                "PARKING FULL!",
                RED
            );

            printCentered(
                e.getMessage()
            );
        }

        saveData();

        waitForKey();
    }

    // ========================================================
    // VEHICLE EXIT
    // ========================================================

    void vehicleExit()
    {
        header("VEHICLE EXIT & FEE COLLECTION");

        cout << endl;

        printLeftPrompt(
            "Enter Vehicle Registration Number : ",
            YELLOW
        );

        showCursor();

        string number;

        cin >> number;

        hideCursor();

        clearInput();

        number = toUpper(number);

        // BA2PA3983 and BA-2-PA-3983 are the same vehicle

        string standardNumber = makeStandardNumber(number);

        if(
            findBike(standardNumber) != -1 ||
            findCar(standardNumber) != -1 ||
            findHeavy(standardNumber) != -1
        )
        {
            number = standardNumber;
        }

        int bikeIndex = findBike(number);

        int carIndex = findCar(number);

        int heavyIndex = findHeavy(number);

        if(
            bikeIndex == -1 &&
            carIndex == -1 &&
            heavyIndex == -1
        )
        {
            cout << endl;

            printCenteredColor(
                "Vehicle not found!",
                RED
            );

            waitForKey();

            return;
        }

        Time exitTime = getCurrentTime();

        // ====================================================
        // TWO WHEELER EXIT
        // ====================================================

        if(bikeIndex != -1)
        {
            TwoWheeler &bike =
                bikes[bikeIndex];

            int minutes =
                exitTime -
                bike.getEntryTime();

            int hours =
                (minutes + 59) / 60;

            if(hours < 1)
            {
                hours = 1;
            }

            double amount =
                hours * bike.getRate();

            stringstream entryStream;
            entryStream << bike.getEntryTime();

            stringstream exitStream;
            exitStream << exitTime;

            cout << endl;

            printCenteredBold(
                "VEHICLE EXIT DETAILS",
                GREEN
            );

            smallLine();

            printDetail(
                "Vehicle Number : " +
                bike.getVehicleNumber()
            );

            printDetail(
                "Vehicle Type   : " +
                bike.getVehicleType()
            );

            printDetail(
                "Slot Number    : " +
                to_string(
                    bike.getSlotNumber()
                )
            );

            printDetail(
                "Entry Time     : " +
                entryStream.str()
            );

            printDetail(
                "Exit Time      : " +
                exitStream.str()
            );

            printDetail(
                "Duration       : " +
                to_string(minutes / 60) +
                " hour(s) " +
                to_string(minutes % 60) +
                " minute(s)"
            );

            printDetail(
                "Rate           : Rs." +
                to_string((int)bike.getRate()) +
                "/hour"
            );

            printDetail(
                "Billed Hours   : " +
                to_string(hours)
            );

            stringstream feeStream;

            feeStream << fixed
                      << setprecision(2)
                      << amount;

            printDetailBold(
                "Total Fee      : Rs." +
                feeStream.str(),
                GREEN
            );

            smallLine();

            if(
                confirm(
                    "Confirm vehicle exit and payment?"
                )
            )
            {
                totalRevenue += amount;

                writeLog(
                    "EXIT | Two-Wheeler | " +
                    number +
                    " | Fee: Rs." +
                    to_string((int)amount)
                );

                bikes[bikeIndex] =
                    TwoWheeler();

                bikeCount--;

                printCenteredColor(
                    "Vehicle Exit Successful!",
                    GREEN
                );
            }
            else
            {
                printCenteredColor(
                    "Vehicle exit cancelled.",
                    YELLOW
                );
            }
        }

        // ====================================================
        // FOUR WHEELER EXIT
        // ====================================================

        else if(carIndex != -1)
        {
            FourWheeler &car =
                cars[carIndex];

            int minutes =
                exitTime -
                car.getEntryTime();

            int hours =
                (minutes + 59) / 60;

            if(hours < 1)
            {
                hours = 1;
            }

            double amount =
                hours * car.getRate();

            stringstream entryStream;
            entryStream << car.getEntryTime();

            stringstream exitStream;
            exitStream << exitTime;

            cout << endl;

            printCenteredBold(
                "VEHICLE EXIT DETAILS",
                GREEN
            );

            smallLine();

            printDetail(
                "Vehicle Number : " +
                car.getVehicleNumber()
            );

            printDetail(
                "Vehicle Type   : " +
                car.getVehicleType()
            );

            printDetail(
                "Slot Number    : " +
                to_string(
                    car.getSlotNumber()
                )
            );

            printDetail(
                "Entry Time     : " +
                entryStream.str()
            );

            printDetail(
                "Exit Time      : " +
                exitStream.str()
            );

            printDetail(
                "Duration       : " +
                to_string(minutes / 60) +
                " hour(s) " +
                to_string(minutes % 60) +
                " minute(s)"
            );

            printDetail(
                "Rate           : Rs." +
                to_string((int)car.getRate()) +
                "/hour"
            );

            printDetail(
                "Billed Hours   : " +
                to_string(hours)
            );

            stringstream feeStream;

            feeStream << fixed
                      << setprecision(2)
                      << amount;

            printDetailBold(
                "Total Fee      : Rs." +
                feeStream.str(),
                GREEN
            );

            smallLine();

            if(
                confirm(
                    "Confirm vehicle exit and payment?"
                )
            )
            {
                totalRevenue += amount;

                writeLog(
                    "EXIT | Four-Wheeler | " +
                    number +
                    " | Fee: Rs." +
                    to_string((int)amount)
                );

                cars[carIndex] =
                    FourWheeler();

                carCount--;

                printCenteredColor(
                    "Vehicle Exit Successful!",
                    GREEN
                );
            }
            else
            {
                printCenteredColor(
                    "Vehicle exit cancelled.",
                    YELLOW
                );
            }
        }

        // ====================================================
        // HEAVY VEHICLE EXIT
        // ====================================================

        else if(heavyIndex != -1)
        {
            HeavyVehicle &vehicle =
                heavy[heavyIndex];

            int minutes =
                exitTime -
                vehicle.getEntryTime();

            int hours =
                (minutes + 59) / 60;

            if(hours < 1)
            {
                hours = 1;
            }

            double amount =
                vehicle.calculateHeavyFee(
                    exitTime
                );

            stringstream entryStream;
            entryStream << vehicle.getEntryTime();

            stringstream exitStream;
            exitStream << exitTime;

            cout << endl;

            printCenteredBold(
                "VEHICLE EXIT DETAILS",
                GREEN
            );

            smallLine();

            printDetail(
                "Vehicle Number : " +
                vehicle.getVehicleNumber()
            );

            printDetail(
                "Vehicle Type   : " +
                vehicle.getVehicleType()
            );

            printDetail(
                "Slot Number    : " +
                to_string(
                    vehicle.getSlotNumber()
                )
            );

            printDetail(
                "Entry Time     : " +
                entryStream.str()
            );

            printDetail(
                "Exit Time      : " +
                exitStream.str()
            );

            printDetail(
                "Duration       : " +
                to_string(minutes / 60) +
                " hour(s) " +
                to_string(minutes % 60) +
                " minute(s)"
            );

            printDetail(
                "Rate           : Rs." +
                to_string((int)vehicle.getRate()) +
                "/hour"
            );

            printDetail(
                "Billed Hours   : " +
                to_string(hours)
            );

            if(amount == 300)
            {
                printDetailColor(
                    "Minimum Charge : Rs.300",
                    YELLOW
                );
            }

            stringstream feeStream;

            feeStream << fixed
                      << setprecision(2)
                      << amount;

            printDetailBold(
                "Total Fee      : Rs." +
                feeStream.str(),
                GREEN
            );

            smallLine();

            if(
                confirm(
                    "Confirm vehicle exit and payment?"
                )
            )
            {
                totalRevenue += amount;

                writeLog(
                    "EXIT | Heavy Vehicle | " +
                    number +
                    " | Fee: Rs." +
                    to_string((int)amount)
                );

                heavy[heavyIndex] =
                    HeavyVehicle();

                heavyCount--;

                printCenteredColor(
                    "Vehicle Exit Successful!",
                    GREEN
                );
            }
            else
            {
                printCenteredColor(
                    "Vehicle exit cancelled.",
                    YELLOW
                );
            }
        }

        saveData();

        waitForKey();
    }

    // ========================================================
    // SEARCH VEHICLE
    // ========================================================

    void searchVehicle()
    {
        header("SEARCH VEHICLE");

        cout << endl;

        printLeftPrompt(
            "Enter Vehicle Registration Number : ",
            YELLOW
        );

        showCursor();

        string number;

        cin >> number;

        hideCursor();

        clearInput();

        number = toUpper(number);

        // BA2PA3983 and BA-2-PA-3983 are the same vehicle

        string standardNumber = makeStandardNumber(number);

        if(
            findBike(standardNumber) != -1 ||
            findCar(standardNumber) != -1 ||
            findHeavy(standardNumber) != -1
        )
        {
            number = standardNumber;
        }

        int bikeIndex = findBike(number);

        int carIndex = findCar(number);

        int heavyIndex = findHeavy(number);

        if(
            bikeIndex == -1 &&
            carIndex == -1 &&
            heavyIndex == -1
        )
        {
            cout << endl;

            printCenteredColor(
                "Vehicle not found!",
                RED
            );

            waitForKey();

            return;
        }

        Time now = getCurrentTime();

        cout << endl;

        printCenteredBold(
            "VEHICLE FOUND!",
            GREEN
        );

        smallLine();

        if(bikeIndex != -1)
        {
            TwoWheeler &bike =
                bikes[bikeIndex];

            int minutes =
                now -
                bike.getEntryTime();

            double fee =
                bike.calculateFee(now);

            stringstream entryStream;
            entryStream << bike.getEntryTime();

            stringstream nowStream;
            nowStream << now;

            stringstream feeStream;
            feeStream << fixed
                      << setprecision(2)
                      << fee;

            printDetail(
                "Vehicle Number : " +
                bike.getVehicleNumber()
            );

            printDetail(
                "Vehicle Type   : " +
                bike.getVehicleType()
            );

            printDetail(
                "Slot Number    : " +
                to_string(
                    bike.getSlotNumber()
                )
            );

            printDetail(
                "Entry Time     : " +
                entryStream.str()
            );

            printDetail(
                "Current Time   : " +
                nowStream.str()
            );

            printDetail(
                "Parked For     : " +
                to_string(minutes / 60) +
                " hour(s) " +
                to_string(minutes % 60) +
                " minute(s)"
            );

            printDetail(
                "Rate           : Rs." +
                to_string((int)bike.getRate()) +
                "/hour"
            );

            printDetail(
                "Estimated Fee  : Rs." +
                feeStream.str()
            );
        }

        else if(carIndex != -1)
        {
            FourWheeler &car =
                cars[carIndex];

            int minutes =
                now -
                car.getEntryTime();

            double fee =
                car.calculateFee(now);

            stringstream entryStream;
            entryStream << car.getEntryTime();

            stringstream nowStream;
            nowStream << now;

            stringstream feeStream;
            feeStream << fixed
                      << setprecision(2)
                      << fee;

            printDetail(
                "Vehicle Number : " +
                car.getVehicleNumber()
            );

            printDetail(
                "Vehicle Type   : " +
                car.getVehicleType()
            );

            printDetail(
                "Slot Number    : " +
                to_string(
                    car.getSlotNumber()
                )
            );

            printDetail(
                "Entry Time     : " +
                entryStream.str()
            );

            printDetail(
                "Current Time   : " +
                nowStream.str()
            );

            printDetail(
                "Parked For     : " +
                to_string(minutes / 60) +
                " hour(s) " +
                to_string(minutes % 60) +
                " minute(s)"
            );

            printDetail(
                "Rate           : Rs." +
                to_string((int)car.getRate()) +
                "/hour"
            );

            printDetail(
                "Estimated Fee  : Rs." +
                feeStream.str()
            );
        }

        else
        {
            HeavyVehicle &vehicle =
                heavy[heavyIndex];

            int minutes =
                now -
                vehicle.getEntryTime();

            double fee =
                vehicle.calculateHeavyFee(now);

            stringstream entryStream;
            entryStream << vehicle.getEntryTime();

            stringstream nowStream;
            nowStream << now;

            stringstream feeStream;
            feeStream << fixed
                      << setprecision(2)
                      << fee;

            printDetail(
                "Vehicle Number : " +
                vehicle.getVehicleNumber()
            );

            printDetail(
                "Vehicle Type   : " +
                vehicle.getVehicleType()
            );

            printDetail(
                "Slot Number    : " +
                to_string(
                    vehicle.getSlotNumber()
                )
            );

            printDetail(
                "Entry Time     : " +
                entryStream.str()
            );

            printDetail(
                "Current Time   : " +
                nowStream.str()
            );

            printDetail(
                "Parked For     : " +
                to_string(minutes / 60) +
                " hour(s) " +
                to_string(minutes % 60) +
                " minute(s)"
            );

            printDetail(
                "Rate           : Rs." +
                to_string((int)vehicle.getRate()) +
                "/hour"
            );

            printDetail(
                "Estimated Fee  : Rs." +
                feeStream.str()
            );
        }

        smallLine();

        waitForKey();
    }

    // ========================================================
    // PARKING OCCUPANCY
    // ========================================================

    void showOccupancy()
    {
        header("PARKING OCCUPANCY");

        int total =
            MAX_BIKES +
            MAX_CARS +
            MAX_HEAVY;

        int occupied =
            bikeCount +
            carCount +
            heavyCount;

        cout << endl;

        printDetail(
            "Total Capacity : " +
            to_string(total)
        );

        printDetailColor(
            "Occupied       : " +
            to_string(occupied),
            RED
        );

        printDetailColor(
            "Available      : " +
            to_string(total - occupied),
            GREEN
        );

        cout << endl;

        printCentered(
            "Green = Free | Red = Occupied"
        );

        // ====================================================
        // TWO WHEELERS
        // ====================================================

        cout << endl;

        printCenteredBold(
            "TWO-WHEELER SECTION",
            YELLOW
        );

        cout << string(GRID_INDENT, ' ');

        for(int i = 0; i < MAX_BIKES; i++)
        {
            int slotNumber = i + 1;

            string slot;

            if(slotNumber < 10)
            {
                slot = "[ " + to_string(slotNumber) + "] ";
            }
            else
            {
                slot = "[" + to_string(slotNumber) + "] ";
            }

            if(bikes[i].isOccupied())
            {
                cout << RED << slot << RESET;
            }
            else
            {
                cout << GREEN << slot << RESET;
            }

            // Start a new row after every 10 slots

            if((i + 1) % 10 == 0 && (i + 1) < MAX_BIKES)
            {
                cout << endl;
                cout << string(GRID_INDENT, ' ');
            }
        }

        cout << endl;

        // ====================================================
        // FOUR WHEELERS
        // ====================================================

        cout << endl;

        printCenteredBold(
            "FOUR-WHEELER SECTION",
            YELLOW
        );

        cout << string(GRID_INDENT, ' ');

        for(int i = 0; i < MAX_CARS; i++)
        {
            int slotNumber = i + 1;

            string slot;

            if(slotNumber < 10)
            {
                slot = "[ " + to_string(slotNumber) + "] ";
            }
            else
            {
                slot = "[" + to_string(slotNumber) + "] ";
            }

            if(cars[i].isOccupied())
            {
                cout << RED << slot << RESET;
            }
            else
            {
                cout << GREEN << slot << RESET;
            }

            // Start a new row after every 10 slots

            if((i + 1) % 10 == 0 && (i + 1) < MAX_CARS)
            {
                cout << endl;
                cout << string(GRID_INDENT, ' ');
            }
        }

        cout << endl;

        // ====================================================
        // HEAVY VEHICLES
        // ====================================================

        cout << endl;

        printCenteredBold(
            "HEAVY VEHICLE SECTION",
            YELLOW
        );

        cout << string(GRID_INDENT, ' ');

        for(int i = 0; i < MAX_HEAVY; i++)
        {
            int slotNumber = i + 1;

            string slot;

            if(slotNumber < 10)
            {
                slot = "[ " + to_string(slotNumber) + "] ";
            }
            else
            {
                slot = "[" + to_string(slotNumber) + "] ";
            }

            if(heavy[i].isOccupied())
            {
                cout << RED << slot << RESET;
            }
            else
            {
                cout << GREEN << slot << RESET;
            }

            // Start a new row after every 10 slots

            if((i + 1) % 10 == 0 && (i + 1) < MAX_HEAVY)
            {
                cout << endl;
                cout << string(GRID_INDENT, ' ');
            }
        }

        cout << endl;

        waitForKey();
    }

    // ========================================================
    // DISPLAY ALL VEHICLES
    // ========================================================

    void displayVehicles()
    {
        header("ALL PARKED VEHICLES");

        int total =
            bikeCount +
            carCount +
            heavyCount;

        if(total == 0)
        {
            cout << endl;

            printCenteredColor(
                "No vehicles are currently parked.",
                YELLOW
            );

            waitForKey();

            return;
        }

        cout << endl;

        string tableHeader =
            makeVehicleRow(
                "Vehicle No",
                "Type",
                "Slot",
                "Entry",
                "Est. Fee"
            );

        printCenteredBold(
            tableHeader,
            CYAN
        );

        smallLine();

        Time now = getCurrentTime();

        // ----------------------------------------------------
        // Bikes
        // ----------------------------------------------------

        for(int i = 0; i < MAX_BIKES; i++)
        {
            if(bikes[i].isOccupied())
            {
                stringstream entryText;
                entryText << bikes[i].getEntryTime();

                stringstream feeText;
                feeText << "Rs."
                         << fixed
                         << setprecision(2)
                         << bikes[i].calculateFee(now);

                printCentered(
                    makeVehicleRow(
                        bikes[i].getVehicleNumber(),
                        bikes[i].getVehicleType(),
                        to_string(bikes[i].getSlotNumber()),
                        entryText.str(),
                        feeText.str()
                    )
                );
            }
        }

        // ----------------------------------------------------
        // Cars
        // ----------------------------------------------------

        for(int i = 0; i < MAX_CARS; i++)
        {
            if(cars[i].isOccupied())
            {
                stringstream entryText;
                entryText << cars[i].getEntryTime();

                stringstream feeText;
                feeText << "Rs."
                         << fixed
                         << setprecision(2)
                         << cars[i].calculateFee(now);

                printCentered(
                    makeVehicleRow(
                        cars[i].getVehicleNumber(),
                        cars[i].getVehicleType(),
                        to_string(cars[i].getSlotNumber()),
                        entryText.str(),
                        feeText.str()
                    )
                );
            }
        }

        // ----------------------------------------------------
        // Heavy Vehicles
        // ----------------------------------------------------

        for(int i = 0; i < MAX_HEAVY; i++)
        {
            if(heavy[i].isOccupied())
            {
                stringstream entryText;
                entryText << heavy[i].getEntryTime();

                stringstream feeText;
                feeText << "Rs."
                         << fixed
                         << setprecision(2)
                         << heavy[i].calculateHeavyFee(now);

                printCentered(
                    makeVehicleRow(
                        heavy[i].getVehicleNumber(),
                        heavy[i].getVehicleType(),
                        to_string(heavy[i].getSlotNumber()),
                        entryText.str(),
                        feeText.str()
                    )
                );
            }
        }

        smallLine();

        printCentered(
            "Total Parked Vehicles : " +
            to_string(total)
        );

        waitForKey();
    }

    // ========================================================
    // REVENUE SUMMARY
    // ========================================================

    void revenueSummary()
    {
        header("REVENUE SUMMARY");

        cout << endl;

        printCenteredBold(
            "CURRENT PARKING STATUS",
            YELLOW
        );

        smallLine();

        printDetail(
            "Two-Wheelers   : " +
            to_string(bikeCount)
        );

        printDetail(
            "Four-Wheelers  : " +
            to_string(carCount)
        );

        printDetail(
            "Heavy Vehicles : " +
            to_string(heavyCount)
        );

        cout << endl;

        smallLine();

        stringstream revenue;

        revenue << fixed
                << setprecision(2)
                << totalRevenue;

        printDetailBold(
            "Total Revenue  : Rs." +
            revenue.str(),
            GREEN
        );

        smallLine();

        waitForKey();
    }

    // ========================================================
    // ACTIVITY LOG
    // ========================================================

    void activityLog()
    {
        header("PARKING ACTIVITY LOG");

        ifstream file(
            LOG_FILE.c_str()
        );

        if(!file)
        {
            cout << endl;

            printCenteredColor(
                "No parking activity recorded yet.",
                YELLOW
            );

            waitForKey();

            return;
        }

        string data;

        int count = 0;

        cout << endl;

        while(getline(file, data))
        {
            // Left aligned, so every log line starts at the same column

            cout << string(5, ' ')
                 << WHITE
                 << data
                 << RESET
                 << endl;

            count++;
        }

        file.close();

        if(count == 0)
        {
            printCenteredColor(
                "Log is empty.",
                YELLOW
            );
        }

        waitForKey();
    }

    // ========================================================
    // SAVE DATA
    // ========================================================

    void saveData()
    {
        ofstream file(
            DATA_FILE.c_str()
        );

        if(!file)
        {
            return;
        }

        // ----------------------------------------------------
        // Save revenue
        // ----------------------------------------------------

        file << totalRevenue << endl;

        // ----------------------------------------------------
        // Save bikes
        // ----------------------------------------------------

        for(int i = 0; i < MAX_BIKES; i++)
        {
            if(bikes[i].isOccupied())
            {
                file << "B "
                     << bikes[i].getVehicleNumber()
                     << " "
                     << bikes[i].getSlotNumber()
                     << " "
                     << bikes[i].getEntryTime().getHour()
                     << " "
                     << bikes[i].getEntryTime().getMinute()
                     << endl;
            }
        }

        // ----------------------------------------------------
        // Save cars
        // ----------------------------------------------------

        for(int i = 0; i < MAX_CARS; i++)
        {
            if(cars[i].isOccupied())
            {
                file << "C "
                     << cars[i].getVehicleNumber()
                     << " "
                     << cars[i].getSlotNumber()
                     << " "
                     << cars[i].getEntryTime().getHour()
                     << " "
                     << cars[i].getEntryTime().getMinute()
                     << endl;
            }
        }

        // ----------------------------------------------------
        // Save heavy vehicles
        // ----------------------------------------------------

        for(int i = 0; i < MAX_HEAVY; i++)
        {
            if(heavy[i].isOccupied())
            {
                file << "H "
                     << heavy[i].getVehicleNumber()
                     << " "
                     << heavy[i].getSlotNumber()
                     << " "
                     << heavy[i].getEntryTime().getHour()
                     << " "
                     << heavy[i].getEntryTime().getMinute()
                     << endl;
            }
        }

        file.close();
    }

    // ========================================================
    // LOAD DATA
    // ========================================================

    void loadData()
    {
        ifstream file(
            DATA_FILE.c_str()
        );

        if(!file)
        {
            return;
        }

        file >> totalRevenue;

        char type;

        string number;

        int slot;
        int hour;
        int minute;

        while(
            file >>
            type >>
            number >>
            slot >>
            hour >>
            minute
        )
        {
            Time entry(
                hour,
                minute
            );

            // ------------------------------------------------
            // Bike
            // ------------------------------------------------

            if(type == 'B')
            {
                if(
                    slot >= 1 &&
                    slot <= MAX_BIKES &&
                    !bikes[slot - 1].isOccupied()
                )
                {
                    bikes[slot - 1] =
                        TwoWheeler(
                            number,
                            slot,
                            entry
                        );

                    bikeCount++;
                }
            }

            // ------------------------------------------------
            // Car
            // ------------------------------------------------

            else if(type == 'C')
            {
                if(
                    slot >= 1 &&
                    slot <= MAX_CARS &&
                    !cars[slot - 1].isOccupied()
                )
                {
                    cars[slot - 1] =
                        FourWheeler(
                            number,
                            slot,
                            entry
                        );

                    carCount++;
                }
            }

            // ------------------------------------------------
            // Heavy
            // ------------------------------------------------

            else if(type == 'H')
            {
                if(
                    slot >= 1 &&
                    slot <= MAX_HEAVY &&
                    !heavy[slot - 1].isOccupied()
                )
                {
                    heavy[slot - 1] =
                        HeavyVehicle(
                            number,
                            slot,
                            entry
                        );

                    heavyCount++;
                }
            }
        }

        file.close();
    }
};

// ============================================================
// CREATE ADMIN FILE
// ============================================================

void createAdminFile()
{
    ifstream checkFile(
        ADMIN_FILE.c_str()
    );

    string username;
    string password;

    bool correctFile = false;

    if(checkFile)
    {
        checkFile >> username;
        checkFile >> password;

        // The file is correct when it has a username and a password
        // (the password can be changed by the administrator)

        if(
            !username.empty() &&
            !password.empty()
        )
        {
            correctFile = true;
        }
    }

    checkFile.close();

    // --------------------------------------------------------
    // Create file (admin / admin123) if missing or incomplete
    // --------------------------------------------------------

    if(!correctFile)
    {
        ofstream createFile(
            ADMIN_FILE.c_str()
        );

        if(createFile)
        {
            createFile
                << "admin"
                << endl;

            createFile
                << "admin123"
                << endl;

            createFile.close();
        }
        else
        {
            clearScreen();

            line();

            printCenteredBold(
                "ERROR: CANNOT CREATE ADMIN.TXT",
                RED
            );

            line();

            cout << endl;

            printCentered(
                "Make sure the program is running"
            );

            printCentered(
                "from a writable folder."
            );

            cout << endl;

            printCentered(
                "Recommended folder:"
            );

            printCentered(
                "C:\\ParkingSystem\\"
            );

            waitForKey();
        }
    }
}

// ============================================================
// PASSWORD INPUT
// ============================================================

string getPassword()
{
    string password;

    char ch;

    showCursor();

    while(true)
    {
        ch = _getch();

        // ----------------------------------------------------
        // ENTER
        // ----------------------------------------------------

        if(ch == 13)
        {
            break;
        }

        // ----------------------------------------------------
        // BACKSPACE
        // ----------------------------------------------------

        if(ch == 8)
        {
            if(password.length() > 0)
            {
                password.pop_back();

                cout << "\b \b";
            }
        }

        // ----------------------------------------------------
        // NORMAL CHARACTER
        // ----------------------------------------------------

        else if(
            ch >= 32 &&
            ch <= 126
        )
        {
            password += ch;

            cout << "*";
        }
    }

    cout << endl;
    hideCursor();

    return password;
}

// ============================================================
// LOGIN
// ============================================================

bool login()
{
    createAdminFile();

    ifstream file(
        ADMIN_FILE.c_str()
    );

    if(!file)
    {
        printCenteredColor(
            "Unable to open admin.txt!",
            RED
        );

        return false;
    }

    string storedUsername;
    string storedPassword;

    file >> storedUsername;
    file >> storedPassword;

    file.close();

    if(
        storedUsername.empty() ||
        storedPassword.empty()
    )
    {
        printCenteredColor(
            "Administrator account is invalid!",
            RED
        );

        return false;
    }

    string username;

    string password;

    cout << endl;

    printLeftPrompt(
        "Username : ",
        CYAN
    );

    showCursor();
    cin >> username;
    clearInput();
    hideCursor();

    cout << endl;

    printLeftPrompt(
        "Password : ",
        CYAN
    );

    password = getPassword();

    if(
        username == storedUsername &&
        password == storedPassword
    )
    {
        return true;
    }

    return false;
}

// ============================================================
// LOGIN ATTEMPTS (3 tries)
// ============================================================

bool loginAttempts()
{
    int attempts = 3;

    while(attempts > 0)
    {
        header("ADMINISTRATOR LOGIN");

        cout << endl;
        if(login())
        {
            clearScreen();

            line();

            printCenteredBold(
                "LOGIN SUCCESSFUL!",
                GREEN
            );

            line();

            cout << endl;

            printCentered(
                "Welcome to the Parking Management System."
            );

            waitForKey();

            return true;
        }

        attempts--;

        cout << endl;

        printCenteredColor(
            "Invalid Username or Password!",
            RED
        );

        printCentered(
            "Remaining Attempts: " +
            to_string(attempts)
        );

        if(attempts > 0)
        {
            waitForKey();
        }
    }

    clearScreen();

    line();

    printCenteredBold(
        "ACCESS DENIED!",
        RED
    );

    line();

    cout << endl;

    printCentered(
        "Maximum login attempts exceeded."
    );

    printCentered(
        "System will now close."
    );

    return false;
}

// ============================================================
// CHANGE ADMIN PASSWORD
// ============================================================

void changeAdminPassword()
{
    header("CHANGE ADMIN PASSWORD");

    createAdminFile();

    ifstream file(
        ADMIN_FILE.c_str()
    );

    if(!file)
    {
        cout << endl;

        printCenteredColor(
            "Unable to open admin.txt!",
            RED
        );

        waitForKey();

        return;
    }

    string storedUsername;
    string storedPassword;

    file >> storedUsername;
    file >> storedPassword;

    file.close();

    string username;
    string oldPassword;
    string newPassword;
    string confirmPassword;

    // --------------------------------------------------------
    // Step 1 : Check the current username and password
    // --------------------------------------------------------

    cout << endl;

    printLeftPrompt(
        "Username         : ",
        CYAN
    );

    showCursor();
    cin >> username;
    clearInput();
    hideCursor();

    cout << endl;

    printLeftPrompt(
        "Current Password : ",
        CYAN
    );

    oldPassword = getPassword();

    if(
        username != storedUsername ||
        oldPassword != storedPassword
    )
    {
        cout << endl;

        printCenteredColor(
            "Invalid Username or Password!",
            RED
        );

        waitForKey();

        return;
    }

    // --------------------------------------------------------
    // Step 2 : Enter the new password two times
    // --------------------------------------------------------

    cout << endl;

    printLeftPrompt(
        "New Password     : ",
        CYAN
    );

    newPassword = getPassword();

    cout << endl;

    printLeftPrompt(
        "Confirm Password : ",
        CYAN
    );

    confirmPassword = getPassword();

    // --------------------------------------------------------
    // Step 3 : Check the new password
    // --------------------------------------------------------

    string error = "";

    if(newPassword.length() < 6)
    {
        error = "Password must have at least 6 characters!";
    }
    else if(newPassword.find(' ') != string::npos)
    {
        error = "Password cannot contain spaces!";
    }
    else if(newPassword != confirmPassword)
    {
        error = "Passwords do not match!";
    }
    else if(newPassword == oldPassword)
    {
        error = "New password must be different!";
    }

    if(error != "")
    {
        cout << endl;

        printCenteredColor(
            error,
            RED
        );

        waitForKey();

        return;
    }

    // --------------------------------------------------------
    // Step 4 : Save the new password
    // --------------------------------------------------------

    ofstream outFile(
        ADMIN_FILE.c_str()
    );

    if(!outFile)
    {
        cout << endl;

        printCenteredColor(
            "Unable to save the new password!",
            RED
        );

        waitForKey();

        return;
    }

    outFile << storedUsername << endl;
    outFile << newPassword << endl;

    outFile.close();

    cout << endl;

    printCenteredBold(
        "PASSWORD CHANGED SUCCESSFULLY!",
        GREEN
    );

    waitForKey();
}

// ============================================================
// LOGIN MENU
// ============================================================

bool authenticate()
{
    int choice;

    string top =
        "+--------------------------------------------------+";

    while(true)
    {
        header("ADMINISTRATOR LOGIN");

        cout << endl;

        printCenteredColor(
            top,
            BLUE
        );

        printCenteredColor(
            "|  [1]  Login                                      |",
            BRIGHTGREEN
        );

        printCenteredColor(
            "|  [2]  Change Admin Password                      |",
            BRIGHTYELLOW
        );

        printCenteredColor(
            "|  [0]  Exit                                       |",
            RED
        );

        printCenteredColor(
            top,
            BLUE
        );

        cout << endl;

        printLeftPrompt(
            "Enter your choice : ",
            YELLOW
        );

        showCursor();
        cin >> choice;
        hideCursor();

        if(cin.fail())
        {
            clearInput();

            printCenteredColor(
                "Invalid choice! Please enter a number from 0-2.",
                RED
            );

            waitForKey();

            continue;
        }

        clearInput();

        // ----------------------------------------------------
        // 1. LOGIN
        // ----------------------------------------------------

        if(choice == 1)
        {
            // Returns false after 3 wrong attempts (program closes)

            return loginAttempts();
        }

        // ----------------------------------------------------
        // 2. CHANGE ADMIN PASSWORD
        // ----------------------------------------------------

        else if(choice == 2)
        {
            changeAdminPassword();
        }

        // ----------------------------------------------------
        // 0. EXIT
        // ----------------------------------------------------

        else if(choice == 0)
        {
            clearScreen();

            line();

            printCenteredBold(
                "Goodbye!",
                CYAN
            );

            line();

            return false;
        }

        // ----------------------------------------------------
        // INVALID
        // ----------------------------------------------------

        else
        {
            printCenteredColor(
                "Invalid choice! Please select 0-2.",
                RED
            );

            waitForKey();
        }
    }
}

// ============================================================
// MAIN MENU
// ============================================================

void showMenu()
{
    header("MAIN MENU");

    cout << endl;

    string top =
        "+--------------------------------------------------+";

    printCenteredColor(
        top,
        BLUE
    );

    printCenteredColor(
        "|  [1]  Vehicle Entry                              |",
        BRIGHTGREEN
    );

    printCenteredColor(
        "|  [2]  Vehicle Exit & Fee Collection              |",
        BRIGHTYELLOW
    );

    printCenteredColor(
        "|  [3]  Search Vehicle                             |",
        BRIGHTCYAN
    );

    printCenteredColor(
        "|  [4]  Parking Occupancy                          |",
        GREEN
    );

    printCenteredColor(
        "|  [5]  Display Parked Vehicles                    |",
        CYAN
    );

    printCenteredColor(
        "|  [6]  Parking Activity Log                       |",
        MAGENTA
    );

    printCenteredColor(
        "|  [7]  Revenue Summary                            |",
        YELLOW
    );

    printCenteredColor(
        "|  [0]  Save & Exit                                |",
        RED
    );

    printCenteredColor(
        top,
        BLUE
    );

    cout << endl;

    printLeftPrompt(
        "Enter your choice : ",
        YELLOW
    );
}

// ============================================================
// MAIN FUNCTION
// ============================================================

int main()
{
    enableAnsiColors();
    hideCursor();

    ParkingLot parkingLot;

    // ========================================================
    // LOGIN
    // ========================================================

    if(!authenticate())
    {
        return 0;
    }

    // ========================================================
    // LOAD PREVIOUS DATA
    // ========================================================

    parkingLot.loadData();

    int choice;

    // ========================================================
    // MAIN PROGRAM LOOP
    // ========================================================

    do
    {
        showMenu();

        showCursor();
        cin >> choice;
        hideCursor();

        if(cin.fail())
        {
            clearInput();

            printCenteredColor(
                "Invalid choice! Please enter a number from 0-7.",
                RED
            );

            waitForKey();

            continue;
        }

        clearInput();

        switch(choice)
        {
            // ------------------------------------------------
            // VEHICLE ENTRY
            // ------------------------------------------------

            case 1:

                parkingLot.vehicleEntry();

                break;

            // ------------------------------------------------
            // VEHICLE EXIT
            // ------------------------------------------------

            case 2:

                parkingLot.vehicleExit();

                break;

            // ------------------------------------------------
            // SEARCH VEHICLE
            // ------------------------------------------------

            case 3:

                parkingLot.searchVehicle();

                break;

            // ------------------------------------------------
            // PARKING OCCUPANCY
            // ------------------------------------------------

            case 4:

                parkingLot.showOccupancy();

                break;

            // ------------------------------------------------
            // DISPLAY VEHICLES
            // ------------------------------------------------

            case 5:

                parkingLot.displayVehicles();

                break;

            // ------------------------------------------------
            // ACTIVITY LOG
            // ------------------------------------------------

            case 6:

                parkingLot.activityLog();

                break;

            // ------------------------------------------------
            // REVENUE
            // ------------------------------------------------

            case 7:

                parkingLot.revenueSummary();

                break;

            // ------------------------------------------------
            // EXIT
            // ------------------------------------------------

            case 0:

                parkingLot.saveData();

                clearScreen();

                line();

                printCenteredBold(
                    "PARKING DATA SAVED SUCCESSFULLY!",
                    GREEN
                );

                line();

                cout << endl;

                printCentered(
                    "Thank you for using"
                );

                printCentered(
                    "Smart Parking Lot System."
                );

                cout << endl;

                printCenteredBold(
                    "Goodbye!",
                    CYAN
                );

                cout << endl;

                break;

            // ------------------------------------------------
            // INVALID
            // ------------------------------------------------

            default:

                printCenteredColor(
                    "Invalid choice! Please select 0-7.",
                    RED
                );

                waitForKey();

                break;
        }

    }
    while(choice != 0);

    showCursor();
    return 0;
}