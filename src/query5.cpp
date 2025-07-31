#include "query5.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <algorithm>


// Helper function to check if a string is numeric (all digits)
bool isNumber(const std::string& s) {
    if (s.empty()) return false;
    bool decimal_found = false;
    for (char c : s) {
        if(c=='.'){
            if(decimal_found)
                return false;

            decimal_found = true;
        }
        else if (c<'0' or c>'9') 
            return false;
    }
    return true;
}

// Function to parse command line arguments
// Example expected arguments:
// --r_name ASIA --start_date 1994-01-01 --end_date 1995-01-01 --threads 4 --table_path /path --result_path /output
bool parseArgs(int argc, char* argv[], 
               std::string& r_name,        // region name
               std::string& start_date,    // query start date
               std::string& end_date,      // query end date
               int& num_threads,           // number of threads to use
               std::string& table_path,    // input data directory
               std::string& result_path)   // output file path
{
    // Loop over arguments in pairs: option name and its value
    for (int argument_index = 1; argument_index < argc; argument_index += 2) {
        if (argument_index + 1 >= argc) break;  // prevent reading beyond argc

        std::string key = argv[argument_index];        // current option name
        std::string val = argv[argument_index + 1];    // current option value

        // Match keys to parameters
        if (key == "--r_name") 
            r_name = val;
        else if (key == "--start_date") 
            start_date = val;
        else if (key == "--end_date") 
            end_date = val;
        else if (key == "--threads") {
            if(!isNumber(val)) return false;         // not a valid thread count
            num_threads = std::stoi(val);            // convert thread count string to integer
        }
        else if (key == "--table_path") 
            table_path = val;
        else if (key == "--result_path") 
            result_path = val;
        else
            return false;  // unknown argument key - invalid input
    }
    return true;  // all arguments processed successfully
}




// Split a string by delimiter
std::vector<std::string> splitStringByDelimiter(const std::string& input, char delim = '|') {
    std::vector<std::string> tokens;
    size_t startPos = 0;                 // Start index of the next substring
    size_t delimPos = input.find(delim); // Position of next delimiter

    while (delimPos != std::string::npos) {
        tokens.emplace_back(input.substr(startPos, delimPos - startPos));
        startPos = delimPos + 1;        // Move start to next possible substring
        delimPos = input.find(delim, startPos);  //finding next delimiter
    }

    // Add the last token (or the whole string if no delimiter found)
    tokens.emplace_back(input.substr(startPos));
    return tokens;
}

// ---------------- Sequential Loaders for smaller tables ----------------

// Function to load the Region table from a file into a vector of Region structs
bool loadRegion(const std::string &filepath, std::vector<Region> &region_data) {
    std::ifstream file(filepath);  // Open the file for reading

    if (!file.is_open())            
        return false;               // Error Opening File

    std::string line;
    // Read each line from the file until End Of File
    while (std::getline(file, line)) {
        std::vector<std::string> tokens = splitStringByDelimiter(line);       // Split the line by delimiter ('|') into tokens

        if (tokens.size() < 4)    
            continue;             // Verify required columns and accommodate dummy or empty values

        if (!isNumber(tokens[0]))
            continue;             // R_REGIONKEY should be a valid number


        // convert and push INTEGER R_REGIONKEY, STRING R_NAME
        region_data.emplace_back(std::stoi(tokens[0]), tokens[1]);
    }

    return true;                    // Return true if loading succeeded
}

// Function to load the Nation table from a file into a vector of Nation structs
bool loadNation(const std::string &filepath, std::vector<Nation> &nation_data) {
    std::ifstream file(filepath);  // Open the file for reading

    if (!file.is_open())            
        return false;               // Error Opening File

    std::string line;
    // Read each line until End Of File
    while (std::getline(file, line)) {
        std::vector<std::string> tokens = splitStringByDelimiter(line);  // Split line by delimiter ('|')
        if (tokens.size() < 4)    
            continue;            // Verify required columns and accommodate dummy or empty values

        if (!isNumber(tokens[0]) || !isNumber(tokens[2]))
            continue;             // N_NATIONKEY or N_REGIONKEY should be a valid number

        // convert and push INTEGER N_NATIONKEY, STRING N_NAME, INTEGER N_REGIONKEY
        nation_data.emplace_back(std::stoi(tokens[0]), tokens[1], std::stoi(tokens[2]));
    }
    return true;                  // Return true if loading succeeded
}

