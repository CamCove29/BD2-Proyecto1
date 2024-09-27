#ifndef RECORD_H
#define RECORD_H

#include <cstring>
#include <vector>
#include <string>
using namespace std;

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
    int next_pos;

    //constructor vacio
    RecordMeet() {
        MeetID = 0;
        memset(MeetPath, 0, MAX_PATH_LENGTH);
        memset(Federation, 0, MAX_FEDERATION_LENGTH);
        memset(Date, 0, MAX_DATE_LENGTH);
        memset(MeetCountry, 0, MAX_COUNTRY_LENGTH);
        memset(MeetState, 0, MAX_STATE_LENGTH);
        memset(MeetTown, 0, MAX_TOWN_LENGTH);
        memset(MeetName, 0, MAX_NAME_LENGTH);
        next_pos = -1;
    }

    // Constructor que acepta const char*
    RecordMeet(int id, const char* path, const char* federation, const char* date,
               const char* country, const char* state, const char* town, 
               const char* name)
        : MeetID(id) {
        strncpy(MeetPath, path, MAX_PATH_LENGTH);
        MeetPath[MAX_PATH_LENGTH - 1] = '\0';
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
    RecordMeet(const vector<string>& atributos)
        : MeetID(stoi(atributos[0])) {
        strncpy(MeetPath, atributos[1].c_str(), MAX_PATH_LENGTH);
        MeetPath[MAX_PATH_LENGTH - 1] = '\0';
        strncpy(Federation, atributos[2].c_str(), MAX_FEDERATION_LENGTH);
        Federation[MAX_FEDERATION_LENGTH - 1] = '\0';
        strncpy(Date, atributos[3].c_str(), MAX_DATE_LENGTH);
        Date[MAX_DATE_LENGTH - 1] = '\0';
        strncpy(MeetCountry, atributos[4].c_str(), MAX_COUNTRY_LENGTH);
        MeetCountry[MAX_COUNTRY_LENGTH - 1] = '\0';
        strncpy(MeetState, atributos[5].c_str(), MAX_STATE_LENGTH);
        MeetState[MAX_STATE_LENGTH - 1] = '\0';
        strncpy(MeetTown, atributos[6].c_str(), MAX_TOWN_LENGTH);
        MeetTown[MAX_TOWN_LENGTH - 1] = '\0';
        strncpy(MeetName, atributos[7].c_str(), MAX_NAME_LENGTH);
        MeetName[MAX_NAME_LENGTH - 1] = '\0';
        next_pos = -1;
    }

    //constructor copia
    RecordMeet(const RecordMeet& other)
        : MeetID(other.MeetID) {
        strncpy(MeetPath, other.MeetPath, MAX_PATH_LENGTH);
        MeetPath[MAX_PATH_LENGTH - 1] = '\0';
        strncpy(Federation, other.Federation, MAX_FEDERATION_LENGTH);
        Federation[MAX_FEDERATION_LENGTH - 1] = '\0';
        strncpy(Date, other.Date, MAX_DATE_LENGTH);
        Date[MAX_DATE_LENGTH - 1] = '\0';
        strncpy(MeetCountry, other.MeetCountry, MAX_COUNTRY_LENGTH);
        MeetCountry[MAX_COUNTRY_LENGTH - 1] = '\0';
        strncpy(MeetState, other.MeetState, MAX_STATE_LENGTH);
        MeetState[MAX_STATE_LENGTH - 1] = '\0';
        strncpy(MeetTown, other.MeetTown, MAX_TOWN_LENGTH);
        MeetTown[MAX_TOWN_LENGTH - 1] = '\0';
        strncpy(MeetName, other.MeetName, MAX_NAME_LENGTH);
        MeetName[MAX_NAME_LENGTH - 1] = '\0';
        next_pos = other.next_pos;
    }

    //operador de asignacion
    RecordMeet& operator=(const RecordMeet& other) {
        if (this != &other) {
            MeetID = other.MeetID;
            strncpy(MeetPath, other.MeetPath, MAX_PATH_LENGTH);
            MeetPath[MAX_PATH_LENGTH - 1] = '\0';
            strncpy(Federation, other.Federation, MAX_FEDERATION_LENGTH);
            Federation[MAX_FEDERATION_LENGTH - 1] = '\0';
            strncpy(Date, other.Date, MAX_DATE_LENGTH);
            Date[MAX_DATE_LENGTH - 1] = '\0';
            strncpy(MeetCountry, other.MeetCountry, MAX_COUNTRY_LENGTH);
            MeetCountry[MAX_COUNTRY_LENGTH - 1] = '\0';
            strncpy(MeetState, other.MeetState, MAX_STATE_LENGTH);
            MeetState[MAX_STATE_LENGTH - 1] = '\0';
            strncpy(MeetTown, other.MeetTown, MAX_TOWN_LENGTH);
            MeetTown[MAX_TOWN_LENGTH - 1] = '\0';
            strncpy(MeetName, other.MeetName, MAX_NAME_LENGTH);
            MeetName[MAX_NAME_LENGTH - 1] = '\0';
            next_pos = other.next_pos;
        }
        return *this;
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

#endif // RECORD_H