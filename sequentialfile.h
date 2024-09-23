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
    string aux_file_name;
    function<KeyT(RecordT*)> keyAccessor;

public:
    // Constructor
    SequentileFile(const std::string& data_file_name, const std::string& aux_file_name,
                   std::function<KeyT(RecordT*)> keyAccessor);

    void init(const std::string& data_name_csv);
    bool add(RecordT* record);
    bool remove(KeyT key); 
    RecordT* search(KeyT key);
    vector<RecordT*> search(KeyT start_key, KeyT end_key);
};

#endif // SEQUENTIALFILE_H


