#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>

#define PI 3.14159265358979323846
#define RADIO_TERRESTRE 6371.0 // Earth's radius in kilometers
#define GRADOS_RADIANES PI / 180
#define RADIANES_GRADOS 180 / PI

// Function to calculate distance using latitude and longitude
long double calcGPSDistance(long double latitude_new, long double longitude_new, long double latitude_old, long double longitude_old)
{
    long double lat_new = latitude_new * GRADOS_RADIANES;
    long double lat_old = latitude_old * GRADOS_RADIANES;
    long double lat_diff = (latitude_new - latitude_old) * GRADOS_RADIANES;
    long double lng_diff = (longitude_new - longitude_old) * GRADOS_RADIANES;

    long double a = sin(lat_diff / 2) * sin(lat_diff / 2) +
                    cos(lat_new) * cos(lat_old) *
                        sin(lng_diff / 2) * sin(lng_diff / 2);
    long double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return RADIO_TERRESTRE * c; // Distance in kilometers
}

// Function to find and process data
void processData(const std::string &file1, const std::string &file2, int userDay, const std::string &userArea)
{
    std::vector<std::vector<std::string>> matchedRows; // Stores filtered rows
    std::vector<std::string> headers;                  // Header row
    long double userLat = 0, userLng = 0;
    bool matchFound = false;

    // Lambda to process a single file
    auto processFile = [&](const std::string &filePath)
    {
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            std::cerr << "Error: Unable to open " << filePath << std::endl;
            return;
        }

        std::string line;
        bool headerProcessed = false;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> columns;
            std::string column;

            // Parse CSV row
            while (std::getline(iss, column, ','))
            {
                columns.push_back(column);
            }

            if (!headerProcessed)
            {
                headers = columns; // Store headers
                headerProcessed = true;
                continue;
            }

            // Check for matching Day and Area
            if (std::stoi(columns[0]) == userDay && columns[1] == userArea)
            {
                userLat = std::stold(columns[6]); // Latitude
                userLng = std::stold(columns[7]); // Longitude
                matchFound = true;
                break;
            }
        }
        file.close();
    };

    // Process both files to find the user's Lat/Lng
    processFile(file1);
    processFile(file2);

    if (!matchFound)
    {
        std::cerr << "Error: No matching data found for Day " << userDay << " and Area " << userArea << std::endl;
        return;
    }

    // Lambda to filter rows based on distance and day constraints
    auto filterRows = [&](const std::string &filePath)
    {
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            std::cerr << "Error: Unable to open " << filePath << std::endl;
            return;
        }

        std::string line;
        bool headerProcessed = false;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> columns;
            std::string column;

            // Parse CSV row
            while (std::getline(iss, column, ','))
            {
                columns.push_back(column);
            }

            if (!headerProcessed)
            {
                headerProcessed = true; // Skip the header row
                continue;
            }

            int day = std::stoi(columns[0]);
            std::string area = columns[1];
            long double lat = std::stold(columns[6]);
            long double lng = std::stold(columns[7]);

            // Calculate distance and filter rows
            long double distance = calcGPSDistance(userLat, userLng, lat, lng);
            if (day <= userDay && day >= userDay - 13 && distance <= 500.0)
            {                                                // 14 days back and 500km distance
                columns.push_back(std::to_string(distance)); // Add distance to the row
                matchedRows.push_back(columns);
            }
        }
        file.close();
    };

    // Filter rows from both files
    filterRows(file1);
    filterRows(file2);

    // Save filtered data
    std::ofstream outFile("filtered_data_Day" + std::to_string(userDay) + "_Area" + userArea + ".csv");
    if (!outFile.is_open())
    {
        std::cerr << "Error: Unable to create output file." << std::endl;
        return;
    }

    // Write header row
    headers.push_back("Distance (km)"); // Add the distance column
    for (size_t i = 0; i < headers.size(); ++i)
    {
        outFile << headers[i];
        if (i < headers.size() - 1)
            outFile << ",";
    }
    outFile << "\n";

    // Write filtered rows
    for (const auto &row : matchedRows)
    {
        for (size_t i = 0; i < row.size(); ++i)
        {
            outFile << row[i];
            if (i < row.size() - 1)
                outFile << ",";
        }
        outFile << "\n";
    }

    outFile.close();
    std::cout << "Filtered data saved to 'filtered_data_Day" << userDay << "_Area" << userArea << ".csv'" << std::endl;
}

int main()
{
    // File paths
    std::string file1 = "joined_daily_differences_1.csv";
    std::string file2 = "joined_daily_differences_2.csv";

    // Get user input
    int userDay;
    std::string userArea;

    std::cout << "Enter Day: ";
    std::cin >> userDay;
    std::cout << "Enter Area: ";
    std::cin >> userArea;

    // Process data based on user input
    processData(file1, file2, userDay, userArea);

    return 0;
}
