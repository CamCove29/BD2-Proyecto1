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
        //writeSize(size+1);
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
RecordT* SequentileFile<RecordT, KeyT>::search_aux(KeyT key, int& mid, int& f_minor) {
    //leer size
    int size = readSize();
    //buscar en los size primeros registros (0, size-1)
    int l = 0;
    int u = size - 1;
    while (u>=l) {
        mid = (l+u)/2;
        RecordT* record = readRecord(mid);
        if (keyAccessor(record) < key)
            l = mid + 1;
        else if (keyAccessor(record) > key)
            u = mid - 1;
        else
            if(record->next_pos != -2) return record;
            else return nullptr;
        delete record;
    }
    f_minor = u;
    //buscar en los los restantes (size, end)
    cout<<"busco en los restantes"<<endl;
    RecordT* record = readRecord(u);
    while (record->next_pos != -1) {
        if (keyAccessor(record) == key && record->next_pos != -2) {
            mid = u;
            return record;
        }
        u = record->next_pos;
        delete record;
        record = readRecord(u);
    }
    //cout <<"mid: "<< mid << endl;
    //cout <<"l: "<< l << endl;
    //cout <<"u: "<< u << endl; //inmediatamente menor
    return nullptr;   
}

template <class RecordT, class KeyT>
RecordT* SequentileFile<RecordT, KeyT>::search(KeyT key) {
    int mid=-1;
    int f_minor=-2;
    RecordT* record = search_aux(key, mid, f_minor);
    if (record != nullptr) {
        cout << "Encontrado" << endl;
        cout << record->MeetID << " " << record->MeetPath << " " << record->Federation << " " << record->Date << " " << record->MeetCountry << " " << record->MeetState << " " << record->MeetTown << " " << record->MeetName << endl;
    } else {
        cout << "No se encontró el registro" << endl;
    }
    return record;
}

template <class RecordT, class KeyT>
bool SequentileFile<RecordT, KeyT>::remove(KeyT key) {
    int my_pos = -1;
    int f_minor = -2;
    RecordT* eliminado = search_aux(key, my_pos, f_minor);

    cout << "registro a eliminar: "
            << " " << eliminado->MeetID << " " << eliminado->MeetPath << " " << eliminado->Federation << " " << eliminado->Date << " " << eliminado->MeetCountry << " " << eliminado->MeetState << " " << eliminado->MeetTown << " " << eliminado->MeetName << " " << eliminado->next_pos << endl;
    
    if (eliminado == nullptr) {
        cout << "No se encontró el registro" << endl;
        return false;
    }
    
    if (eliminado->next_pos == -2) {
        cout << "Registro ya eliminado" << endl;
        delete eliminado; // Limpiar
        return false;
    }

    //¿Where is prev?
    int size = readSize();
    cout <<"size: "<<size << endl;
    cout <<"my_pos: "<<my_pos << endl; 
    int fileSize = getFileSize();
    if (my_pos < size) {
        cout << "eliminado is in the n first records" << endl;
        //eliminado is int the n first records
        int prev_pos = my_pos;
        RecordT* prev = eliminado;
        while (prev->next_pos != my_pos) {
            if(prev->next_pos != -2 && prev_pos != my_pos) {
                //buscar prev en k restantes
                cout << "1"<<endl;
                for(int i = size; i < fileSize; i++){
                    RecordT* record = readRecord(i);
                    if(record->next_pos == my_pos){
                        //descuelgo
                        record->next_pos = eliminado->next_pos;
                        eliminado->next_pos = -2;
                        writeRecord(eliminado, my_pos);
                        writeRecord(record, i);
                        delete record;
                        return true;
                    }
                    delete record;
                }
                return false;
            }
            if(prev_pos == 0){
                cout << "2" <<endl;
                int first = readFirstPos();
                if(first == my_pos){
                    //descuelgo
                    writeFirstPos(eliminado->next_pos);
                    eliminado->next_pos = -2;
                    writeRecord(eliminado, my_pos);
                } else {
                    //busco prev en los k restantes
                    for(int i = size; i < fileSize; i++){
                        RecordT* record = readRecord(i);
                        if(record->next_pos == my_pos){
                            //descuelgo
                            record->next_pos = eliminado->next_pos;
                            eliminado->next_pos = -2;
                            writeRecord(eliminado, my_pos);
                            writeRecord(record, i);
                            delete record;
                            return true;
                        }
                        delete record;
                    }
                    return false;
                }
            }
            cout << "88" <<endl;
            delete prev;
            prev_pos = prev_pos - 1;
            prev = readRecord(prev_pos); 
        }
        //consegui prev
        cout << "7" <<endl;
        prev->next_pos = eliminado->next_pos;
        eliminado->next_pos = -2;
        writeRecord(eliminado, my_pos);
        writeRecord(prev, prev_pos);
        RecordT* prev2 = readRecord(my_pos);
        cout << "leer: " << prev2->MeetID << " " << prev2->MeetPath << " " << prev2->Federation << " " << prev2->Date << " " << prev2->MeetCountry << " " << prev2->MeetState << " " << prev2->MeetTown << " " << prev2->MeetName << " " << prev2->next_pos << endl;
        delete prev;
        return true;

    } else {
        cout << "eliminado is in the k rest of records" << endl;    
        //eliminado is in the k rest of records
        //buscar prev en k restantes
        for(int i = size; i < fileSize; i++){
            RecordT* record = readRecord(i);
            if(record->next_pos == my_pos){
                cout << "9" <<endl;
                //descuelgo
                record->next_pos = eliminado->next_pos;
                eliminado->next_pos = -2;
                writeRecord(eliminado, my_pos);
                writeRecord(record, i);
                delete record;
                return true;
            }
            delete record;
        }
        //buscar prev en los n primeros
        int prev_pos = f_minor;
        RecordT* prev = readRecord(prev_pos);
        cout << "10" <<endl;

        while(prev->next_pos != my_pos){
            if(prev_pos == 0){
                int first = readFirstPos();
                if(first == my_pos){
                    //descuelgo
                    writeFirstPos(eliminado->next_pos);
                    eliminado->next_pos = -2;
                    writeRecord(eliminado, my_pos);
                } 
            }
            delete prev;
            prev_pos = prev_pos - 1;
            prev = readRecord(prev_pos);
        }
        //consegui prev
        prev->next_pos = eliminado->next_pos;
        eliminado->next_pos = -2;
        writeRecord(eliminado, my_pos);
        writeRecord(prev, prev_pos);
        cout << "11" <<endl;
        return true;
    }
}


