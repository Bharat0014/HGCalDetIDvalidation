import FWCore.ParameterSet.Config as cms
from Configuration.Eras.Era_Phase2C17I13M9_cff import Phase2C17I13M9

process = cms.Process('PROD', Phase2C17I13M9)


# Load necessary configurations
process.load('Configuration.Geometry.GeometryExtended2026D110Reco_cff')
process.load('Geometry.HGCalGeometry.HGCalGeometryESProducer_cfi')
# Define the source
process.source = cms.Source("EmptySource")

# Set the maximum number of events to process
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)  # Process only 1 event
)

# Define the DetIdNewProducer module
process.HGCalRawDettest = cms.EDProducer('HGCalRawDettest',
    infoFileName = cms.string("/eos/user/b/bsirasva/DetIdGeneration/detid_EElayer26.csv"),
)

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('step1.root')
   # outputCommands = cms.untracked.vstring('keep *')
)

# Define the processing path
process.p = cms.Path(process.HGCalRawDettest)

# Define the end path for the output module
process.e = cms.EndPath(process.out)

# Schedule definition
process.schedule = cms.Schedule(process.p, process.e)

