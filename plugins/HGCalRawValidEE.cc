// -*- C++ -*-
//
// Package:    HGCalDetIDvalidation/HGCalRawValidEE
// Class:      
//
/*

 Description: Example module for raw detID validation for EESilicon

 Implementation:
    
*/
//
// Original Author:  Bsirasva
//    




#ifndef HGCALRAWVALIDEE_H
#define HGCALRAWVALIDEE_H

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

class HGCalRawValidEE : public edm::stream::EDProducer<> {
public:
  explicit HGCalRawValidEE(const edm::ParameterSet&);
  ~HGCalRawValidEE() override {}

  void produce(edm::Event&, const edm::EventSetup&) override;
  
private:
  std::string infoFileName_;

  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenEE_;
   
    TGraph* graphType0;
    TGraph* graphType1;
    TGraph* graphType2;
    TGraph* graphType21;
    TGraph* graphType3;
};

HGCalRawValidEE::HGCalRawValidEE(const edm::ParameterSet& iConfig)
    : infoFileName_(iConfig.getParameter<std::string>("infoFileName")),
      geomTokenEE_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalEESensitive"})){
        
      graphType0 = new TGraph();
      graphType1 = new TGraph();
      graphType2 = new TGraph();
      graphType3 = new TGraph();
      graphType21 = new TGraph();
      }

void HGCalRawValidEE::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
   
  
  edm::ESHandle<HGCalGeometry> geomHandleEE;
  
  // Attempt to retrieve HGCalGeometry using the provided token
  try {
    geomHandleEE = iSetup.getHandle(geomTokenEE_);
  } catch (cms::Exception& e) {
    edm::LogError("HGCalRawDet") << "Failed to retrieve HGCalGeometry: " << e.what();
    throw;
  }

  // Check if the handle is valid before proceeding
  if (!geomHandleEE.isValid() ) {
    edm::LogError("HGCalRawDet") << "Failed to retrieve valid HGCalGeometry!";
    throw cms::Exception("DataError") << "Failed to retrieve valid HGCalGeometry!";
  }
  const HGCalGeometry& geomEE = *geomHandleEE;
   
  // Debug output to confirm HGCalGeometry is loaded
  edm::LogInfo("HGCalRawDet") << "Successfully retrieved HGCalGeometry.";

 std::ofstream csvFile;
 csvFile.open("valid_siliconEE.csv", std::ios::out); // Overwrite mode
 
 std::ofstream csvFile3;
 csvFile3.open("invalid_siliconEE.csv", std::ios::out); // Overwrite mode

std::ofstream csvFile5;
csvFile5.open("Index_16_2_10_2_validDetId.csv", std::ios::out); // Overwrite mode

std::ofstream csvFile6;
csvFile6.open("Index_23_0_2_1_validDetId.csv", std::ios::out); // Overwrite mode

std::ofstream csvFile7;
csvFile7.open("Index_13_2_10_5_validDetId.csv", std::ios::out); // Overwrite mode


int pointIndex0 = 0, pointIndex1 = 0, pointIndex2 = 0,  pointIndex3 = 0, pointIndex21 = 0;

std::array<int, 4> validHitsPerwaferEE = {};    // For EE
std::array<int, 100> validHitsPartialmethod1 = {};

std::ifstream inputFile(infoFileName_);
  if (!inputFile.is_open()) {
    throw cms::Exception("FileOpenError") << "Failed to open file: " << infoFileName_;
  }

  std::string line;
  int lineNumber = 0;
  while (std::getline(inputFile, line) && lineNumber<10000) {
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
    
    
 
   bool validEE = false; 
   double xs =0, ys=0;
   int wafer_type =0;
   if(det_type == 8){
     validEE = geomEE.valid(DetID);
   if(validEE){
   
    HGCSiliconDetId hgcalDetId(DetID);
    GlobalPoint posEE = geomEE.getPosition(hgcalDetId.rawId());
    xs = posEE.x();
    ys = posEE.y();
    const auto& hgc = geomEE.topology().dddConstants();
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
     int wafer = hgcalDetId.waferU();

     csvFile << hgcalDetId.rawId() << ","
            << detector_type << ","
            << wafer_type << ","
            << z_side << ","
            << layer_number << ","
            << abs_u << ","
            << abs_v << ","
            << u_coordinate << ","
            << v_coordinate << ","
	    << wafer <<"\n";
    
    std::cout << "DetId: " << hgcalDetId.rawId()
              << "  Detector Type: " << detector_type
              << "  Wafer Type: " << wafer_type
              << "  Z-side: " << z_side
              << "  Layer Number: " << layer_number 
	      << "  Wafer U: " << abs_u 
	      << "  Wafer V: " << abs_v
              << "  Cell U: " << u_coordinate
              << "  Cell V: " << v_coordinate
              << "  wafer : " << wafer
	      //<< "  subdet: " << subdet
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
	   
      if(z_side == 1 && abs_u == 10 && abs_v == 2 ){
	 csvFile5<<layer_number<<","<<index<<","<<wafer_type<<","<<partialType<<","<<z_side<<","<<abs_u<<","<<abs_v<<","<<u_coordinate<<","<<v_coordinate<<","<<hgcalDetId.rawId()<<std::endl;
	  graphType2->SetPoint(pointIndex2++, xs, ys);
       }

       if(z_side == 1 && abs_u == 2 && abs_v ==1 ){
         csvFile6<<layer_number<<","<<index<<","<<wafer_type<<","<<partialType<<","<<z_side<<","<<abs_u<<","<<abs_v<<","<<u_coordinate<<","<<v_coordinate<<","<<hgcalDetId.rawId()<<std::endl;
          graphType0->SetPoint(pointIndex0++, xs, ys);

	}

       if(z_side == 1 && abs_u == 10 && abs_v ==5 ){
         csvFile7<<layer_number<<","<<index<<","<<wafer_type<<","<<partialType<<","<<z_side<<","<<abs_u<<","<<abs_v<<","<<u_coordinate<<","<<v_coordinate<<","<<hgcalDetId.rawId()<<std::endl;
          graphType21->SetPoint(pointIndex21++, xs, ys);

        }


    validHitsPerwaferEE[wafer_type]++;
    validHitsPartialmethod1[info.part]++;
   }
    else{
    csvFile3 <<det_type<<","
	    <<DetID<<"\n";
   
     }
   }
 

 

}

 inputFile.close();
   std::cout<<pointIndex0<<"   "<<pointIndex1<<"  "<<pointIndex2<<std::endl;
 
 std::ofstream eeFile("valid_waferType_1_layer_EE.csv");
