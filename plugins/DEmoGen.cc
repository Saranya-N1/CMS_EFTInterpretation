// -*- C++ -*-
//
// Package:    Analysis/DemoGen
// Class:      DemoGen
//
/**\class DemoGen DemoGen.cc Analysis/DemoGen/plugins/DemoGen.cc
 Description: Analyze the Higgs boson and its decay chain
*/
//
// Original Author:  Saranya Nandakumar
//         Created:  10/02/2025
//
//

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "TH1F.h"
#include <iostream>

class DemoGen : public edm::one::EDAnalyzer<edm::one::SharedResources> {
   public:
      explicit DemoGen(const edm::ParameterSet&);
      ~DemoGen();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      edm::EDGetTokenT<std::vector<reco::GenParticle>> genParticlesToken_;

      // Histograms
      TH1F* h_higgs_pt;
      TH1F* h_higgs_eta;
      TH1F* h_higgs_mass;
      TH1F* h_daughter_pdg;
      TH1F* h_daughter_pt;
      TH1F* h_daughter_eta;

      void printDecayChain(const reco::GenParticle& particle, int depth = 0);
};

DemoGen::DemoGen(const edm::ParameterSet& iConfig)
 :
  genParticlesToken_(consumes<std::vector<reco::GenParticle>>(iConfig.getParameter<edm::InputTag>("genParticles")))
{
   usesResource("TFileService");
}

DemoGen::~DemoGen() {}

void DemoGen::beginJob()
{
   edm::Service<TFileService> fs;
   h_higgs_pt = fs->make<TH1F>("h_higgs_pt", "Higgs Transverse Momentum;p_{T} (GeV/c);Entries", 100, 0, 400);
   h_higgs_eta = fs->make<TH1F>("h_higgs_eta", "Higgs Pseudorapidity;#eta;Entries", 60, -3, 3);
   h_higgs_mass = fs->make<TH1F>("h_higgs_mass", "Higgs Mass;Mass (GeV/c^2);Entries", 50, 110, 140);
   h_daughter_pdg = fs->make<TH1F>("h_daughter_pdg", "Daughter PDG ID;PDG ID;Entries", 50, -25, 25);
   h_daughter_pt = fs->make<TH1F>("h_daughter_pt", "Daughter p_{T};p_{T} (GeV/c);Entries", 100, 0, 400);
   h_daughter_eta = fs->make<TH1F>("h_daughter_eta", "Daughter Eta;#eta;Entries", 60, -3, 3);
}

void DemoGen::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    edm::Handle<std::vector<reco::GenParticle>> genParticles;
    iEvent.getByToken(genParticlesToken_, genParticles);

    for (const auto& particle : *genParticles) {
        if (particle.pdgId() == 25) { // Higgs boson
            h_higgs_pt->Fill(particle.pt());
            h_higgs_eta->Fill(particle.eta());
            h_higgs_mass->Fill(particle.mass());

            std::cout << "Higgs found! pT: " << particle.pt() 
                      << ", Eta: " << particle.eta() 
                      << ", Mass: " << particle.mass() << std::endl;
            
            printDecayChain(particle);
        }
    }
}

// Recursive function to print and analyze the decay chain
void DemoGen::printDecayChain(const reco::GenParticle& particle, int depth)
{
    std::string indent(depth * 2, ' '); // Indentation for readability
    std::cout << indent << "PDG ID: " << particle.pdgId()
              << " | Status: " << particle.status()
              << " | pT: " << particle.pt()
              << " | Eta: " << particle.eta() << std::endl;

    for (unsigned int i = 0; i < particle.numberOfDaughters(); ++i) {
        const reco::GenParticle* daughter = dynamic_cast<const reco::GenParticle*>(particle.daughter(i));
        if (daughter) {
            h_daughter_pdg->Fill(daughter->pdgId());
            h_daughter_pt->Fill(daughter->pt());
            h_daughter_eta->Fill(daughter->eta());
            printDecayChain(*daughter, depth + 1); // Recursive call
        }
    }
}

void DemoGen::endJob() {}

void DemoGen::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("genParticles", edm::InputTag("genParticles"));
    descriptions.add("demoGen", desc);
}

// Define as a CMSSW plugin
DEFINE_FWK_MODULE(DemoGen);
