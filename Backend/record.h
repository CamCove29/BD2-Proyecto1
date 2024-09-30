#ifndef RECORD_H
#define RECORD_H

#include <cstring>
#include <vector>
#include <string>
#include <utility> // Para std::pair

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

    RecordMeet();
    RecordMeet(int id, const char* path, const char* federation, const char* date,
               const char* country, const char* state, const char* town, 
               const char* name);
    static bool checkVector(const vector<string>& atributos);
    RecordMeet(const vector<string>& atributos);
    RecordMeet(const RecordMeet& other);
    RecordMeet& operator=(const RecordMeet& other);
    std::pair<std::string, std::string> getStringAttribute(std::string key);
};

class RecordCompetition {
public:
    static const int MAX_NAME_LENGTH = 20;
    static const int MAX_EQUIPMENT_LENGTH = 15;
    static const int MAX_DIVISION_LENGTH = 15;

    int MeetID;
    char Name[MAX_NAME_LENGTH];
    char Sex;
    char Equipment[MAX_EQUIPMENT_LENGTH];
    int Age;
    char Division[MAX_DIVISION_LENGTH];
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
    int next_pos;

    RecordCompetition();
    RecordCompetition(int id, const char* name, char sex, const char* equipment, int age,
                      const char* division, double bodyweight, double weightclass, double squat4,
                      double bestsquat, double bench4, double bestbench, double deadlift4,
                      double bestdeadlift, double total, int place, double wilks);
    RecordCompetition(const vector<string>& atributos);
    RecordCompetition(const RecordCompetition& other);
    RecordCompetition& operator=(const RecordCompetition& other);
    std::pair<std::string, std::string> getStringAttribute(std::string key);
};

#endif // RECORD_H