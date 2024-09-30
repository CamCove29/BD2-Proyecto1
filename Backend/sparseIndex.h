#ifndef SPARSEINDEX_H
#define SPARSEINDEX_H

#include "indextype.h"
#include "record.h"
#include <functional>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <fstream>

#define buffer_size 1024
#define max_length 20

using namespace std;

template <class RecordT>
struct PageNode
{
    static const int N = (buffer_size - 8) / (sizeof(RecordT));
    RecordT record[N];
    int count;
    int nextPage;
    int getN()
    {
        return N;
    }
};

static const int M = (buffer_size - 4) / (max_length + 4);

struct IndexNode
{
    char keys[M][max_length];
    int children[M];
    int count;
    int getM()
    {
        return M;
    }
};

template <class RecordT>
class SparseIndex : public IndexType<RecordT>
{
private:
    string first_index_name;
    string second_index_name;
    string third_index_name;

    void writePage(PageNode<RecordT> *page, int pos);
    void writeIndex(IndexNode *index, int pos, int level);
    int get_last_page_pos();
    bool binarySearch(int nivel, int pospage, string key, int &inm_minor);

public:
    IndexNode *readIndex(int pos, int level);
    PageNode<RecordT> *readPage(int pos, string data_file_name);
    SparseIndex(const string &data_file_name, const string &index_file_name, std::string key_name);
    void init(string data_name_csv);
    bool add(RecordT *record);
    bool remove(std::string key);
    RecordT *search(std::string key);
    vector<RecordT *> range_search(std::string start_key, std::string end_key);
};


//remove
template <class RecordT>
bool SparseIndex<RecordT>::remove(std::string key) {
    //buscamos en el primer indice
    int inm_minor;
    binarySearch(1, 0, key, inm_minor);

    //buscamos en el segundo indice
    int pos_page = inm_minor;
    binarySearch(2, pos_page, key, inm_minor);

    //buscamos en el tercer indice
    pos_page = inm_minor;
    binarySearch(3, pos_page, key, inm_minor);

    //buscamos en el data file
    PageNode<RecordT>* page = readPage(inm_minor, this->data_file_name);
    //buscamos secuencialmente en la pagina
    for(int i = 0; i < page->count; i++){
        if(this->compare(page->record[i].getStringAttribute(this->key_name).second, key, "==")){
            //encontramos el registro
            //swapeamos con el ultimo
            page->record[i] = page->record[page->count-1];
            page->count--;
            writePage(page, inm_minor);
            return true;
        }
    }
    //buscamos en las paginas encadenadas
    while(page->nextPage != -1){
        inm_minor = page->nextPage;
        page = readPage(inm_minor, this->data_file_name);
        for(int i = 0; i < page->count; i++){
            if(this->compare(page->record[i].getStringAttribute(this->key_name).second, key, "==")){
                //encontramos el registro
                //swapeamos con el ultimo
                page->record[i] = page->record[page->count-1];
                page->count--;
                writePage(page, inm_minor);
                return true;
            }
        }
    }

    return false;
    
}

