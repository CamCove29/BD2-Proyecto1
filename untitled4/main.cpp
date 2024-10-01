#include <iostream>
#include <string>
#include <vector>
#include "HashIndex.h" // Asegúrate de incluir el encabezado correcto
#include "Bucket.h"
#include "SMH.h"

// Definición de la clase Record
class Record {
private:
    int id;  // Suponiendo que cada registro tiene un ID único
    std::string data; // Otros datos que pueda tener

public:
    Record() : id(0), data("") {}
    Record(int id, const std::string& data) : id(id), data(data) {}

    int getID() const { return id; }
    std::string getData() const { return data; }

    // Método para obtener la clave primaria
    int getPrimaryKey() const { return id; }

    // Método para verificar si una clave coincide
    bool equalToKey(int key) const { return id == key; }
};

// Tipo de clave para el índice (puedes cambiarlo si es necesario)
using Key = int;

int main() {
    // Crear el índice con los nombres de archivo para el índice y los buckets
    HashIndex<Record, Key> index("indexFile.dat", "bucketFile.dat");

    // Crear un registro de prueba
    Record testRecord(1, "Test Data"); // Asegúrate de usar datos válidos para tu caso

    // Intenta insertar un único registro
    index.insert(testRecord);

    // Busca el registro usando la clave primaria
    auto results = index.search(testRecord.getPrimaryKey());

    // Comprueba resultados
    if (!results.empty()) {
        std::cout << "Registro encontrado: " << results[0].getData() << std::endl;
    } else {
        std::cout << "Registro no encontrado." << std::endl;
    }

    // Ejemplo de eliminación
    index.remove(testRecord.getPrimaryKey());

    // Verificar si el registro fue eliminado
    results = index.search(testRecord.getPrimaryKey());
    if (results.empty()) {
        std::cout << "Registro eliminado correctamente." << std::endl;
    } else {
        std::cout << "Error: el registro aún existe." << std::endl;
    }

    return 0;
}
