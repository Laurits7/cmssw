// -*- C++ -*-
//
// Package:    L1Trigger/VertexFinder
// Class:      InputDataProducer
//
/**\class InputDataProducer InputDataProducer.cc L1Trigger/VertexFinder/plugins/InputDataProducer.cc

 Description: Produces an InputData object which stores generator level information for further analysis

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Alexx Perloff
//         Created:  Fri, 05 Feb 2021 23:42:17 GMT
//
//

// system include files
#include <memory>
#include <vector>

// user include files
#include "DataFormats/L1TrackTrigger/interface/TTTypes.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "L1Trigger/VertexFinder/interface/AnalysisSettings.h"
#include "L1Trigger/VertexFinder/interface/InputData.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimTracker/TrackTriggerAssociation/interface/TTClusterAssociationMap.h"
#include "SimTracker/TrackTriggerAssociation/interface/TTStubAssociationMap.h"


//
// class declaration
//

class InputDataProducer : public edm::stream::EDProducer<> {
public:
  explicit InputDataProducer(const edm::ParameterSet&);
  ~InputDataProducer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::Event&, const edm::EventSetup&) override;

  // ----------constants, enums and typedefs ---------
  typedef edmNew::DetSetVector<TTStub<Ref_Phase2TrackerDigi_>> DetSetVec;
  typedef TTStubAssociationMap<Ref_Phase2TrackerDigi_> TTStubAssMap;
  typedef TTClusterAssociationMap<Ref_Phase2TrackerDigi_> TTClusterAssMap;

  // ----------member data ---------------------------
  const std::string outputCollectionName_;
  l1tVertexFinder::AnalysisSettings settings_;
  const edm::EDGetTokenT<edm::HepMCProduct> hepMCToken_;
  const edm::EDGetTokenT<edm::View<reco::GenParticle>> genParticlesToken_;
  const edm::EDGetTokenT<TrackingParticleCollection> tpToken_;
  const edm::EDGetTokenT<DetSetVec> stubToken_;
  const edm::EDGetTokenT<TTStubAssMap> stubTruthToken_;
  const edm::EDGetTokenT<TTClusterAssMap> clusterTruthToken_;
};

//
// constructors and destructor
//
InputDataProducer::InputDataProducer(const edm::ParameterSet& iConfig)
  : outputCollectionName_(iConfig.getParameter<std::string>("outputCollectionName")),
    settings_(iConfig),
    hepMCToken_(consumes<edm::HepMCProduct>(iConfig.getParameter<edm::InputTag>("hepMCInputTag"))),
    genParticlesToken_(
        consumes<edm::View<reco::GenParticle>>(iConfig.getParameter<edm::InputTag>("genParticleInputTag"))),
    tpToken_(consumes<TrackingParticleCollection>(iConfig.getParameter<edm::InputTag>("tpInputTag"))),
    stubToken_(consumes<DetSetVec>(iConfig.getParameter<edm::InputTag>("stubInputTag"))),
    stubTruthToken_(consumes<TTStubAssMap>(iConfig.getParameter<edm::InputTag>("stubTruthInputTag"))),
    clusterTruthToken_(consumes<TTClusterAssMap>(iConfig.getParameter<edm::InputTag>("clusterTruthInputTag"))) {

  // Define EDM output to be written to file (if required)
  produces<l1tVertexFinder::InputData>(outputCollectionName_);

  //now do what ever other initialization is needed
}

InputDataProducer::~InputDataProducer()
{
}

//
// member functions
//

// ------------ method called to produce the data  ------------
void InputDataProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  std::unique_ptr<l1tVertexFinder::InputData> product(new l1tVertexFinder::InputData(iEvent,
                                                                                     iSetup,
                                                                                     settings_,
                                                                                     hepMCToken_,
                                                                                     genParticlesToken_,
                                                                                     tpToken_,
                                                                                     stubToken_,
                                                                                     stubTruthToken_,
                                                                                     clusterTruthToken_));

  iEvent.put(std::move(product), outputCollectionName_);
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void InputDataProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(InputDataProducer);
