#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric> // for std::accumulate

// Define the MapReduce interface
class IMapReduce {
public:
    virtual std::vector<std::pair<std::string, int>> Map(const std::string& filename) = 0;
    virtual int Reduce(const std::string& key, const std::vector<int>& values) = 0;
};

// Concrete implementation of the MapReduce interface
class WordCountMapReduce : public IMapReduce {
public:
    std::vector<std::pair<std::string, int>> Map(const std::string& filename) override {
        std::vector<std::pair<std::string, int>> result;
        std::ifstream file(filename);
        std::string word;
        while (file >> word) {
            // Convert to lowercase and remove non-alphabetic characters
            word.erase(std::remove_if(word.begin(), word.end(), [](char c) { return !std::isalpha(c); }), word.end());
            std::transform(word.begin(), word.end(), word.begin(), [](char c) { return std::tolower(c); });
            result.emplace_back(word, 1);
        }
        return result;
    }

    int Reduce(const std::string& key, const std::vector<int>& values) override {
        return std::accumulate(values.begin(), values.end(), 0);
    }
};

// The MapReduce orchestrator
class MapReduceEngine {
public:
    // Constructor takes an instance of a class implementing the IMapReduce interface
    MapReduceEngine(IMapReduce& map_reduce) : map_reduce_(map_reduce) {}

    void Run(const std::vector<std::string>& input_files) {
        // Map phase: apply Map function to each input file
        std::vector<std::vector<std::pair<std::string, int>>> map_outputs;
        for (const auto& file : input_files) {
            map_outputs.push_back(map_reduce_.Map(file));
        }

        // Shuffle phase: group the intermediate key-value pairs by key
        std::map<std::string, std::vector<int>> shuffled_outputs;
        for (const auto& output : map_outputs) {
            for (const auto& pair : output) {
                shuffled_outputs[pair.first].push_back(pair.second);
            }
        }

        // Reduce phase: apply Reduce function to each key and its corresponding values
        std::map<std::string, int> reduce_outputs;
        for (const auto& pair : shuffled_outputs) {
            reduce_outputs.emplace(pair.first, map_reduce_.Reduce(pair.first, pair.second));
        }

        // Output the results
        for (const auto& pair : reduce_outputs) {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
    }

private:
    IMapReduce& map_reduce_;
};

int main() {
    // Input files
    std::vector<std::string> files = {"file1.txt", "file2.txt", "file3.txt"};

    // Instantiate the MapReduce implementation
    WordCountMapReduce word_count_map_reduce;

    // Instantiate the MapReduce engine with the implementation
    MapReduceEngine engine(word_count_map_reduce);

    // Run the MapReduce engine on the input files
    engine.Run(files);

    return 0;
}
