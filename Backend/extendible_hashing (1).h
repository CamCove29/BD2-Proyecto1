#include "record.h"
#include <cstring>
#include "indextype.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <bitset>

#define max_length 20
#define buffer_size_hash 8192
#define globalD 18

// funcion de hash que solo recibe strings
/*

*/

// cabe en una pagina
template <class RecordT>
struct BucketPage
{
    static const int FB = (buffer_size_hash - 8) / sizeof(RecordT);
    RecordT records[FB];
    int count;
    int next_bucket;
    int local_depth;
    int sufix;
    BucketPage()
    {
        count = 0;
        next_bucket = -1;
        local_depth = 0;
        sufix = 0;
    }
};

// el index file va a tener 2^18 enteros y 2^18 enteros que en realidad son punteros
// 2^18
struct indexExt
{
    int posPages[262144];
};

template <class RecordT>
class ExtendibleHash : public IndexType<RecordT>
{
private:
    string data_file_name;
    string index_file_name;
    int global_depth;
    std::hash<std::string> hash_fn;
    void split(BucketPage<RecordT> *my_bucket, int my_bucket_pos, RecordT *record, indexExt *my_index);
    void update_index_recursive(int sufix, int pos, int& cont, indexExt* my_index, int sufix_length);
    int get_datafile_size();

    BucketPage<RecordT> *read_bucket(int pos);
    void write_bucket(BucketPage<RecordT> *bucket, int pos);
    indexExt *read_index();
    void write_index(indexExt *index);

public:
    ExtendibleHash(string data_file_name, string index_file_name, string key_name);
    void init(string data_name_csv);
    bool add(RecordT *record);
    bool remove(string key);
    RecordT *search(string key);
    vector<RecordT *> range_search(string key1, string key2);
};

template <class RecordT>
vector<RecordT *> ExtendibleHash<RecordT>::range_search(string key1, string key2)
{
    std::cout << "Range_search no soportado por ExtendibleHash" << std::endl;

    return vector<RecordT *>();
}

template <class RecordT>
RecordT *ExtendibleHash<RecordT>::search(string key)
{
    // 1. Obtengo el index de la posición donde está el puntero a mi bycket:    Hash(key) % 2^global_depth o obtener los ultimos global_depth bits
    int index = hash_fn(key) & ((1 << global_depth) - 1);

    // 2. Leo el index file y obtengo el puntero a mi bucket
    indexExt *indexObj = read_index();
    int pos = indexObj->posPages[index];

    // 4. Busco el record en el bucket
    int next_pos = pos;
    while (next_pos != -1)
    {
        BucketPage<RecordT> *bucket = read_bucket(next_pos);
        for (int i = 0; i < bucket->count; i++)
        {
            string key_record = this->keyAccessor(&bucket->records[i]);
            if (key_record == key)
            {
                return &bucket->records[i];
            }
        }
        next_pos = bucket->next_bucket;
    }

    return nullptr;
}

template <class RecordT>
int ExtendibleHash<RecordT>::get_datafile_size()
{
    ifstream data_file;
    data_file.open(data_file_name, ios::binary);
    if (!data_file.is_open())
    {
        cout << "Error al abrir el archivo de datos" << endl;
        return -1;
    }
    data_file.seekg(0, ios::end);
    int size = data_file.tellg();
    data_file.close();
    return size / sizeof(BucketPage<RecordT>);
}

template <class RecordT>
indexExt *ExtendibleHash<RecordT>::read_index()
{
    ifstream index_file;
    index_file.open(index_file_name, ios::binary);
    if (!index_file.is_open())
    {
        cout << "Error al abrir el archivo de índice" << endl;
        return nullptr;
    }
    indexExt *index = new indexExt();
    index_file.read((char *)index, sizeof(indexExt));
    index_file.close();
    return index;
}

template <class RecordT>
void ExtendibleHash<RecordT>::write_index(indexExt *index)
{
    fstream index_file;
    index_file.open(index_file_name, ios::binary | ios::in | ios::out);
    if (!index_file.is_open())
    {
        cout << "Error al abrir el archivo de índice" << endl;
        return;
    }
    index_file.write((char *)index, sizeof(indexExt));
    index_file.close();
}

template <class RecordT>
BucketPage<RecordT> *ExtendibleHash<RecordT>::read_bucket(int pos)
{
    ifstream data_file;
    data_file.open(data_file_name, ios::binary);
    if (!data_file.is_open())
    {
        cout << "Error al abrir el archivo de datos" << endl;
        return nullptr;
    }
    data_file.seekg(pos * sizeof(BucketPage<RecordT>));
    BucketPage<RecordT> *bucket = new BucketPage<RecordT>();
    data_file.read((char *)bucket, sizeof(BucketPage<RecordT>));
    data_file.close();
    return bucket;
}

