import csv

def generate_all_hgcal_scintillator_detids():
    """Generates all possible detector IDs for HE Scintillator"""
    combinations = []
    counter = 0  # Initialize a counter for combinations

    for detector_type in [10]:  # [28:31] (1010) for HEScintillator
        for tile_type in range(1, 3):  # [26:27] (1 for type "c", 2 for type "m")
            for z_side in range(0, 2):  # [25:25] (0 for +z, 1 for -z)
                for granularity in range(0, 2):  # [24:24] (0 for normal, 1 for fine)
                    for sipm_type in range(0, 2):  # [23:23] (0 for Small, 1 for Large)
                        for trigger_cell in range(2):  # [22:22] (0 or 1)
                            for layer_number in [21]:  # [17:21] (8 to 21)
                                for ring_index in range(1, 50):  # [9:16] (1 to 45)
                                    for iphi_index in range(0, 380):  # [0:8] (0 to 320)
                                        # Combine all segments into a 32-bit binary number
                                        detid_bin = (
                                            (detector_type << 28) |
                                            (tile_type << 26) |
                                            (z_side << 25) |
                                            (granularity << 24) |
                                            (sipm_type << 23) |
                                            (trigger_cell << 22) |
                                            (layer_number << 17) |
                                            (ring_index << 9) |
                                            iphi_index
                                        )
                                        combinations.append((detector_type, detid_bin))
                                        counter += 1  # Increment the counter
                                        print(f"Combination #{counter}: Generated - DetectorType: {detector_type}, DetID: {detid_bin}")
    return combinations

# Generate all combinations for scintillator
detid_list = generate_all_hgcal_scintillator_detids()

# Write the detector IDs to a CSV file
with open('detid_HESclayer21.csv', mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["DetectorType", "DetID"])  # Write header with DetectorType and DetID
    for det_type, detid in detid_list:
        writer.writerow([det_type, detid])  # Write DetectorType and DetID in the file

print(f"\nGenerated all possible HGCal Scintillator DetIDs and stored them in 'scintillator_detid_list.csv'")

