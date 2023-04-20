#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric> // for std::accumulate

// Map function: takes a file name and outputs a vector of key-value pairs
std::vector<std::pair<std::string, int>> Map(const std::string& filename) {
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

// Reduce function: takes a key and a vector of values and outputs a single value
int Reduce(const std::string& key, const std::vector<int>& values) {
    return std::accumulate(values.begin(), values.end(), 0);
}

int main() {
    // Input files
    std::vector<std::string> files = {"file1.txt", "file2.txt", "file3.txt"};

    // Map phase: apply Map function to each input file
    std::vector<std::vector<std::pair<std::string, int>>> map_outputs;
    std::transform(files.begin(), files.end(), std::back_inserter(map_outputs), Map);

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
        reduce_outputs[pair.first] = Reduce(pair.first, pair.second);
    }

    // Output the results
    for (const auto& pair : reduce_outputs) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    return 0;
}