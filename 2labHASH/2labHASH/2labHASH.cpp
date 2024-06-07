#include <iostream>
#include <list>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>
using namespace std;


unsigned int pearson_hash(const std::string& key) {
    static const unsigned char table[256] = {
    98, 6, 85, 150, 36, 23, 112, 164, 135, 207, 169, 9, 55, 64, 90, 171,
    198, 166, 208, 128, 181, 148, 114, 194, 231, 6, 160, 117, 86, 96, 126, 110,
    138, 39, 185, 100, 97, 5, 220, 203, 18, 67, 253, 242, 64, 208, 115, 117,
    79, 36, 191, 216, 155, 224, 90, 21, 171, 53, 136, 197, 39, 113, 55, 145,
    117, 183, 203, 198, 54, 27, 174, 139, 33, 58, 134, 22, 102, 30, 186, 38,
    115, 57, 202, 199, 29, 75, 87, 216, 252, 243, 209, 32, 88, 226, 181, 43,
    115, 202, 150, 146, 63, 185, 48, 191, 102, 242, 179, 8, 98, 13, 68, 169,
    195, 43, 86, 15, 31, 184, 82, 138, 200, 124, 251, 59, 44, 253, 0, 15,
    135, 151, 15, 15, 94, 202, 221, 88, 223, 101, 89, 245, 58, 119, 197, 250,
    30, 61, 58, 94, 225, 169, 244, 232, 33, 75, 76, 53, 223, 153, 158, 130,
    33, 4, 84, 21, 11, 136, 203, 194, 71, 16, 75, 110, 67, 197, 86, 237,
    251, 238, 21, 51, 237, 79, 82, 36, 116, 107, 3, 248, 246, 5, 219, 160,
    80, 211, 199, 174, 91, 141, 8, 239, 189, 244, 118, 39, 213, 36, 7, 162,
    248, 124, 87, 199, 9, 0, 19, 159, 50, 129, 142, 178, 48, 151, 250, 11,
    49, 77, 96, 53, 214, 165, 13, 168, 25, 194, 55, 113, 66, 107, 100, 125,
    80, 214, 33, 173, 191, 63, 187, 60, 182, 52, 116, 27, 8, 122, 178, 209,
    };

    unsigned int hash = 0;
    for (char c : key) {
        hash = table[hash ^ static_cast<unsigned char>(c)]; // XOR BETWEEN VALUE OF HASH AND ASCII CODE OF C(CHAR)
    }
    return hash;
}

bool compare_hashes(const std::string& str1, const std::string& str2) {
    unsigned int hash1 = pearson_hash(str1);
    unsigned int hash2 = pearson_hash(str2);

    return hash1 == hash2;
}

class MyUnorderedMap {
private:
    struct KeyValuePair {
        int key;
        int value;
        KeyValuePair(const int& k, const int& v) : key(k), value(v) {}
    };

    // vector list for chains
    std::vector<std::list<KeyValuePair>> table;
    unsigned int size;

    unsigned int hash_function(const int& key) const {
        std::stringstream ss;
        ss << key;
        std::string key_str = ss.str();
        return pearson_hash(key_str) % size;
    }

public:
    explicit MyUnorderedMap(unsigned int table_size) : size(table_size) {
        table.resize(size);
    }

    MyUnorderedMap(unsigned int table_size, unsigned int num_entries) : size(table_size) {
        table.resize(size);
        srand(static_cast<unsigned int>(time(nullptr)));
        for (unsigned int i = 0; i < num_entries; ++i) {
            int key = rand();
            int value = rand();
            insert(key, value);
        }
    }

    // copy constr
    MyUnorderedMap(const MyUnorderedMap& other) : size(other.size), table(other.table) {}

    ~MyUnorderedMap() {}

    MyUnorderedMap& operator=(const MyUnorderedMap& other) {
        if (this != &other) {
            size = other.size;
            table = other.table;
        }
        return *this;
    }

    void print() const {
        for (unsigned int i = 0; i < size; ++i) {
            for (const auto& pair : table[i]) {
                std::cout << pair.key << ": " << pair.value << std::endl;
            }
        }
    }

    // insert value | key
    void insert(const int& key, const int& value) {
        unsigned int index = hash_function(key);
        for (auto& pair : table[index]) {
            if (pair.key == key) {
                return;
            }
        }
        table[index].push_back(KeyValuePair(key, value));
    }

    // insert or assign value | key
    void insert_or_assign(const int& key, const int& value) {
        unsigned int index = hash_function(key);
        for (auto& pair : table[index]) {
            if (pair.key == key) {
                pair.value = value;
                return;
            }
        }
        table[index].push_back(KeyValuePair(key, value));
    }

    bool contains(const int& key) const {
        unsigned int index = hash_function(key);
        for (const auto& pair : table[index]) {
            if (pair.key == key) {
                return true;
            }
        }
        return false;
    }

    int* search(const int& key) {
        unsigned int index = hash_function(key);
        for (auto& pair : table[index]) {
            if (pair.key == key) {
                return &(pair.value);
            }
        }
        return nullptr;
    }

    bool erase(const int& key) {
        unsigned int index = hash_function(key);
        for (auto it = table[index].begin(); it != table[index].end(); ++it) {
            if (it->key == key) {
                table[index].erase(it);
                return true;
            }
        }
        return false;
    }

    int count(const int& key) const {
        unsigned int index = hash_function(key);
        return table[index].size();
    }
};

int main() {
    MyUnorderedMap map(10);

    map.insert(1, 10);
    map.insert(2, 20);
    map.insert(2, 30);

    map.print();

    std::cout << "value for key 2: " << *map.search(2) << std::endl;

    map.erase(2);
    std::cout << "after erasing key 2:" << std::endl;
    map.print();


    std::string input1, input2;
    std::cout << "enter the first string: ";
    std::getline(std::cin, input1);
    std::cout << "enter the second string: ";
    std::getline(std::cin, input2);

    if (compare_hashes(input1, input2)) {
        std::cout << "hashes equally." << std::endl;
    }
    else {
        std::cout << "hashes do not equally." << std::endl;
    }

    return 0;
}