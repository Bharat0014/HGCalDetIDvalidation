// -*- C++ -*-
//
// Package:    HGCalDetIDvalidation/HGCalRawValidHESilicon
// Class:      
//
/*

 Description: Example module for raw detID validation for HESilicon

 Implementation:
    
*/
//
// Original Author:  Bsirasva
//                     




#ifndef HGCALRAWVALIDHESILICON_H
#define HGCALRAWVALIDHESILICON_H

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
#include "Geometry/HGCalCommonData/interface/HGCalWaferType.h"

class HGCalRawValidHESilicon : public edm::stream::EDProducer<> {
public:
  explicit HGCalRawValidHESilicon(const edm::ParameterSet&);
  ~HGCalRawValidHESilicon() override {}

  void produce(edm::Event&, const edm::EventSetup&) override;
  
private:
  std::string infoFileName_;

 edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenHESil_;
  
    TGraph* graphType2;

};

HGCalRawValidHESilicon::HGCalRawValidHESilicon(const edm::ParameterSet& iConfig)
    : infoFileName_(iConfig.getParameter<std::string>("infoFileName")),
     geomTokenHESil_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalHESiliconSensitive"})){
       
       graphType2 = new TGraph();
       
      }

void HGCalRawValidHESilicon::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
   
  
 edm::ESHandle<HGCalGeometry> geomHandleHESil;
 
  // Attempt to retrieve HGCalGeometry using the provided token
  try {
   geomHandleHESil = iSetup.getHandle(geomTokenHESil_);
 } catch (cms::Exception& e) {
    edm::LogError("HGCalRawDet") << "Failed to retrieve HGCalGeometry: " << e.what();
    throw;
  }

  // Check if the handle is valid before proceeding
  if (!geomHandleHESil.isValid()) {
    edm::LogError("HGCalRawDet") << "Failed to retrieve valid HGCalGeometry!";
    throw cms::Exception("DataError") << "Failed to retrieve valid HGCalGeometry!";
  }
 const HGCalGeometry& geomHESil = *geomHandleHESil;
  
// Debug output to confirm HGCalGeometry is loaded
  edm::LogInfo("HGCalRawDet") << "Successfully retrieved HGCalGeometry.";

 std::ofstream csvFile;
 csvFile.open("valid_siliconHELayer9.csv", std::ios::out); // Overwrite mode
 
 std::ofstream csvFile3;
 csvFile3.open("invalid_siliconHELayer9.csv", std::ios::out); // Overwrite mode


  std::ofstream csvFile5;
  csvFile5.open("Index_HE27_2_2_validDetId.csv", std::ios::out); // Overwrite mode

 
 int pointIndex0 = 0, pointIndex1 = 0, pointIndex2 = 0,  pointIndex3 = 0;

std::array<int, 4> validHitsPerwaferEE = {};    // For EE
std::array<int, 100> validHitsPartialmethod1 = {};


