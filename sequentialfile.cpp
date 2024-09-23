#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include "sequentialfile.h"
#include "record.h"
using namespace std;

template <class RecordT, class KeyT>
SequentileFile<RecordT, KeyT>::SequentileFile(const string& data_file_name, const string& aux_file_name,
                                              function<KeyT(RecordT*)> keyAccessor)
    : data_file_name(data_file_name), aux_file_name(aux_file_name), keyAccessor(keyAccessor) {
}

template <class RecordT>
vector<RecordT*> load_csv(string fileName, int cant){
    vector<RecordT*> records;
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error al abrir el archivo" << endl;
        return records;
    }
    string line;
    getline(file, line);
    for (int i = 0; i < cant; i++) {
        getline(file, line);
        stringstream ss(line);
        string value;
        vector<string> values;
        while (getline(ss, value, ',')) {
            values.push_back(value);
        }

        RecordT* record = new RecordT(values);
        records.push_back(record);        
    }
    file.close();
    return records;

}
/*
template <class RecordT, class KeyT>
void SequentileFile<RecordT, KeyT>::init(const std::string& data_name_csv) {
    // Abrimos el archivo CSV
    std::ifstream data_file(data_name_csv);

    if (!data_file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo CSV." << std::endl;
        return;
    }

    std::string line;
    
    // Omitimos la primera línea si es la cabecera del CSV
    std::getline(data_file, line); // Suponiendo que la primera línea es la cabecera

    // Procesamos cada línea del archivo CSV
    while (std::getline(data_file, line)) {
        std::stringstream ss(line);
        std::string value;
        std::vector<std::string> values;

        // Leemos los valores separados por comas y los almacenamos en el vector
        while (std::getline(ss, value, ',')) {
            values.push_back(value);
        }

        // Aquí es donde construimos el objeto RecordT desde los valores del CSV
        // Debes ajustar este constructor para que coincida con la estructura de RecordT
        RecordT* record = new RecordT(values);

        // Llamamos a la función add para añadir el registro al archivo secuencial
        if (!add(record)) {
            std::cerr << "Error al agregar el registro." << std::endl;
            delete record;
        }
    }

    // Cerramos el archivo CSV
    data_file.close();
}
*/


int main() {
    SequentileFile<RecordMeet, int> meetFile("meets.dat", "meets_aux.dat", 
                                             [](RecordMeet* record) { return record->MeetID; });

    SequentileFile<RecordCompetition, int> competitionFile("competitions.dat", "competitions_aux.dat", 
                                                           [](RecordCompetition* record) { return record->MeetID; });

    cout << "Hello, World!" << endl;
    cout << meetFile.data_file_name << endl;
    cout << competitionFile.data_file_name << endl;

    vector<RecordMeet*> meets = load_csv<RecordMeet>("datasets/meets.csv", 10);
    for (RecordMeet* meet : meets) {
        cout << meet->MeetID << " " << meet->MeetPath << " " << meet->Federation << " " << meet->Date << " " << meet->MeetCountry << " " << meet->MeetState << " " << meet->MeetTown << " " << meet->MeetName << endl;
    }
    return 0;
}

