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
#include <cmath>
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
    file.seekg(2*sizeof(int) + pos*sizeof(RecordT));
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
    file.seekp(2*sizeof(int) + pos * sizeof(RecordT));
    file.write(reinterpret_cast<char*>(record), sizeof(RecordT));
    file.close();
}

template <class RecordT, class KeyT>
int SequentileFile<RecordT, KeyT>::readFirstPos() {
    ifstream file(this->data_file_name, ios::binary);
    if (!file.is_open()) {
        return -1;
    }
    int first_pos;
    file.seekg(sizeof(int));
    file.read(reinterpret_cast<char*>(&first_pos), sizeof(int));
    file.close();
    return first_pos;
}

template <class RecordT, class KeyT>
void SequentileFile<RecordT, KeyT>::writeFirstPos(int pos) {
    fstream file(this->data_file_name, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        file.open(this->data_file_name, ios::binary | ios::out);
    }
    file.seekp(sizeof(int));
    file.write(reinterpret_cast<char*>(&pos), sizeof(int));
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
    return (size-2*sizeof(int))/sizeof(RecordT);
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

    for (int i = 0; i < records.size()-1; i++) {
        records[i]->next_pos = i+1;
    }
    records[records.size()-1]->next_pos = -1;

    //escribir en el archivo binario
    cout <<"aa:" <<records.size() << endl;
    writeSize(records.size());
    writeFirstPos(0);
    i = 0;
    for (RecordT* record : records) {
        writeRecord(record, i);
        i++;
    }
    //leer el ultimo record
    cout <<"ultimo record" << endl;
    RecordT* record2 = readRecord(records.size()-1);
    cout <<record2->MeetID<<" "<<record2->MeetPath<<" "<<record2->Federation<<" "<<record2->Date<<" "<<record2->MeetCountry<<" "<<record2->MeetState<<" "<<record2->MeetTown<<" "<<record2->MeetName<<" "<<record2->next_pos<<endl;
    //--------------usando add ----------------
    for (int i = 0; i < 8; i++) {
        //seguir leyendo csv
        cout <<"b:" <<i << endl;
        getline(data_file, line);
        stringstream ss(line);
        string item;
        vector<string> items;
        while (getline(ss, item, ',')) {
            items.push_back(item);
        }
        RecordT* record = new RecordT(items);
        cout <<"intentando agregar "<< endl;
        cout <<record->MeetID<<" "<<record->MeetPath<<" "<<record->Federation<<" "<<record->Date<<" "<<record->MeetCountry<<" "<<record->MeetState<<" "<<record->MeetTown<<" "<<record->MeetName<<" "<<record->next_pos<<endl;
    
        add(record);
        delete record;

    }
    data_file.close();
}

template <class RecordT, class KeyT>
bool SequentileFile<RecordT, KeyT>::add(RecordT* record) {
    if (pseudo_add(record)) {
        int size = readSize();
        writeSize(size+1);
        /*
        if(getFileSize()-size > log2(size)) {
            rebuild();
        }
        */
        cout << "Registro agregado" << endl;
        return true;
    }
    return false;
}

