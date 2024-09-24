#ifndef SEQUENTIALFILE_H 
#define SEQUENTIALFILE_H

#define buffer_size 1024

#include "record.h" 
#include <functional>
#include <string>
#include <vector>

using namespace std;

template <class RecordT, class KeyT>
class SequentileFile {  
public:
    string data_file_name;
    function<KeyT(RecordT*)> keyAccessor;
    int limit = buffer_size/sizeof(RecordT);

    RecordT* readRecord(int pos);
    void writeRecord(RecordT* record, int pos);
    int readFirstPos();
    void writeFirstPos(int pos);
    int readSize();
    void writeSize(int size);
    int getFileSize();

    bool pseudo_add(RecordT* record);
    RecordT* search_aux(KeyT key, int& mid, int& f_minor);

public:
    // Constructor
    SequentileFile(const std::string& data_file_name, std::function<KeyT(RecordT*)> keyAccessor);

    void init(const std::string& data_name_csv); //
    bool add(RecordT* record); //tests
    bool remove(KeyT key); //
    bool rebuild(); //
    RecordT* search(KeyT key); //
    vector<RecordT*> range_search(KeyT start_key, KeyT end_key);
};

#endif // SEQUENTIALFILE_H


