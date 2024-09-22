#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include "sequentialfile.h"
using namespace std;

template <class RecordT, class KeyT>
SequentileFile<RecordT, KeyT>::SequentileFile(const string& data_file_name, const string& aux_file_name,
                                              function<KeyT(RecordT*)> keyAccessor)
    : data_file_name(data_file_name), aux_file_name(aux_file_name), keyAccessor(keyAccessor) {
}

template <class RecordT, class KeyT>
void SequentileFile<RecordT, KeyT>::init(const string& data_name_csv) {
}
int main() {
    SequentileFile<RecordMeet, int> meetFile("meets.dat", "meets_aux.dat", 
                                             [](RecordMeet* record) { return record->MeetID; });

    SequentileFile<RecordCompetition, int> competitionFile("competitions.dat", "competitions_aux.dat", 
                                                           [](RecordCompetition* record) { return record->MeetID; });

    cout << "Hello, World!" << endl;
    cout << meetFile.data_file_name << endl;
    cout << competitionFile.data_file_name << endl;
    return 0;
}

