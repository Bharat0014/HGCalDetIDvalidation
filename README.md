# HGCal Raw Data Processing and SimHit Producer

This repository contains the development and validation steps for handling raw data of the High Granularity Calorimeter (HGCal) detector in the Compact Muon Solenoid (CMS) experiment.

## Overview

Processing raw data for the HGCal detector involves converting Raw Hits (SimHits) into `pCaloHits` and validating detector identifiers (DetIDs) against HGCal geometry. The key steps in this workflow ensure accurate data handling, visualization, and preparation for further analysis.

---

### 1. Set Up the CMSSW Environment
First, ensure you are using the correct CMSSW version. Set up the environment as follows:

```
# Load the CMSSW version
cd /path/to/your/working/directory
cmsrel CMSSW_14_1_0_pre2
cd CMSSW_14_1_0_pre2/src
cmsenv
```

### 2. Unpack necessary Package for HGcal Geometery
```
git cms-addpkg Geometry/CMSCommonData
git cms-addpkg Configuration/Geometry
git cms-addpkg Geometry/HGCalCommonData
git cms-addpkg Geometry/HGCalGeometry
git cms-addpkg Geometry/Records
git cms-addpkg Fireworks

```

### 3. Git clone this repo.
```
git clone https://github.com/Bharat0014/HGCalDetIDvalidation.git
scram b -j8

```
## Features

### Code to Generate All Possible Combinations of Raw DetIDs (Input csv for Producer)  

**Description**  
This script generates all possible combinations of raw DetIDs in csv formate  for the following HGCal subdetectors:  
- **EE (Electromagnetic Endcap)**  
- **HESilicon (Hadronic Endcap Silicon Layers)**  
- **HEScintillator (Hadronic Endcap Scintillator Layers)**  

**Key Features**  

