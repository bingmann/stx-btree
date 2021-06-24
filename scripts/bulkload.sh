#!/bin/bash


# **************************** longitudes ****************************
./build/benchmark --keys_file=../data/longitudes-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=100000000 \
--batch_size=10000000 \
--insert_frac=0.0 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/longititude_100M.txt
echo "Bulkload for [longitudes] Finished!"

# **************************** longlat ****************************
./build/benchmark --keys_file=../data/longlat-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=100000000 \
--batch_size=10000000 \
--insert_frac=0.0 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/longlat_100M.txt
echo "Bulkload for [longlat] Finished!"

# **************************** lognormal ****************************
./build/benchmark_uint64 --keys_file=../data/lognormal-190M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=100000000 \
--batch_size=10000000 \
--insert_frac=0.0 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/lognormal_100M.txt
echo "Bulkload for [lognormal] Finished!"

# **************************** ycsb ****************************
./build/benchmark_640bit --keys_file=../data/ycsb-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=100000000 \
--batch_size=10000000 \
--insert_frac=0.0 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/ycsb_100M.txt
echo "Bulkload for [ycsb] Finished!"