// Function to load the Customer table from a file into a vector of Customer structs
bool loadCustomer(const std::string &filepath, std::vector<Customer> &customer_data) {
    std::ifstream file(filepath);  // Open the file for reading

    if (!file.is_open())            
        return false;               // Error Opening File

    std::string line;    

    // Read each line until End Of File
    while (std::getline(file, line)) {
        std::vector<std::string> tokens = splitStringByDelimiter(line);  // Split line by delimiter ('|')

        if (tokens.size() < 8)    // Verify required columns and accommodate dummy or empty values
            continue;             

        if (!isNumber(tokens[0]) || !isNumber(tokens[3]))   // C_CUSTKEY or C_NATIONKEY should be a valid numbers
            continue;             

        
        customer_data.emplace_back(std::stoi(tokens[0]), std::stoi(tokens[3])); //convert and push INTEGER C_CUSTKEY, INTEGER C_NATIONKEY
    }
    return true;
}

// Function to load the Supplier table from a file into a vector of Supplier structs
bool loadSupplier(const std::string &filepath, std::vector<Supplier> &supplier_data) {
    std::ifstream file(filepath);  // Open the file for reading

    if (!file.is_open())            
        return false;               // Error Opening File

    std::string line;

    // Read each line until End Of File
    while (std::getline(file, line)) {
        std::vector<std::string> tokens = splitStringByDelimiter(line);  // Split line by delimiter ('|')

        if (tokens.size() < 7)     // Verify required columns and accommodate dummy or empty values
            continue;            

        if (!isNumber(tokens[0]) || !isNumber(tokens[3]))   // S_SUPPKEY or S_SUPPKEY should be a valid numbers
            continue;             

    
        supplier_data.emplace_back(std::stoi(tokens[0]), std::stoi(tokens[3]));    //convert and push INTEGER S_SUPPKEY, INTEGER S_SUPPKEY
    }
    return true;
}

// Function to load the Orders table from a file into a vector of Orders structs
bool loadOrders(const std::string &filepath, std::vector<Orders> &orders_data) {
    std::ifstream file(filepath);  // Open the file for reading

    if (!file.is_open())            
        return false;               // Error Opening File

    std::string line;   

    // Read each line until End Of File
    while (std::getline(file, line)) {
        std::vector<std::string> tokens = splitStringByDelimiter(line);  // Split line by delimiter ('|')

        if (tokens.size() < 9)    // Verify required columns and accommodate dummy or empty values
            continue;             

        if (!isNumber(tokens[0]) || !isNumber(tokens[1]))   // O_ORDERKEY or O_CUSTKEY should be a valid numbers
            continue;             

    
        orders_data.emplace_back(std::stoi(tokens[0]), std::stoi(tokens[1]), tokens[4]);    //convert and push INTEGER O_ORDERKEY, INTEGER O_CUSTKEY, STRING O_ORDERDATE
    }
    return true;
}


// ---------------- Multithreaded load for LineItem Chunk ----------------
void loadLineItemChunk(
    const std::string &filepath,
    size_t start_offset,
    size_t end_offset,
    std::vector<LineItem> &lineitem_data)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return;
    }

    // Move file read cursor to the specified start offset
    file.seekg(start_offset);

    if (start_offset != 0) {
        // If start_offset is not zero, it may be in middle of a line,
        // so discard the partial line by reading to the next newline
        std::string dummy;
        std::getline(file, dummy);
    }

    std::string line;

    std::streampos current_pos = file.tellg();      // Track current reading position in the file

    // Read each line until End Of File
    while (current_pos < (std::streampos)end_offset && std::getline(file, line)) {
        std::vector<std::string> tokens = splitStringByDelimiter(line);  // Split line by delimiter ('|')

        if (tokens.size() < 16)    // Verify required columns and accommodate dummy or empty values
            continue;     

        if (!isNumber(tokens[0]) || !isNumber(tokens[2]) || !isNumber(tokens[5]) || !isNumber(tokens[6]))   // L_ORDERKEY or L_SUPPKEY or L_EXTENDEDPRICE or L_DISCOUNT should be a valid numbers
            continue;             

        lineitem_data.emplace_back(std::stoi(tokens[0]),std::stoi(tokens[2]),std::stod(tokens[5]),std::stod(tokens[6]));    //convert and push INTEGER L_ORDERKEY, INTEGER L_SUPPKEY, DECIMAL L_EXTENDEDPRICE, DECIMAL L_DISCOUNT

        current_pos = file.tellg();

        if (current_pos == -1) break; // EOF

    }   

}

