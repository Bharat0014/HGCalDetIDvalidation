// -*- C++ -*-
//
// Package:    HGCalDetIDvalidation/HGCalRawValidationHEScintillator
// Class:      
//
/*

 Description: Example module for raw detID validation for HEScintillator

 Implementation:
    
*/
//
// Original Author:  Bsirasva
//    

#ifndef HGCALRAWVALIDHESCINTILLATOR_H
#define HGCALRAWVALIDHESCINTILLATOR_H

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

class HGCalRawValidHEScintillator : public edm::stream::EDProducer<> {
public:
  explicit HGCalRawValidHEScintillator(const edm::ParameterSet&);
  ~HGCalRawValidHEScintillator() override {}

  void produce(edm::Event&, const edm::EventSetup&) override;
  
private:
  std::string infoFileName_;

   edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenHESci_;

  
    TGraph* graphType0;

};

HGCalRawValidHEScintillator::HGCalRawValidHEScintillator(const edm::ParameterSet& iConfig)
    : infoFileName_(iConfig.getParameter<std::string>("infoFileName")),
     geomTokenHESci_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalHEScintillatorSensitive"})) {	      
        
      graphType0 = new TGraph();
       
      }

void HGCalRawValidHEScintillator::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
   
  
   edm::ESHandle<HGCalGeometry> geomHandleHESci;
  
  // Attempt to retrieve HGCalGeometry using the provided token
  try {
       geomHandleHESci = iSetup.getHandle(geomTokenHESci_);
  } catch (cms::Exception& e) {
    edm::LogError("HGCalRawDet") << "Failed to retrieve HGCalGeometry: " << e.what();
    throw;
  }

  // Check if the handle is valid before proceeding
  if ( !geomHandleHESci.isValid()) {
    edm::LogError("HGCalRawDet") << "Failed to retrieve valid HGCalGeometry!";
    throw cms::Exception("DataError") << "Failed to retrieve valid HGCalGeometry!";
  }
  const HGCalGeometry& geomHESci = *geomHandleHESci;

  // Debug output to confirm HGCalGeometry is loaded
  edm::LogInfo("HGCalRawDet") << "Successfully retrieved HGCalGeometry.";

 std::ofstream csvFile;
 csvFile.open("valid_scintillatorHELayer8.csv", std::ios::out); // Overwrite mode
 
 std::ofstream csvFile2;
 csvFile2.open("invalid_ScintillatorHELayer8.csv", std::ios::out); // Overwrite mode

 
 int  pointIndex1 = 0;

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
    
    
 
   bool validHEsc = false; 
   double xs =0, ys=0;
   if(det_type == 10){
    validHEsc = geomHESci.valid(DetID);

   if(validHEsc){
    HGCScintillatorDetId hgcalDetId(DetID);
    GlobalPoint posHEsc = geomHESci.getPosition(hgcalDetId.rawId());
    xs = posHEsc.x();
    ys = posHEsc.y();
    
    int detector_type = hgcalDetId.det();
    int tile_type = hgcalDetId.type();
    int z_side = hgcalDetId.zside();
    int sipm_type = hgcalDetId.sipm();
    int trigger_cell = hgcalDetId.trigger();
    int layer_number = hgcalDetId.layer();
    int ring_index = hgcalDetId.ring();
    int iphi_index = hgcalDetId.iphi();
    int granularity = (hgcalDetId.rawId() >> 24) & 0x1;

    std::cout << "Detector Type: " << detector_type
              << ", Tile Type: " << tile_type
              << ", Z-Side: " << z_side
              << ", SiPM Type: " << sipm_type
              << ", Granularity: " << granularity
              << ", Trigger Cell: " << trigger_cell
              << ", Layer Number: " << layer_number
              << ", Ring Index: " << ring_index
              << ", IPhi Index: " << iphi_index
              << std::endl;


    if (z_side == 1){
    graphType0->SetPoint(pointIndex1++, xs, ys);
    }
   

    
   csvFile<<detector_type<<","<<layer_number<<","<<z_side<<","<<DetID<<std::endl;

    

    }
    else{
    csvFile2 <<det_type<<","
            <<DetID<<"\n";

     }
   }
 

 

}

inputFile.close();
std::cout<<pointIndex1<<std::endl;
 
std::ofstream HEFile("valid_DetID_HEscintillator_layer8.csv");
HEFile << "HE Hits\n";
HEFile <<pointIndex1 << "\n";

HEFile.close();


TCanvas c3("c3", "Scintillator", 800, 600);
graphType0->SetTitle("HESc_Layer_8;X (cm);Y (cm)");
graphType0->SetMarkerColor(kBlue);
graphType0->SetMarkerStyle(6);
graphType0->GetXaxis()->SetLimits(-280, 280); // X range from 0 to 5
graphType0->SetMinimum(-280);                // Y minimum
graphType0->SetMaximum(280);
graphType0->Draw("AP");
c3.SaveAs("HESc_Layer8.png");



delete graphType0;


  }

DEFINE_FWK_MODULE(HGCalRawValidHEScintillator);

#endif // HGCALRAWVALIDHESCINTILLATOR_H

