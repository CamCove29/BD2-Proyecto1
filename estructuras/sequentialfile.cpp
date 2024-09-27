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


template <class RecordT>
bool SequentileFile<RecordT>::compare(std::string data1, std::string data2, std::string op){
    if (data_type == "int") {
        int data1_int = stoi(data1);
        int data2_int = stoi(data2);
        if (op == "<") {
            return data1_int < data2_int;
        } else if (op == "<=") {
            return data1_int <= data2_int;
        } else if (op == ">") {
            return data1_int > data2_int;
        } else if (op == ">=") {
            return data1_int >= data2_int;
        } else if (op == "==") {
            return data1_int == data2_int;
        } else if (op == "!=") {
            return data1_int != data2_int;
        }
    } else if (data_type == "string" || data_type == "nonetype") { //cuando no tiene tipo se asume que es string
        if (op == "<") {
            return data1 < data2;
        } else if (op == "<=") {
            return data1 <= data2;
        } else if (op == ">") {
            return data1 > data2;
        } else if (op == ">=") {
            return data1 >= data2;
        } else if (op == "==") {
            return data1 == data2;
        } else if (op == "!=") {
            return data1 != data2;
        }
    }

    return false;
}

template <class RecordT>
std::string SequentileFile<RecordT>::get_type_from_string(std::string data){
    try {
        int data_int = stoi(data);
        return "int";
    } catch (const std::invalid_argument& ia) {
        return "string";
    }

    return "nonetype";
}

template <class RecordT>
SequentileFile<RecordT>::SequentileFile(const string& data_file_name, std::string key_name) {
    this->data_file_name = data_file_name;

    this->key_name = key_name;

    this->keyAccessor = [key_name](RecordT* record) {
        std::pair<std::string, std::string> result = record->getStringAttribute(key_name);
        return result.second;
    };
}

template <class RecordT>
RecordT* SequentileFile<RecordT>::readRecord(int pos) {
    ifstream file(this->data_file_name, ios::binary);
    if (!file.is_open()) {
        //cout << "Error al abrir el archivo1" << endl;
        return nullptr;
    }
    file.seekg(2*sizeof(int) + pos*sizeof(RecordT));
    RecordT* record = new RecordT();
    file.read(reinterpret_cast<char*>(record), sizeof(RecordT));
    file.close();
    return record;
}

template <class RecordT>
void SequentileFile<RecordT>::writeRecord(RecordT* record, int pos) {
    fstream file(this->data_file_name, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        // Si no existe, crearlo y abrirlo
        file.open(this->data_file_name, ios::binary | ios::out);
        if (!file.is_open()) {
            //cout << "Error al abrir o crear el archivo" << endl;
            return;
        }
    }
    file.seekp(2*sizeof(int) + pos * sizeof(RecordT));
    file.write(reinterpret_cast<char*>(record), sizeof(RecordT));
    file.close();
}