// Function to load the Lineitem table from a file into a vector of vector of Lineitem structs using given no. of threads
bool loadLineItemMultithreaded(
    const std::string &filepath,
    std::vector<std::vector<LineItem>> &lineitem_data,
    int num_threads) 
{
    // Open file in text mode with get pointer initially at the end (to get file size)
    std::ifstream file(filepath, std::ios::ate);
    if (!file.is_open()) {
        return false;                                  // Error Opening File
    }

    size_t file_size = file.tellg();                   // Get total size of the file (from end position)
    size_t chunk_size = (file_size  + num_threads - 1 )/ num_threads;       // Calculate chunk size per thread

    std::vector<std::thread> threads(num_threads);     // Create thread objects container of size num_threads
    lineitem_data.resize(num_threads);                  // Creating chunks of lineitem of size num_threads

    // Launch threads to load chunks of the file
    for (int i = 0; i < num_threads; ++i) {
        size_t start_offset = i * chunk_size;          // Compute starting byte offset for current chunk
        size_t end_offset = std::min(start_offset + chunk_size, file_size);  //Compute ending byte offset for current chunk

        // Start thread to load chunk [start_offset, end_offset) into its portion of lineitem_data
        threads[i] = std::thread(loadLineItemChunk, filepath, start_offset, end_offset,
                                 std::ref(lineitem_data[i]));
    }

    // Wait for all threads to finish loading their respective chunks
    for (std::thread &t : threads)
        t.join();

    return true;   // Indicate successful loading
}



// Function to read all TPCH tables from specified path into vectors
bool readTPCHData(
    const std::string& table_path, 
    std::vector<Customer> &customer_data,
    std::vector<Orders> &orders_data,
    std::vector<std::vector<LineItem>> &lineitem_data,
    std::vector<Supplier> &supplier_data,
    std::vector<Nation> &nation_data,
    std::vector<Region> &region_data,
    const int num_threads)
{
    // Load small tables sequentially, return false if any fail
    if (!loadRegion(table_path + "/region.tbl", region_data) ||
        !loadNation(table_path + "/nation.tbl", nation_data) ||
        !loadCustomer(table_path + "/customer.tbl", customer_data) ||
        !loadOrders(table_path + "/orders.tbl", orders_data) ||
        !loadSupplier(table_path + "/supplier.tbl", supplier_data)) {
        return false;
    }

    // Load the large lineitem table using multithreading, 
    // storing data in multiple chunks (vectors) per thread
    if (!loadLineItemMultithreaded(table_path + "/lineitem.tbl", lineitem_data, num_threads)) {
        return false;
    }

    return true;  // All tables loaded successfully
}




// Stores filtered keys and mappings for efficient query processing
struct PreprocessedData {
    std::unordered_set<int> regionKeys;                     // Region keys matching filter
    std::unordered_set<int> nationKeys;                     // Nation keys within selected regions
    std::unordered_map<int, std::string> nationNames;       // Nation key -> Nation name map
    std::unordered_set<int> customerKeys;                   // Customers belonging to selected nations
    std::unordered_map<int, int> customerToNation;          // Customer key -> Nation key map
    std::unordered_set<int> orderKeys;                      // Orders filtered by date and customer
    std::unordered_map<int, int> orderToCustomerNation;     // Order key -> Customer's nation key
    std::unordered_set<int> supplierKeys;                   // Suppliers in selected nations
    std::unordered_map<int, int> supplierToNation;          // Supplier key -> Nation key map
};

// Preprocess involved tables to filter and create lookup structures for efficient query execution
PreprocessedData preprocess(
    const std::string& regionName,                           // Region filter name
    const std::string& startDate,                            // Start date filter for orders (inclusive)
    const std::string& endDate,                              // End date filter for orders (exclusive)
    const std::vector<Customer>& customer_data,
    const std::vector<Orders>& orders_data,
    const std::vector<Supplier>& supplier_data,
    const std::vector<Nation>& nation_data,
    const std::vector<Region>& region_data)
{
    PreprocessedData processed_data;

    // Select region keys matching the given region name
    for (const Region& r : region_data)
        if (r.R_NAME == regionName)
            processed_data.regionKeys.insert(r.R_REGIONKEY);

    // Select nation keys belonging to the filtered regions and record nation names
    for (const Nation& n : nation_data)
        if (processed_data.regionKeys.count(n.N_REGIONKEY)) {
            processed_data.nationKeys.insert(n.N_NATIONKEY);
            processed_data.nationNames[n.N_NATIONKEY] = n.N_NAME;
        }

    // Select customers belonging to the selected nations and map customer -> nation
    for (const Customer& c : customer_data)
        if (processed_data.nationKeys.count(c.C_NATIONKEY)) {
            processed_data.customerKeys.insert(c.C_CUSTKEY);
            processed_data.customerToNation[c.C_CUSTKEY] = c.C_NATIONKEY;
        }

    // Select orders placed within the date range by filtered customers;
    // map order -> customer's nation key for fast lookup
    for (const Orders& o : orders_data)
        if (o.O_ORDERDATE >= startDate && o.O_ORDERDATE < endDate 
            && processed_data.customerKeys.count(o.O_CUSTKEY)) {
            processed_data.orderKeys.insert(o.O_ORDERKEY);
            processed_data.orderToCustomerNation[o.O_ORDERKEY] = processed_data.customerToNation[o.O_CUSTKEY];
        }

    // Select suppliers in the filtered nations and map supplier -> nation
    for (const Supplier& s : supplier_data)
        if (processed_data.nationKeys.count(s.S_NATIONKEY)) {
            processed_data.supplierKeys.insert(s.S_SUPPKEY);
            processed_data.supplierToNation[s.S_SUPPKEY] = s.S_NATIONKEY;
        }

    return processed_data;
}


