#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include "sequentialfile.h"
#include "record.h"
#include <algorithm>
#include <cstring>
using namespace std;

template <class RecordT, class KeyT>
SequentileFile<RecordT, KeyT>::SequentileFile(const string& data_file_name, function<KeyT(RecordT*)> keyAccessor) {
    this->data_file_name = data_file_name;
    this->keyAccessor = keyAccessor;
}

template <class RecordT, class KeyT>
RecordT* SequentileFile<RecordT, KeyT>::readRecord(int pos) {
    ifstream file(this->data_file_name, ios::binary);
    if (!file.is_open()) {
        cout << "Error al abrir el archivo1" << endl;
        return nullptr;
    }
    file.seekg(sizeof(int) + pos*sizeof(RecordT));
    RecordT* record = new RecordT();
    file.read(reinterpret_cast<char*>(record), sizeof(RecordT));
    file.close();
    return record;
}

template <class RecordT, class KeyT>
void SequentileFile<RecordT, KeyT>::writeRecord(RecordT* record, int pos) {
    fstream file(this->data_file_name, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        // Si no existe, crearlo y abrirlo
        file.open(this->data_file_name, ios::binary | ios::out);
        if (!file.is_open()) {
            cout << "Error al abrir o crear el archivo" << endl;
            return;
        }
    }
    file.seekp(sizeof(int) + pos * sizeof(RecordT));
    file.write(reinterpret_cast<char*>(record), sizeof(RecordT));
    file.close();
}


template <class RecordT, class KeyT>
int SequentileFile<RecordT, KeyT>::readSize() {
    ifstream file(this->data_file_name, ios::binary);
    if (!file.is_open()) {
        cout << "Error al abrir el archivo3" << endl;
        return -1;
    }
    int size;
    file.seekg(0);
    file.read(reinterpret_cast<char*>(&size), sizeof(int));
    file.close();
    return size;
}

template <class RecordT, class KeyT>
void SequentileFile<RecordT, KeyT>::writeSize(int size) {
    fstream file(this->data_file_name, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        file.open(this->data_file_name, ios::binary | ios::out);
    }
    file.seekp(0, ios::beg);
    file.write(reinterpret_cast<char*>(&size), sizeof(int));
    file.close();
}

template <class RecordT, class KeyT>
int SequentileFile<RecordT, KeyT>::getFileSize() {
    ifstream file(this->data_file_name, ios::binary);
    if (!file.is_open()) {
        cout << "Error al abrir el archivo5" << endl;
        return -1;
    }
    file.seekg(0, ios::end);
    int size = file.tellg();
    file.close();
    return (size-sizeof(int))/sizeof(RecordT);
}

template <class RecordT, class KeyT>
void SequentileFile<RecordT, KeyT>::init(const string& data_name_csv) {
    //--------------inicializar el dataFile ----------------
    vector<RecordT*> records;
    ifstream data_file(data_name_csv);
    if (!data_file.is_open()) {
        cout << "Error al abrir el archivo6" << endl;
        return;
    }
    string line;
    getline(data_file, line);  // Leer la primera línea (encabezados)
    int i = 1;
    while(data_file.peek() != EOF && records.size() < limit*3) {
        getline(data_file, line);
        stringstream ss(line);
        string item;
        vector<string> items;
        while (getline(ss, item, ',')) {
            items.push_back(item);
        }
        RecordT* record = new RecordT(items);
        record->next_pos = i;
        records.push_back(record);
        i++;
    }
    getline(data_file, line);
    stringstream ss(line);
    string item;
    vector<string> items;
    while (getline(ss, item, ',')) {
        items.push_back(item);
    }
    RecordT* record = new RecordT(items);
    records.push_back(record);

    //ordenar vector
    sort(records.begin(), records.end(), [this](RecordT* a, RecordT* b) {
        return keyAccessor(a) < keyAccessor(b);
    });
    data_file.close();

    //escribir en el archivo binario
    cout <<"aa:" <<records.size() << endl;
    writeSize(records.size());
    i = 0;
    for (RecordT* record : records) {
        writeRecord(record, i);
        i++;
    }
    //--------------usando add ----------------

}

template <class RecordT, class KeyT>
bool SequentileFile<RecordT, KeyT>::add(RecordT* record) {

    return true;
}

template <class RecordT, class KeyT>
RecordT* SequentileFile<RecordT, KeyT>::search(KeyT key) {
    //leer size
    int size = readSize();
    //buscar en los los restantes (size, end) 
    int l = size;
    int u = getFileSize()-1;
    while (u>=l) {
        int mid = (l+u)/2;
        RecordT* record = readRecord(mid);
        if (keyAccessor(record) < key)
            l = mid + 1;
        else if (keyAccessor(record) > key)
            u = mid - 1;
        else
            if(record->next_pos != -2) return record;
        delete record;
    }
    //buscar en los size primeros registros (0, size-1)
    l = 0;
    u = size - 1;
    while (u>=l) {
        int mid = (l+u)/2;
        RecordT* record = readRecord(mid);
        if (keyAccessor(record) < key)
            l = mid + 1;
        else if (keyAccessor(record) > key)
            u = mid - 1;
        else
            if(record->next_pos != -2) return record;
        delete record;
    }
    return nullptr;   
}


int main() {
    SequentileFile<RecordMeet, int> meetFile("meets.dat",
                                             [](RecordMeet* record) { return record->MeetID; });

    //SequentileFile<RecordMeet, string> meetFile2("meets2.dat",
                                            // [](RecordMeet* record) { return record->Date; });
    cout <<"a:" <<meetFile.getFileSize() << endl;
    if(meetFile.getFileSize() == 0)
        meetFile.init("datasets/meets.csv");

    //lee el archivo binario
    ifstream file("meets.dat", ios::binary);
    if (!file.is_open()) {
        cout << "Error al abrir el archivo" << endl;
        return 1;
    }
    RecordMeet meet;
    file.seekg(sizeof(int));
    while (file.read(reinterpret_cast<char*>(&meet), sizeof(RecordMeet))) {
        cout << meet.MeetID << " " << meet.MeetPath << " " << meet.Federation << " " << meet.Date << " " << meet.MeetCountry << " " << meet.MeetState << " " << meet.MeetTown << " " << meet.MeetName << endl;
    }
    file.close();

    //buscar
    RecordMeet* meet2 = meetFile.search(5);

    if (meet2 != nullptr) {
        cout<<"Encontrado"<<endl;
        cout << meet2->MeetID << " " << meet2->MeetPath << " " << meet2->Federation << " " << meet2->Date << " " << meet2->MeetCountry << " " << meet2->MeetState << " " << meet2->MeetTown << " " << meet2->MeetName << endl;
    } else {
        cout << "No se encontró el registro" << endl;
    }

    
    return 0;
}