eeFile << "Layer,EE Hits\n";
for (size_t i = 0; i < 4; ++i) {
    eeFile << (i) << "," << validHitsPerwaferEE[i] << "\n";
}
eeFile.close();
 
std::ofstream eeFile1("valid_partialwafer1_1_layer_EE.csv");
eeFile1 << "Layer,EE Hits\n";
for (size_t i = 0; i < 99; ++i) {
    eeFile1 << (i) << "," << validHitsPartialmethod1[i] << "\n";
}
eeFile1.close();

    
TCanvas c1("c1", "Wafer Type Graph 23 0 2 1", 800, 600);
graphType0->SetTitle("Wafer Type0 23_0_2_1;X (cm);Y (cm)");
graphType0->SetMarkerColor(kRed);  
graphType0->SetMarkerStyle(6);    
graphType0->GetXaxis()->SetLimits(-40, -15); 
graphType0->SetMinimum(0);                // Y minimum
graphType0->SetMaximum(30);
graphType0->Draw("AP"); 
c1.SaveAs("EElayer1_xVsy_type0_23_0_2_1.png");
/*
TCanvas c2("c2", "Wafer Type 1", 800, 600);
graphType1->SetTitle("Wafer Type 1;X (cm);Y (cm)");
graphType1->SetMarkerColor(kYellow);
graphType1->SetMarkerStyle(6);
graphType1->GetXaxis()->SetLimits(-200, 200); 
graphType1->SetMinimum(-200);                // Y minimum
    graphType1->SetMaximum(200);
graphType1->Draw("AP");
c2.SaveAs("EElayer1_xVsy_type1.png");
*/
TCanvas c3("c3", "Wafer Type2 16 2 10 2", 800, 600);
graphType2->SetTitle("Wafer Type2 16_2_10_2;X (cm);Y (cm)");
graphType2->SetMarkerColor(kBlue);
graphType2->SetMarkerStyle(6);
graphType2->GetXaxis()->SetLimits(-170, -130); // X range from 0 to 5
graphType2->SetMinimum(10);                // Y minimum
graphType2->SetMaximum(50);
graphType2->Draw("AP");
c3.SaveAs("EElayer1_xVsy_type2_16_2_10_2.png");
/*   
TCanvas c4("c4", "Wafer Type 3", 800, 600);
graphType3->SetTitle("Wafer Type 3;X (cm);Y (cm)");
graphType3->SetMarkerColor(40);
graphType3->SetMarkerStyle(6);
graphType3->GetXaxis()->SetLimits(-200, 200); // X range from 0 to 5
graphType3->SetMinimum(-200);                // Y minimum
graphType3->SetMaximum(200);
graphType3->Draw("AP");
c4.SaveAs("EElayer1_xVsy_type3.png");
*/

TCanvas c5("c5", "Wafer Type2 13 2 10 5", 800, 600);
graphType21->SetTitle("Wafer Type2 13_2_10_5;X (cm);Y (cm)");
graphType21->SetMarkerColor(kBlue);
graphType21->SetMarkerStyle(6);
graphType21->GetXaxis()->SetLimits(-150, -110); // X range from 0 to 5
graphType21->SetMinimum(50);                // Y minimum
graphType21->SetMaximum(90);
graphType21->Draw("AP");
c5.SaveAs("EElayer1_xVsy_type2_13_2_10_5.png");


delete graphType0;
//delete graphType1;
delete graphType2;
delete graphType3;
delete graphType21;

  }

DEFINE_FWK_MODULE(HGCalRawValidEE);

#endif // HGCALRAWVALIDEE_H


