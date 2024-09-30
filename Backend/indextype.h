#ifndef INDEXTYPE_H
#define INDEXTYPE_H

#include "record.h"
#include <iostream>
#include <functional>

using namespace std;

template <class RecordT>
class IndexType {
protected:
    std::string data_type = "nonetype";
    std::string key_name = "";
    string data_file_name;
    std::function<string(RecordT*)> keyAccessor;
    bool compare(const string& data1, const string& data2, string op) {
        std::string data_type = get_type_from_string(data1);
        std::string data_type2 = get_type_from_string(data2);
        if (data_type != data_type2) {
            data_type = "string";
        }


        if (data_type == "int") {
            int data1_int = stoi(data1);
            int data2_int = stoi(data2);
            if (op == "<") {
                return data1_int < data2_int;
            } else if (op == "<=") {
                return data1_int <= data2_int;
            } else if (op == ">") {
                return data1_int > data2_int;
            } else if (op == ">=") {
                return data1_int >= data2_int;
            } else if (op == "==") {
                return data1_int == data2_int;
            } else if (op == "!=") {
                return data1_int != data2_int;
            }
        } else if (data_type == "float") {
            float data1_float = stof(data1);
            float data2_float = stof(data2);
            if (op == "<") {
                return data1_float < data2_float;
            } else if (op == "<=") {
                return data1_float <= data2_float;
            } else if (op == ">") {
                return data1_float > data2_float;
            } else if (op == ">=") {
                return data1_float >= data2_float;
            } else if (op == "==") {
                return data1_float == data2_float;
            } else if (op == "!=") {
                return data1_float != data2_float;
            }
        } else if (data_type == "string" || data_type == "nonetype") {
            if (op == "<") {
                return data1 < data2;
            } else if (op == "<=") {
                return data1 <= data2;
            } else if (op == ">") {
                return data1 > data2;
            } else if (op == ">=") {
                return data1 >= data2;
            } else if (op == "==") {
                return data1 == data2;
            } else if (op == "!=") {
                return data1 != data2;
            }
        }
        return false;
    }
    string get_type_from_string(string data) {
        try {
            stoi(data);
            return "int";
        } catch (const std::invalid_argument& ia) {
            try {
                stof(data);
                return "float";
            } catch (const std::invalid_argument& ia) {
                return "string";
            }
        }
    }
public:
    virtual vector<RecordT*> range_search(string key1, string key2) = 0;
    virtual void init(string data_name_csv) = 0;
    virtual bool add(RecordT* record) = 0;
    virtual RecordT* search(string key) = 0;
    virtual bool remove(string key) = 0;
};


#endif // INDEXTYPE_H