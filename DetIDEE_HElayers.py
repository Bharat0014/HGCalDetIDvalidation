import csv

def generate_all_hgcal_silicon_detids():
    """Generates all possible detector IDs for EE or HE Silicon"""
    combinations = []
    counter = 0  # Initialize a counter for combinations
    
    for detector_type in [8]:  # [28:31] (1000 or 1001) (8 for EE , 9 for HESilicon)
        # Set the layer range based on the detector typ
        for wafer_type in range(0, 4):  # [26:27] (00, 01, 10, 11)
            for z_side in range(0, 2):  # [25:25] (0 or 1)
                for layer_number in [26]:  # Use the defined range for each dettype
                    for sign_v in range(0, 2):  # [19:19] (0 or 1)
                        for abs_v in range(0, 14):  # [15:18] (0 to 13)
                            for sign_u in range(0, 2):  # [14:14] (0 or 1)
                                for abs_u in range(0, 14):  # [10:13] (0 to 13)
                                    # Determine ranges based on wafer_type
                                    if wafer_type == 1 or wafer_type == 2:
                                        # Iterate over all possible combinations of (v_coordinate, u_coordinate) for wafer_type 0
                                        for v_coordinate in range(0, 16):   # [5:9] (5 bits for wafer_type 0)
                                            for u_coordinate in range(0, 16):   # [0:4] (5 bits for wafer_type 0)
                                                detid_bin = (
                                                    (detector_type << 28) |
                                                    (wafer_type << 26) |
                                                    (z_side << 25) |
                                                    (layer_number << 20) |
                                                    (sign_v << 19) |
                                                    (abs_v << 15) |
                                                    (sign_u << 14) |
                                                    (abs_u << 10) |
                                                    (v_coordinate << 5) |
                                                    u_coordinate
                                                )
                                                combinations.append((detector_type, detid_bin))
                                                counter += 1
                                                print(f"Combination #{counter}: Generated - DetectorType: {detector_type}, DetID: {detid_bin}")
                                    else:
                                        # Iterate over all possible combinations of (v_coordinate, u_coordinate) for wafer_type 1 and 2
                                        for v_coordinate in range(0, 24):   # [5:9] (4 bits for wafer_type 1 and 2)
                                            for u_coordinate in range(0, 24):   # [0:4] (4 bits for wafer_type 1 and 2)
                                                detid_bin = (
                                                    (detector_type << 28) |
                                                    (wafer_type << 26) |
                                                    (z_side << 25) |
                                                    (layer_number << 20) |
                                                    (sign_v << 19) |
                                                    (abs_v << 15) |
                                                    (sign_u << 14) |
                                                    (abs_u << 10) |
                                                    (v_coordinate << 5) |
                                                    u_coordinate
                                                )
                                                combinations.append((detector_type, detid_bin))
                                                counter += 1
                                                print(f"Combination #{counter}: Generated - DetectorType: {detector_type}, DetID: {detid_bin}")
    return combinations
# Generate all combinations
silicon_detid_list = generate_all_hgcal_silicon_detids()

# Combine both lists
combined_detid_list = silicon_detid_list

# Write the detector IDs to a CSV file
with open('detid_EElayer26.csv', mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["DetectorType", "DetID"])  # Write header with DetectorType and DetID
    for det_type, detid in combined_detid_list:
        writer.writerow([det_type, detid])  # Write DetectorType and DetID in the file

print(f"\nGenerated all possible HGCal DetIDs (EE and HESilicon)")

