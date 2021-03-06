#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <MyAnalysis/MyAlgorithm.h>

#include <xAODMissingET/MissingETContainer.h>

#include <TH1.h>


// this is needed to distribute the algorithm to the workers
ClassImp(MyAlgorithm)



MyAlgorithm :: MyAlgorithm ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
}



EL::StatusCode MyAlgorithm :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

  job.useXAOD();

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: initialize ()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.

  book (TH1D ("nmuons", "number of muons", 20, 0, 20));
  book (TH1D ("muonpt", "muon pt after a cut", 20, 0, 200));
  book (TH1D ("leadpt", "lead muon pt after a cut", 20, 0, 200));
  book (TH1D ("subleadpt", "sublead muon pt after a cut", 20, 0, 200));

  book (TH1D ("Cutflow", "Cutflow", 10, -0.5, 9.5));

  std::unique_ptr<ana::QuickAna> myQuickAna (new ana::QuickAna ("quickana"));
  myQuickAna->setConfig (*this);
  quickAna = std::move (myQuickAna);
  if (quickAna->initialize().isFailure()){
    return EL::StatusCode::FAILURE;
  }
  
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.


  // run QuickAna for the current event:
  if(quickAna->process().isFailure())
    return EL::StatusCode::FAILURE;

  double wgt = quickAna->weight();

  double cutnr = 0;

  hist("Cutflow")->Fill(cutnr++,wgt);
  
  auto nmuons = quickAna->muons()->size();

  hist("nmuons")->Fill (nmuons, wgt);

  if(nmuons<2){
    return EL::StatusCode::SUCCESS;
  }

  hist("Cutflow")->Fill(cutnr++,wgt);

  const double GeV = 1000.0;
  auto muons = quickAna->muons();
  for (const auto& m : *muons){
    hist("muonpt")->Fill(m->pt()/GeV,wgt);
  }
  hist("leadpt")->Fill(muons->at(0)->pt()/GeV,wgt);
  hist("subleadpt")->Fill(muons->at(1)->pt()/GeV,wgt);

  if(! ((muons->at(0)->pt()/GeV) > 40)) {
        return EL::StatusCode::SUCCESS;
  }
  hist("Cutflow")->Fill(cutnr++,wgt);
  
  if(! ((muons->at(1)->pt()/GeV) > 30)) {
        return EL::StatusCode::SUCCESS;
  }
  hist("Cutflow")->Fill(cutnr++,wgt);
  

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.
  return EL::StatusCode::SUCCESS;
}
