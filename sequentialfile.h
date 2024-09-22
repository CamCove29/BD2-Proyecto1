#ifndef SEQUENTIALFILE_H 
#define SEQUENTIALFILE_H

#include "record.h" 
#include <functional>
#include <string>
#include <vector>

template <class RecordT, class KeyT>
class SequentileFile {
public:
    std::string data_file_name;
    std::string aux_file_name;
    std::function<KeyT(RecordT*)> keyAccessor;

public:
    // Constructor
    SequentileFile(const std::string& data_file_name, const std::string& aux_file_name,
                   std::function<KeyT(RecordT*)> keyAccessor);

    void init(const std::string& data_name_csv);
    bool add(RecordT* record);
    bool remove(KeyT key); 
    RecordT* search(KeyT key);
    std::vector<RecordT*> search(KeyT start_key, KeyT end_key);
};

#endif // SEQUENTIALFILE_H


