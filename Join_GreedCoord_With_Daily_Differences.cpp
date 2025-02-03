#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

// Helper function to trim spaces
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" ");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" ");
    return str.substr(first, last - first + 1);
}

void joinTable(const std::string& csvFile, const std::string& greedFilePath, const std::string& outputFilePath) {
    std::unordered_map<std::string, std::pair<std::string, std::string>> coordMap; // Area -> (Lat, Lng)

    // Read greed_coord.dat and populate coordMap
    std::ifstream greedFile(greedFilePath);
    if (!greedFile.is_open()) {
        std::cerr << "Error: Unable to open " << greedFilePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(greedFile, line)) {
        std::istringstream iss(line);
        std::string area, temp, lat, lng;

        // Read columns from greed_coord.dat
        std::getline(iss, area, '\t'); // Area (1st column)
        std::getline(iss, temp, '\t'); // Skip column 2
        std::getline(iss, temp, '\t'); // Skip column 3
        std::getline(iss, lng, '\t');  // Lng (4th column)
        std::getline(iss, lat, '\t');  // Lat (5th column)

        area = trim(area);
        coordMap[area] = {trim(lat), trim(lng)};
    }
    greedFile.close();

    // Open CSV file
    std::ifstream file(csvFile);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open " << csvFile << std::endl;
        return;
    }

    // Open output file
    std::ofstream outputFile(outputFilePath);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to create " << outputFilePath << std::endl;
        return;
    }

    // Process CSV file
    std::string header;
    std::getline(file, header); // Read the header
    outputFile << header << ",Lat,Lng\n"; // Append Lat and Lng columns to the header

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string column, area;
        std::vector<std::string> columns;
        while (std::getline(iss, column, ',')) {
            columns.push_back(column);
        }

        if (columns.size() > 1) {
            area = trim(columns[1]); // Area is the second column in daily_differences CSV files

            // Check if the area exists in coordMap
            if (coordMap.find(area) != coordMap.end()) {
                outputFile << line << "," << coordMap[area].first << "," << coordMap[area].second << "\n";
            } else {
                outputFile << line << ",,\n"; // Append empty columns if no match
            }
        }
    }

    file.close();
    outputFile.close();
    std::cout << "Joined table saved to " << outputFilePath << std::endl;
}

int main() {
    // File paths
    std::string dailyDifferences1 = "daily_differences_1.csv";
    std::string dailyDifferences2 = "daily_differences_2.csv";
    std::string greedFilePath = "greed_coord.dat";

    // Output file paths
    std::string outputFile1 = "joined_daily_differences_1.csv";
    std::string outputFile2 = "joined_daily_differences_2.csv";

    // Process the first file
    joinTable(dailyDifferences1, greedFilePath, outputFile1);

    // Process the second file
    joinTable(dailyDifferences2, greedFilePath, outputFile2);

    return 0;
}
