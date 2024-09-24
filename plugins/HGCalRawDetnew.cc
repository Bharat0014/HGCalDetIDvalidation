#ifndef HGCALRAWDETNEW_H
#define HGCALRAWDETNEW_H

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm> // For std::min_element and std::max_element
#include <iterator>

#include "TCanvas.h"
#include "TH2F.h"
#include "TStyle.h"
#include <TPolyMarker.h>
#include <TGraph.h>
#include <TColor.h>

#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
#include "DataFormats/DetId/interface/DetId.h"

#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"

#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"

#include "DataFormats/ForwardDetId/interface/ForwardSubdetector.h"
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCScintillatorDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCalTriggerDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetIdToModule.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetIdToROC.h"
#include "DataFormats/ForwardDetId/interface/HGCHEDetId.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EventSetupRecord.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "FWCore/Framework/interface/ModuleFactory.h"

#include "Geometry/Records/interface/HGCalGeometryRecord.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloTopology/interface/HGCalTopology.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "Geometry/HGCalCommonData/interface/HGCalParameters.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"
#include "Geometry/HGCalCommonData/interface/HGCalDDDConstants.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/ForwardGeometry/interface/CastorGeometry.h"

class HGCalRawDetnew : public edm::stream::EDProducer<> {
public:
  explicit HGCalRawDetnew(const edm::ParameterSet&);
  ~HGCalRawDetnew() override {}

  void produce(edm::Event&, const edm::EventSetup&) override;
  
private:
  std::string infoFileName_;

  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenEE_;
  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenHESil_;
  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenHESci_;

    edm::PCaloHitContainer HGCHitsEE;
    edm::PCaloHitContainer HGCHitsHEback;
    edm::PCaloHitContainer HGCHitsHEfront;

};

HGCalRawDetnew::HGCalRawDetnew(const edm::ParameterSet& iConfig)
    : infoFileName_(iConfig.getParameter<std::string>("infoFileName")),
      geomTokenEE_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalEESensitive"})),
      geomTokenHESil_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalHESiliconSensitive"})),
      geomTokenHESci_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalHEScintillatorSensitive"})) {	      
      produces<edm::PCaloHitContainer>("HGCHitsEE");
      produces<edm::PCaloHitContainer>("HGCHitsHEback");
      produces<edm::PCaloHitContainer>("HGCHitsHEfront");
       
      }

void HGCalRawDetnew::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
    auto  HGCHitsEE = std::make_unique<edm::PCaloHitContainer>();
    auto HGCHitsHEback = std::make_unique<edm::PCaloHitContainer>();
    auto HGCHitsHEfront = std::make_unique<edm::PCaloHitContainer>();
 
// TPolyMarker *marker = new TPolyMarker();      

     int pointIndex0 = 0, pointIndex1 = 0, pointIndex2 = 0;
  edm::ESHandle<HGCalGeometry> geomHandleEE;
  edm::ESHandle<HGCalGeometry> geomHandleHESil;
  edm::ESHandle<HGCalGeometry> geomHandleHESci;
  
  // Attempt to retrieve HGCalGeometry using the provided token
  try {
    geomHandleEE = iSetup.getHandle(geomTokenEE_);
    geomHandleHESil = iSetup.getHandle(geomTokenHESil_);
    geomHandleHESci = iSetup.getHandle(geomTokenHESci_);
  } catch (cms::Exception& e) {
    edm::LogError("HGCalRawDet") << "Failed to retrieve HGCalGeometry: " << e.what();
    throw;
  }

  // Check if the handle is valid before proceeding
  if (!geomHandleEE.isValid() || !geomHandleHESil.isValid() || !geomHandleHESci.isValid()) {
    edm::LogError("HGCalRawDet") << "Failed to retrieve valid HGCalGeometry!";
    throw cms::Exception("DataError") << "Failed to retrieve valid HGCalGeometry!";
  }
  const HGCalGeometry& geomEE = *geomHandleEE;
  const HGCalGeometry& geomHESil = *geomHandleHESil;
  const HGCalGeometry& geomHESci = *geomHandleHESci;

  // Debug output to confirm HGCalGeometry is loaded
  edm::LogInfo("HGCalRawDet") << "Successfully retrieved HGCalGeometry.";

 std::ofstream csvFile;
 csvFile.open("valid_EE_Only.csv", std::ios::out); // Overwrite mode
 
 std::ofstream csvFile1;
 csvFile1.open("valid_SiHE_Only.csv", std::ios::out); // Overwrite mode

 std::ofstream csvFile2;
 csvFile2.open("valid_ScHE_Only.csv", std::ios::out); // Overwrite mode

 std::ofstream csvFile3;
 csvFile3.open("valid_EE_HESi_Both.csv", std::ios::out); // Overwrite mode

 std::ofstream csvFile4;
 csvFile4.open("valid_EE_HESc_Both.csv", std::ios::out); // Overwrite mode

 std::ofstream csvFile5;
 csvFile5.open("valid_HESi_HESc_Both.csv", std::ios::out); // Overwrite mode
std::ofstream csvFile6;
 csvFile6.open("valid_EE_HESi_HESC_All.csv", std::ios::out); // Overwrite mode

 std::ofstream csvFile7;
 csvFile7.open("Invalid_EE_HESi_HESC_All.csv", std::ios::out); // Overwrite mode

std::array<int, 26> validHitsPerLayerEE = {};    // For EE
std::array<int, 21> validHitsPerLayerHESi = {};  // For HESi
std::array<int, 14> validHitsPerLayerHESc = {};  // For HESc

