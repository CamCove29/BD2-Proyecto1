#include "record.h"
using namespace std;

template <class RecordT>
class indexType {
protected:
    bool compare(string data1, string data2, string op);
    string get_type_from_string(string data);
public:
    function<string(RecordT*)> keyAccessor;
    void init(string index_type, string key, string data_file_name, function<string(RecordT*)> keyAccessor) = 0;
    bool add(RecordT* record) = 0;
    RecordT* search(string key) = 0;
    bool remove(string key) = 0;
    vector<RecordT*> range_search(string key1, string key2) = 0;
};