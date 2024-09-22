#include <cstring>

class RecordMeet {
public:
    static const int MAX_PATH_LENGTH = 20;
    static const int MAX_FEDERATION_LENGTH = 15;
    static const int MAX_DATE_LENGTH = 11; // YYYY-MM-DD
    static const int MAX_COUNTRY_LENGTH = 5;
    static const int MAX_STATE_LENGTH = 15;
    static const int MAX_TOWN_LENGTH = 15;
    static const int MAX_NAME_LENGTH = 50;

    int MeetID;
    char MeetPath[MAX_PATH_LENGTH];
    char Federation[MAX_FEDERATION_LENGTH];
    char Date[MAX_DATE_LENGTH];
    char MeetCountry[MAX_COUNTRY_LENGTH];
    char MeetState[MAX_STATE_LENGTH];
    char MeetTown[MAX_TOWN_LENGTH];
    char MeetName[MAX_NAME_LENGTH];

    RecordMeet(int id, const char* path, const char* federation, const char* date,
               const char* country, const char* state, const char* town, 
               const char* name) 
        : MeetID(id) {
        strncpy(MeetPath, path, MAX_PATH_LENGTH);
        MeetPath[MAX_PATH_LENGTH - 1] = '\0'; // Asegurar que sea null-terminated
        strncpy(Federation, federation, MAX_FEDERATION_LENGTH);
        Federation[MAX_FEDERATION_LENGTH - 1] = '\0';
        strncpy(Date, date, MAX_DATE_LENGTH);
        Date[MAX_DATE_LENGTH - 1] = '\0';
        strncpy(MeetCountry, country, MAX_COUNTRY_LENGTH);
        MeetCountry[MAX_COUNTRY_LENGTH - 1] = '\0';
        strncpy(MeetState, state, MAX_STATE_LENGTH);
        MeetState[MAX_STATE_LENGTH - 1] = '\0';
        strncpy(MeetTown, town, MAX_TOWN_LENGTH);
        MeetTown[MAX_TOWN_LENGTH - 1] = '\0';
        strncpy(MeetName, name, MAX_NAME_LENGTH);
        MeetName[MAX_NAME_LENGTH - 1] = '\0';
    }
};


class RecordCompetition {
public:
    static const int MAX_NAME_LENGTH = 20;
    static const int MAX_EQUIPMENT_LENGTH = 15;
    static const int MAX_DIVISION_LENGTH = 15;

    int MeetID;
    std::string Name;
    char Sex;
    std::string Equipment;
    int Age;
    std::string Division;
    double BodyweightKg;
    double WeightClassKg;
    double Squat4Kg;
    double BestSquatKg;
    double Bench4Kg;
    double BestBenchKg;
    double Deadlift4Kg;
    double BestDeadliftKg;
    double TotalKg;
    int Place;
    double Wilks;

    RecordCompetition(int meetID, const std::string& name, char sex, const std::string& equipment,
                      int age, const std::string& division, double bodyweightKg, double weightClassKg,
                      double squat4Kg, double bestSquatKg, double bench4Kg, double bestBenchKg,
                      double deadlift4Kg, double bestDeadliftKg, double totalKg, int place, double wilks)
        : MeetID(meetID), Name(name), Sex(sex), Equipment(equipment), Age(age), Division(division),
          BodyweightKg(bodyweightKg), WeightClassKg(weightClassKg), Squat4Kg(squat4Kg),
          BestSquatKg(bestSquatKg), Bench4Kg(bench4Kg), BestBenchKg(bestBenchKg),
          Deadlift4Kg(deadlift4Kg), BestDeadliftKg(bestDeadliftKg), TotalKg(totalKg),
          Place(place), Wilks(wilks) {}
};