std::ifstream inputFile(infoFileName_);
  if (!inputFile.is_open()) {
    throw cms::Exception("FileOpenError") << "Failed to open file: " << infoFileName_;
  }

  std::string line;
  int lineNumber = 0;
  while (std::getline(inputFile, line)) {
    lineNumber++;
    // std::cout<<"linenumber" <<lineNumber<<std::endl;
    std::istringstream iss(line);
    uint32_t DetID;
    int det_type;
  char delimiter;  
    if (!(iss >>det_type>> delimiter >> DetID)) {
      edm::LogWarning("HGCalRawDet") << "Skipping invalid data at line " << lineNumber<< ": " << line;
      continue;
    }
    
    
  
  std::cout << "Processing DetID: " << DetID << " at line " << lineNumber << std::endl;    

   bool validEE = false;
   bool validHESi = false;
   bool validHESc = false;
   int layer_number = 0;
    float time=0, em=0, energy=0;
    double xs =0, ys=0, zs=0, rs=0;
    PCaloHit newHit;  
    
    DetId detId = DetId(DetID); 
    if(det_type==8 || det_type==9){
    validEE = geomEE.topology().valid(detId);
    
    validHESi = geomHESil.topology().valid(detId);
    }
    if(det_type ==10){
    validHESc = geomHESci.topology().valid(detId);  
    }
    if(validEE && !validHESi && !validHESc){
    HGCSiliconDetId hgcalDetId(detId);
    //GlobalPoint posEE = geomEE.getPosition(hgcalDetId.rawId());
    layer_number = hgcalDetId.layer(); 
    newHit = PCaloHit(hgcalDetId.rawId(),energy, time, 0, em, 0);
    
    csvFile <<DetID<<","<<hgcalDetId.rawId() << std::endl; 
    validHitsPerLayerEE[layer_number - 1]++;
    }
    else if(validHESi && !validEE && !validHESc){
    HGCSiliconDetId hgcalDetId(detId); 
    //GlobalPoint posHESi = geomHESil.getPosition(hgcalDetId.rawId());
     layer_number = hgcalDetId.layer();
    newHit = PCaloHit(hgcalDetId.rawId(),energy, time, 0, em, 0);
    csvFile1 <<DetID<<","<<hgcalDetId.rawId() << std::endl;
    validHitsPerLayerHESi[layer_number - 1]++;
    }
    else if (validHESc && !validEE && !validHESi){
    HGCScintillatorDetId hgcalDetId(detId);
    //GlobalPoint posHESc = geomHESci.getPosition(hgcalDetId.rawId());
     layer_number = hgcalDetId.layer();
    newHit = PCaloHit(hgcalDetId.rawId(), energy, time, 0, em, 0);
    csvFile2 <<DetID<<","<<hgcalDetId.rawId() << std::endl;
    validHitsPerLayerHESc[layer_number - 8]++;
    } 
    
    else if (validEE && validHESi && !validHESc ){
    csvFile3 <<DetID << std::endl;
    }
    
    else if (validEE && !validHESi && validHESc ){
    csvFile4 <<DetID << std::endl;
    }
    else if (!validEE && validHESi && validHESc ){
    csvFile5 <<DetID << std::endl;
    }
    
    else if (validEE && validHESi && validHESc ){
    csvFile6 <<DetID << std::endl;
    }
    else if (!validEE && !validHESi && !validHESc ){
    csvFile7 <<DetID << std::endl;
    }
    if (validEE) {
        HGCHitsEE->push_back(newHit);
      } 
    else if (((validHESi && !validHESc && !validEE ) || (!validHESi && validHESc && !validEE)) && (layer_number>=1 && layer_number<=11) ) {
        HGCHitsHEfront->push_back(newHit);
      } else if( layer_number>11 && ((validHESi && !validHESc && !validEE ) || (!validHESi && validHESc && !validEE))){
        HGCHitsHEback->push_back(newHit);
      }

   }

  inputFile.close();
  iEvent.put(std::move(HGCHitsEE), "HGCHitsEE");
  iEvent.put(std::move(HGCHitsHEback), "HGCHitsHEback");
  iEvent.put(std::move(HGCHitsHEfront), "HGCHitsHEfront");

  std::ofstream eeFile("valid_hits_per_layer_EE.csv");
eeFile << "Layer,EE Hits\n";
for (size_t i = 0; i < 26; ++i) {
    eeFile << (i + 1) << "," << validHitsPerLayerEE[i] << "\n";
}
eeFile.close();

// File for HESilicon hits (1 to 21)
std::ofstream heSiFile("valid_hits_per_layer_HESi.csv");
heSiFile << "Layer,HESilicon Hits\n";
for (size_t i = 0; i < 21; ++i) {
    heSiFile << (i + 1) << "," << validHitsPerLayerHESi[i] << "\n";
}
heSiFile.close();

// File for HEScintillator hits (8 to 21)
std::ofstream heScFile("valid_hits_per_layer_HESc.csv");
heScFile << "Layer,HEScintillator Hits\n";
for (size_t i = 7; i < 21; ++i) {  // Starts from layer 8
    heScFile << (i + 1) << "," << validHitsPerLayerHESc[i - 7] << "\n";  // Adjusted index for HESc
}
heScFile.close();



  }

DEFINE_FWK_MODULE(HGCalRawDetnew);

#endif // HGCALRAWDETNEW_H