std::ifstream inputFile(infoFileName_);
  if (!inputFile.is_open()) {
    throw cms::Exception("FileOpenError") << "Failed to open file: " << infoFileName_;
  }

  std::string line;
  int lineNumber = 0;
  while (std::getline(inputFile, line)) {
    lineNumber++;
     //std::cout<<"linenumber" <<lineNumber<<std::endl;
    std::istringstream iss(line);
    uint32_t DetID;
    int det_type;
  char delimiter;  
    if (!(iss >>det_type>> delimiter >> DetID)) {
      edm::LogWarning("HGCalRawDet") << "Skipping invalid data at line " << lineNumber<< ": " << line;
      continue;
    }
    
    
 
   bool validHEsi = false; 
   double xs =0, ys=0;
   int wafer_type =0;
   if(det_type == 9){
    validHEsi = geomHESil.valid(DetID);
   
    if(validHEsi){
    HGCSiliconDetId hgcalDetId(DetID);
    GlobalPoint posHE = geomHESil.getPosition(hgcalDetId.rawId());
    xs = posHE.x();
    ys = posHE.y();
    
    const auto& hgc = geomHESil.topology().dddConstants();
    const auto& info = hgc.waferInfo(hgcalDetId.layer(), hgcalDetId.waferU(), hgcalDetId.waferV());
    std::cout<<" info  " << info.part <<"   "<<info.type<<std::endl;
 
   
   
    int detector_type = hgcalDetId.det();       // Detector type
     wafer_type = hgcalDetId.type();         // Wafer type
     int z_side = hgcalDetId.zside();            // z-side
     int layer_number = hgcalDetId.layer();      // Layer number
     int abs_v = hgcalDetId.waferV();            // Absolute value of wafer v
     int abs_u = hgcalDetId.waferU();            // Absolute value of wafer u
     int v_coordinate = hgcalDetId.cellV();      // v-coordinate of the cell
     int u_coordinate = hgcalDetId.cellU();      // u-coordinate of the cell
    
    csvFile << hgcalDetId.rawId() << ","
            << detector_type << ","
            << wafer_type << ","
            << z_side << ","
            << layer_number << ","
            << abs_u << ","
            << abs_v << ","
            << u_coordinate << ","
            << v_coordinate << "\n";
    
    std::cout << "DetId: " << hgcalDetId.rawId()
              << "  Detector Type: " << detector_type
              << "  Wafer Type: " << wafer_type
              << "  Z-side: " << z_side
              << "  Layer Number: " << layer_number 
              << "  Wafer U: " << abs_u 
              << "  Wafer V: " << abs_v
              << "  Cell U: " << u_coordinate
              << "  Cell V: " << v_coordinate
              <<"\n";
    
  int layertype = hgc.layerType(layer_number);
  int frontBack = HGCalTypes::layerFrontBack(layertype);
  int index = HGCalWaferIndex::waferIndex(layer_number, abs_u, abs_v);
  int partialType = HGCalWaferType::getPartial(index, hgc.getParameter()->waferInfoMap_);
  int orient = HGCalWaferType::getOrient(index, hgc.getParameter()->waferInfoMap_);
  int placeIndex = HGCalCell::cellPlacementIndex(z_side, frontBack, orient);
  int waferType_ = HGCalWaferType::getType(index, hgc.getParameter()->waferInfoMap_);
  
  std::cout << "Layer Number: " << layer_number
          << " Layer Type: " << layertype

          << " Partial Type: " << partialType

          << " Wafer Type: " << waferType_
          << "Wafer Index  "<< index
          << std::endl;

   if(z_side == 1 && abs_u == 2 && abs_v == 2 ){
         csvFile5<<layer_number<<","<<index<<","<<wafer_type<<","<<partialType<<","<<z_side<<","<<abs_u<<","<<abs_v<<","<<u_coordinate<<","<<v_coordinate<<","<<hgcalDetId.rawId()<<std::endl;
          graphType2->SetPoint(pointIndex2++, xs, ys);
       }

     


    validHitsPerwaferEE[wafer_type]++;
    validHitsPartialmethod1[info.part]++;
     }
    else {
     csvFile3<<det_type<<","<<DetID<<std::endl;
    }
   
   }
   
 

 

}

 inputFile.close();
std::ofstream HEFile("valid_waferType_27_layer_HEsilicon.csv");
HEFile << "Layer,HE Hits\n";
for (size_t i = 0; i < 4; ++i) {
    HEFile << (i) << "," << validHitsPerwaferEE[i] << "\n";
}
HEFile.close();

std::ofstream HEFile1("valid_partialwafer_27_layer_HEsilicon.csv");
HEFile1 << "Layer,HE Hits\n";
for (size_t i = 0; i < 99; ++i) {
    HEFile1 << (i) << "," << validHitsPartialmethod1[i] << "\n";
}
HEFile1.close();



TCanvas c3("c3", "Wafer Type 2 2", 800, 600);
graphType2->SetTitle("Wafer Type 2_2;X (cm);Y (cm)");
graphType2->SetMarkerColor(kBlue);
graphType2->SetMarkerStyle(6);
graphType2->GetXaxis()->SetLimits(-50, 0); // X range from 0 to 5
graphType2->SetMinimum(0);                // Y minimum
graphType2->SetMaximum(50);
graphType2->Draw("AP");
c3.SaveAs("HElayer27_xVsy_2_2.png");
    


delete graphType2;

  }

DEFINE_FWK_MODULE(HGCalRawValidHESilicon);

#endif // HGCALRAWVALIDHESILICON_H