//range_search
template <class RecordT>
vector<RecordT*> SparseIndex<RecordT>::range_search(std::string start_key, std::string end_key) {
    vector<RecordT*> records;
    int inm_minor;

    // Buscar en el primer nivel
    binarySearch(1, 0, start_key, inm_minor);

    // Buscar en el segundo nivel
    int pos_page = inm_minor;
    binarySearch(2, pos_page, start_key, inm_minor);

    // Buscar en el tercer nivel
    pos_page = inm_minor;
    binarySearch(3, pos_page, start_key, inm_minor);

    // Buscar en el archivo de datos
    PageNode<RecordT>* page = readPage(inm_minor, this->data_file_name);
    if (!page) {
        std::cout << "Error al leer la página." << std::endl;
        return records;
    }

    bool continue_searching = true;

    #define search_complete_page    \
    for (int i = 0; i < page->count; i++) { \
        if (this->compare(page->record[i].getStringAttribute(this->key_name).second, start_key, ">=") &&    \
            this->compare(page->record[i].getStringAttribute(this->key_name).second, end_key, "<=")) {  \
            RecordT* record = new RecordT(page->record[i]); \
            records.push_back(record);  \
        }   \
        else if (this->compare(page->record[i].getStringAttribute(this->key_name).second, end_key, ">"))  \
        {   \
            continue_searching = false; \
            break;  \
        }   \
    }   \
    while(page->nextPage != -1){    \
        page = readPage(page->nextPage, this->data_file_name);  \
        for (int i = 0; i < page->count; i++) { \
            if (this->compare(page->record[i].getStringAttribute(this->key_name).second, start_key, ">=") &&    \
                this->compare(page->record[i].getStringAttribute(this->key_name).second, end_key, "<=")) {  \
                RecordT* record = new RecordT(page->record[i]); \
                records.push_back(record);  \
            }   \
            else if (this->compare(page->record[i].getStringAttribute(this->key_name).second, end_key, ">"))  \
            {   \
                continue_searching = false; \
                break;  \
            }   \
        }   \
    }   \

    search_complete_page


    while (continue_searching) {
        pos_page += 1;
        page = readPage(pos_page, this->data_file_name);
        if (!page) {
            std::cout << "Error al leer la página." << std::endl;
            return records;
        }

        search_complete_page
    }



    return records;
}

template <class RecordT>
SparseIndex<RecordT>::SparseIndex(const string &data_file_name, const string &index_file_name, std::string key_name)
{
    this->data_file_name = data_file_name;
    this->first_index_name = index_file_name + "_first";
    this->second_index_name = index_file_name + "_second";
    this->third_index_name = index_file_name + "_third";

    this->key_name = key_name;

    this->keyAccessor = [key_name](RecordT *record)
    {
        std::pair<std::string, std::string> result = record->getStringAttribute(key_name);
        return result.second;
    };
}

template <class RecordT>
PageNode<RecordT> *SparseIndex<RecordT>::readPage(int pos, string filename)
{
    ifstream file(filename, ios::binary | ios::in);
    if (!file.is_open())
    {
        return nullptr;
    }
    file.seekg(pos * sizeof(PageNode<RecordT>));
    PageNode<RecordT> *page = new PageNode<RecordT>();
    file.read(reinterpret_cast<char *>(page), sizeof(PageNode<RecordT>));
    file.close();
    return page;
}

template <class RecordT>
void SparseIndex<RecordT>::writePage(PageNode<RecordT> *page, int pos)
{
    fstream file(this->data_file_name, ios::binary | ios::in | ios::out);
    if (!file.is_open())
    {
        file.open(this->data_file_name, ios::binary | ios::out);
        if (!file.is_open())
        {
            return;
        }
    }
    file.seekp(pos * sizeof(PageNode<RecordT>));
    file.write(reinterpret_cast<char *>(page), sizeof(PageNode<RecordT>));
    file.close();
}

template <class RecordT>
IndexNode *SparseIndex<RecordT>::readIndex(int pos, int level)
{
    ifstream file; // sino exite no abre nada
    if (level == 1)
    {
        file.open(this->first_index_name, ios::binary);
    }
    else if (level == 2)
    {
        file.open(this->second_index_name, ios::binary);
    }
    else if (level == 3)
    {
        file.open(this->third_index_name, ios::binary);
    }
    else
    {
        return nullptr;
    }

    if (!file.is_open())
    {
        return nullptr;
    }

    file.seekg(pos * sizeof(IndexNode));
    IndexNode *index = new IndexNode();
    file.read(reinterpret_cast<char *>(index), sizeof(IndexNode));
    file.close();
    return index;
}

template <class RecordT>
void SparseIndex<RecordT>::writeIndex(IndexNode *index, int pos, int level)
{
    fstream file; // sino exite no abre nada
    if (level == 1)
    {
        file.open(this->first_index_name, ios::binary | ios::in | ios::out);
    }
    else if (level == 2)
    {
        file.open(this->second_index_name, ios::binary | ios::in | ios::out);
    }
    else if (level == 3)
    {
        file.open(this->third_index_name, ios::binary | ios::in | ios::out);
    }
    else
    {
        return;
    }

    if (!file.is_open())
    {
        if (level == 1)
        {
            file.open(this->first_index_name, ios::binary | ios::out);
        }
        else if (level == 2)
        {
            file.open(this->second_index_name, ios::binary | ios::out);
        }
        else if (level == 3)
        {
            file.open(this->third_index_name, ios::binary | ios::out);
        }
        if (!file.is_open())
        {
            cout << "aaa, todo mal" << endl;
            return;
        }
    }

    file.seekp(pos * sizeof(IndexNode));
    file.write(reinterpret_cast<char *>(index), sizeof(IndexNode));
    file.close();
}

