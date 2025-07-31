#include "query5.hpp"
#include <iostream>
#include <string>
#include <vector>

// TODO: Include additional headers as needed

int main(int argc, char* argv[]) {

    // auto start = std::chrono::high_resolution_clock::now();//  Runtime Started
    std::string r_name, start_date, end_date, table_path, result_path;
    int num_threads;

    if (!parseArgs(argc, argv, r_name, start_date, end_date, num_threads, table_path, result_path)) {
        std::cerr << "Failed to parse command line arguments." << std::endl;
        return 1;
    }

    std::vector<Customer> customer_data;
    std::vector<Orders> orders_data;
    std::vector<std::vector<LineItem>> lineitem_data;
    std::vector<Supplier> supplier_data;
    std::vector<Nation> nation_data;
    std::vector<Region> region_data;
    if (!readTPCHData(table_path, customer_data, orders_data, lineitem_data, supplier_data, nation_data, region_data,num_threads)) {
        std::cerr << "Failed to read TPCH data." << std::endl;
        return 1;
    }

    std::vector<Nation_Revenue> results;
    if (!executeQuery5(r_name, start_date, end_date, num_threads, customer_data, orders_data, lineitem_data, supplier_data, nation_data, region_data, results)) {
        std::cerr << "Failed to execute TPCH Query 5." << std::endl;
        return 1;
    }

    if (!outputResults(result_path, results)) {
        std::cerr << "Failed to output results." << std::endl;
        return 1;
    }

    std::cout << "\n\nTPCH Query 5 implementation completed.\n";


    // auto end = std::chrono::high_resolution_clock::now();// Runtime Completed
    // std::chrono::duration<double, std::milli> duration = end - start;
    // std::cout<<"\nTotal RunTime: "<<duration.count()<<"ms\n";

    return 0;
} 