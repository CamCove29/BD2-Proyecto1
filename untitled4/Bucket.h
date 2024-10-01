
#ifndef PROJECTBD_BUCKET_H
#define PROJECTBD_BUCKET_H

#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

template <class Record, class Key>
class Bucket {
private:
    vector<Record> records;  // Cambiar a vector para mayor flexibilidad
    int nextBucket;
    int D = 4;  // Capacidad máxima
    int count;

public:
    int nextDeleted;
    int next_pos;


    Bucket() : nextBucket(-1), nextDeleted(0), count(0) {}

    bool isFull() const {
        return count >= D;
    }

    void setNextBucket(int nextPosition) {
        nextBucket = nextPosition;
    }

    int getNextBucket() const {
        return nextBucket;
    }

    void add(const Record& record) {
        if (count < D) {
            records.push_back(record);  // Agregar el registro al vector
            ++count;
        }
    }

    void sortBucket() {
        sort(records.begin(), records.end(), compareRecords);
    }

    static bool compareRecords(const Record& a, const Record& b) {
        return a.getID() < b.getID();  // Asegúrate de que `getID()` esté en la clase Record
    }

    vector<Record> getRecords() const {
        return records;  // Devuelve el vector de registros
    }

    void setRecords(const vector<Record>& newRecords) {
        records = newRecords;  // Asigna el nuevo vector directamente
        count = records.size();  // Actualiza el conteo
    }

    bool empty() const {
        return records.empty();  // Comprueba si el vector está vacío
    }

    vector<Record> getAllDifferentRecords(const Key& key) {
        vector<Record> output;
        for (const auto& record : getRecords()) {
            if (!record.equalToKey(key))  // Asegúrate de que `equalToKey` esté en la clase Record
                output.push_back(record);
        }
        return output;
    }
};

#endif //PROJECTBD_BUCKET_H