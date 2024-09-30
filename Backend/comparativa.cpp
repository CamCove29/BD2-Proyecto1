#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <fstream>
#include "indextype.h"
#include "record.h"
#include "sequentialfile.h" // Asegúrate de que estas clases se incluyan
#include "sparseIndex.h"
#include "extendible_hashing.h"

using namespace std;

template <class RecordT>
class PerformanceTester {
private:
    vector<RecordT*> records;
    string data_file;
    string initial_data;

public:
    PerformanceTester(const string& data_file, const string& initial_data ) : data_file(data_file), initial_data(initial_data) {
        loadData(data_file);
    }

    void loadData(const string& data_file) {
        ifstream infile(data_file);
        string line;

        while (getline(infile, line)) {
            // Procesar cada línea del CSV para crear registros
            // Asume que el CSV tiene un formato adecuado
            // Aquí solo se ejemplifica; debe ser adaptado al formato real de tus datos
            vector<string> fields;
            stringstream ss(line);
            string field;

            while (getline(ss, field, ',')) {
                fields.push_back(field);
            }

            // Crear el registro (cambiar RecordMeet a RecordCompetition según corresponda)
            RecordT* record = new RecordT(fields);
            records.push_back(record);
        }
    }

    void testSequentialFile(int n) {
        SequentileFile<RecordT> sequentialFile("sequential.dat", "MeetID");
        sequentialFile.init(initial_data);

        // Medir inserciones
        auto startInsert = chrono::high_resolution_clock::now();
        //for (auto record : records) {
//            sequentialFile.add(record);
  //      }

        for (int i = 0; i < n; i++) {
            sequentialFile.add(records[i]);
        }
        auto endInsert = chrono::high_resolution_clock::now();

        // Medir búsquedas
        auto startSearch = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++) {
            sequentialFile.search(to_string(records[i]->MeetID));
        }
        auto endSearch = chrono::high_resolution_clock::now();

        cout << "Sequential File:\n";
        cout << "Insert time: " << chrono::duration_cast<chrono::milliseconds>(endInsert - startInsert).count() << " ms\n";
        cout << "Search time: " << chrono::duration_cast<chrono::milliseconds>(endSearch - startSearch).count() << " ms\n";
    }

    void testSparseIndex(int n) {
        SparseIndex<RecordT> sparseIndex("sparse_index.dat", "sparse_data.dat", "MeetID");
        sparseIndex.init(initial_data);

        // Medir inserciones
        auto startInsert = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++) {
            sparseIndex.add(records[i]);
        }
        auto endInsert = chrono::high_resolution_clock::now();

        // Medir búsquedas
        auto startSearch = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++) {
            sparseIndex.search(to_string(records[i]->MeetID));
        }
        auto endSearch = chrono::high_resolution_clock::now();

        cout << "Sparse Index:\n";
        cout << "Insert time: " << chrono::duration_cast<chrono::milliseconds>(endInsert - startInsert).count() << " ms\n";
        cout << "Search time: " << chrono::duration_cast<chrono::milliseconds>(endSearch - startSearch).count() << " ms\n";
    }

    void testExtendibleHash(int n) {
        ExtendibleHash<RecordT> extendibleHash("extendible_hash.dat", "hash_index.dat", "MeetID");
        extendibleHash.init(initial_data);

        // Medir inserciones
        auto startInsert = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++) {
            extendibleHash.add(records[i]);
        }
        auto endInsert = chrono::high_resolution_clock::now();

        // Medir búsquedas
        auto startSearch = chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++) {
            extendibleHash.search(to_string(records[i]->MeetID));
        }
        auto endSearch = chrono::high_resolution_clock::now();

        cout << "Extendible Hash:\n";
        cout << "Insert time: " << chrono::duration_cast<chrono::milliseconds>(endInsert - startInsert).count() << " ms\n";
        cout << "Search time: " << chrono::duration_cast<chrono::milliseconds>(endSearch - startSearch).count() << " ms\n";
    }

    void runTests() {
        int n = 200;
        testSequentialFile(n);
        testSparseIndex(n);
        //testExtendibleHash();
    }
};

int main() {
    PerformanceTester<RecordMeet> tester("dataSet/meet2.csv", "dataSet/meet1.csv");
    tester.runTests();
    return 0;
}
