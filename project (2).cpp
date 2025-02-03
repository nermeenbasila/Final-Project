#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip> // For formatting output
#include <limits>  // For finding max and min
#include <cmath>   // For square root

int main()
{
    std::ifstream file("data_integr_2024_13.csv");
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open the file." << std::endl;
        return 1;
    }

    std::vector<std::vector<double>> data; // To store temperatures
    std::string line;

    // Read and skip the header row (first row)
    if (!std::getline(file, line))
    {
        std::cerr << "Error: File is empty or has invalid format." << std::endl;
        return 1;
    }

    // Read remaining rows (excluding the first row)
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string value;
        std::vector<double> row;

        int colIndex = 0; // Track column index to skip the first column
        while (std::getline(ss, value, ','))
        {
            if (colIndex > 0)
            { // Skip the first column (day index)
                try
                {
                    row.push_back(std::stod(value)); // Convert string to double
                }
                catch (const std::invalid_argument &e)
                {
                    row.push_back(0.0); // Handle invalid data as 0
                }
            }
            colIndex++;
        }
        data.push_back(row); // Add row without index column
    }
    file.close();

    // Ensure data has at least one row and column
    if (data.empty() || data[0].empty())
    {
        std::cerr << "Error: The file has no valid data." << std::endl;
        return 1;
    }

    int days = data.size();     // Total days (rows without header)
    int areas = data[0].size(); // Total areas (columns without index column)
    int period = 14;            // Chunk size: 14 days
    int fileAreaLimit = 7000;   // Number of areas per file

    // Prepare files
    std::ofstream avgFile("area_avg_14_days.csv");
    if (!avgFile.is_open())
    {
        std::cerr << "Error: Could not create area averages file.\n";
        return 1;
    }
    avgFile << "Period,Area,Average Temperature,Standard Deviation,Max Temperature (Day),Min Temperature (Day)\n";

    std::ofstream ratiosFile("ratios_above_3.csv");
    if (!ratiosFile.is_open())
    {
        std::cerr << "Error: Could not create output file for ratios above 3.\n";
        return 1;
    }
    ratiosFile << "Day,Area,Temperature,Average,Difference,Ratio\n";

    int fileIndex = 1; // To track file numbers
    for (int startArea = 0; startArea < areas; startArea += fileAreaLimit)
    {
        // Create a new output file for the current subset of areas
        std::ofstream outFile("daily_differences_" + std::to_string(fileIndex) + ".csv");
        if (!outFile.is_open())
        {
            std::cerr << "Error: Could not create the output file for fileIndex " << fileIndex << "." << std::endl;
            return 1;
        }

        outFile << "Day,Area,Temperature,Average,Difference,Ratio\n";

        for (int area = startArea; area < std::min(startArea + fileAreaLimit, areas); ++area)
        {
            for (int startDay = 0; startDay < days; startDay += period)
            {
                double sum = 0.0;
                int count = 0;

                double maxTemp = std::numeric_limits<double>::lowest();
                int maxDay = -1;

                double minTemp = std::numeric_limits<double>::max();
                int minDay = -1;

                std::vector<double> periodTemps;

                for (int day = startDay; day < startDay + period && day < days; ++day)
                {
                    double temp = data[day][area];
                    if (temp == 0.0)
                        continue;

                    sum += temp;
                    count++;
                    periodTemps.push_back(temp);

                    if (temp > maxTemp)
                    {
                        maxTemp = temp;
                        maxDay = day;
                    }
                    if (temp < minTemp)
                    {
                        minTemp = temp;
                        minDay = day;
                    }
                }

                if (count == 0)
                    continue;

                double average = sum / count;
                double varianceSum = 0.0;
                for (double temp : periodTemps)
                {
                    varianceSum += (temp - average) * (temp - average);
                }
                double standardDeviation = std::sqrt(varianceSum / count);

                avgFile << "Days " << startDay << "-" << std::min(startDay + period - 1, days - 1)
                        << ",Area " << area
                        << "," << std::fixed << std::setprecision(4) << average
                        << "," << standardDeviation
                        << "," << maxTemp << " (Day " << maxDay << ")"
                        << "," << minTemp << " (Day " << minDay << ")\n";

                for (int day = startDay; day < startDay + period && day < days; ++day)
                {
                    double temp = data[day][area];
                    if (temp == 0.0)
                        continue;

                    double difference = temp - average;
                    double ratio = (standardDeviation > 0.0) ? difference / standardDeviation : 0.0;
                    outFile << "" << day
                            << "," << area
                            << "," << temp
                            << "," << average
                            << "," << difference
                            << "," << ratio << "\n";

                    if (ratio > 3.0)
                    {
                        ratiosFile << day
                                   << "," << area
                                   << "," << temp
                                   << "," << average
                                   << "," << difference
                                   << "," << ratio << "\n";
                        outFile << "" << day
                                << "," << area
                                << "," << temp
                                << "," << average
                                << "," << difference
                                << "," << ratio << "\n";
                    }
                }
            }
        }

        outFile.close();
        std::cout << "File 'daily_differences_" << fileIndex << ".csv' written successfully.\n";
        ++fileIndex;
    }

    avgFile.close();
    ratiosFile.close();

    std::cout << "File 'area_avg_14_days.csv' written successfully.\n";
    std::cout << "File 'ratios_above_3.csv' written successfully.\n";
    return 0;
}
