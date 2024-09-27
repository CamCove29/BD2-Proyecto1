#ifndef SEQUENTIALFILE_H 
#define SEQUENTIALFILE_H

#define buffer_size 1024

#include "record.h" 
#include <functional>
#include <string>
#include <vector>

using namespace std;
/*
int -> firstPost-> posicion del registro con menor tamaño

--------------
ordenado (value, pos del next)
firspos = 8
n size del espacio ordenado
1 -> 1
3 -> 2
7 -> 3
24 -> 8
100 -> 5
106 -> 6
108 -> -1
--------------
0 -> 0
*/


template <class RecordT>
class SequentileFile {  
public:
    std::string data_type = "nonetype"; //*tipo de dato de los registros
    std::string key_name = ""; //*nombre de la clave indexada
    bool compare(std::string data1, std::string data2, std::string op); //*compara dos strings
    std::string get_type_from_string(std::string data); //*obtiene el tipo de dato de un string

    //MELIS WORK
    string data_file_name; //* nombre del archivo del binario
    function<std::string(RecordT*)> keyAccessor; //* lambda para acceder a la clave indexada de un registro
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