template <class RecordT>
int SparseIndex<RecordT>::get_last_page_pos()
{
    ifstream file(this->data_file_name, ios::binary);
    if (!file.is_open())
    {
        return -1;
    }
    file.seekg(0, ios::end);
    int size = file.tellg();
    file.close();
    return (size / sizeof(PageNode<RecordT>) - 1);
}

template <class RecordT>
bool SparseIndex<RecordT>::binarySearch(int nivel, int pospage, string key, int& inm_minor) {
    IndexNode* index = readIndex(pospage, nivel);

    //imprimir los datos del nodo
    if (index != nullptr) {
        for (int i = 0; i < index->count; i++) {
            //pasar de char [] a string
            string key;
            for (int j = 0; j < max_length; j++) {
                if (index->keys[i][j] == '\0') {
                    break;
                }
                key += index->keys[i][j];
            }
        }
    } else {
    }

    if (!index) {
        return false;
    }

    int page_limit = index->getM();
    int l = 0; // Cambiar a 0
    int u = index->count - 1; // Cambiar a index->count - 1
    bool found = false;
    int mid;

    while (u >= l) {
        mid = (l + u) / 2;
        // Aquí se asume que this->compare devuelve verdadero si index->keys[mid] < key
        if (this->compare(index->keys[mid], key, "<")) { //
            l = mid +1;
        } else if (this->compare(index->keys[mid], key, ">")) {
            u = mid - 1;
        } else { // Si son iguales
            found = true;
            break; // Salimos del bucle si se encuentra la clave
        }
    }

    // Determinar el índice de inm_minor según si se encontró la clave o no
    if (found) {
        inm_minor = index->children[mid];
    } else {
        inm_minor = index->children[u];
    }


    return found; // Retornar si se encontró o no
}