template <class RecordT>
void ExtendibleHash<RecordT>::write_bucket(BucketPage<RecordT> *bucket, int pos)
{
    fstream data_file;
    data_file.open(data_file_name, ios::binary | ios::in | ios::out);
    if (!data_file.is_open())
    {
        cout << "Error al abrir el archivo de datos" << endl;
        return;
    }
    data_file.seekp(pos * sizeof(BucketPage<RecordT>));
    data_file.write((char *)bucket, sizeof(BucketPage<RecordT>));
    data_file.close();
}

template <class RecordT>
void ExtendibleHash<RecordT>::split(BucketPage<RecordT> *my_bucket, int my_bucket_pos, RecordT *record, indexExt *my_index)
{
    // 1. Crear un nuevo bucket
    BucketPage<RecordT> *new_bucket = new BucketPage<RecordT>();
    new_bucket->local_depth = my_bucket->local_depth + 1; // Aumentar la profundidad local
    new_bucket->next_bucket = -1; // Inicialmente no apunta a ningún otro bucket
    new_bucket->count = 0; // Inicialmente está vacío

    // 2. Determinar el sufijo (sufix) para el nuevo bucket
    int sufix = my_bucket->sufix + (1 << my_bucket->local_depth); // Modificamos el sufijo
    new_bucket->sufix = sufix;

    // 3. Redistribuir los registros entre el bucket original y el nuevo
    for (int i = 0; i < my_bucket->count; i++)
    {
        RecordT &record_item = my_bucket->records[i];
        string key_record = this->keyAccessor(&record_item);
        int index = hash_fn(key_record) & ((1 << new_bucket->local_depth) - 1); // Calcular el nuevo índice

        // Si el índice corresponde al nuevo bucket, mover el registro
        if (index == (sufix & ((1 << new_bucket->local_depth) - 1)))
        {
            new_bucket->records[new_bucket->count] = record_item; // Copiar el registro al nuevo bucket
            new_bucket->count++;
            // Eliminar el registro del bucket original
            my_bucket->records[i] = my_bucket->records[--my_bucket->count]; // Mover el último al índice actual y reducir el conteo
            i--; // Asegurarse de revisar el mismo índice de nuevo
        }
    }

    // 4. Actualizar la estructura de datos
    write_bucket(my_bucket, my_bucket_pos); // Escribir el bucket original
    int new_bucket_pos = get_datafile_size() / sizeof(BucketPage<RecordT>); // Obtener la nueva posición del bucket
    write_bucket(new_bucket, new_bucket_pos); // Escribir el nuevo bucket

    // 5. Actualizar el índice para apuntar al nuevo bucket
    update_index_recursive(sufix, new_bucket_pos, my_bucket->local_depth, my_index, new_bucket->local_depth);
}


template <class RecordT>
void ExtendibleHash<RecordT>::update_index_recursive(int sufix, int pos, int& cont, indexExt* my_index, int sufix_length){
    for (int i = 0; i < 262144; i++)
    {
        if ((i & ((1 << sufix_length) - 1)) == sufix)
        {
            my_index->posPages[i] = pos;
        }
    }
}

