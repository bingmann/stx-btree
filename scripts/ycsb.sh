#!/bin/bash


# **************************** ycsb ****************************

# Read-only
./build/benchmark_640bit --keys_file=../data/ycsb-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.0 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/ycsb_uniform_100M_200M_10M_0.0.txt
echo "Task for [ycsb Read-only uniform 10M_batch] Finished!"

./build/benchmark_640bit --keys_file=../data/ycsb-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.0 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/ycsb_zipf_100M_200M_10M_0.0.txt
echo "Task for [ycsb Read-only zipf 10M_batch] Finished!"

# Read-heavy
./build/benchmark_640bit --keys_file=../data/ycsb-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.05 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/ycsb_uniform_100M_200M_10M_0.05.txt
echo "Task for [ycsb Read-heavy uniform 10M_batch] Finished!"

./build/benchmark_640bit --keys_file=../data/ycsb-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.05 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/ycsb_zipf_100M_200M_10M_0.05.txt
echo "Task for [ycsb Read-heavy zipf 10M_batch] Finished!"

# Write-heavy
./build/benchmark_640bit --keys_file=../data/ycsb-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.5 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/ycsb_uniform_100M_200M_10M_0.5.txt
echo "Task for [ycsb Write-heavy uniform 10M_batch] Finished!"

./build/benchmark_640bit --keys_file=../data/ycsb-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.5 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/ycsb_zipf_100M_200M_10M_0.5.txt
echo "Task for [ycsb Write-heavy zipf 10M_batch] Finished!"

# Write-only
./build/benchmark_640bit --keys_file=../data/ycsb-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=1 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/ycsb_uniform_100M_200M_10M_1.0.txt
echo "Task for [ycsb Write-only uniform 10M_batch] Finished!"

./build/benchmark_640bit --keys_file=../data/ycsb-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=1 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/ycsb_zipf_100M_200M_10M_1.0.txt
echo "Task for [ycsb Write-only zipf 10M_batch] Finished!"