template <class RecordT>
void SparseIndex<RecordT>::init(string data_name_csv)
{
    ifstream file_csv(data_name_csv);

    IndexNode *index = new IndexNode();
    PageNode<RecordT> *page = new PageNode<RecordT>();
    int index_limit = index->getM();
    int page_limit = page->getN() * 0.8; // 80% de la capacidad de la página

    vector<RecordT *> records_vector_A;
    vector<string> keys_vector_B, keys_vector_C, keys_vector_D;
    int i = 0, j = 0, k = 0, l = 0; // i: contador de registros, j: contador de páginas, k: contador de nodos de indice nivel 3, l: contador de nodos de indice nivel 2
    // enel primer nivel solo debe existir un nodo
    int index_node_count = 0; // Contador para nodos de índice de nivel 3

    if (!file_csv.is_open())
    {
        cout << "Error al abrir el archivo" << endl;
        return;
    }

    string line;
    getline(file_csv, line); // Leer la primera línea (encabezados)

    while (getline(file_csv, line))
    {
        // Obtener el record
        stringstream ss(line);
        string item;
        vector<string> items;
        while (getline(ss, item, ','))
        {
            items.push_back(item);
        }
        RecordT *record = new RecordT(items);

        records_vector_A.push_back(record);

        // Manejar páginas de datos
        if ((i + 1) % page_limit == 0)
        {
            PageNode<RecordT> *page = new PageNode<RecordT>();
            for (size_t a = 0; a < records_vector_A.size(); a++)
            {
                page->record[a] = *records_vector_A[a];
            }
            page->count = records_vector_A.size();
            page->nextPage = -1;

            writePage(page, j);
            keys_vector_B.push_back(records_vector_A[0]->getStringAttribute(this->key_name).second);
            records_vector_A.clear();

            j++; //cantidad de paginas


            // Manejar índices de tercer nivel
            if ((j + 1) % index_limit == 0)
            {
                IndexNode *index = new IndexNode();
                for (size_t b = 0; b < keys_vector_B.size(); b++)
                {
                    strncpy(index->keys[b], keys_vector_B[b].c_str(), max_length - 1);
                    index->keys[b][max_length - 1] = '\0'; // Asegurar terminación null
                    index->children[b] = j-keys_vector_B.size() + b;
                }
                index->count = keys_vector_B.size();
                writeIndex(index, k, 3);
                keys_vector_C.push_back(keys_vector_B[0]);
                keys_vector_B.clear();
                k++;

                //manejar indice de segundo nivel
                if ((k + 1) % index_limit == 0)
                {
                    IndexNode *index = new IndexNode();
                    for (size_t c = 0; c < keys_vector_C.size(); c++)
                    {
                        strncpy(index->keys[c], keys_vector_C[c].c_str(), max_length - 1);
                        index->keys[c][max_length - 1] = '\0'; // Asegurar terminación null
                        index->children[c] = k-keys_vector_C.size() + c;
                    }
                    index->count = keys_vector_C.size();
                    writeIndex(index, l, 2);
                    keys_vector_D.push_back(keys_vector_C[0]);
                    keys_vector_C.clear();
                    l++;
                }
            }
        }

        i++;
    }

    file_csv.close();


    //para los ultimos registros

    //si quedan registros en records_vector_A
    if (records_vector_A.size() > 0)
    {
        PageNode<RecordT> *page = new PageNode<RecordT>();
        for (size_t a = 0; a < records_vector_A.size(); a++)
        {
            page->record[a] = *records_vector_A[a];
        }
        page->count = records_vector_A.size();
        page->nextPage = -1;

        writePage(page, j);
        keys_vector_B.push_back(records_vector_A[0]->getStringAttribute(this->key_name).second);
        records_vector_A.clear();
        j++;

        // Manejar índices de tercer nivel
        if ((j + 1) % index_limit == 0)
        {
            IndexNode *index = new IndexNode();
            for (size_t b = 0; b < keys_vector_B.size(); b++)
            {
                strncpy(index->keys[b], keys_vector_B[b].c_str(), max_length - 1);
                index->keys[b][max_length - 1] = '\0'; // Asegurar terminación null
                index->children[b] = j-keys_vector_B.size() + b;
            }
            index->count = keys_vector_B.size();
            writeIndex(index, k, 3);
            keys_vector_C.push_back(keys_vector_B[0]);
            keys_vector_B.clear();
            k++;

            //manejar indice de segundo nivel
            if ((k + 1) % index_limit == 0)
            {
                IndexNode *index = new IndexNode();
                for (size_t c = 0; c < keys_vector_C.size(); c++)
                {
                    strncpy(index->keys[c], keys_vector_C[c].c_str(), max_length - 1);
                    index->keys[c][max_length - 1] = '\0'; // Asegurar terminación null
                    index->children[c] = k-keys_vector_C.size() + c;
                }
                index->count = keys_vector_C.size();
                writeIndex(index, l, 2);
                keys_vector_D.push_back(keys_vector_C[0]);
                keys_vector_C.clear();
                l++;
            }
        }
    }


    //si quedan registros en keys_vector_B
    if (keys_vector_B.size() > 0)
    {
        IndexNode *index = new IndexNode();
        for (size_t b = 0; b < keys_vector_B.size(); b++)
        {
            strncpy(index->keys[b], keys_vector_B[b].c_str(), max_length - 1);
            index->keys[b][max_length - 1] = '\0'; // Asegurar terminación null
            index->children[b] = b + (index_node_count * index_limit);
        }
        index->count = keys_vector_B.size();
        writeIndex(index, k, 3);
        keys_vector_C.push_back(keys_vector_B[0]);
        keys_vector_B.clear();

        //manejar indice de segundo nivel
        if ((k + 1) % index_limit == 0)
        {
            IndexNode *index = new IndexNode();
            for (size_t c = 0; c < keys_vector_C.size(); c++)
            {
                strncpy(index->keys[c], keys_vector_C[c].c_str(), max_length - 1);
                index->keys[c][max_length - 1] = '\0'; // Asegurar terminación null
                index->children[c] = c + (l * index_limit);
            }
            index->count = keys_vector_C.size();
            writeIndex(index, l, 2);
            keys_vector_D.push_back(keys_vector_C[0]);
            keys_vector_C.clear();
            l++;
        }
    }

    //si quedan registros en keys_vector_C
    if (keys_vector_C.size() > 0)
    {
        IndexNode *index = new IndexNode();
        for (size_t c = 0; c < keys_vector_C.size(); c++)
        {
            strncpy(index->keys[c], keys_vector_C[c].c_str(), max_length - 1);
            index->keys[c][max_length - 1] = '\0'; // Asegurar terminación null
            index->children[c] = c + (l * index_limit);
        }
        index->count = keys_vector_C.size();
        writeIndex(index, l, 2);
        keys_vector_D.push_back(keys_vector_C[0]);
        keys_vector_C.clear();
        l++;
    }

    // Crear índice de primer nivel
    index = new IndexNode();
    for (size_t d = 0; d < keys_vector_D.size(); d++)
    {
        strncpy(index->keys[d], keys_vector_D[d].c_str(), max_length - 1);
        index->keys[d][max_length - 1] = '\0'; // Asegurar terminación null
        index->children[d] = d;
    }

    index->count = keys_vector_D.size();
    writeIndex(index, 0, 1);


    // Liberar memoria
    delete index;
    delete page;

    // Liberar memoria de los registros
    for (auto &record : records_vector_A)
    {
        delete record;
    }
}

