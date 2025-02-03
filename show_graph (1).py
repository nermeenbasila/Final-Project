import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
import re  # For extracting area number from filename

# Define file path
file_path = "filtered_data_Day206_Area12586.csv"

# Extract area number and day from file name using regex
match = re.search(r'Area(\d+)', file_path)
day_match = re.search(r'Day(\d+)', file_path)
if match and day_match:
    highlight_area = int(match.group(1))  # Convert extracted number to integer
    highlight_day = int(day_match.group(1))  # Extract the day
else:
    raise ValueError(
        f"Could not extract area or day from filename: {file_path}")

print(f"Highlighting Area: {highlight_area}, Day: {highlight_day}")

# Check if file exists
if not os.path.exists(file_path):
    raise FileNotFoundError(
        f"File '{file_path}' not found. Ensure the correct path.")

# Load CSV file
df = pd.read_csv(file_path)

# Extract unique areas
areas = sorted(df['Area'].unique())  # Sorted for consistency

# Optimized color palette: First five are highly distinct (Blue, Green, Yellow, Pink, Red), then move to Brown, Gray, etc.
optimized_colors = [
    # Blue, Green, Yellow, Pink, Red
    "#0000FF", "#008000", "#FFD700", "#FF1493", "#FF0000",
    # Brown, Gray, Indigo, Cyan, Crimson
    "#8B4513", "#708090", "#4B0082", "#00FFFF", "#DC143C",
    # SaddleBrown, CadetBlue, Chocolate, YellowGreen, DarkRed
    "#A52A2A", "#5F9EA0", "#D2691E", "#9ACD32", "#8B0000",
    # DodgerBlue, Chartreuse, OrangeRed, LimeGreen, PeachPuff
    "#1E90FF", "#7FFF00", "#FF4500", "#32CD32", "#FFDAB9",
    # Maroon, Black (reserved for highlight), DarkSlateBlue, GreenYellow, LightCoral
    "#800000", "#000000", "#483D8B", "#ADFF2F", "#F08080"
]

# Assign colors to areas (cycling through the palette if needed)
colors = {}
for i, area in enumerate(areas):
    if area == highlight_area:
        colors[area] = 'black'  # Highlighted line is black
    else:
        # Cycle through the optimized palette
        colors[area] = optimized_colors[i % len(optimized_colors)]

# Create figure with three subplots (including legend)
fig, axes = plt.subplots(1, 3, figsize=(18, 6), gridspec_kw={
                         'width_ratios': [1, 1, 0.2]}, sharex=True)

# Plot Day vs Temperature
for area in areas:
    area_data = df[df['Area'] == area]

    # Set line thickness
    if area == highlight_area:
        line_width = 6  # Highlighted line (black) is 3x thicker
    else:
        line_width = 2  # Normal line thickness for other areas

    # Plot the lines
    axes[0].plot(area_data['Day'], area_data['Temperature'], marker='o', linestyle='-',
                 color=colors[area], linewidth=line_width, label=f'Area {area}')

    # Add a red dot for the specific point (Day 208 for Area 2515)
    if area == highlight_area and highlight_day in area_data['Day'].values:
        temp_value = area_data[area_data['Day'] ==
                               highlight_day]['Temperature'].values[0]
        axes[0].scatter([highlight_day], [temp_value], color='red',
                        s=150, zorder=5, label=f'Day {highlight_day} Highlight')  # Red dot

axes[0].set_title('Temperature Trends by Day and Area', fontsize=14)
axes[0].set_xlabel('Day', fontsize=12)
axes[0].set_ylabel('Temperature', fontsize=12)
axes[0].grid(True, linestyle='--', alpha=0.7)

# Plot Day vs Ratio
for area in areas:
    area_data = df[df['Area'] == area]

    # Set line thickness
    if area == highlight_area:
        line_width = 6  # Highlighted line (black) is 3x thicker
    else:
        line_width = 2  # Normal line thickness for other areas

    # Plot the lines
    axes[1].plot(area_data['Day'], area_data['Ratio'], marker='o', linestyle='-',
                 color=colors[area], linewidth=line_width)

    # Add a red dot for the specific point (Day 208 for Area 2515)
    if area == highlight_area and highlight_day in area_data['Day'].values:
        ratio_value = area_data[area_data['Day']
                                == highlight_day]['Ratio'].values[0]
        axes[1].scatter([highlight_day], [ratio_value],
                        color='red', s=150, zorder=5)  # Red dot

axes[1].set_title('Ratio Trends by Day and Area', fontsize=14)
axes[1].set_xlabel('Day', fontsize=12)
axes[1].set_ylabel('Ratio', fontsize=12)
axes[1].grid(True, linestyle='--', alpha=0.7)

# Create a legend panel
axes[2].axis('off')  # Hide axes
handles, labels = axes[0].get_legend_handles_labels()
axes[2].legend(handles, labels, title='Areas', loc='center', fontsize=12)

# Adjust layout
plt.tight_layout()

# Generate output file name and save figure
output_file = os.path.splitext(file_path)[0] + ".png"
plt.savefig(output_file, dpi=300, bbox_inches='tight')

print(f"Plot saved successfully as '{output_file}'")
