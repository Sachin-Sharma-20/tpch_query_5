# TPCH Query 5 C++ Multithreading Project

This project implements the TPCH Query 5 benchmark using C++ with multithreading to optimize performance on large datasets. It efficiently loads data, preprocesses it, and executes the query in parallel to significantly reduce runtime while maintaining accuracy.

---

## Report

A detailed report on the project, including challenges faced, solutions applied, and performance analysis, is available in the `report/report.txt` file.

---

## Screenshots

Relevant screenshots demonstrating the program execution and results are located in the `screenshots/` folder.

---

## Prerequisites

- **CMake** version 3.10 or higher
- **C++ compiler** supporting C++20 standard (e.g., GCC 9+, Clang 10+, MSVC 2019+)
- For Windows user Build using `cmake --build .`)

Make sure these tools are installed and properly configured in your system PATH before building the project.


---

## Building the Project

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd tpch-query5-cpp
   ```

2. Create a build directory and navigate into it:
   ```bash
   mkdir build
   cd build
   ```

3. Generate the build files with CMake:
   ```bash
   cmake ..
   ```

4. Compile the project:  

- **Recommended (cross-platform):**  

   ```bash
   cmake --build .
   ```

- **Alternatively, on Linux/macOS:**  

   ```bash
   make 
   ```

## Running the Program
### Single-Threaded Execution
To run the program in single-threaded mode, use the following command:
```bash
./tpch_query5 --r_name ASIA --start_date 1994-01-01 --end_date 1995-01-01 --threads 1 --table_path /path/to/tables --result_path ../output_path/query5_output.txt

```

### Multi-Threaded Execution
To run the program in multi-threaded mode, specify the number of threads (e.g., 4):
```bash
./tpch_query5 --r_name ASIA --start_date 1994-01-01 --end_date 1995-01-01 --threads 4 --table_path /path/to/tables --result_path ../output_path/query5_output.txt

```

