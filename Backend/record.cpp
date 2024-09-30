#include "record.h"

// Implementación de RecordMeet

RecordMeet::RecordMeet() {
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

RecordMeet::RecordMeet(int id, const char* path, const char* federation, const char* date,
                       const char* country, const char* state, const char* town, 
                       const char* name) : MeetID(id) {
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

RecordMeet::RecordMeet(const vector<string>& atributos) : MeetID(stoi(atributos[0])) {
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

RecordMeet::RecordMeet(const RecordMeet& other) : MeetID(other.MeetID) {
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

RecordMeet& RecordMeet::operator=(const RecordMeet& other) {
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

bool RecordMeet::checkVector(const vector<string>& atributos) {
    if (atributos.size() != 8) {
        return false;
    }
    if (atributos[0].empty() || atributos[1].empty() || atributos[2].empty() || 
        atributos[3].empty() || atributos[4].empty() || atributos[5].empty() || 
        atributos[6].empty() || atributos[7].empty()) {
        return false;
    }
    return true;
}

std::pair<std::string, std::string> RecordMeet::getStringAttribute(std::string key) {
    if (key == "MeetID") {
        return std::make_pair("int", std::to_string(MeetID));
    } else if (key == "MeetPath") {
        return std::make_pair("string", std::string(MeetPath));
    } else if (key == "Federation") {
        return std::make_pair("string", std::string(Federation));
    } else if (key == "Date") {
        return std::make_pair("string", std::string(Date));
    } else if (key == "MeetCountry") {
        return std::make_pair("string", std::string(MeetCountry));
    } else if (key == "MeetState") {
        return std::make_pair("string", std::string(MeetState));
    } else if (key == "MeetTown") {
        return std::make_pair("string", std::string(MeetTown));
    } else if (key == "MeetName") {
        return std::make_pair("string", std::string(MeetName));
    } else {
        return std::make_pair("nonetype", "");
    }
}

// Implementación de RecordCompetition

RecordCompetition::RecordCompetition() {
    MeetID = 0;
    memset(Name, 0, MAX_NAME_LENGTH);
    Sex = ' ';
    memset(Equipment, 0, MAX_EQUIPMENT_LENGTH);
    Age = 0;
    memset(Division, 0, MAX_DIVISION_LENGTH);
    BodyweightKg = 0.0;
    WeightClassKg = 0.0;
    Squat4Kg = 0.0;
    BestSquatKg = 0.0;
    Bench4Kg = 0.0;
    BestBenchKg = 0.0;
    Deadlift4Kg = 0.0;
    BestDeadliftKg = 0.0;
    TotalKg = 0.0;
    Place = 0;
    Wilks = 0.0;
    next_pos = -1;
}

RecordCompetition::RecordCompetition(int id, const char* name, char sex, const char* equipment, 
                                     int age, const char* division, double bodyweight, 
                                     double weightclass, double squat4, double bestsquat, 
                                     double bench4, double bestbench, double deadlift4, 
                                     double bestdeadlift, double total, int place, double wilks) 
    : MeetID(id), Sex(sex), Age(age), BodyweightKg(bodyweight), 
      WeightClassKg(weightclass), Squat4Kg(squat4), BestSquatKg(bestsquat), 
      Bench4Kg(bench4), BestBenchKg(bestbench), Deadlift4Kg(deadlift4), 
      BestDeadliftKg(bestdeadlift), TotalKg(total), Place(place), Wilks(wilks) {
    strncpy(Name, name, MAX_NAME_LENGTH);
    Name[MAX_NAME_LENGTH - 1] = '\0';
    strncpy(Equipment, equipment, MAX_EQUIPMENT_LENGTH);
    Equipment[MAX_EQUIPMENT_LENGTH - 1] = '\0';
    strncpy(Division, division, MAX_DIVISION_LENGTH);
    Division[MAX_DIVISION_LENGTH - 1] = '\0';
    next_pos = -1;
}

RecordCompetition::RecordCompetition(const vector<string>& atributos) : MeetID(stoi(atributos[0])), 
    Sex(atributos[1][0]), Age(stoi(atributos[2])), 
    BodyweightKg(stod(atributos[3])), WeightClassKg(stod(atributos[4])), 
    Squat4Kg(stod(atributos[5])), BestSquatKg(stod(atributos[6])), 
    Bench4Kg(stod(atributos[7])), BestBenchKg(stod(atributos[8])), 
    Deadlift4Kg(stod(atributos[9])), BestDeadliftKg(stod(atributos[10])), 
    TotalKg(stod(atributos[11])), Place(stoi(atributos[12])), 
    Wilks(stod(atributos[13])) {
    strncpy(Name, atributos[14].c_str(), MAX_NAME_LENGTH);
    Name[MAX_NAME_LENGTH - 1] = '\0';
    strncpy(Equipment, atributos[15].c_str(), MAX_EQUIPMENT_LENGTH);
    Equipment[MAX_EQUIPMENT_LENGTH - 1] = '\0';
    strncpy(Division, atributos[16].c_str(), MAX_DIVISION_LENGTH);
    Division[MAX_DIVISION_LENGTH - 1] = '\0';
    next_pos = -1;
}

RecordCompetition::RecordCompetition(const RecordCompetition& other) : MeetID(other.MeetID), 
    Sex(other.Sex), Age(other.Age), BodyweightKg(other.BodyweightKg), 
    WeightClassKg(other.WeightClassKg), Squat4Kg(other.Squat4Kg), 
    BestSquatKg(other.BestSquatKg), Bench4Kg(other.Bench4Kg), 
    BestBenchKg(other.BestBenchKg), Deadlift4Kg(other.Deadlift4Kg), 
    BestDeadliftKg(other.BestDeadliftKg), TotalKg(other.TotalKg), 
    Place(other.Place), Wilks(other.Wilks) {
    strncpy(Name, other.Name, MAX_NAME_LENGTH);
    Name[MAX_NAME_LENGTH - 1] = '\0';
    strncpy(Equipment, other.Equipment, MAX_EQUIPMENT_LENGTH);
    Equipment[MAX_EQUIPMENT_LENGTH - 1] = '\0';
    strncpy(Division, other.Division, MAX_DIVISION_LENGTH);
    Division[MAX_DIVISION_LENGTH - 1] = '\0';
    next_pos = other.next_pos;
}

RecordCompetition& RecordCompetition::operator=(const RecordCompetition& other) {
    if (this != &other) {
        MeetID = other.MeetID;
        strncpy(Name, other.Name, MAX_NAME_LENGTH);
        Name[MAX_NAME_LENGTH - 1] = '\0';
        Sex = other.Sex;
        strncpy(Equipment, other.Equipment, MAX_EQUIPMENT_LENGTH);
        Equipment[MAX_EQUIPMENT_LENGTH - 1] = '\0';
        Age = other.Age;
        strncpy(Division, other.Division, MAX_DIVISION_LENGTH);
        Division[MAX_DIVISION_LENGTH - 1] = '\0';
        BodyweightKg = other.BodyweightKg;
        WeightClassKg = other.WeightClassKg;
        Squat4Kg = other.Squat4Kg;
        BestSquatKg = other.BestSquatKg;
        Bench4Kg = other.Bench4Kg;
        BestBenchKg = other.BestBenchKg;
        Deadlift4Kg = other.Deadlift4Kg;
        BestDeadliftKg = other.BestDeadliftKg;
        TotalKg = other.TotalKg;
        Place = other.Place;
        Wilks = other.Wilks;
        next_pos = other.next_pos;
    }
    return *this;
}

std::pair<std::string, std::string> RecordCompetition::getStringAttribute(std::string key) {
    if (key == "MeetID") {
        return std::make_pair("int", std::to_string(MeetID));
    } else if (key == "Name") {
        return std::make_pair("string", std::string(Name));
    } else if (key == "Sex") {
        return std::make_pair("char", std::string(1, Sex));
    } else if (key == "Equipment") {
        return std::make_pair("string", std::string(Equipment));
    } else if (key == "Age") {
        return std::make_pair("int", std::to_string(Age));
    } else if (key == "Division") {
        return std::make_pair("string", std::string(Division));
    } else if (key == "BodyweightKg") {
        return std::make_pair("double", std::to_string(BodyweightKg));
    } else if (key == "WeightClassKg") {
        return std::make_pair("double", std::to_string(WeightClassKg));
    } else if (key == "Squat4Kg") {
        return std::make_pair("double", std::to_string(Squat4Kg));
    } else if (key == "BestSquatKg") {
        return std::make_pair("double", std::to_string(BestSquatKg));
    } else if (key == "Bench4Kg") {
        return std::make_pair("double", std::to_string(Bench4Kg));
    } else if (key == "BestBenchKg") {
        return std::make_pair("double", std::to_string(BestBenchKg));
    } else if (key == "Deadlift4Kg") {
        return std::make_pair("double", std::to_string(Deadlift4Kg));
    } else if (key == "BestDeadliftKg") {
        return std::make_pair("double", std::to_string(BestDeadliftKg));
    } else if (key == "TotalKg") {
        return std::make_pair("double", std::to_string(TotalKg));
    } else if (key == "Place") {
        return std::make_pair("int", std::to_string(Place));
    } else if (key == "Wilks") {
        return std::make_pair("double", std::to_string(Wilks));
    } else {
        return std::make_pair("nonetype", "");
    }
}
