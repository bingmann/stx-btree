#include "../stx/btree.h"

#include <iostream>
#include <iomanip>
#include <chrono>
#include "flags.h"
#include "utils.h"

#define KEY_TYPE uint64_t
#define PAYLOAD_TYPE uint64_t

int main(int argc, char *argv[])
{
      auto flags = parse_flags(argc, argv);
      std::string keys_file_path = get_required(flags, "keys_file");
      std::string keys_file_type = get_required(flags, "keys_file_type");
      auto init_num_keys = stoi(get_required(flags, "init_num_keys"));
      auto total_num_keys = stoi(get_required(flags, "total_num_keys"));
      auto batch_size = stoi(get_required(flags, "batch_size"));
      auto insert_frac = stod(get_with_default(flags, "insert_frac", "0.5"));
      std::string lookup_distribution =
          get_with_default(flags, "lookup_distribution", "uniform");
      auto time_limit = stod(get_with_default(flags, "time_limit", "0.5"));
      bool print_batch_stats = get_boolean_flag(flags, "print_batch_stats");

      // Read keys from file
      auto keys = new KEY_TYPE[total_num_keys];
      if (keys_file_type == "binary")
      {
            load_binary_data(keys, total_num_keys, keys_file_path);
      }
      else if (keys_file_type == "text")
      {
            load_text_data(keys, total_num_keys, keys_file_path);
      }
      else
      {
            std::cerr << "--keys_file_type must be either 'binary' or 'text'"
                      << std::endl;
            return 1;
      }

      std::vector<std::pair<KEY_TYPE, PAYLOAD_TYPE>> pairs(init_num_keys);
      std::mt19937_64 gen_payload(std::random_device{}());
      for (int i = 0; i < init_num_keys; i++)
      {
            pairs[i].first = keys[i];
            pairs[i].second = static_cast<PAYLOAD_TYPE>(gen_payload());
      }

      // Create STX B+ Tree and bulk load
      stx::btree<KEY_TYPE, PAYLOAD_TYPE> index;
      std::sort(pairs.begin(), pairs.end(),
                [](auto const &a, auto const &b)
                { return a.first < b.first; }); // https://www.geeksforgeeks.org/sorting-a-vector-in-c/
      index.bulk_load(pairs.begin(), pairs.end());

      auto stats = index.get_stats();
      printStat_uint64_t(stats);

      // find the node with the minimum key
      // stx::btree<KEY_TYPE, PAYLOAD_TYPE>::leaf_node * min_node = index.lower_bound_node(-180.00);  
 
      // std::cout << "min_iter.key(): " << min_iter.key() << ", min_iter.data(): " << min_iter.data()
      //       << std::endl;  
      // index.print_all_leaves(-180.00);
      index.print_root_depth();

      // Run workload
      int i = init_num_keys;
      long long cumulative_inserts = 0;
      long long cumulative_lookups = 0;
      int num_inserts_per_batch = static_cast<int>(batch_size * insert_frac);
      int num_lookups_per_batch = batch_size - num_inserts_per_batch;
      double cumulative_insert_time = 0;
      double cumulative_lookup_time = 0;

      auto workload_start_time = std::chrono::high_resolution_clock::now();
      int batch_no = 0;
      PAYLOAD_TYPE sum = 0;
      std::cout << std::fixed; // scientific
      std::cout << std::setprecision(4);
      while (true)
      {
            batch_no++;

            // Do lookups
            KEY_TYPE *lookup_keys = nullptr;
            if (lookup_distribution == "uniform")
            {
                  lookup_keys = get_search_keys(keys, i, num_lookups_per_batch);
            }
            else if (lookup_distribution == "zipf")
            {
                  lookup_keys = get_search_keys_zipf(keys, i, num_lookups_per_batch);
            }
            else
            {
                  std::cerr << "--lookup_distribution must be either 'uniform' or 'zipf'"
                            << std::endl;
                  return 1;
            }
            auto lookups_start_time = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < num_lookups_per_batch; j++)
            {
                  KEY_TYPE key = lookup_keys[j];
                  // PAYLOAD_TYPE *payload = index.get_payload(key);
                  stx::btree<KEY_TYPE, PAYLOAD_TYPE>::iterator payload = index.find(key);
                  if (payload.data())
                  {
                        sum += payload.data();
                  }
            }
            auto lookups_end_time = std::chrono::high_resolution_clock::now();
            double batch_lookup_time =
                std::chrono::duration_cast<std::chrono::nanoseconds>(lookups_end_time -
                                                                     lookups_start_time)
                    .count();
            cumulative_lookup_time += batch_lookup_time;
            cumulative_lookups += num_lookups_per_batch;
            delete[] lookup_keys;

            // Do inserts
            int num_actual_inserts =
                std::min(num_inserts_per_batch, total_num_keys - i);
            int num_keys_after_batch = i + num_actual_inserts;
            auto inserts_start_time = std::chrono::high_resolution_clock::now();
            for (; i < num_keys_after_batch; i++)
            {
                  index.insert(keys[i], static_cast<PAYLOAD_TYPE>(gen_payload()));
            }
            auto inserts_end_time = std::chrono::high_resolution_clock::now();
            double batch_insert_time =
                std::chrono::duration_cast<std::chrono::nanoseconds>(inserts_end_time -
                                                                     inserts_start_time)
                    .count();
            cumulative_insert_time += batch_insert_time;
            cumulative_inserts += num_actual_inserts;

            if (print_batch_stats)
            {
                  int num_batch_operations = num_lookups_per_batch + num_actual_inserts;
                  double batch_time = batch_lookup_time + batch_insert_time;
                  long long cumulative_operations = cumulative_lookups + cumulative_inserts;
                  double cumulative_time = cumulative_lookup_time + cumulative_insert_time;
                  // debugging
                  std::cout << "num_actual_inserts : " << num_actual_inserts
                            << "; num_inserts_per_batch : " << num_inserts_per_batch
                            << "; total_num_keys : " << total_num_keys
                            << "; current_num_keys : " << i
                            << std::endl;
                  std::cout << "Batch " << batch_no
                            << ", cumulative ops: " << cumulative_operations
                            << "\n\tbatch throughput:\t"
                            << num_lookups_per_batch / batch_lookup_time * 1e9
                            << " lookups/sec,\t"
                            << num_actual_inserts / batch_insert_time * 1e9
                            << " inserts/sec,\t" << num_batch_operations / batch_time * 1e9
                            << " ops/sec"
                            << "\n\tcumulative throughput:\t"
                            << cumulative_lookups / cumulative_lookup_time * 1e9
                            << " lookups/sec,\t"
                            << cumulative_inserts / cumulative_insert_time * 1e9
                            << " inserts/sec,\t"
                            << cumulative_operations / cumulative_time * 1e9 << " ops/sec"
                            << std::endl;
            }

            // Check for workload end conditions
            if (num_actual_inserts < num_inserts_per_batch)
            {
                  // End if we have inserted all keys in a workload with inserts
                  break;
            }
            double workload_elapsed_time =
                std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::high_resolution_clock::now() - workload_start_time)
                    .count();
            if (workload_elapsed_time > time_limit * 1e9 * 60)
            {
                  break;
            }
      }

      long long cumulative_operations = cumulative_lookups + cumulative_inserts;
      double cumulative_time = cumulative_lookup_time + cumulative_insert_time;

      // debugging
      stats = index.get_stats();
      printStat_uint64_t(stats);
      index.print_root_depth();
      // index.print_all_data_nodes();
      // std::cout << "The maximum key is : " << index.get_max_key() << ". The minimum key is : " << index.get_min_key()
      //           << std::endl;

      std::cout << "Cumulative stats: " << batch_no << " batches, "
                << cumulative_operations << " ops (" << cumulative_lookups
                << " lookups, " << cumulative_inserts << " inserts)"
                << "\n\tcumulative throughput:\t"
                << cumulative_lookups / cumulative_lookup_time * 1e9
                << " lookups/sec,\t"
                << cumulative_inserts / cumulative_insert_time * 1e9
                << " inserts/sec,\t"
                << cumulative_operations / cumulative_time * 1e9 << " ops/sec"
                << std::endl;

      delete[] keys;
      pairs.clear();

      return 0;
}