template<typename RecordT, typename KeyAccessor>
bool SequentileFile<RecordT, KeyAccessor>::pseudo_add(RecordT* record) {
    int l = 0;
    int u = readSize() - 1;
    int mid;
    
    // Binary search to find insertion point
    while (u >= l) {
        mid = (l + u) / 2;
        RecordT* current = readRecord(mid);
        if (current == nullptr) {
            std::cerr << "Error reading record at position " << mid << std::endl;
            return false;
        }
        
        if (keyAccessor(current) < keyAccessor(record)) {
            l = mid + 1;
        } else if (keyAccessor(current) > keyAccessor(record)) {
            u = mid - 1;
        } else {
            delete current;
            return false; // Duplicate key, insertion failed
        }
        delete current;
    }
    
    // Handle insertion at the beginning
    if (u == -1) {
        record->next_pos = readFirstPos();
        writeFirstPos(getFileSize());
        writeRecord(record, getFileSize());
        return true;
    }
    
    // Find the correct insertion point, skipping deleted records
    RecordT* prev = readRecord(u);
    while (prev->next_pos != -1) {
        RecordT* current = readRecord(prev->next_pos);
        if (current == nullptr) {
            delete prev;
            return false;
        }
        
        if (current->next_pos == -2) {
            // Skip deleted record
            int next_pos = current->next_pos;
            delete current;
            prev->next_pos = next_pos;
            writeRecord(prev, u);
            continue;
        }
        
        if (keyAccessor(current) > keyAccessor(record)) {
            // Insert between prev and current
            record->next_pos = prev->next_pos;
            prev->next_pos = getFileSize();
            writeRecord(record, getFileSize());
            writeRecord(prev, u);
            delete current;
            delete prev;
            return true;
        }
        
        // Move to next record
        u = prev->next_pos;
        delete prev;
        prev = current;
    }
    
    // Insert at the end of the chain
    record->next_pos = -1;
    prev->next_pos = getFileSize();
    writeRecord(record, getFileSize());
    writeRecord(prev, u);
    delete prev;
    return true;
}

    template <class RecordT, class KeyT>
    bool SequentileFile<RecordT, KeyT>::rebuild(){
        int first = readFirstPos();
        int newSz = getFileSize();
        RecordT* record = readRecord(first);
        string name = this->data_file_name + "_temp";
        ofstream temp_file(name, ios::binary);
        temp_file.write(reinterpret_cast<char*>(&newSz), sizeof(int));
        temp_file.write(reinterpret_cast<char*>(&first), sizeof(int));
        int i = 1;
        while(record->next_pos != -1){
            record->next_pos = i;
            temp_file.write(reinterpret_cast<char*>(record), sizeof(RecordT));
            delete record;
            record = readRecord(record->next_pos);
            i++;
        }
        record->next_pos = -1;
        temp_file.write(reinterpret_cast<char*>(record), sizeof(RecordT));
        delete record;
        temp_file.close();
        std::remove(this->data_file_name.c_str());
        std::rename(name.c_str(), this->data_file_name.c_str());
        return true;
    }

template <class RecordT, class KeyT>
RecordT* SequentileFile<RecordT, KeyT>::search(KeyT key) {
    //leer size
    int size = readSize();
    //buscar en los los restantes (size, end) 
    int l = size;
    int u = getFileSize()-1;
    int mid;
    while (u>=l) {
        mid = (l+u)/2;
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
        mid = (l+u)/2;
        RecordT* record = readRecord(mid);
        if (keyAccessor(record) < key)
            l = mid + 1;
        else if (keyAccessor(record) > key)
            u = mid - 1;
        else
            if(record->next_pos != -2) return record;
        delete record;
    }
    //cout <<"mid: "<< mid << endl;
    //cout <<"l: "<< l << endl;
    //cout <<"u: "<< u << endl; //inmediatamente menor
    return nullptr;   
}


int main() {
    SequentileFile<RecordMeet, int> meetFile("meets.dat",
                                             [](RecordMeet* record) { return record->MeetID; });

    //SequentileFile<RecordMeet, string> meetFile2("meets2.dat",
                                            // [](RecordMeet* record) { return record->Date; });
    cout <<"a:" <<meetFile.getFileSize() << endl;
    if(meetFile.getFileSize() <= 0)
        meetFile.init("datasets/meets.csv");

    meetFile.rebuild();

    //lee el archivo binario
    ifstream file("meets.dat", ios::binary);
    if (!file.is_open()) {
        cout << "Error al abrir el archivo" << endl;
        return 1;
    }
    RecordMeet meet;
    int sz;
    file.read(reinterpret_cast<char*>(&sz), sizeof(int));
    int fp;
    file.read(reinterpret_cast<char*>(&fp), sizeof(int));
    cout << "size: " << sz << endl;
    cout << "first_pos: " << fp << endl;
    while (file.read(reinterpret_cast<char*>(&meet), sizeof(RecordMeet))) {
        cout << meet.MeetID << " " << meet.MeetPath << " " << meet.Federation << " " << meet.Date << " " << meet.MeetCountry << " " << meet.MeetState << " " << meet.MeetTown << " " << meet.MeetName << " " << meet.next_pos << endl;
    }
    file.close();
    /*
    //buscar
    RecordMeet* meet2 = meetFile.search(9);
    RecordMeet* meet3 = meetFile.search(5);
    RecordMeet* meet4 = meetFile.search(-1);

    if (meet2 != nullptr) {
        cout<<"Encontrado"<<endl;
        cout << meet2->MeetID << " " << meet2->MeetPath << " " << meet2->Federation << " " << meet2->Date << " " << meet2->MeetCountry << " " << meet2->MeetState << " " << meet2->MeetTown << " " << meet2->MeetName << endl;
    } else {
        cout << "No se encontró el registro" << endl;
    }
    */
    //rebuild


    
    return 0;
}