template <class RecordT>
bool ExtendibleHash<RecordT>::remove(string key)
{
    return false;
}
template <class RecordT>
bool ExtendibleHash<RecordT>::add(RecordT *record)
{
    // 1. Obtener el índice donde se debe insertar el registro
    int index = hash_fn(this->keyAccessor(record)) & ((1 << global_depth) - 1);
    
    // 2. Leer el índice y obtener la posición del bucket
    indexExt *indexObj = read_index();
    int pos = indexObj->posPages[index];

    // 3. Leer el bucket correspondiente
    BucketPage<RecordT> *bucket = read_bucket(pos);
    
    // 4. Comprobar si hay espacio en el bucket
    if (bucket->count < BucketPage<RecordT>::FB)
    {
        // 5. Si hay espacio, añadir el registro
        bucket->records[bucket->count++] = *record; // Copiar el registro al bucket
        write_bucket(bucket, pos); // Guardar el bucket actualizado
        delete indexObj; // Liberar memoria
        delete bucket; // Liberar memoria
        return true;
    }

    // 6. Si el bucket está lleno, verificar el local depth
    if (bucket->local_depth < global_depth)
    {
        // Realizar un split
        split(bucket, pos, record, indexObj);
    }
    else
    {
        // Encadenar buckets si el local depth es mayor o igual al global depth
        BucketPage<RecordT> *new_bucket = new BucketPage<RecordT>();
        new_bucket->local_depth = bucket->local_depth + 1; // Incrementar la profundidad local
        new_bucket->next_bucket = -1; // Inicialmente no apunta a otro bucket
        new_bucket->count = 0; // Inicialmente vacío
        
        // Actualizar el índice para apuntar al nuevo bucket
        int new_bucket_pos = get_datafile_size() / sizeof(BucketPage<RecordT>);
        update_index_recursive(bucket->sufix, new_bucket_pos, new_bucket->local_depth, indexObj, new_bucket->local_depth);
        
        // Redistribuir registros entre el bucket original y el nuevo
        for (int i = 0; i < bucket->count; i++)
        {
            RecordT &record_item = bucket->records[i];
            string key_record = this->keyAccessor(&record_item);
            int new_index = hash_fn(key_record) & ((1 << new_bucket->local_depth) - 1); // Calcular el nuevo índice

            // Si el índice corresponde al nuevo bucket, mover el registro
            if (new_index == (new_bucket->sufix & ((1 << new_bucket->local_depth) - 1)))
            {
                new_bucket->records[new_bucket->count++] = record_item; // Copiar el registro al nuevo bucket
                bucket->records[i] = bucket->records[--bucket->count]; // Mover el último al índice actual y reducir el conteo
                i--; // Asegurarse de revisar el mismo índice de nuevo
            }
        }
        
        // Escribir ambos buckets
        write_bucket(bucket, pos);
        write_bucket(new_bucket, new_bucket_pos);

        // Añadir el nuevo registro al nuevo bucket
        new_bucket->records[new_bucket->count++] = *record; // Copiar el registro al nuevo bucket
        write_bucket(new_bucket, new_bucket_pos); // Guardar el nuevo bucket

        // Liberar memoria
        delete bucket; // Liberar memoria
        delete indexObj; // Liberar memoria
    }

    return true;
}



template <class RecordT>
ExtendibleHash<RecordT>::ExtendibleHash(string data_file_name, string index_file_name, string key_name)
{
    this->data_file_name = data_file_name;
    this->index_file_name = index_file_name;
    this->key_name = key_name;
    this->keyAccessor = [key_name](RecordT *record)
    {
        std::pair<std::string, std::string> result = record->getStringAttribute(key_name);
        return result.second;
    };

    this->global_depth = 18;
}

// init
template <class RecordT>
void ExtendibleHash<RecordT>::init(string data_name_csv)
{
    // crear el archivo de índice
    ofstream index_file;
    ofstream data_file;

    index_file.open(index_file_name, ios::binary);

    // crear el array indexExt con 2^18 posiciones, como solo hay dos buckets, y se usa el sufijo para apuntar entonces se necesita un solo array
    indexExt indexobj;
    for (int i = 0; i < 262144; i++)
    {
        if ((i & 1) == 0)
        {
            indexobj.posPages[i] = 0;
        }
        else
        {
            indexobj.posPages[i] = 1;
        }
    }

    index_file.write((char *)&indexobj, sizeof(indexExt));
    index_file.close();

    // en el datafile solo crearemos los dos buckets
    data_file.open(data_file_name, ios::binary);
    BucketPage<RecordT> bucket1;
    BucketPage<RecordT> bucket2;

    bucket1.sufix = 0;
    bucket2.sufix = 1;

    data_file.write((char *)&bucket1, sizeof(BucketPage<RecordT>));
    data_file.write((char *)&bucket2, sizeof(BucketPage<RecordT>));
    data_file.close();

    // leer el archivo CSV y agregar los registros usando la función add
    ifstream data_csv;
    data_csv.open(data_name_csv);
    if (!data_csv.is_open())
    {
        cout << "Error al abrir el archivo CSV de datos" << endl;
        return;
    }

    string line;
    getline(data_csv, line); // Leer la primera línea (encabezados)

    while (getline(data_csv, line))
    {
        stringstream ss(line);
        string item;
        vector<string> items;
        while (getline(ss, item, ','))
        {
            items.push_back(item);
        }

        // Crear un nuevo registro y agregarlo al archivo
        RecordT *record = new RecordT(items);
        if (!add(record))
        {
            cout << "Error al agregar el registro con key: " << this->keyAccessor(record) << endl;
        }
        delete record;
    }

    data_csv.close();
}