template <class RecordT, class KeyT>
vector<RecordT*> SequentileFile<RecordT, KeyT>::range_search(KeyT start_key, KeyT end_key) {
    vector<RecordT*> records;
    int mid = -1;
    int f_minor = -2;
    RecordT* record = search_aux(start_key, mid, f_minor);
    if (record == nullptr) {
        cout << "No se encontró el registro" << endl;
        return records;
    }
    cout << "empieza: "<<endl;
    while (keyAccessor(record) <= end_key) {
        cout << record->MeetID << " " << record->MeetPath << " ";
        records.push_back(record);
        if (record->next_pos == -1) {
            break;
        }
        record = readRecord(record->next_pos);
    }
    cout << "termina: "<<endl;
    for (RecordT* record : records) {
        cout << record->MeetID << " " << record->MeetPath << " " << record->Federation << " " << record->Date << " " << record->MeetCountry << " " << record->MeetState << " " << record->MeetTown << " " << record->MeetName << " " << record->next_pos << endl;
    }
    return records;
}




int main() {
    SequentileFile<RecordMeet, int> meetFile("meets.dat",
                                             [](RecordMeet* record) { return record->MeetID; });

    //SequentileFile<RecordMeet, string> meetFile2("meets2.dat",
                                            // [](RecordMeet* record) { return record->Date; });
    cout <<"a:" <<meetFile.getFileSize() << endl;
    if(meetFile.getFileSize() <= 0)
        meetFile.init("datasets/meets.csv");

    //meetFile.rebuild();

    cout << "eliminando -------------------------------------------------"<<endl;
    cout<<"eliminado ? "<<meetFile.remove(17);
    cout<<"eliminado ? "<<meetFile.remove(19);
    cout<<"eliminado ? "<<meetFile.remove(1); //0
    RecordMeet* meet2 = meetFile.search(10);

    cout <<"lentura de archivo-----------------------------------------" << endl;
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
    int hola = 0;
    cout << "size: " << meetFile.getFileSize() <<endl;
    cout << "first_pos: " << fp << endl;
    while (file.read(reinterpret_cast<char*>(&meet), sizeof(RecordMeet))) {
        cout <<hola<<" -> "<< meet.MeetID << " " << meet.MeetPath << " " << meet.Federation << " " << meet.Date << " " << meet.MeetCountry << " " << meet.MeetState << " " << meet.MeetTown << " " << meet.MeetName << " " << meet.next_pos << endl;
        hola++;
    }
    file.close();
    RecordMeet* meet3 = meetFile.readRecord(16);
    cout << meet3->MeetID << " " << meet3->MeetPath << " " << meet3->Federation << " " << meet3->Date << " " << meet3->MeetCountry << " " << meet3->MeetState << " " << meet3->MeetTown << " " << meet3->MeetName << " " << meet3->next_pos << endl;
    cout <<"--------------busqueda por rango-------------------"<<endl;
    vector<RecordMeet*> meets = meetFile.range_search(10, 20);
    cout <<"aaaaaaaa"<<endl;
    cout <<meets.size()<<endl;
    for (RecordMeet* meet : meets) {
        cout << meet->MeetID << " " << meet->MeetPath << " " << meet->Federation << " " << meet->Date << " " << meet->MeetCountry << " " << meet->MeetState << " " << meet->MeetTown << " " << meet->MeetName << " " << meet->next_pos << endl;
    }
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

