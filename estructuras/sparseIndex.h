#include "indextype.h"
#include "record.h"
#include <functional>
#include <string>

#define buffer_size 1024

using namespace std;


class pageNode{
    std::string keys[4];
};

template <class RecordT>
class SparseIndex : public IndexType<RecordT> {
private:
    string index_file_name;

public:
    SparseIndex(const string& data_file_name, const string& index_file_name, std::string key_name);
    void init(const string& data_name_csv);
    bool add(RecordT* record);
    bool remove(std::string key);
    RecordT* search(std::string key);
    vector<RecordT*> range_search(std::string start_key, std::string end_key);
};



