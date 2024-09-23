#ifndef HGCALRAWDETTEST_H
#define HGCALRAWDETTEST_H

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

class HGCalRawDettest : public edm::stream::EDProducer<> {
public:
  explicit HGCalRawDettest(const edm::ParameterSet&);
  ~HGCalRawDettest() override {}

  void produce(edm::Event&, const edm::EventSetup&) override;
  
private:
  std::string infoFileName_;

  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenEE_;
  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenHESil_;
  edm::ESGetToken<HGCalGeometry, IdealGeometryRecord> geomTokenHESci_;

  
    TGraph* graphType0;
    TGraph* graphType1;
    TGraph* graphType2;
};

HGCalRawDettest::HGCalRawDettest(const edm::ParameterSet& iConfig)
    : infoFileName_(iConfig.getParameter<std::string>("infoFileName")),
      geomTokenEE_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalEESensitive"})),
      geomTokenHESil_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalHESiliconSensitive"})),
      geomTokenHESci_(esConsumes<HGCalGeometry, IdealGeometryRecord>(edm::ESInputTag{"", "HGCalHEScintillatorSensitive"})) {	      
        
      graphType0 = new TGraph();
      graphType1 = new TGraph();
      graphType2 = new TGraph();
        
      }

void HGCalRawDettest::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
   
  
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
 csvFile.open("valid_siliconEE.csv", std::ios::out); // Overwrite mode
 
 std::ofstream csvFile3;
 csvFile3.open("invalid_siliconEE.csv", std::ios::out); // Overwrite mode

int pointIndex0 = 0, pointIndex1 = 0, pointIndex2 = 0;



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
    
    
 
   bool validEE = false; 
   double xs =0, ys=0;
    
   if(det_type == 8){
    validEE = geomEE.topology().valid(DetID);
    
    if(validEE){
   
    csvFile <<det_type<<"," <<DetID<<"\n";
 
    HGCSiliconDetId hgcalDetId(DetID);
    GlobalPoint posEE = geomEE.getPosition(hgcalDetId.rawId());
    xs = posEE.x();
    ys = posEE.y();
    
    
   
   
    int detector_type = hgcalDetId.det();       // Detector type
     int wafer_type = hgcalDetId.type();         // Wafer type
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
    
	   
       if(z_side == 1){
	 if (wafer_type == 0) {
            graphType0->SetPoint(pointIndex0++, xs, ys);
        } else if (wafer_type == 1) {
            graphType1->SetPoint(pointIndex1++, xs, ys);
        } else if (wafer_type == 2) {
            graphType2->SetPoint(pointIndex2++, xs, ys);
        }
       }
    

  
    }
    else{
    csvFile3 <<det_type<<","
	    <<DetID<<"\n";
   
     }
   }
    
 

}

  inputFile.close();
 

    
TCanvas c1("c1", "Wafer Type Graph 0", 800, 600);
graphType0->SetMarkerColor(kRed);  
graphType0->SetMarkerStyle(6);    
graphType0->GetXaxis()->SetLimits(-200, 200); 
graphType0->SetMinimum(-200);                // Y minimum
graphType0->SetMaximum(200);
graphType0->Draw("AP"); 
c1.SaveAs("EE_xVsy_type0.png");

TCanvas c2("c2", "Wafer Type 1", 800, 600);
graphType1->SetMarkerColor(kYellow);
graphType1->SetMarkerStyle(6);
graphType1->GetXaxis()->SetLimits(-200, 200); 
graphType1->SetMinimum(-200);                // Y minimum
    graphType1->SetMaximum(200);
graphType1->Draw("AP");
c2.SaveAs("EE_xVsy_type1.png");

TCanvas c3("c3", "Wafer Type 2", 800, 600);
graphType2->SetMarkerColor(kBlue);
graphType2->SetMarkerStyle(6);
graphType2->GetXaxis()->SetLimits(-200, 200); // X range from 0 to 5
graphType2->SetMinimum(-200);                // Y minimum
graphType2->SetMaximum(200);
graphType2->Draw("AP");
c3.SaveAs("EE_xVsy_type2.png");
    

delete graphType0;
delete graphType1;
delete graphType2;


  }

DEFINE_FWK_MODULE(HGCalRawDettest);

#endif // HGCALRAWDETTEST_H

