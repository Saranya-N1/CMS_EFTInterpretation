import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('histo_higgs.root')
                                  )

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'root://eoshome-s.cern.ch//eos/user/s/snandaku/Analysis/CMSSW_10_6_17/src/Demo/DemoGen/test/output10000.root'  # Update this with your actual ROOT file path
    )
)

process.demo = cms.EDAnalyzer('DemoGen',
    genParticles = cms.InputTag("genParticles")
)

process.p = cms.Path(process.demo)
