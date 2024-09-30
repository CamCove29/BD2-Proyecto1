#ifndef SEQUENTIALFILE_H 
#define SEQUENTIALFILE_H

#define buffer_size 1024

#include "indextype.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cmath>


using namespace std;

template <class RecordT>
class SequentileFile : public IndexType<RecordT> { 
private:
    //MELIS WORK
    int limit = buffer_size/sizeof(RecordT); // limite de registros en el buffer

    RecordT* readRecord(int pos);  // lee un registro en la posicion pos
    void writeRecord(RecordT* record, int pos); // escribe un registro en la posicion pos
    int readFirstPos(); // lee la pos regitro con menor key (posición lógica)
    void writeFirstPos(int pos); // escribe la pos regitro con menor key (posición lógica)
    int readSize(); // lee la cantidad de records del espacio ordenado
    void writeSize(int size); // escribe la cantidad de records del espacio ordenado
    int getFileSize(); // lee la cantidad de records totales

    bool pseudo_add(RecordT* record); //helper para hacer el add
    RecordT* search_aux(std::string key, int& mid, int& f_minor); //otro helper pero pal search
    bool rebuild(); // reconstruye el archivo binario para que el espacio ordenado sea el 100% del archivo

public:
    SequentileFile(std::string data_file_name, std::string key_name);  //constructor: recibe el nombre del archivo y la lambda para acceder a la clave indexada de un registro
    void init(std::string data_name_csv); //* carga el csv en el archivo binario, ordena en ram una parte, usa add
    bool add(RecordT* record); //*agrega un registro
    bool remove(std::string key); //*elimina un registro
    RecordT* search(std::string key); //*busca un registro
    vector<RecordT*> range_search(std::string start_key, std::string end_key); // busca un rango de registros
};



template <class RecordT>
SequentileFile<RecordT>::SequentileFile(string data_file_name, std::string key_name) {
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


//asume que el archivo esta ordenado
template <class RecordT>
void SequentileFile<RecordT>::init(string data_name_csv) {
    ifstream file(data_name_scv);
    if (!file.is_open()) {
        cout << "Error al abrir el archivo" << endl;
        return;
    }

    string line;
    getline(file, line); // Ignorar la primera línea

    // Leer el archivo CSV y agregar los registros al archivo binario
    int size = 0;
    writeFirstPos(0);
    while (getline(file, line)) {
        vector<string> fields;
        stringstream ss(line);
        string field;

        while (getline(ss, field, ',')) {
            fields.push_back(field);
        }

        RecordT* record = new RecordT(fields);
        writeRecord(record, size);
        size++;
    }
    writeSize(size);
}

template <class RecordT>
bool SequentileFile<RecordT>::add(RecordT* record) {
    if (pseudo_add(record)) {
        int size = readSize();
        //writeSize(size+1);
        if(getFileSize()-size > log2(size)) {
            rebuild();
            writeSize(getFileSize());
        }
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
        
        if (this->compare(this->keyAccessor(current), this->keyAccessor(record), "<")) {
            l = mid + 1;
        } else if (this->compare(this->keyAccessor(current), this->keyAccessor(record), ">")) {
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
        
        if (this->compare(this->keyAccessor(current), this->keyAccessor(record), ">")) {
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
        if (this->compare(this->keyAccessor(record), key, "<"))
            l = mid + 1;
        else if (this->compare(this->keyAccessor(record), key, ">"))
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
        if (this->compare(this->keyAccessor(record), key, "==")
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


    //buscar en los k ultimos registros
    for (int i = size; i < fileSize; i++) {
        RecordT* record = readRecord(i);
        if (record->next_pos == my_pos) {
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

    //si llego aqui no esta en los k ultimos registros
    
    //buscar para arriba hasta encontrar prev

    int prev_pos = my_pos - 1;
    while (prev_pos != -1) {
        RecordT* prev = readRecord(prev_pos);
        if (prev->next_pos == my_pos) {
            //descuelgo
            prev->next_pos = eliminado->next_pos;
            eliminado->next_pos = -2;
            writeRecord(eliminado, my_pos);
            writeRecord(prev, prev_pos);
            delete prev;
            return true;
        }
        prev_pos--;
        delete prev;
    }

    //si llego aqui no esta en los k primeros registros y no encontro prev
    //cout << "No se encontró el prev" << endl;
    return false;
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
    while (this->compare(this->keyAccessor(record), end_key, "<=")) {
        records.push_back(record);
        if (record->next_pos <= -1) {
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

#endif // SEQUENTIALFILE_H


