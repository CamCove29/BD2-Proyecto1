// File: test_sequential_file.cpp

#include "sequentialfile.cpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

int asserted = 0;
int tests = 0;

// Test de inicialización de la clase y escritura inicial
void testInit() {
    //std::cout << "---- TEST DE INICIALIZACIÓN ----" << std::endl;
    tests += 1;
    try {
        std::string csv_file = "test_data.csv";
        std::string binary_file = "test_data.bin";

        // Inicializamos el archivo secuencial
        SequentileFile<RecordMeet, int> seqFile(binary_file, [](RecordMeet* rec) { return rec->MeetID; });
        seqFile.init(csv_file);
        
        asserted += 1;
        
    std::cout << "Tests: " << tests << ", Asserted: " << asserted << "/" << tests << std::endl;
    } catch (const std::exception& e) {
        asserted += 0;
        std::cout << "ASSERTED FAILED ON TEST INIT: " << tests << ", Asserted: " << asserted << "/" << tests << std::endl;
    }

}

// Test de agregar registros
void testAdd() {
    //std::cout << "---- TEST DE AGREGAR REGISTROS ----" << std::endl;
    tests += 1;

    std::string binary_file = "test_data.bin";
    SequentileFile<RecordMeet, int> seqFile(binary_file, [](RecordMeet* rec) { return rec->MeetID; });

    //remove record with ID 4 if it exists
    seqFile.remove(4);

    RecordMeet newRecord(4, "/path/to/meet4", "Federation4", "2024-01-04", "USA", "State4", "Town4", "Meet4");
    seqFile.add(&newRecord); // 1 si se agrega correctamente, 0 si no

    //search for the added record
    RecordMeet* found = seqFile.search(4);
    if (found) {
        /* std::cout << "Registro encontrado: " << found->MeetID << ", " << found->MeetName << std::endl;
        std::cout << "Registro agregado correctamente." << std::endl; */
        if (found->MeetID == 4) {
            asserted += 1;
            std::cout << "Tests: " << tests << ", Asserted: " << asserted << "/" << tests << std::endl;
        } else {
            asserted += 0;
            std::cout << "ASSERTED FAILED ON TEST ADD, FOUND UNEXPECTED: " << tests << ", Asserted: " << asserted << "/" << tests << std::endl;
        }
    } else {
        asserted += 0;
        std::cout << "ASSERTED FAILED ON TEST ADD NOT FOUND: " << tests << ", Asserted: " << asserted << "/" << tests << std::endl;
    }

}

// Test de búsqueda de registros
void testSearch() {
    //std::cout << "---- TEST DE BÚSQUEDA DE REGISTROS ----" << std::endl;
    std::string binary_file = "test_data.bin";
    SequentileFile<RecordMeet, int> seqFile(binary_file, [](RecordMeet* rec) { return rec->MeetID; });

    tests += 1;

    bool oneFound = false;

    for (int i = 1; i <= 4; i++) {
        int searchID = i;
        RecordMeet* found = seqFile.search(searchID);
        if (found) {
            /* std::cout << "Registro encontrado: " << found->MeetID << ", " << found->MeetName << std::endl; */
            if (found->MeetID == searchID) {
                oneFound = true;
            }
        } else {
            /* std::cout << "Registro con ID " << searchID << " no encontrado." << std::endl; */
        }
    }



    if (oneFound) {
        asserted += 1;
    std::cout << "Tests: " << tests << ", Asserted: " << asserted << "/" << tests << std::endl;
    } else {
        asserted += 0;
        std::cout << "ASSERTED FAILED ON TEST SEARCH: " << tests << ", Asserted: " << asserted << "/" << tests << std::endl;
    }



    
}

