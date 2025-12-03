# Draft Doctor: Hextech Miner
Draft Doctor is a C++17 command-line tool that mines League of Legends champion synergies from professional match drafts.  
Given a large CSV of pick/ban data (e.g., Oracle’s Elixir), the program:
- Converts each team’s picks/bans into a transaction of champion IDs
- Runs Apriori and FP-Growth to find frequent itemsets
- Lets you compare their performance on the same dataset (“race mode”)
- Prints the top champion combinations (by support) as human-readable names
This project was built for the COP3530 - Data Structures and Algorithms course at the University of Florida.
---
## Project Structure
```text
CMakeLists.txt         # Top-level build file (fetches Catch2 + Google Benchmark)
include/
  apriori.hpp
  fpgrowth.hpp
  utils.hpp
src/
  main.cpp             # CLI entry point
  apriori.cpp          # Apriori implementation
  fpgrowth.cpp         # FP-Growth implementation
  utils.cpp            # Data loading and helper utilities
tests/
  CMakeLists.txt       # Builds a unit-test binary (skeleton)
  test_miners.cpp
benchmarks/
  CMakeLists.txt       # Builds a benchmark binary (skeleton)
  bench_main.cpp
````
---
## Requirements
* C++17-compatible compiler (e.g., `g++` ≥ 10, `clang++` ≥ 10, or MSVC 2019+)
* CMake ≥ 3.14
* Internet access on first configure so CMake can fetch:
  * [Catch2](https://github.com/catchorg/Catch2) for tests
  * [Google Benchmark](https://github.com/google/benchmark) for benchmarks
* A CSV dataset with pick/ban columns (see below)
---
## Dataset
Draft Doctor expects CSV data similar to Oracle’s Elixir League of Legends stats tables:
* There must be a header row.
* Any column whose name contains `"pick"` or `"ban"` is treated as a champion field
  (e.g., `ban1`, `ban2`, `pick1`, `blue_pick4`, etc.).
* Each row corresponds to a single team in a single game.
* Empty values and the string `"None"` are ignored.
At run time, the loader:
1. Scans the header row and collects the indices of all columns whose names contain `"pick"` or `"ban"`.
2. For each row, reads those columns, cleans out quotes, and discards `None` values.
3. Maps each champion name to an integer ID and builds a `Transaction` (set of IDs).
> For the course project requirement of ≥ 100,000 rows, you can concatenate multiple Oracle’s Elixir seasons/leagues into a single CSV.
---
## Building
```bash
# Clone the repository
git clone https://github.com/sokramerz/draftDoctor.git
cd draftDoctor
# Configure the build (will fetch Catch2 and Google Benchmark on first run)
cmake -S . -B build
# Build the main CLI, tests, and benchmarks
cmake --build build
```
The main executable will be named `draft-doctor` and placed in `build/` (or your build directory of choice).
---
## Running
From the `build/` directory (or adjust paths accordingly):
### 1. Basic usage (load dataset as argument)
```bash
./draft-doctor ../data/oracles_elixir_combined.csv
```
If a path is provided as the first argument:
* Draft Doctor attempts to load that CSV immediately.
* On success, it prints basic dataset stats (number of transactions and unique champions).
* You are then taken to the main menu.
### 2. Interactive usage (no arguments)
```bash
./draft-doctor
```
You’ll first see a banner:
```
-------------------------------------------
      DRAFT DOCTOR: HEXTECH MINER v1.0
-------------------------------------------
```
Then the main menu:
```
--- MAIN MENU ---
1. Load Data File
2. Run Apriori Mining
3. Run FP-Growth Mining
4. RACE: Apriori vs FP-Growth
5. Exit
Select:
```
**Option 1 – Load Data File**
* Enter a path to a CSV with pick/ban columns.
* On success, the program prints dataset statistics:
  * Number of transactions (team rows)
  * Number of unique champions discovered
**Option 2 – Run Apriori Mining**
* Prompts for a minimum support percentage (e.g., `1` for 1%).
* Converts the percentage to an integer support count based on the number of transactions (with a minimum of 1).
* Runs the Apriori miner and prints:
  * Total number of frequent patterns found
  * Elapsed time in milliseconds
* Displays the top 10 multi-champion itemsets (by support), showing champion names and support counts.
**Option 3 – Run FP-Growth Mining**
* Same interaction as Apriori:
  * Ask for support percentage
  * Run FP-Growth
  * Report number of patterns and elapsed time
  * Show top 10 synergies by support
**Option 4 – RACE: Apriori vs FP-Growth**
* Asks for a support percentage once.
* Runs both algorithms back-to-back on the same dataset.
* Prints their individual runtimes and a simple speedup factor (how many times faster FP-Growth was than Apriori).
**Option 5 – Exit**
* Quits the program.
---
## Implementation Notes
* **Transactions & Items**
  * Each transaction is a `Transaction` struct containing a `std::vector<int>` of champion IDs.
  * Champion names are mapped to integer IDs via `DataLoader` for compact storage; reverse lookup is used for printing.
* **Apriori**
  * Finds frequent 1-itemsets, then iteratively generates candidate `k`-itemsets from frequent `(k–1)`-itemsets.
  * Uses `std::includes` on sorted item vectors to test subset membership.
  * Stores frequent patterns as a `Pattern` struct: `{ std::set<int> items; int support; }`.
* **FP-Growth**
  * Builds an FP-tree with a header table linking nodes by item ID.
  * Mines conditional pattern bases recursively to generate frequent patterns without explicit candidate generation.
  * Uses a frequency-based comparator so more frequent items appear higher in the tree.
* **Utilities**
  * `clean_cell` strips surrounding quotes from CSV cells.
  * `DataLoader::load_data` handles CSV parsing and transaction construction.
  * `DataLoader::print_stats` prints a short dataset summary.
---
## Running Tests and Benchmarks
The CMake configuration also builds:
* `unit_tests` – a Catch2-based test binary (currently a minimal skeleton).
* `bench_run` – a Google Benchmark harness for future performance experiments.
From the `build/` directory:
```bash
# Run tests (once tests are added)
ctest
# Run benchmarks
./bench_run
```
(If you have not added tests/benchmarks yet, these targets will simply run and exit.)
---
## Authors
* **Stephen Kramer** (`sokramerz`) – primary implementation lead (core code, CLI, data loading, algorithms).
* **Alejandro Simon** (`asimon-ufl`) – report author, documentation, FP-Growth design contributions, and polish.
* **Jacob McDaniel** – README author, contributed to selected features and testing support.
---
## License
This project is for educational purposes as part of a university course.
If you plan to reuse or extend it, please check with the authors/instructor about licensing and attribution.
