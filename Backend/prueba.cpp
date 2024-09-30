#include "sparseIndex.h"


int main(int argc, char const *argv[])
{
    SparseIndex<RecordMeet> index("CustomerSparseindex_file", "CustomerData_file", "MeetID");


  /*   std::cout << "pages =======================" << std::endl;
    for (int i = 0; i < 100; i++) {
        PageNode<RecordMeet>* page = index.readPage(i, "CustomerSparseindex_file");
        cout << "Pagina " << i << endl;
        if (page != nullptr) {
            for (int i = 0; i < page->count; i++) {
                cout << page->record[i].getStringAttribute("MeetID").second << endl;
            }
        } else {
            cout << "Pagina vacia" << endl;
        }
    }
 */

    std::cout << "NODOS INTERNOS =======================" << std::endl;
    for (int level = 1; level < 4; level++) {
        for (int i = 0; i < 10; i++) {
            IndexNode* index_node = index.readIndex(i, level);
            cout << "Nodo interno " << i << endl;
            if (index_node != nullptr) {
                for (int i = 0; i < index_node->count; i++) {
                    //pasar de char [] a string
                    string key;
                    for (int j = 0; j < max_length; j++) {
                        if (index_node->keys[i][j] == '\0') {
                            break;
                        }
                        key += index_node->keys[i][j];
                    }
                    cout << index_node->keys[i] << " " << index_node->children[i] << ": " << key << endl;
                }
            } else {
                cout << "Nodo interno vacio" << endl;
            }
        }
    }


    std::cout<<"SEARCH ======================="<<std::endl;

    RecordMeet *record = index.search("485");
    if (record != nullptr) {
        cout << "Record encontrado: " << record->getStringAttribute("MeetID").second << endl;
    } else {
        cout << "Record no encontrado" << endl;
    }
 
    return 0;
}
