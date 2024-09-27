#ifndef SEQUENTIALFILE_H 
#define SEQUENTIALFILE_H

#define buffer_size 1024

#include "indextype.h"

#include "record.h" 
#include <functional>
#include <string>
#include <vector>

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
    SequentileFile(const std::string& data_file_name, std::string key_name);  //constructor: recibe el nombre del archivo y la lambda para acceder a la clave indexada de un registro
    void init(const std::string& data_name_csv); //* carga el csv en el archivo binario, ordena en ram una parte, usa add
    bool add(RecordT* record); //*agrega un registro
    bool remove(std::string key); //*elimina un registro
    RecordT* search(std::string key); //*busca un registro
    vector<RecordT*> range_search(std::string start_key, std::string end_key); // busca un rango de registros
};

#endif // SEQUENTIALFILE_H


