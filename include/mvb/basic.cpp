// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/*
 * This short sample program demonstrates STX B+ Tree's API.
 */

#include "../stx/btree.h"

#include <iostream>
#include <random>

#define KEY_TYPE int
#define PAYLOAD_TYPE int

int main(int, char**) {
  std::mt19937_64 gen(std::random_device{}());
  std::uniform_int_distribution<PAYLOAD_TYPE> dis;
  stx::btree<KEY_TYPE, PAYLOAD_TYPE> index;

  const std::vector<KEY_TYPE> demo_keys = {10, 20, 30, 40, 50, 60, 70, 80, 90, 15, 25, 35, 45, 55, 65, 75, 85, 95};
  for (int i=0; i< (int)demo_keys.size(); i++)
  {
    index.insert(demo_keys[i], dis(gen));
  } 

  // auto stats = index.get_stats();
  index.print_root_depth();
  // index.print_all_leaves(0);
  index.print_btree();
}