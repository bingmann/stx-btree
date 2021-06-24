// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "btree.h"

#include "zipf.h"
#include <string>
#include <random>
#include <fstream>  // https://stackoverflow.com/questions/15080231/c-variable-stdifstream-ifs-has-initializer-but-incomplete-type

// 80-byte payload
struct big_payload {
    uint64_t first_value;
    char remaining_values[72];

    bool operator< (const big_payload &other) const {
        return first_value < other.first_value;
    }
};


template <class T>
bool load_binary_data(T data[], int length, const std::string& file_path) {
  std::ifstream is(file_path.c_str(), std::ios::binary | std::ios::in);
  if (!is.is_open()) {
    return false;
  }
  is.read(reinterpret_cast<char*>(data), std::streamsize(length * sizeof(T)));
  is.close();
  return true;
}

template <class T>
bool load_text_data(T array[], int length, const std::string& file_path) {
  std::ifstream is(file_path.c_str());
  if (!is.is_open()) {
    return false;
  }
  int i = 0;
  std::string str;
  while (std::getline(is, str) && i < length) {
    std::istringstream ss(str);
    ss >> array[i];
    i++;
  }
  is.close();
  return true;
}

template <class T>
T* get_search_keys(T array[], int num_keys, int num_searches) {
  std::mt19937_64 gen(std::random_device{}());
  std::uniform_int_distribution<int> dis(0, num_keys - 1);
  auto* keys = new T[num_searches];
  for (int i = 0; i < num_searches; i++) {
    int pos = dis(gen);
    keys[i] = array[pos];
  }
  return keys;
}

template <class T>
T* get_search_keys_zipf(T array[], int num_keys, int num_searches) {
  auto* keys = new T[num_searches];
  ScrambledZipfianGenerator zipf_gen(num_keys);
  for (int i = 0; i < num_searches; i++) {
    int pos = zipf_gen.nextValue();
    keys[i] = array[pos];
  }
  return keys;
}



// Look at some stats
void printStat_double(stx::btree<double, double>::tree_stats stats)
{
  std::cout << "Num keys: " << stats.itemcount
            << std::endl;  
  std::cout << "Num Inner Nodes: " << stats.innernodes
            << std::endl;  
  std::cout << "Num Leaf Nodes: " << stats.leaves
            << std::endl;  
  std::cout << "The number of key/data slots in each inner node: " << stats.innerslots
            << std::endl;  
  std::cout << "The number of key/data slots in each leaf: " << stats.leafslots
            << std::endl;  
  std::cout << "The average fill of leaves: " << stats.avgfill_leaves()
            << std::endl;  
}



// Look at some stats
void printStat_uint64_t(stx::btree<uint64_t, uint64_t>::tree_stats stats)
{
  std::cout << "Num keys: " << stats.itemcount
            << std::endl;  
  std::cout << "Num Inner Nodes: " << stats.innernodes
            << std::endl;  
  std::cout << "Num Leaf Nodes: " << stats.leaves
            << std::endl;  
  std::cout << "The number of key/data slots in each inner node: " << stats.innerslots
            << std::endl;  
  std::cout << "The number of key/data slots in each leaf: " << stats.leafslots
            << std::endl;  
  std::cout << "The average fill of leaves: " << stats.avgfill_leaves()
            << std::endl;  
}



// Look at some stats
void printStat_640bit(stx::btree<uint64_t, big_payload>::tree_stats stats)
{
  std::cout << "Num keys: " << stats.itemcount
            << std::endl;  
  std::cout << "Num Inner Nodes: " << stats.innernodes
            << std::endl;  
  std::cout << "Num Leaf Nodes: " << stats.leaves
            << std::endl;  
  std::cout << "The number of key/data slots in each inner node: " << stats.innerslots
            << std::endl;  
  std::cout << "The number of key/data slots in each leaf: " << stats.leafslots
            << std::endl;  
  std::cout << "The average fill of leaves: " << stats.avgfill_leaves()
            << std::endl;  
}

