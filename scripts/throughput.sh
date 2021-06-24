#!/bin/bash


# **************************** longitudes ****************************

# Read-only
./build/benchmark --keys_file=../data/longitudes-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.0 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/longititude_uniform_100M_200M_10M_0.0.txt
echo "Task for [longitudes Read-only uniform 10M_batch] Finished!"

./build/benchmark --keys_file=../data/longitudes-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.0 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/longititude_zipf_100M_200M_10M_0.0.txt
echo "Task for [longitudes Read-only zipf 10M_batch] Finished!"

# Read-heavy
./build/benchmark --keys_file=../data/longitudes-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.05 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/longititude_uniform_100M_200M_10M_0.05.txt
echo "Task for [longitudes Read-heavy uniform 10M_batch] Finished!"

./build/benchmark --keys_file=../data/longitudes-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.05 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/longititude_zipf_100M_200M_10M_0.05.txt
echo "Task for [longitudes Read-heavy zipf 10M_batch] Finished!"

# Write-heavy
./build/benchmark --keys_file=../data/longitudes-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.5 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/longititude_uniform_100M_200M_10M_0.5.txt
echo "Task for [longitudes Write-heavy uniform 10M_batch] Finished!"

./build/benchmark --keys_file=../data/longitudes-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.5 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/longititude_zipf_100M_200M_10M_0.5.txt
echo "Task for [longitudes Write-heavy zipf 10M_batch] Finished!"

# Write-only
./build/benchmark --keys_file=../data/longitudes-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=1 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/longititude_uniform_100M_200M_10M_1.0.txt
echo "Task for [longitudes Write-only uniform 10M_batch] Finished!"

./build/benchmark --keys_file=../data/longitudes-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=1 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/longititude_zipf_100M_200M_10M_1.0.txt
echo "Task for [longitudes Write-only zipf 10M_batch] Finished!"

# **************************** longlat ****************************
# Read-only
./build/benchmark --keys_file=../data/longlat-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.0 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/longlat_uniform_100M_200M_10M_0.0.txt
echo "Task for [longlat Read-only uniform 10M_batch] Finished!"

./build/benchmark --keys_file=../data/longlat-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.0 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/longlat_zipf_100M_200M_10M_0.0.txt
echo "Task for [longlat Read-only zipf 10M_batch] Finished!"

# Read-heavy
./build/benchmark --keys_file=../data/longlat-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.05 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/longlat_uniform_100M_200M_10M_0.05.txt
echo "Task for [longlat Read-heavy uniform 10M_batch] Finished!"

./build/benchmark --keys_file=../data/longlat-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.05 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/longlat_zipf_100M_200M_10M_0.05.txt
echo "Task for [longlat Read-heavy zipf 10M_batch] Finished!"

# Write-heavy
./build/benchmark --keys_file=../data/longlat-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.5 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/longlat_uniform_100M_200M_10M_0.5.txt
echo "Task for [longlat Write-heavy uniform 10M_batch] Finished!"

./build/benchmark --keys_file=../data/longlat-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=0.5 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/longlat_zipf_100M_200M_10M_0.5.txt
echo "Task for [longlat Write-heavy zipf 10M_batch] Finished!"

# Write-only
./build/benchmark --keys_file=../data/longlat-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=1 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/longlat_uniform_100M_200M_10M_1.0.txt
echo "Task for [longlat Write-only uniform 10M_batch] Finished!"

./build/benchmark --keys_file=../data/longlat-200M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=200000000 \
--batch_size=10000000 \
--insert_frac=1 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/longlat_zipf_100M_200M_10M_1.0.txt
echo "Task for [longlat Write-only zipf 10M_batch] Finished!"


# **************************** lognormal ****************************

# Read-only
./build/benchmark_uint64 --keys_file=../data/lognormal-190M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=190000000 \
--batch_size=10000000 \
--insert_frac=0.0 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/lognormal_uniform_100M_190M_10M_0.0.txt
echo "Task for [lognormal Read-only uniform 10M_batch] Finished!"

./build/benchmark_uint64 --keys_file=../data/lognormal-190M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=190000000 \
--batch_size=10000000 \
--insert_frac=0.0 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/lognormal_zipf_100M_190M_10M_0.0.txt
echo "Task for [lognormal Read-only zipf 10M_batch] Finished!"

# Read-heavy
./build/benchmark_uint64 --keys_file=../data/lognormal-190M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=190000000 \
--batch_size=10000000 \
--insert_frac=0.05 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/lognormal_uniform_100M_190M_10M_0.05.txt
echo "Task for [lognormal Read-heavy uniform 10M_batch] Finished!"

./build/benchmark_uint64 --keys_file=../data/lognormal-190M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=190000000 \
--batch_size=10000000 \
--insert_frac=0.05 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/lognormal_zipf_100M_190M_10M_0.05.txt
echo "Task for [lognormal Read-heavy zipf 10M_batch] Finished!"

# Write-heavy
./build/benchmark_uint64 --keys_file=../data/lognormal-190M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=190000000 \
--batch_size=10000000 \
--insert_frac=0.5 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/lognormal_uniform_100M_190M_10M_0.5.txt
echo "Task for [lognormal Write-heavy uniform 10M_batch] Finished!"

./build/benchmark_uint64 --keys_file=../data/lognormal-190M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=190000000 \
--batch_size=10000000 \
--insert_frac=0.5 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/lognormal_zipf_100M_190M_10M_0.5.txt
echo "Task for [lognormal Write-heavy zipf 10M_batch] Finished!"

# Write-only
./build/benchmark_uint64 --keys_file=../data/lognormal-190M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=190000000 \
--batch_size=10000000 \
--insert_frac=1 \
--lookup_distribution=uniform \
--time_limit=1 \
--print_batch_stats > ./reports/lognormal_uniform_100M_190M_10M_1.0.txt
echo "Task for [lognormal Write-only uniform 10M_batch] Finished!"

./build/benchmark_uint64 --keys_file=../data/lognormal-190M.bin.data \
--keys_file_type=binary \
--init_num_keys=100000000 \
--total_num_keys=190000000 \
--batch_size=10000000 \
--insert_frac=1 \
--lookup_distribution=zipf \
--time_limit=1 \
--print_batch_stats > ./reports/lognormal_zipf_100M_190M_10M_1.0.txt
echo "Task for [lognormal Write-only zipf 10M_batch] Finished!"

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