// Test de búsqueda de registros no encontrados
void testSearchNotFound() {
    //std::cout << "---- TEST DE BÚSQUEDA DE REGISTROS NO ENCONTRADOS ----" << std::endl;
    tests += 1;

    std::string binary_file = "test_data.bin";
    SequentileFile<RecordMeet, int> seqFile(binary_file, [](RecordMeet* rec) { return rec->MeetID; });

    int searchID = 46531;
    RecordMeet* found = seqFile.search(searchID);
    if (found) {
        /* std::cout << "Registro encontrado: " << found->MeetID << ", " << found->MeetName << std::endl; */
        asserted += 0;
        std::cout << "ASSERTED FAILED ON TEST SEARCH NOT FOUND: " << tests << ", Asserted: " << asserted << "/" << tests << std::endl;
    } else {
        /* std::cout << "Registro con ID " << searchID << " no encontrado." << std::endl; */
        asserted += 1;
        std::cout << "Tests: " << tests << ", Asserted: " << asserted << "/" << tests << std::endl;
    }

}

// Test de eliminación de registros
void testRemove() {
    //std::cout << "---- TEST DE ELIMINACIÓN DE REGISTROS ----" << std::endl;
    tests += 1;

    std::string binary_file = "test_data.bin";
    SequentileFile<RecordMeet, int> seqFile(binary_file, [](RecordMeet* rec) { return rec->MeetID; });

    int removeID = 3;
    seqFile.remove(removeID);

    //search for the removed record
    RecordMeet* found = seqFile.search(removeID);
    if (found) {
        /* std::cout << "Registro encontrado: " << found->MeetID << ", " << found->MeetName << std::endl;
        std::cout << "Registro eliminado incorrectamente." << std::endl; */
        asserted += 0;

    std::cout << "Tests: " << tests << ", Asserted: " << asserted << "/" << tests << std::endl;
    } else {
        /* std::cout << "Registro con ID " << removeID << " no encontrado." << std::endl;
        std::cout << "Registro eliminado correctamente." << std::endl;
         */

        asserted += 1;
        std::cout << "Tests: " << tests << ", Asserted: " << asserted << "/" << tests << std::endl;
    }

}

// Test de búsqueda por rango
void testRangeSearch() {
    //std::cout << "---- TEST DE BÚSQUEDA POR RANGO ----" << std::endl;
    tests += 1;

    std::string binary_file = "test_data.bin";
    SequentileFile<RecordMeet, int> seqFile(binary_file, [](RecordMeet* rec) { return rec->MeetID; });

    // Agregar registros del 50 al 60
    for (int i = 50; i <= 60; i++) {
        std::string meetName = "Meet" + std::to_string(i);

        RecordMeet newRecord(i, "/path/to/meet" + i, "Federation" + i, "2024-01-" + i, "USA", "State" + i, "Town" + i, "Meet_" + i);
        seqFile.add(&newRecord);
    }

    // Realizar búsqueda por rango del 54 al 60
    std::vector<RecordMeet*> results = seqFile.range_search(54, 60);
    bool allFound = true;

    for (int id = 54; id <= 60; id++) {
        bool found = false;
        for (const auto& rec : results) {
            if (rec->MeetID == id) {
                found = true;
                break;
            }
        }
        if (!found) {
            allFound = false;
            /* std::cout << "Registro con ID " << id << " no encontrado en el rango." << std::endl; */
        }
    }

    if (allFound) {
        asserted += 1;
        
    std::cout << "Tests: " << tests << ", Asserted: " << asserted << "/" << tests << std::endl;
    } else {
        asserted += 0;
        std::cout << "ASSERTED FAILED ON TEST RANGE SEARCH: " << tests << ", Asserted: " << asserted << "/" << tests << std::endl;
    }

}


int main() {
    std::cout << "Test de la clase SequentialFile" << std::endl;

    // Llamamos a las funciones de prueba
    testInit();        // Test de inicialización
    testAdd();         // Test de agregar registros
    testSearch();      // Test de búsqueda
    testSearchNotFound(); // Test de búsqueda de registros no encontrados
    testRemove();      // Test de eliminación
    testRangeSearch(); // Test de búsqueda por rango

    return 0;
}