![DetID Generation Parameter](https://github.com/Bharat0014/HGCalDetIDvalidation/blob/master/DetIDGeneration.png)
- Ensures that all valid combinations align with HGCal geometry definitions.  

**Usage**  
```
python3 DetIDEE_HElayers.py
python3 DetIDHEScintillator.py
```


## 1. Validation of DetID

To check and count the total number of valid DetIDs and plotes for each HGCal geometry, we provide separate codes for the following geometries to minimize complexity and confusion:
- **HGCalEESensitive**  
- **HGCalHESiliconSensitive**  
- **HGCalHEScintillatorSensitive**  

#### Code and Configuration Files
- Each geometry has a dedicated validation code:
  - **HGCalEESensitive**: [HGCalRawValidEE.cc](https://github.com/Bharat0014/HGCalDetIDvalidation/blob/master/plugins/HGCalRawValidEE.cc)   
  - **HGCalHESiliconSensitive**: [HGCalRawValidHESilicon.cc](https://github.com/Bharat0014/HGCalDetIDvalidation/blob/master/plugins/HGCalRawValidHESilicon.cc)  
  - **HGCalHEScintillatorSensitive**: [HGCalRawValidHEScintillator.cc](https://github.com/Bharat0014/HGCalDetIDvalidation/blob/master/plugins/HGCalRawValidHEScintillator.cc)  
- The corresponding configuration files are also provided for ease of use.

#### Validation plots For EE layer 1:

**Usage** 
```
cd HGCalDetIDvalidation/python
cmsRun HGCalRawValidEE_cfi.py
```


<div style="display: flex; justify-content: space-around;">
  <img src="https://github.com/Bharat0014/HGCalDetIDvalidation/blob/master/EElayer1_xVsy_type0.png" alt="Image 1" width="30%" />
  <img src="https://github.com/Bharat0014/HGCalDetIDvalidation/blob/master/EElayer1_xVsy_type1.png" alt="Image 2" width="30%" />
  <img src="https://github.com/Bharat0014/HGCalDetIDvalidation/blob/master/EElayer1_xVsy_type2.png" alt="Image 3" width="30%" />
</div>

#### Validation plots For HESilicon layer 27:

**Usage** 
```
cd HGCalDetIDvalidation/python
cmsRun HGCalRawValidHESilicon_cfi.py
```

<div style="display: flex; justify-content: space-around;">
  <img src="https://github.com/Bharat0014/HGCalDetIDvalidation/blob/master/HElayer1_xVsy_type0.png" alt="Image 1" width="30%" />
  <img src="https://github.com/Bharat0014/HGCalDetIDvalidation/blob/master/HElayer1_xVsy_type1.png" alt="Image 2" width="30%" />
  <img src="https://github.com/Bharat0014/HGCalDetIDvalidation/blob/master/HElayer1_xVsy_type2.png" alt="Image 3" width="30%" />
</div>

#### Validation plots For HEScintillator layer 45:

**Usage** 
```
cd HGCalDetIDvalidation/python
cmsRun HGCalRawValidHEScintillator_cfi.py
```

<p align="center">
  <img src="https://github.com/Bharat0014/HGCalDetIDvalidation/blob/master/HESc_Layer19.png" alt="Centered Image" width="50%" />
</p>

#### Key Features
- **Input**: The validation code takes a CSV file containing data for a particular layer as input.
- **Capabilities**:
  - Checks specific wafer cell hits for the given geometry layer and can get total number of valid detIDs as per wafer type, as per layer and as per detector type.
  - For **EE** and **HESilicon**, includes validation of partial wafers, comparing them against the raw data for consistency.

Using this structured approach, the validation process ensures precise alignment of DetIDs with HGCal geometry.  

## 2. Development of SimHit Producer
A specialized producer was implemented to:
- Process raw SimHit data.
- Convert RawHits(DetIds) into `pCaloHits`.
- Store intermediate results in `step1.root` for validation and subsequent processing.

#### Step1.root Generation
- The `HGCalRawProducer.cc` code, available in the `plugins` folder, is used for generating the `step1.root` file.
- The corresponding configuration file, `HGCalRawProducer_cfi.py`, is located in the `python` folder.
- You must specify the input raw DetIDs file in the configuration file to run the process.

#### Workflow
1. **Input CSV File**: The process begins with a CSV file containing the required Det_type and DetID and information. This file serves as the input for the producer.
2. **Populating g4SimHits**: The producer generates all the necessary `g4SimHits` branches. However, only the relevant branches—`HGCHitsEE`, `HGCHitsHEback`, and `HGCHitsHEfront`—are populated with data, while the rest remain empty.
3. **Additional Branches**: 
   - Additional branches such as `HEPMCProducer` and `genProduct` are created using a **Pythia8 generator**, defined in the configuration file.
   - The Pythia8 generator ensures that all required branches are present in the `step1.root` file without interfering with the data filled by the producer.

```
cd HGCalDetIDvalidation/python
cmsRun HGCalRawProducer_cfi.py
```

Using this method, the `step1.root` file is generated successfully, with all necessary branches in place. When this file is used as input for the step2 process, it works without any errors or conflicts.


## 3. Multi-Step Processing Pipeline
The raw data processing involves the following steps:
1. **`step1.root`**: Generated by the SimHit producer, serving as the initial processed output.
2. **`step2.root`**: Produced using `step1.root` as input, incorporating additional validations and refined processing.
```
cmsDriver.py step2  -s DIGI:pdigi_valid,L1TrackTrigger,L1,DIGI2RAW,HLT:@relval2026 --conditions auto:phase2_realistic_T33 --datatier GEN-SIM-DIGI-RAW -n 1 --eventcontent FEVTDEBUGHLT --geometry Extended2026D110 --era Phase2C17I13M9 --filein  file:updatedstep1.root  --fileout file:step2.root --nThreads 4 > step2.log  2>&1

```
4. **`step3.root`**: Final output generated using `step2.root` for extended validation and analysis.
```
cmsDriver.py step3  -s RAW2DIGI,RECO,RECOSIM,PAT,VALIDATION:@phase2Validation+@miniAODValidation,DQM:@phase2+@miniAODDQM --conditions auto:phase2_realistic_T33 --datatier GEN-SIM-RECO,MINIAODSIM,DQMIO -n 1 --eventcontent FEVTDEBUGHLT,MINIAODSIM,DQM --geometry Extended2026D110 --era Phase2C17I13M9 --filein  file:step2.root  --fileout file:step3.root > step3.log  2>&1

```

## 4. Visualization with Fireworks

The Fireworks visualization tool was configured to:  

#### Steps to Visualize Raw DetIDs
1. **Generate the Geometry File**:
   - Use the following command to create a geometry file for the 2026 tag D110 configuration:  
     ```
     cd $CMSSW/src
     cmsRun Fireworks/Geometry/python/dumpSimGeometry_cfg.py tag=2026 version=D110
     ```
   - This command generates the geometry file: `cmsSimGeom-2026D110.root`.

2. **Run Fireworks for Visualization**:
   - Use the generated geometry file and the `step3.root` file to visualize the DetIDs:  
     ```
     cmsShow --sim-geom-file ../cmsSimGeom-2026D110.root step3.root
     ```

This process enables an interactive and detailed inspection of raw DetIDs, ensuring accuracy in the data processing workflow.


---

## Repository Contents
- **`SimHitProducer/`**: Contains the implementation of the SimHit producer.
- **`Validation/`**: Scripts and tools for DetID validation, including layer and radius checks.
- **`Visualization/`**: Configuration files for Fireworks visualization of raw DetIDs.
- **`README.md`**: This file, describing the purpose and workflow of the repository.

---

