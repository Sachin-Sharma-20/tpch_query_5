#ifndef QUERY5_HPP
#define QUERY5_HPP

#include <string>
#include <vector>


// ---------------- Structs for relevant columns ----------------
struct Region {
    int R_REGIONKEY;
    std::string R_NAME;
};

struct Nation {
    int N_NATIONKEY;
    std::string N_NAME;
    int N_REGIONKEY;
};

struct Customer {
    int C_CUSTKEY;
    int C_NATIONKEY;
};

struct Supplier {
    int S_SUPPKEY;
    int S_NATIONKEY;
};

struct Orders {
    int O_ORDERKEY;
    int O_CUSTKEY;
    std::string O_ORDERDATE; // stored as string for easy lexicographic date range compare
};

struct LineItem {
    int L_ORDERKEY;
    int L_SUPPKEY;
    double L_EXTENDEDPRICE;
    double L_DISCOUNT;
};

struct Nation_Revenue{
    std::string NATION;
    double REVENUE;
};


// Function to parse command line arguments
bool parseArgs(int argc, char* argv[], std::string& r_name, std::string& start_date, std::string& end_date, int& num_threads, std::string& table_path, std::string& result_path);

// Function to read TPCH data from the specified paths
bool readTPCHData(
	const std::string& table_path, 
    std::vector<Customer> &customer_data,
    std::vector<Orders> &orders_data,
    std::vector<std::vector<LineItem>> &lineitem_data,
    std::vector<Supplier> &supplier_data,
    std::vector<Nation> &nation_data,
    std::vector<Region> &region_data,
    const int num_threads);

// Function to execute TPCH Query 5 using multithreading
bool executeQuery5(
    const std::string &r_name,
    const std::string &start_date,
    const std::string &end_date,
    int num_threads,
    const std::vector<Customer> &customer_data,
    const std::vector<Orders> &orders_data,
    const std::vector<std::vector<LineItem>> &lineitem_data,
    const std::vector<Supplier> &supplier_data,
    const std::vector<Nation> &nation_data,
    const std::vector<Region> &region_data,
    std::vector<Nation_Revenue> &results);

// Function to output results to the specified path
bool outputResults(const std::string& result_path, const std::vector<Nation_Revenue>& results);

#endif // QUERY5_HPP 