template <class RecordT>
int SequentileFile<RecordT>::readFirstPos() {
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

template <class RecordT>
void SequentileFile<RecordT>::writeFirstPos(int pos) {
    fstream file(this->data_file_name, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        file.open(this->data_file_name, ios::binary | ios::out);
    }
    file.seekp(sizeof(int));
    file.write(reinterpret_cast<char*>(&pos), sizeof(int));
    file.close();
}


template <class RecordT>
int SequentileFile<RecordT>::readSize() {
    ifstream file(this->data_file_name, ios::binary);
    if (!file.is_open()) {
        //cout << "Error al abrir el archivo3" << endl;
        return -1;
    }
    int size;
    file.seekg(0);
    file.read(reinterpret_cast<char*>(&size), sizeof(int));
    file.close();
    return size;
}

template <class RecordT>
void SequentileFile<RecordT>::writeSize(int size) {
    fstream file(this->data_file_name, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        file.open(this->data_file_name, ios::binary | ios::out);
    }
    file.seekp(0, ios::beg);
    file.write(reinterpret_cast<char*>(&size), sizeof(int));
    file.close();
}

template <class RecordT>
int SequentileFile<RecordT>::getFileSize() {
    ifstream file(this->data_file_name, ios::binary);
    if (!file.is_open()) {
        //cout << "Error al abrir el archivo5" << endl;
        return -1;
    }
    file.seekg(0, ios::end);
    int size = file.tellg();
    file.close();
    return (size-2*sizeof(int))/sizeof(RecordT);
}

template <class RecordT>
void SequentileFile<RecordT>::init(const string& data_name_csv) {
    // inicializar el data_type
    //revisar la columna de key_name en el csv y determinar si es int o string
    ifstream csv_file_helper(data_name_csv);
    if (!csv_file_helper.is_open()) {
        cout << "Error al abrir el archivo" << endl;
        exit(1);
    }

    //leer las dos primeras lineas, un encabezado y un registro. El encabezado es para saber que columna es key_name
    //el registro para obtener elvalor y preguntar si es int o string
    string encabezado, registro;
    getline(csv_file_helper, encabezado);
    getline(csv_file_helper, registro);

    /* std::cout << "encabezado: " << encabezado << std::endl;
    std::cout << "registro: " << registro << std::endl;*/
    stringstream ss_encabezado(encabezado);
    stringstream ss_registro(registro);

    string item_encabezado, item_registro;
    vector<string> items_encabezado;
    vector<string> items_registro;

    while (getline(ss_encabezado, item_encabezado, ',')) {
        items_encabezado.push_back(item_encabezado);
    }

    while (getline(ss_registro, item_registro, ',')) {
        items_registro.push_back(item_registro);
    }

    int i = 0;
    for (string item : items_encabezado) {
        if (item == key_name) {
            data_type = get_type_from_string(items_registro[i]);
            break;
        }
        i++;
    }

    csv_file_helper.close();



    //--------------inicializar el dataFile ----------------
    vector<RecordT*> records;
    ifstream data_file(data_name_csv);
    if (!data_file.is_open()) {
        //cout << "Error al abrir el archivo6" << endl;
        return;
    }
    string line;
    getline(data_file, line);  // Leer la primera línea (encabezados)
    i = 1;
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
        return compare(keyAccessor(a), keyAccessor(b), "<");
    });

    for (int i = 0; i < records.size()-1; i++) {
        records[i]->next_pos = i+1;
    }
    records[records.size()-1]->next_pos = -1;

    //escribir en el archivo binario
    //cout <<"aa:" <<records.size() << endl;
    writeSize(records.size());
    writeFirstPos(0);
    i = 0;
    for (RecordT* record : records) {
        writeRecord(record, i);
        i++;
    }
    //leer el ultimo record
    //cout <<"ultimo record" << endl;
    RecordT* record2 = readRecord(records.size()-1);
    //cout <<record2->MeetID<<" "<<record2->MeetPath<<" "<<record2->Federation<<" "<<record2->Date<<" "<<record2->MeetCountry<<" "<<record2->MeetState<<" "<<record2->MeetTown<<" "<<record2->MeetName<<" "<<record2->next_pos<<endl;
    //--------------usando add ----------------
    for (int i = 0; i < 8; i++) {
        //seguir leyendo csv
        //cout <<"b:" <<i << endl;
        getline(data_file, line);
        stringstream ss(line);
        string item;
        vector<string> items;
        while (getline(ss, item, ',')) {
            items.push_back(item);
        }
        RecordT* record = new RecordT(items);
        //cout <<"intentando agregar "<< endl;
        //cout <<record->MeetID<<" "<<record->MeetPath<<" "<<record->Federation<<" "<<record->Date<<" "<<record->MeetCountry<<" "<<record->MeetState<<" "<<record->MeetTown<<" "<<record->MeetName<<" "<<record->next_pos<<endl;
    
        add(record);
        delete record;

    }
    data_file.close();
}

template <class RecordT>
bool SequentileFile<RecordT>::add(RecordT* record) {
    if (pseudo_add(record)) {
        int size = readSize();
        //writeSize(size+1);
        /*
        if(getFileSize()-size > log2(size)) {
            rebuild();
        }
        */
        //cout << "Registro agregado" << endl;
        return true;
    }
    return false;
}