template <class RecordT>
RecordT *SparseIndex<RecordT>::search(std::string key)
{
    int inm_minor;

    // Buscar en el primer nivel
    if (!binarySearch(1, 0, key, inm_minor))
    {
    }

    // Buscar en el segundo nivel
    int pos_page = inm_minor;
    if (!binarySearch(2, pos_page, key, inm_minor))
    {
    }

    // Buscar en el tercer nivel
    pos_page = inm_minor;
    if (!binarySearch(3, pos_page, key, inm_minor))
    {
    }


    // Buscar en el archivo de datos
    PageNode<RecordT> *page = readPage(inm_minor, this->data_file_name);
    if (!page)
    {
        return nullptr;
    }

    // Buscar en la página
    for (int i = 0; i < page->count; i++)
    {
        if (this->compare(page->record[i].getStringAttribute(this->key_name).second, key, "=="))
        {
            RecordT *record = new RecordT(page->record[i]);
            return record; // Retornar el registro encontrado
        }
    }

    return nullptr; // Retornar nullptr si no se encontró
}

template <class RecordT>
bool SparseIndex<RecordT>::add(RecordT *record)
{
    // busco en el primer nivel
    int inm_minor;
    binarySearch(1, 0, record->getStringAttribute(this->key_name).second, inm_minor);

    // busco en el segundo nivel
    int pos_page = inm_minor;
    binarySearch(2, pos_page, record->getStringAttribute(this->key_name).second, inm_minor);

    // busco en el tercer nivel
    pos_page = inm_minor;
    binarySearch(3, pos_page, record->getStringAttribute(this->key_name).second, inm_minor);

    // busco en el data file
    PageNode<RecordT> *page = readPage(inm_minor, this->data_file_name);
    if (page->count < page->getN())
    {
        page->record[page->count] = *record;
        page->count++;
        writePage(page, inm_minor);
    }
    // revisar, esta mal, falta encademaniento, falta recorrer las paginas
    while (page->count == page->getN())
    {
        PageNode<RecordT> *new_page = new PageNode<RecordT>();
        for (int i = 0; i < page->getN() / 2; i++)
        {
            new_page->record[i] = page->record[i + page->getN() / 2];
        }
        new_page->count = page->getN() / 2;
        new_page->nextPage = page->nextPage;
        page->nextPage = get_last_page_pos();
        page->count = page->getN() / 2;
        writePage(page, inm_minor);
        writePage(new_page, get_last_page_pos());
        page = new_page;
    }

    return false;
}

#endif // SPARSEINDEX_H