// ---------------- Threaded Query Processing ----------------
void threadProcessLineitems(
    const PreprocessedData& processed_data,                 // Pre-filtered data and mappings for efficient lookup
    const std::vector<LineItem>& lineitem_data,             // Chunk of LineItem rows assigned to this thread
    std::map<std::string, double>& threadResult) {          // Per-thread aggregated result map (nation -> revenue)

    // Iterate over each LineItem in this thread's chunk
    for (int line_index = 0; line_index < lineitem_data.size(); ++line_index) {
        const LineItem& row = lineitem_data[line_index];

        // Filter: skip if order key is not in the preprocessed set (filtered orders)
        if (!processed_data.orderKeys.count(row.L_ORDERKEY)) continue;

        // Filter: skip if supplier key is not in the preprocessed set (filtered suppliers)
        if (!processed_data.supplierKeys.count(row.L_SUPPKEY)) continue;

        // Lookup customer and supplier nations via preprocessed maps
        int custNation = processed_data.orderToCustomerNation.at(row.L_ORDERKEY);
        int suppNation = processed_data.supplierToNation.at(row.L_SUPPKEY);

        // Include only if customer and supplier nations are the same
        if (custNation == suppNation) {
            // Calculate revenue for this line item after discount
            double revenue = row.L_EXTENDEDPRICE * (1.0 - row.L_DISCOUNT);

            // Get supplier nation name for aggregation key
            const std::string& nationName = processed_data.nationNames.at(suppNation);

            // Accumulate revenue in thread-local result map
            threadResult[nationName] += revenue;
        }
    }
}


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
    std::vector<Nation_Revenue> &results) 
{
    // Preprocess data (apply filters, prepare lookup tables)
    PreprocessedData processed_data = preprocess(r_name, start_date, end_date, 
                                       customer_data, orders_data,
                                       supplier_data, nation_data, region_data);

    std::vector<std::map<std::string, double>> partial_results(num_threads);  // Thread-local results
    std::vector<std::thread> threads(num_threads);

    // Launch threads to process LineItem chunks in parallel
    for (int thread_id = 0; thread_id < num_threads; ++thread_id) {
        threads[thread_id] = std::thread(threadProcessLineitems, processed_data, lineitem_data[thread_id], 
                                 std::ref(partial_results[thread_id]));
    }

    for (std::thread &th : threads) th.join();   // Wait for threads to finish

    // Merge partial thread results into one aggregated map
    std::map<std::string, double> merged_results;
    for (const std::map<std::string, double> &partial_result : partial_results)
        for (const std::pair<const std::string, double> & cur_result : partial_result)
            merged_results[cur_result.first] += cur_result.second;

    // Convert map to vector for sorting
    for (const std::pair<const std::string, double> &nation_revenue : merged_results)
        // push nation,revenue into result
        results.emplace_back(nation_revenue.first, nation_revenue.second);

    // Sort results by revenue descending
    std::sort(results.begin(), results.end(),
              [&](const Nation_Revenue &a, const Nation_Revenue &b) {
                  return a.REVENUE > b.REVENUE;
              });

    // Output results with high precision
    std::cout.precision(15);
    for (const Nation_Revenue& nation_revenue : results)
        std::cout << nation_revenue.NATION << "|" << nation_revenue.REVENUE << "\n";

    return true;
}



// Function to output results to the specified path
bool outputResults(const std::string& result_path, const std::vector<Nation_Revenue>& results) {
    // TODO: Implement outputting results to a file
    std::ofstream output_file(result_path);
    if(!output_file){
        return false;
    }

    // Print results
    output_file.precision(15);
    for (const Nation_Revenue& nation_revenue: results) {
        output_file<<nation_revenue.NATION<<"|"<<nation_revenue.REVENUE<<"\n";
    }

    output_file.close();
    return true;
} 