template<typename RecordT>
bool SequentileFile<RecordT>::pseudo_add(RecordT* record) {
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
        
        if (compare(keyAccessor(current), keyAccessor(record), "<")) {
            l = mid + 1;
        } else if (compare(keyAccessor(current), keyAccessor(record), ">")) {
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
        
        if (compare(keyAccessor(current), keyAccessor(record), ">")) {
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

template <class RecordT>
bool SequentileFile<RecordT>::rebuild(){
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

template <class RecordT>
RecordT* SequentileFile<RecordT>::search_aux(std::string key, int& mid, int& f_minor) {
    //leer size
    int size = readSize();
    //buscar en los size primeros registros (0, size-1)
    int l = 0;
    int u = size - 1;
    while (u>=l) {
        mid = (l+u)/2;
        RecordT* record = readRecord(mid);
        if (compare(keyAccessor(record), key, "<"))
            l = mid + 1;
        else if (compare(keyAccessor(record), key, ">"))
            u = mid - 1;
        else
            if(record->next_pos != -2) return record;
            else return nullptr;
        delete record;
    }
    f_minor = u;
    //buscar en los los restantes (size, end)
    //cout<<"busco en los restantes"<<endl;
    RecordT* record = readRecord(u);
    while (record->next_pos != -1) {
        if (compare(keyAccessor(record), key, "==")
        && record->next_pos != -2) {
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

template <class RecordT>
RecordT* SequentileFile<RecordT>::search(std::string key) {
    int mid=-1;
    int f_minor=-2;
    RecordT* record = search_aux(key, mid, f_minor);
    if (record != nullptr) {
        //cout << "Encontrado" << endl;
        //cout << record->MeetID << " " << record->MeetPath << " " << record->Federation << " " << record->Date << " " << record->MeetCountry << " " << record->MeetState << " " << record->MeetTown << " " << record->MeetName << endl;
    } else {
        //cout << "No se encontró el registro" << endl;
    }
    return record;
}

template <class RecordT>
bool SequentileFile<RecordT>::remove(std::string key) {
    int my_pos = -1;
    int f_minor = -2;
    RecordT* eliminado = search_aux(key, my_pos, f_minor);

    //cout << "registro a eliminar: "<< " " << eliminado->MeetID << " " << eliminado->MeetPath << " " << eliminado->Federation << " " << eliminado->Date << " " << eliminado->MeetCountry << " " << eliminado->MeetState << " " << eliminado->MeetTown << " " << eliminado->MeetName << " " << eliminado->next_pos << endl;
    
    if (eliminado == nullptr) {
        //cout << "No se encontró el registro" << endl;
        return false;
    }
    
    if (eliminado->next_pos == -2) {
        //cout << "Registro ya eliminado" << endl;
        delete eliminado; // Limpiar
        return false;
    }

    //¿Where is prev?
    int size = readSize();
    //cout <<"size: "<<size << endl;
    //cout <<"my_pos: "<<my_pos << endl; 
    int fileSize = getFileSize();
    if (my_pos < size) {
        //cout << "eliminado is in the n first records" << endl;
        //eliminado is int the n first records
        int prev_pos = my_pos;
        RecordT* prev = eliminado;
        while (prev->next_pos != my_pos) {
            if(prev->next_pos != -2 && prev_pos != my_pos) {
                //buscar prev en k restantes
                //cout << "1"<<endl;
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
                //cout << "2" <<endl;
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
            //cout << "88" <<endl;
            delete prev;
            prev_pos = prev_pos - 1;
            prev = readRecord(prev_pos); 
        }
        //consegui prev
        //cout << "7" <<endl;
        prev->next_pos = eliminado->next_pos;
        eliminado->next_pos = -2;
        writeRecord(eliminado, my_pos);
        writeRecord(prev, prev_pos);
        RecordT* prev2 = readRecord(my_pos);
        //cout << "leer: " << prev2->MeetID << " " << prev2->MeetPath << " " << prev2->Federation << " " << prev2->Date << " " << prev2->MeetCountry << " " << prev2->MeetState << " " << prev2->MeetTown << " " << prev2->MeetName << " " << prev2->next_pos << endl;
        delete prev;
        return true;

    } else {
        //cout << "eliminado is in the k rest of records" << endl;    
        //eliminado is in the k rest of records
        //buscar prev en k restantes
        for(int i = size; i < fileSize; i++){
            RecordT* record = readRecord(i);
            if(record->next_pos == my_pos){
                //cout << "9" <<endl;
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
        //cout << "10" <<endl;

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
        //cout << "11" <<endl;
        return true;
    }
}



template <class RecordT>
vector<RecordT*> SequentileFile<RecordT>::range_search(std::string start_key, std::string end_key) {
    vector<RecordT*> records;
    int mid = -1;
    int f_minor = -2;
    RecordT* record = search_aux(start_key, mid, f_minor);
    if (record == nullptr) {
        //cout << "No se encontró el registro" << endl;
        return records;
    }
    //cout << "empieza: "<<endl;
    while (compare(keyAccessor(record), end_key, "<=")) {
        //cout << record->MeetID << " " << record->MeetPath << " ";
        records.push_back(record);
        if (record->next_pos == -1) {
            break;
        }
        record = readRecord(record->next_pos);
    }
    //cout << "termina: "<<endl;
    for (RecordT* record : records) {
        //cout << record->MeetID << " " << record->MeetPath << " " << record->Federation << " " << record->Date << " " << record->MeetCountry << " " << record->MeetState << " " << record->MeetTown << " " << record->MeetName << " " << record->next_pos << endl;
    }
    return records;
}
