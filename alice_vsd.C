#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <TGFileDialog.h>
#include <TRootHelpDialog.h>

#include "TImage.h"
#include "TRecorder.h"

#include <TGClient.h>
#include <TGFrame.h>
#include <TGLayout.h>
#include <TGSplitter.h>
#include <TGComboBox.h>
#include <TGTextView.h>

#include <TGPicture.h>

#include <TG3DLine.h>
#include <TAxis3D.h>
#include <TStyle.h>
#include <TPaveText.h>

#include <TApplication.h>
#include <TEveManager.h>
#include <TEveEventManager.h>
#include <TEveWindowManager.h>
#include <TEveVSD.h>
#include <TEveVSDStructs.h>

#include <TCanvas.h>
#include <TEveTrack.h>
#include <TEveTrackPropagator.h>
#include <TEveGeoShape.h>

#include <TEveSelection.h>

#include <TCanvas.h>
#include <TParticlePDG.h>

#include <TH1F.h>
#include <TH1.h>
#include <TH2F.h>
#include <TGraph.h>
#include <TLine.h>

#include <TGTextEntry.h>

#include <TGLabel.h>

#include <TGTab.h>
#include <TGButton.h>
#include <TTimer.h>

#include <TFile.h>
#include <TKey.h>
#include <TSystem.h>
#include <TPRegexp.h>

#include <TGNumberEntry.h>

#include "MultiView.C"

using namespace std;


class TVSDReader;
MultiView *gMultiView = 0;




Bool_t gEnableQuickAnalysis = kFALSE;  

TH1F *gMinvHist = 0;
TH1F *geeHist = 0;
TH1F *gppHist = 0;
TH1F *geHist = 0;
TH1F *gpHist = 0;
TH1F *gJpsiHist = 0;
TH2F *gEnergyLoss = 0;

Float_t gArrP[300] = {0};
Float_t gArrdEdx[300] = {0};
Float_t el[100][4];
Float_t po[100][4];
Int_t nPositrons = 0;
Int_t nElectrons = 0;
Int_t nSelectedTracks  = 0;
Int_t nEvents = 0;




TCanvas *pad2 = 0;
TCanvas *pad = 0;


TGCheckButton *gCheckboxLoadTracks = 0;
TGCheckButton *gCheckboxApplyTrackCuts = 0;
TGCheckButton *gCheckboxQuickAnalysis= 0;

TGTextButton *gButtonLoadTracks = 0;
TGTextButton *gButtonFillPidHistos = 0;
TGTextButton *gButtonDefineTrackCuts = 0;
TGTextButton *gButtonApplayTrackCuts = 0;
TGTextButton *gButtonFillInvMass = 0;
TGTextButton *gButtonJumpEvents = 0;

TGLabel *gLabelEventNumber = 0;

Int_t globalDataset = 0;


Float_t p1,p2,de1,de2;

TLine *l1 = 0;
TLine *l2 = 0;
TLine *l3 = 0;
TLine *l4 = 0;
TLine *l5 = 0;
TLine *l6 = 0;


//______________________________________________________________________________
//______________________________________________________________________________
class AliceDetectorInfo : public TGMainFrame
{

protected:

  TGPictureButton *b1;

public:

  AliceDetectorInfo(const TGWindow *p, UInt_t w, UInt_t h) :
    TGMainFrame(p, w, h) {

    //DontCallClose();

    b1 = 0;
    TGTextButton *b2 = 0;
    TGGroupFrame *gf = new TGGroupFrame(this, "ALICE Detector");

    TGHorizontalFrame *hf = new TGHorizontalFrame(gf, 250, 250);
    b1 = new TGPictureButton(hf, gClient->GetPicture("eve_files/dEdx.png"));

    hf->AddFrame(b1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
    gf->AddFrame(hf);

    hf = new TGHorizontalFrame(gf);
    {
      b2 = new TGTextButton(hf, "Close");
      b2->Connect("Clicked()", "AliceDetectorInfo", this, "UnmapWindow()");
      hf->AddFrame(b2, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
    }

    gf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));

    AddFrame(gf);

    SetWindowName("Particle Identification based on specific energy loss");
    MapSubwindows();

    // Initialize the layout algorithm via Resize()
    Resize(GetDefaultSize());

    // Map main frame
    MapWindow();
  }

  ClassDef(AliceDetectorInfo, 0);
};


AliceDetectorInfo *gAliceDetectorInfo = 0;


// Track Selection Window
//______________________________________________________________________________
//______________________________________________________________________________

class AliceSelector : public TGMainFrame
{
protected:
  TVSDReader* parent;
  TGTextButton *b;
 
  
  
  TGNumberEntryField *fp1;
  TGNumberEntryField *fp2;

  TGNumberEntryField *fde1;
  TGNumberEntryField *fde2;

  TGLabel *val1;

public:

  AliceSelector(const TGWindow *p, TVSDReader* parent) :
    TGMainFrame(p) {
    DontCallClose();

    TGGroupFrame *gf = new TGGroupFrame(this, "");


    const TGFont *font = gClient->GetFont("-*-times-bold-r-*-*-16-*-*-*-*-*-*-*");
    FontStruct_t buttonFont = font->GetFontStruct();
    ULong_t buttonRedColor;
    gClient->GetColorByName("red", buttonRedColor);
    TGTextButton *b = new TGTextButton(gf,"Instructions");
    b->SetTextColor(buttonRedColor);
    b->SetFont(buttonFont);
    b->Connect("Clicked()", "AliceSelector", this, "Instructions()");


    gf->AddFrame(b, new TGLayoutHints(kLHintsExpandX,  2, 2, 5, 2));


    TGHorizontalFrame *hf = new TGHorizontalFrame(gf, 200, 20, kFixedWidth);

    val1 = new TGLabel(hf, "    ");
    val1->SetTextJustify(kTextCenterX);
    hf->AddFrame(val1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));

    val1 = new TGLabel(hf, "min");
    val1->SetTextJustify(kTextCenterX);
    hf->AddFrame(val1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));

    val1 = new TGLabel(hf, "max");
    val1->SetTextJustify(kTextCenterX);
    hf->AddFrame(val1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));

    gf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));

    hf = new TGHorizontalFrame(gf, 200, 20, kFixedWidth);

    val1 = new TGLabel(hf, "p");
    fp1 = new TGNumberEntryField(hf);
    fp1->SetNumber(p1);
    fp2 = new TGNumberEntryField(hf);
    fp2->SetNumber(p2);
    
    hf->AddFrame(val1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
    hf->AddFrame(fp1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
    hf->AddFrame(fp2, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));

    gf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 2));

    hf = new TGHorizontalFrame(gf, 200, 20, kFixedWidth);

    val1 = new TGLabel(hf, "dE/dx");
    fde1 = new TGNumberEntryField(hf);
    fde1->SetNumber(de1);
    fde2 = new TGNumberEntryField(hf);
    fde2->SetNumber(de2);

    hf->AddFrame(val1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
    hf->AddFrame(fde1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
    hf->AddFrame(fde2, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));

    gf->AddFrame(hf,new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 2));

    b = new TGTextButton(gf, "Engage");
    b->Connect("Pressed()", "AliceSelector", this, "Datapointer()");
    b->Connect("Pressed()", "AliceSelector", this, "SetLineValue1()");
    b->Connect("Pressed()", "TVSDReader", parent, "ApplyTrackCuts()");
    b->Connect("Pressed()", "AliceSelector", this, "UnmapWindow()");
    gf->AddFrame(b, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 2));


    b = new TGTextButton(gf, "Close");
    b->Connect("Pressed()", "AliceSelector", this, "UnmapWindow()");
    gf->AddFrame(b, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 2));
    AddFrame(gf, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));

    SetWindowName("Track Cuts");

    // Set a name to the main frame

    MapSubwindows();

    // Initialize the layout algorithm via Resize()
    Resize(GetDefaultSize());
    // Map main frame
    MapWindow();
  }

  void Instructions() {

    TRootHelpDialog *instructions = new TRootHelpDialog(gClient->GetRoot(), "ANALYSIS WINDOW INSTRUCTIONS", 700, 400);

    instructions->SetText("\
   Instructions: Track Cuts \n\n\
      The \"Track Cuts\" is the tool that allows you to select only tracks which are \n\
      candidates for being electrons and positrons, originating from a J/psi decay,\n\n\
      based on following quantities:\n\
      p     \t- momentum\n\
      dE/dx \t- specific energy loss of the particle inside the detector\n\
      Enter your desired minimum/maximum values and click on \"Engage\".\n\
      In the \"Specific Energy Loss\" histogram in the \"Particle Identification\" window\n\
      a rectangle which defines the selected area will be drawn. \n\
      If you want to see only the selected tracks in the 3D model, click\n\
      \"Selected Tracks\"in the menu \"Load\" on the left side.");

    instructions->Popup();

  }


  void SetLineValue1() {

    l1->SetX1(p1);
    l1->SetY1(de1);
    l1->SetX2(p1);
    l1->SetY2(de2);
    l1->SetLineColor(2);
    l1->SetLineStyle(5);

    l2->SetX1(p2);
    l2->SetY1(de1);
    l2->SetX2(p2);
    l2->SetY2(de2);
    l2->SetLineColor(2);
    l2->SetLineStyle(5);

    l3->SetX1(p1);
    l3->SetY1(de1);
    l3->SetX2(p2);
    l3->SetY2(de1);
    l3->SetLineColor(2);
    l3->SetLineStyle(5);

    l4->SetX1(p1);
    l4->SetY1(de2);
    l4->SetX2(p2);
    l4->SetY2(de2);
    l4->SetLineColor(2);
    l4->SetLineStyle(5);

  }

  void PlotLines1() {

    pad2->cd();
    SetLineValue1();
    l1->Draw();
    l2->Draw();
    l3->Draw();
    l4->Draw();
    pad2->cd()->Update();
  }



  void Datapointer() {
    p1 = fp1->GetNumber();
    p2 = fp2->GetNumber();
    de1 = fde1->GetNumber();
    de2 = fde2->GetNumber();
  }



  ClassDef(AliceSelector,0);
};


AliceSelector *gAliceSelector = 0;

// Signal Extraction Window
//______________________________________________________________________________
//______________________________________________________________________________

class AliceExtractor : public TGMainFrame
{
protected:
  TGTextButton *b;

  TGNumberEntryField *fm1;
  TGNumberEntryField *fm2;

  TGNumberEntryField *JpsiCount;
  TGNumberEntryField *Sigma;
  TGNumberEntryField *SonB;


  Int_t numJpsi;
  Int_t bg;
  Float_t significance;
  Float_t sb;
  Float_t maxm,minm;
  Float_t epsilon;
  TGLabel *val1;

public:

  AliceExtractor(const TGWindow *p) :
    TGMainFrame(p),numJpsi(0), bg(0.), significance(0.), sb(0.), maxm(0.), minm(0.),epsilon (0.001) {
    //DontCallClose();


    TGGroupFrame  *gf = new TGGroupFrame(this,"");

    const TGFont *font = gClient->GetFont("-*-times-bold-r-*-*-16-*-*-*-*-*-*-*");
    FontStruct_t buttonFont = font->GetFontStruct();
    ULong_t buttonRedColor;
    gClient->GetColorByName("red", buttonRedColor);

    TGTextButton *b = new TGTextButton(gf,"Instructions");
    b->SetTextColor(buttonRedColor);
    b->SetFont(buttonFont);
    b->Connect("Clicked()", "AliceExtractor", this, "Instructions()");


    gf->AddFrame(b, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));



    TGHorizontalFrame *hf = new TGHorizontalFrame(gf, 120, 20, kFixedWidth);

    val1 = new TGLabel(hf, "    ");
    val1->SetTextJustify(kTextCenterX);
    hf->AddFrame(val1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));

    val1 = new TGLabel(hf, "min"); 
    val1->SetTextJustify(kTextCenterX);
    hf->AddFrame(val1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));

    val1 = new TGLabel(hf, "max");
    val1->SetTextJustify(kTextCenterX);
    hf->AddFrame(val1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));

    gf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));

    hf = new TGHorizontalFrame(gf, 240, 20, kFixedWidth);

    val1 = new TGLabel(hf, "mass range");
    fm1 = new TGNumberEntryField(hf);
    fm2 = new TGNumberEntryField(hf);

    hf->AddFrame(val1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
    hf->AddFrame(fm1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
    hf->AddFrame(fm2, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));

    gf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));

    b = new TGTextButton(gf, "Extract Signal");
    b->Connect("Pressed()", "AliceExtractor", this, "JpsiIntegration()");
    b->Connect("Pressed()", "AliceExtractor", this, "PlotLines2()");
    b->Connect("Pressed()", "AliceExtractor", this, "Autosave()");
    gf->AddFrame(b, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));

    hf = new TGHorizontalFrame(gf, 240, 20, kFixedWidth);
    val1 = new TGLabel(hf, "Number of Jpsis");
    val1->SetTextJustify(kTextCenterX);
    hf->AddFrame(val1, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
    JpsiCount = new TGNumberEntryField(hf);
    JpsiCount->SetEnabled(kFALSE);
    hf->AddFrame(JpsiCount, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
    gf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));

    hf = new TGHorizontalFrame(gf, 240, 20, kFixedWidth);
    val1 = new TGLabel(hf, "Signal/Background");
    val1->SetTextJustify(kTextCenterX);
    hf->AddFrame(val1, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
    SonB = new TGNumberEntryField(hf);
    SonB->SetEnabled(kFALSE);
    hf->AddFrame(SonB, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
    gf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
    
    
    hf = new TGHorizontalFrame(gf, 240, 20, kFixedWidth);
    val1 = new TGLabel(hf, "Significance");
    val1->SetTextJustify(kTextCenterX);
    hf->AddFrame(val1, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
    Sigma = new TGNumberEntryField(hf);
    Sigma->SetEnabled(kFALSE);
    hf->AddFrame(Sigma, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
    gf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
    

    b = new TGTextButton(gf, "Close");
    b->Connect("Pressed()", "AliceExtractor", this, "UnmapWindow()");
    gf->AddFrame(b, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
    AddFrame(gf, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));

    SetWindowName("Signal Extraction");

    // Set a name to the main frame

    MapSubwindows();

    // Initialize the layout algorithm via Resize()
    Resize(GetDefaultSize());
    // Map main frame
    MapWindow();
  }

  void Instructions() {

    TRootHelpDialog *instructions = new TRootHelpDialog(gClient->GetRoot(), "ANALYSIS WINDOW INSTRUCTIONS", 700, 400);

    instructions->SetText("\
  Instructions: Signal Extraction\n\n\
     With this tool you can extract the J/psi signal from the Invariant mass distribution.\n\
     You can set an invariant mass range from which the signal should be extracted. \n\
     The tool will calculate in the chosen mass range \n\
      - the signal S by integrating the counts in the \"Unlike - Like Sign Mass Distribution\" \n\
      - the background B by integrating the counts in the \"Like Sign Mass Distribution\" \n\
      - the signal over background ratio S/B\n\
      - the significance, defined as S/sqrt(S+B)\n\
     It will also store your current values for the track cuts and the in variant mass window\n\
     together with the obtained signal, S/B and significance in a file called  \"masterclass.save\".");

    instructions->Popup();

  }


  void Autosave() {



    ofstream myresult;
    myresult.open ("masterclass.save", std::ofstream::out | std::ofstream::app);

    myresult << "Momentum region: " << p1 <<" < p < "  << p2 << endl;
    myresult << "Specific energy loss: " << de1 <<" < dE/dx < " << de2 << endl;
    myresult << "Invariant mass window: " << minm <<"< m <" << maxm << endl;
    myresult << "Number of events = " << nEvents << endl;
    myresult << "Number of Jpsi's: " << numJpsi << endl;
    myresult << "Signal/Background: " << sb << endl;
    myresult << "Significance: " << significance << endl;
    myresult << endl;
    
    myresult.close();
  }

  void JpsiIntegration() {
    minm = fm1->GetNumber();
    maxm = fm2->GetNumber();
    IntegralCalculation(minm,maxm);
    JpsiCount->SetNumber(numJpsi);
    Sigma->SetNumber(significance);
    SonB->SetNumber(sb);
  }

  void IntegralCalculation(Float_t m1, Float_t m2) {
    Int_t massBin1 = gJpsiHist->FindBin( m1 + epsilon );
    Int_t massBin2 = gJpsiHist->FindBin( m2 - epsilon );
    
    cout << massBin1 << "<m<"<<massBin2<<endl;
    numJpsi = gJpsiHist->Integral(massBin1,massBin2);
    bg = (geeHist->Integral(massBin1,massBin2)) + (gppHist->Integral(massBin1,massBin2));
    if(numJpsi + bg> 0.) {
      significance = numJpsi/(TMath::Sqrt(numJpsi + bg));
    }
    if(bg > 0.0) {
      sb = numJpsi/bg;
    }
    else if(numJpsi > 0.0){
     sb =  9999.;
    }
  }

  void SetLineValue2() {

    l5->SetX1(minm);
    l5->SetY1(-100);
    l5->SetX2(minm);
    l5->SetY2(100);
    l5->SetLineColor(1);
    l5->SetLineStyle(5);

    l6->SetX1(maxm);
    l6->SetY1(-100);
    l6->SetX2(maxm);
    l6->SetY2(100);
    l6->SetLineColor(1);
    l6->SetLineStyle(5);
  }

  void PlotLines2() {

    pad->cd(2);
    SetLineValue2();
    l5->Draw();
    l6->Draw();
    pad->cd(2)->Update();

  }

  ClassDef(AliceExtractor,0);
};


AliceExtractor *gAliceExtractor = 0;
//______________________________________________________________________________
//______________________________________________________________________________

class GroupBox : public TGGroupFrame {
private:
  TGNumberEntryField *fEntry;

public:
  GroupBox(const TGWindow *p, const char *name, const char *title);
  TGNumberEntryField *GetEntry() const { return fEntry; }
  Float_t GetNumber() const { return fEntry->GetNumber(); }

  ClassDef(GroupBox, 0)
};

GroupBox::GroupBox(const TGWindow *p, const char *name, const char *title) :
  TGGroupFrame(p, name) {

  TGHorizontalFrame *hf = new TGHorizontalFrame(this);

  TGLabel *label = new TGLabel(hf, title);
  hf->AddFrame(label, new TGLayoutHints(kLHintsLeft | kLHintsCenterY));

  AddFrame(hf, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));

  fEntry = new TGNumberEntryField(this);
  AddFrame(fEntry, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));

}

//______________________________________________________________________________
//______________________________________________________________________________



class TVSDReader
{
public:
  // ----------------------------------------------------------
  // File / Event Data
  // ----------------------------------------------------------

  TFile      *fFile;
  TDirectory *fDirectory;
  TObjArray  *fEvDirKeys;
  TEveVSD    *fVSD;
  Int_t       fMaxEv, fCurEv, fMaxR, fCheckEv;
  Bool_t      fLoadTracks, fGeometrySet, fApplyTrackCuts;

  // ----------------------------------------------------------
  // Event visualization structures
  // ----------------------------------------------------------

  TEveTrackList *fTrackList;


  TGLabel *LoopNumber;
  TGNumberEntryField *fNLoop;

public:
  TVSDReader(const char *file_name) :
    fFile(0), fDirectory(0), fEvDirKeys(0),
    fVSD(0),

    fMaxEv(-1), fCurEv(-1), fMaxR(600), fCheckEv(-1),

    
    fLoadTracks(kFALSE),
    fGeometrySet(kTRUE), fApplyTrackCuts(kFALSE),
    fTrackList(0)

  {
    fFile = TFile::Open(file_name);
    if (!fFile) {
      Error("VSD_Reader", "Can not open file '%s' ... terminating.", file_name);
      gSystem->Exit(1);
    }

    fEvDirKeys = new TObjArray;
    TPMERegexp name_re("Event\\d+");
    TObjLink *lnk = fFile->GetListOfKeys()->FirstLink();
    while (lnk) {
      if (name_re.Match(lnk->GetObject()->GetName())) {
        fEvDirKeys->Add(lnk->GetObject());
      }
      lnk = lnk->Next();
    }

    fMaxEv = fEvDirKeys->GetEntriesFast();
    if (fMaxEv == 0) {
      Error("VSD_Reader", "No events to show ... terminating.");
      gSystem->Exit(1);
    }

    fVSD = new TEveVSD;
  }

  virtual ~TVSDReader()
  {
    // Destructor.

    DropEvent();
    delete fVSD;
    delete fEvDirKeys;

    fFile->Close();
    delete fFile;
  }

  void Exit() {
    gEve->GetBrowser()->UnmapWindow();
    gSystem->CleanCompiledMacros();
  }

  void Selector() {
    gAliceSelector = new AliceSelector(gClient->GetRoot(), this);
  }

  void Extractor() {
    gAliceExtractor = new AliceExtractor(gClient->GetRoot());
  }

  void DetectorInfo() {
    gAliceDetectorInfo = new AliceDetectorInfo(gClient->GetRoot(), 100, 100);
  }

  void Instructions() {

    TGMainFrame *instructions = new TGMainFrame(gClient->GetRoot(), 800, 600);
    
    instructions->SetWindowName("Instructions: ALICE masterclass on J/psi production");
    TGGroupFrame *gf = new TGGroupFrame(instructions, "");
    
    TGHorizontalFrame* hf = new TGHorizontalFrame(gf, 750, 350, kFixedWidth);
    TGTextView * helpText = new TGTextView(hf, 760, 400);
    helpText->LoadBuffer( "\
Your task is to reconstruct signals of the production of the J/psi particle in data recorded in\n\
proton-proton or proton-lead collisions with the ALICE detector at the LHC at CERN.\n\
The J/psi particle can be produced in the collision and then immediately decay into a pair\n\
of an electron and a positron. These \"daughter particles we can detect in the detector. \n\
In order to reconstruct the particle they decayed from, the \"mother particle\" we can make use\n\
of the \"invariant mass\". For a single particle, this is just is rest mass (3.1 GeV/c^2 for a J/psi).\n\
If the particle decays, this quantity is conserved and can  be calculated from the momenta and the\n\
masses of the decay daughters. \n\n\
So in order to reconstruct J/psi particles from collision data we have to do the following:\n\n\
  1.) Select only electrons and positrons out of the many particles that are produced in a collision\n\
    (e.g. pions, kaons, protons,...) For this we can use the particle identification capabilities \n\
    of the detector, based on the specific energy loss in the time projection chamber \n\
    (see attached plots). But even if we only select electrons, they can still come from many\n\
    different sources, for example a conversion of a photon to an electron-positron pair or \n\
    direct production of electron-positron pairs. \n\
    To reject electrons from other sources one possibility is to select only tracks that have a high\n\
    momentum (for kinetic reasons, the average momentum will be somewhere around half the J/psi mass).\n\
  2.)Combine all electrons with all positrons and calculate for each pair the invariant mass.\n\
     The signal of the J/psi will sit on a background from random combinations of electrons and \n\
     positrons. In order to subtract this background and obtain the pure signal, we must be able\n\
     to describe it. One method is to make combinations of electrons with electrons and of positrons\n\
     with positrons and calculate the invariant mass of these pairs.\n\
    \
  3.) Do the same for a large number of collisions. (For publications, we typically analyze some \n\
  hundred million collisions.) \n\
\n\n\
For your task you have several tools at hand, which can be found on the left side of the program:\n\
  Event Navigation: section you can browser through the recorded collisions events.\n\
  Display: you can choose whether to display the reconstructed particle tracks\n\
    of the selected event, and whether to show only those tracks which match the selection criteria \n\
    defined in the \"Track Cuts\" window (see below). Additionally you can choose whether to show or\n\
    not the detectors and a coordinate system   and you can change the background color.\n\
  Analysis Tools:\n\
    Track cuts: Here you can choose PID and momentum cuts for the tracks, in order to select only\n\
      electrons and positrons, optimally only those coming from a J/psi decay.\n\
    Signal extraction: Calculate from the invariant mass plots the number of found J/psis,\n\
      the signal over background ratio and the significance of the signal.\n\
  Plots: Plot for the selected tracks the specific energy loss as a function of the track momentum\n\
    and for the pairs of these tracks the invariant mass distributions.\n\
  Quick analysis: Once you have found satisfying track cuts and tested them on some events,\n\
    you can analyze more events more quickly by automatically looping over a given number\n\
    of events and filling the histograms automatically.\n\
  On the main part of the window you find several plots and visualizations, grouped in 3 tabs:\n\
  Multi View:\n\
    In this tab you find a 3D representation of the ALICE detector , together with 2 seperate\n\
    2D projections. With your mouse you can zoom, rotate and shift the representations. \n\
    \
  Particle Identification:\n\
    In this tab you find a 2 dimensional histogram which shows you for each track its specific energy\n\
    loss in the TPC as a function of its momentum. Lines indicate which tracks you are selecting with\n\
    your current cut settings.\n\
  Invariant Mass:\n\
    In this tab you find the invariant mass distributions for combinations of \n\
      - electrons with electron\n\
      - positrons with positrons\n\
      - electrons with positrons\n\
    and a plot where the electron-electron and positron positron distribution is subtracted from the \n\
    electron-positron distribution.");
   
    hf->AddFrame(helpText, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
    gf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
    
    hf = new TGHorizontalFrame(gf, 100, 400, kFixedWidth);
    TGTextButton *b = new TGTextButton(hf,"Particle Identification Plot");
    b->Connect("Clicked()","TVSDReader", this,"DetectorInfo()");
    hf->AddFrame(b, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
    
    b = new TGTextButton(hf,"Close");
    b->Connect("Clicked()", "TGMainFrame", instructions, "UnmapWindow()");
    hf->AddFrame(b, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
    gf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));

    
    instructions->AddFrame(gf, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
    instructions->MapSubwindows();
    
    instructions->Resize(instructions->GetDefaultSize());
    instructions->MapWindow();
    
    
    //instructions->Popup();
  }


  void StudentSet() {

    //Instructions();

    TEveBrowser *browser = gEve->GetBrowser();
    browser->GetMainFrame()->DontCallClose();
    browser->GetMainFrame()->SetWindowName("ALICE J/psi Master Class");

    if(browser->GetTabRight()->GetTabTab(3))
      browser->GetTabRight()->RemoveTab(3);

    if(browser->GetTabRight()->GetTabTab(2))
      browser->GetTabRight()->RemoveTab(2);

    if(browser->GetTabLeft()->GetTabTab(0))
      browser->GetTabLeft()->RemoveTab(0);

    if(browser->GetTabBottom()->GetTabTab(0))
      browser->GetTabBottom()->RemoveTab(0);

    browser->StartEmbedding(TRootBrowser::kLeft);

    TGMainFrame *frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
    frmMain->SetWindowName("XX GUI");
    frmMain->SetCleanup(kDeepCleanup);

    TString icondir(TString::Format("%s/icons/", gSystem->Getenv("ROOTSYS")) );
    TGPictureButton *b1 = 0;
    TGTextButton *b = 0;
    TGLabel *label = 0;
    TGGroupFrame *gf = 0;
    TGVerticalFrame *vf = 0;
    TGHorizontalFrame *hf = 0;
    ULong_t buttonRedColor;

    gf = new TGGroupFrame(frmMain, "Instructions");
    {
      vf = new TGVerticalFrame(gf, 200, 20, kFixedWidth);
      {
        const TGFont *font = gClient->GetFont("-*-times-bold-r-*-*-16-*-*-*-*-*-*-*");
        FontStruct_t buttonFont = font->GetFontStruct();
        gClient->GetColorByName("red", buttonRedColor);
        b = new TGTextButton(vf,"Instructions");
        b->SetTextColor(buttonRedColor);
        b->SetFont(buttonFont);
        vf->AddFrame(b, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
        b->Connect("Clicked()", "TVSDReader", this, "Instructions()");
      }
      gf->AddFrame(vf, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));
    }
    frmMain->AddFrame(gf);

    gf = new TGGroupFrame(frmMain, "Event Navigation");
    {
      vf = new TGVerticalFrame(gf, 200, 20, kFixedWidth);
      {
        hf = new TGHorizontalFrame(vf, 200, 20, kFixedWidth);
        {
          label = new TGLabel(hf, "Previous");
          hf->AddFrame(label, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
          label = new TGLabel(hf, "Current");
          hf->AddFrame(label, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
          label = new TGLabel(hf, "Next");
          hf->AddFrame(label, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
        }
        vf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));

        hf = new TGHorizontalFrame(vf, 200, 20, kFixedWidth);
        {
          b1 = new TGPictureButton(hf, gClient->GetPicture(icondir + "GoBack.gif"));
          hf->AddFrame(b1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
          b1->Connect("Clicked()", "TVSDReader", this, "PrevEvent()");
          gLabelEventNumber = new TGLabel(hf);
          gLabelEventNumber->SetText(1);
          hf->AddFrame(gLabelEventNumber, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
          b1 = new TGPictureButton(hf, gClient->GetPicture(icondir + "GoForward.gif"));
          hf->AddFrame(b1, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
          b1->Connect("Clicked()", "TVSDReader", this, "NextEvent()");
        }
        vf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));
      }
      gf->AddFrame(vf, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));
    }
    frmMain->AddFrame(gf);

    gf = new TGGroupFrame(frmMain, "Analysis Tools");
    {
      vf = new TGVerticalFrame(gf, 200, 20, kFixedWidth);
      {
        hf = new TGHorizontalFrame(vf, 200, 20, kFixedWidth);
        {
          gButtonLoadTracks = new TGTextButton(hf, "Load Tracks");
          gButtonLoadTracks->Connect("Clicked()", "TVSDReader", this, "LoadTracks()");
          hf->AddFrame(gButtonLoadTracks, new TGLayoutHints(kLHintsExpandX));
          
          gCheckboxLoadTracks = new TGCheckButton(hf, "", 10);
          gCheckboxLoadTracks->SetEnabled(kFALSE);
          hf->AddFrame(gCheckboxLoadTracks);
        }
        vf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
        
        
        gButtonFillPidHistos = new TGTextButton(vf,"Fill PID Histogram");
        gButtonFillPidHistos->Connect("Pressed()","TVSDReader",this,"FillEnergyLossHisto()");
        vf->AddFrame(gButtonFillPidHistos, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
        
        gButtonDefineTrackCuts = new TGTextButton(vf, "Define Track Cuts");
        gButtonDefineTrackCuts->Connect("Pressed()", "TVSDReader", this, "Selector()");
        vf->AddFrame(gButtonDefineTrackCuts,new TGLayoutHints(kLHintsExpandX,1, 1, 1, 1));
        
        
        hf = new TGHorizontalFrame(vf, 200, 20, kFixedWidth);
        {
          gButtonApplayTrackCuts = new TGTextButton(hf, "Apply Track Cuts");
          gButtonApplayTrackCuts->Connect("Clicked()", "TVSDReader", this, "SwapApplyTrackCuts()");
          hf->AddFrame(gButtonApplayTrackCuts, new TGLayoutHints(kLHintsExpandX));
          
          gCheckboxApplyTrackCuts = new TGCheckButton(hf, "", 10);
          gCheckboxApplyTrackCuts->SetEnabled(kFALSE);
          hf->AddFrame(gCheckboxApplyTrackCuts);
        }
        vf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
        
        gButtonFillInvMass = new TGTextButton(vf, "Fill Invariant Mass Histograms");
        gButtonFillInvMass->Connect("Pressed()", "TVSDReader", this, "FillInvariantMassHistos()");
        vf->AddFrame(gButtonFillInvMass, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
        
        
        b = new TGTextButton(vf, "Extract Signal");
        b->Connect("Pressed()", "TVSDReader", this, "Extractor()");
        vf->AddFrame(b,new TGLayoutHints(kLHintsExpandX,1, 1, 1, 1));
        
      }
      gf->AddFrame(vf, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));
    }  
    frmMain->AddFrame(gf);
    
    gf = new TGGroupFrame(frmMain, "Steering");
    {
      vf = new TGVerticalFrame(gf, 200, 20, kFixedWidth);
      {
        b = new TGTextButton(vf, "Show Detector");
        b->Connect("Clicked()", "TVSDReader", this, "ChangeGeometry()");
        vf->AddFrame(b, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));

        
        b = new TGTextButton(vf, "Background Color");
        b->Connect("Clicked()", "TVSDReader", this, "ChangeBackgroundColor()");
        vf->AddFrame(b, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
        
        
        b = new TGTextButton(vf, "Clear Histograms");
        b->Connect("Pressed()", "TVSDReader", this, "ClearHisto()");
        vf->AddFrame(b, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 1));
        
        
        b = new TGTextButton(vf, "Restart");
        b->Connect("Pressed()", "TVSDReader", this, "ClearHisto()");
        b->Connect("Pressed()", "TVSDReader", this, "BeginAgain()");
        vf->AddFrame(b, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
        
      }
      gf->AddFrame(vf, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));
    }
    frmMain->AddFrame(gf);
    
    



    gf = new TGGroupFrame(frmMain, "Quick Analysis");
    {
      vf = new TGVerticalFrame(gf, 200, 20, kFixedWidth);
      {
        hf = new TGHorizontalFrame(vf, 200, 20, kFixedWidth);
        {
          b = new TGTextButton(hf, "Quick Analysis");
          b->Connect("Clicked()", "TVSDReader", this, "SwapQuickAnalysis()");
          gCheckboxQuickAnalysis = new TGCheckButton(hf, "", 10);
          gCheckboxQuickAnalysis->SetEnabled(kFALSE);
          hf->AddFrame(b, new TGLayoutHints(kLHintsExpandX));
          hf->AddFrame(gCheckboxQuickAnalysis);
        }
        vf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
      }
      {
        hf = new TGHorizontalFrame(vf, 200, 20, kFixedWidth);
        {
          LoopNumber = new TGLabel(hf, " N       = ");
          fNLoop = new TGNumberEntryField(hf);
          hf->AddFrame(LoopNumber, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
          hf->AddFrame(fNLoop, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
        }
        vf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
      }
      {
        hf = new TGHorizontalFrame(vf, 200, 20, kFixedWidth);
        {
          gButtonJumpEvents = new TGTextButton(hf, "Jump N Events");
          gButtonJumpEvents->Connect("Clicked()", "TVSDReader", this, "EventsJump()");
          gButtonJumpEvents->SetEnabled(kFALSE);
          hf->AddFrame(gButtonJumpEvents, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
        }
        vf->AddFrame(hf, new TGLayoutHints(kLHintsExpandX, 1, 1, 1, 1));
      }
      gf->AddFrame(vf, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY));
    }
    frmMain->AddFrame(gf);

    frmMain->MapSubwindows();
    frmMain->Resize();
    frmMain->MapWindow();

    browser->StopEmbedding("Tools");

    browser->StartEmbedding(TRootBrowser::kBottom);

    frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600, kHorizontalFrame);
    frmMain->SetWindowName("XX GUI");
    frmMain->SetCleanup(kDeepCleanup);

    const TGFont *font = gClient->GetFont("-*-helvetica-bold-r-normal-*-20-*-*-*-*-*-*-*");
    FontStruct_t buttonFont = font->GetFontStruct();


    gClient->GetColorByName("red", buttonRedColor);
    b = new TGTextButton(frmMain,"Exit");
    b->SetTextColor(buttonRedColor);
    b->SetFont(buttonFont);
    b->Connect("Clicked()", "TApplication", gApplication, "Terminate()");

    frmMain->AddFrame(b, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY , 1, 1, 1, 1));
    frmMain->MapSubwindows();
    frmMain->Resize();
    frmMain->MapWindow();

    browser->StopEmbedding(" ");
    browser->StartEmbedding(TRootBrowser::kRight);

    pad2 = new TCanvas();
    gPad->SetLogx(1);
    gEnergyLoss->Draw();

    browser->StopEmbedding("Particle Identification");
  //  GotoEvent(0);
    browser->GetTabRight()->SetTab(1);

    browser->StopEmbedding(" ");
    browser->StartEmbedding(TRootBrowser::kRight);

    pad = new TCanvas();
    pad->Divide(2,2);

    pad->cd(1);
    gMinvHist->Draw("ep");

    pad->cd(2);
    gJpsiHist->Draw("ep");

    pad->cd(3);
    geeHist->Draw("ep");

    pad->cd(4);
    gppHist->Draw("ep");

    browser->StopEmbedding("Invariant Mass");
  //  GotoEvent(0);
    browser->GetTabRight()->SetTab(1);

  }

  void DestroyGeometry() {
    TEveSceneList *sceneList1=gEve->GetScenes();
    TEveElement::List_i i = sceneList1->BeginChildren();
    TEveSceneList *sceneList2 = (TEveSceneList *) *i;
    sceneList2->RemoveElements();
    i++;
    i++;
    TEveSceneList *sceneList3 = (TEveSceneList *) *i;
    sceneList3->RemoveElements();
    i++;
    TEveSceneList *sceneList4 = (TEveSceneList *) *i;
    sceneList4->RemoveElements();

    gMultiView->SetDepth(-10);
    gMultiView->DestroyGeomRPhi();
    gMultiView->DestroyGeomRhoZ();
    gMultiView->SetDepth(0);
    gMultiView->SetRPhiAxes();
    gMultiView->SetRhoZAxes();
  }

  void GeometryDefault() {

    TEveSceneList *sceneList1=gEve->GetScenes();
    TEveElement::List_i i = sceneList1->BeginChildren();
    TEveSceneList *sceneList2 = (TEveSceneList *) *i;
    sceneList2->RemoveElements();
    i++;
    i++;
    TEveSceneList *sceneList3 = (TEveSceneList *) *i;
    sceneList3->RemoveElements();
    i++;
    TEveSceneList *sceneList4 = (TEveSceneList *) *i;
    sceneList4->RemoveElements();

    TEveGeoShape *gentle_geom = 0;

    TFile *geom = TFile::Open("alice_mini_geom.root");
    if (!geom)
      return;
    TEveGeoShapeExtract *gse = (TEveGeoShapeExtract *) geom->Get("Gentle");
    gentle_geom = TEveGeoShape::ImportShapeExtract(gse, 0);
    geom->Close();
    delete geom;

    gentle_geom->SetRnrSelf(kFALSE);

    //ITS

    i = gentle_geom->BeginChildren();

    TEveGeoShape *lvl1 = (TEveGeoShape *) *i;

    //TPC

    i++;

    //TRD+TOF

    i++;

    lvl1 = (TEveGeoShape *) *i;
    lvl1->SetRnrSelf(kFALSE);
    TEveElement::List_i j = lvl1->BeginChildren();

    TEveGeoShape *trd1 = (TEveGeoShape *) *j;
    trd1->SetRnrSelf(kTRUE);
    trd1->SetMainTransparency(80);
    j++;

    trd1 = (TEveGeoShape *) *j;
    trd1->SetRnrSelf(kTRUE);
    trd1->SetMainTransparency(80);
    j++;

    j++;
    j++;
    j++;
    j++;

    trd1 = (TEveGeoShape *) *j;
    trd1->SetRnrSelf(kTRUE);
    trd1->SetMainTransparency(80);
    j++;

    trd1 = (TEveGeoShape *) *j;
    trd1->SetRnrSelf(kTRUE);
    trd1->SetMainTransparency(80);
    j++;

    trd1 = (TEveGeoShape *) *j;
    trd1->SetRnrSelf(kTRUE);
    trd1->SetMainTransparency(80);
    j++;

    j++;
    j++;
    j++;
    j++;
    j++;

    trd1 = (TEveGeoShape *) *j;
    trd1->SetRnrSelf(kTRUE);
    trd1->SetMainTransparency(80);
    j++;

    trd1 = (TEveGeoShape *) *j;
    trd1->SetRnrSelf(kTRUE);
    trd1->SetMainTransparency(80);
    j++;

    trd1 = (TEveGeoShape *) *j;
    trd1->SetRnrSelf(kTRUE);
    trd1->SetMainTransparency(80);
    j++;

    trd1 = (TEveGeoShape *) *j;
    trd1->SetRnrSelf(kTRUE);
    trd1->SetMainTransparency(80);
    j++;

    gEve->AddGlobalElement(gentle_geom);

    gMultiView->SetDepth(-10);
    gMultiView->DestroyGeomRPhi();
    gMultiView->ImportGeomRPhi(gentle_geom);
    gMultiView->DestroyGeomRhoZ();
    gMultiView->ImportGeomRhoZ(gentle_geom);
    gMultiView->SetDepth(0);
    gMultiView->SetRPhiAxes();
    gMultiView->SetRhoZAxes();
  }

  void ChangeGeometry() {

    if(fGeometrySet) {
      DestroyGeometry();
      fGeometrySet = kFALSE;
    } else {
      GeometryDefault();
      fGeometrySet = kTRUE;
    }
  }

  void ChangeBackgroundColor() {
    gEve->GetViewers()->SwitchColorSet();
  }

  void AttachEvent() {
    // Attach event data from current directory.
    fVSD->LoadTrees();
    fVSD->SetBranchAddresses();
  }

  void DropEvent() {
    

    // Drop currently held event data, release current directory.

    // Drop old visualization structures.
    gEve->GetViewers()->DeleteAnnotations();
    gEve->GetCurrentEvent()->DestroyElements();

    if(fTrackList)
      //fTrackList->DestroyElements();
    //delete fTrackList;
    fTrackList = 0;
    
    // Drop old event-data.
    fVSD->DeleteTrees();
    delete fDirectory;
    fDirectory = 0;
  }

  //---------------------------------------------------------------------------
  // Event navigation
  //---------------------------------------------------------------------------
  void BeginAgain() {
    p1=0.;
    p2=0.;
    de1=0.;
    de2=0.;
    
    fLoadTracks = kFALSE;
    fApplyTrackCuts = kFALSE;
    gEnableQuickAnalysis = kFALSE;
    
    gButtonLoadTracks->SetEnabled( kTRUE );
    gButtonFillPidHistos->SetEnabled( kTRUE );
    gButtonDefineTrackCuts->SetEnabled( kTRUE);
    gButtonApplayTrackCuts->SetEnabled( kTRUE );
    gButtonFillInvMass->SetEnabled( kTRUE );
    
    gCheckboxQuickAnalysis->SetDisabledAndSelected(kFALSE);
    gCheckboxApplyTrackCuts->SetDisabledAndSelected(kFALSE);
    gCheckboxLoadTracks->SetDisabledAndSelected(kFALSE);
    gButtonJumpEvents->SetEnabled(kFALSE);
    gCheckboxApplyTrackCuts->SetEnabled(kFALSE);
    
    fCheckEv = -1;
    GotoEvent(0);
  }


  void NextEvent( ) {
    if(fCurEv < fMaxEv-1){
      ResetMatrices();
      GotoEvent(fCurEv + 1 );
    }
  }

  void PrevEvent() {
    if(fCurEv!= 0){
      ResetMatrices();
      GotoEvent(fCurEv - 1);
    }
  }

  Bool_t GotoEvent(Int_t ev) {

    //cout << "going to event " << ev<<endl;

    if (ev < 0 || ev >= fMaxEv) {
      Warning("GotoEvent", "fMaxEv = fEvDirKeys->GetEntriesFast();Invalid event id %d.", ev);
      return kFALSE;
    }
      
    DropEvent();

    fCurEv = ev;
    fDirectory = (TDirectory *) ((TKey *) fEvDirKeys->At(fCurEv))->ReadObj();
    fVSD->SetDirectory(fDirectory);
    AttachEvent();
    gLabelEventNumber->SetText(TString::Format("%i / %i",fCurEv+1,fMaxEv));

    // Load event data into visualization structures.


    if( fLoadTracks ){
      LoadTracksFromTree(fMaxR);
    }
    if(gEnableQuickAnalysis) {
      FillInvariantMassHistos();
      FillEnergyLossHisto();
    }

    else{
      
      TEveElement *top = gEve->GetCurrentEvent();
      if(gMultiView) {
        gMultiView->DestroyEventRPhi();
        gMultiView->ImportEventRPhi(top);
        gMultiView->DestroyEventRhoZ();
        gMultiView->ImportEventRhoZ(top);
        gEve->Redraw3D(kFALSE, kTRUE);
      }
    }
    return kTRUE;
  }




  void SwapQuickAnalysis() {
    Bool_t enable = !gEnableQuickAnalysis;
    
    
    gButtonLoadTracks->SetEnabled( !enable );
    gButtonFillPidHistos->SetEnabled( !enable );
    gButtonDefineTrackCuts->SetEnabled( !enable);
    gButtonApplayTrackCuts->SetEnabled( !enable );
    gButtonFillInvMass->SetEnabled( !enable );
    
    
    gEnableQuickAnalysis = enable;
    gCheckboxQuickAnalysis->SetDisabledAndSelected(enable);
    gButtonJumpEvents->SetEnabled(enable);
    
    if(enable){
     fLoadTracks = kTRUE;
     gCheckboxApplyTrackCuts->SetEnabled(kFALSE);
     gCheckboxLoadTracks->SetDisabledAndSelected(kTRUE);
    }
    
  }

  void EventsJump() {
    if(gEnableQuickAnalysis) {
      Int_t nLoop;
      if(fNLoop->GetNumber() + fCurEv < fMaxEv){
        nLoop = fNLoop->GetNumber();
      }
      else {
        TRootHelpDialog *startMessage = new TRootHelpDialog(gClient->GetRoot(),"warning",250,100);
        startMessage->SetText("Too many events selected!\n");
        startMessage->Popup();
        nLoop = 0;
      }
      for(int i = 0; i < nLoop; i++) {
        NextEvent( );
      }
      pad2->cd()->Update();
      pad->cd(1)->Update();
      pad->cd(2)->Update();
      pad->cd(3)->Update();
      pad->cd(4)->Update();
    }
    else {
      TRootHelpDialog *startMessage = new TRootHelpDialog(gClient->GetRoot(),"warning",250,100);
      startMessage->SetText("To use this function you \nhave to enable \"Quick Analysis\".\n");
      startMessage->Popup();
    }
  }


  void LoadTracks() {
    if(fLoadTracks) {
      fLoadTracks=kFALSE;
      GotoEvent(fCurEv);
      if(gCheckboxLoadTracks)
        gCheckboxLoadTracks->SetDisabledAndSelected(kFALSE);
    } 
    else {
      fLoadTracks=kTRUE;
      printf("Loading Esd Tracks...\n");
      GotoEvent(fCurEv);
      printf("Esd Tracks loaded.\n");
      gCheckboxLoadTracks->SetDisabledAndSelected(kTRUE);
    }
  }


  void SwapApplyTrackCuts() {

    if( fApplyTrackCuts ) {
      fApplyTrackCuts=kFALSE;
      GotoEvent(fCurEv);
      gCheckboxApplyTrackCuts->SetDisabledAndSelected(kFALSE);
      pad2->cd();
      gEnergyLoss->Draw("colz");
      pad2->cd()->Update();
    }
    else {
      ApplyTrackCuts();
    }
  }
  
  void ApplyTrackCuts(){
      fApplyTrackCuts = kTRUE;
      printf("Loading Electron Tracks...\n");
      GotoEvent(fCurEv);
      printf("Electron Tracks loaded.\n");
      gCheckboxApplyTrackCuts->SetDisabledAndSelected(kTRUE);
      pad2->cd();
      l1->Draw();
      l2->Draw();
      l3->Draw();
      l4->Draw();
      pad2->cd()->Update();
  }


  //---------------------------------------------------------------------------
  // Track loading
  //---------------------------------------------------------------------------
  
void LoadTracksFromTree(Int_t maxR)
  {
    //cout<<"LoadTracksFromTree called"<<endl;
    if ( !fTrackList ) {
      fTrackList = new TEveTrackList("ESD Tracks");
      fTrackList->SetMainColor(kBlue);
      //fTrackList->IncDenyDestroy();
    }  
    else {
      fTrackList->DestroyElements();
    }

    TEveTrackPropagator *trkProp = fTrackList->GetPropagator();
    trkProp->SetMaxR(maxR);


    TTree *fTreeR = fVSD->fTreeR;
    if(fTreeR){
      Int_t nTracks = fTreeR->GetEntries();

      ResetMatrices();
      ResetDedxArray();
      
      for (Int_t i = 0; i < nTracks; ++i) {

        fTreeR->GetEntry(i);
        TEveTrack *track = new TEveTrack(&fVSD->fR, trkProp);

        Float_t px = track->GetMomentum().fX;
        Float_t py = track->GetMomentum().fY;
        Float_t pz = track->GetMomentum().fZ;
        Float_t dedx = track->GetStatus();
        Int_t charge = track->GetCharge();
        Float_t pSquared = px*px + py*py + pz*pz;
        
        if( fApplyTrackCuts &&  (dedx >= de2 || dedx < de1 || pSquared >= p2*p2 || pSquared <= p1*p1)    ){
          track->Destroy();
          continue;
        }
        nSelectedTracks++;
// Fill arrays for energy loss histogram
        gArrP[i] = TMath::Sqrt(px*px + py*py + pz*pz);
        gArrdEdx[i] = dedx;

        
// Fill arrays for invariant mass calculation        
        
// if energy loss below threshold, assume particle is a pion, otherwise assume it is an electron        
        Float_t massSquared = dedx>62 ? 0. : 0.019;
        Float_t e = TMath::Sqrt( pSquared + massSquared );
        if(charge > 0) {
          po[nPositrons][0] = e;
          po[nPositrons][1] = px;
          po[nPositrons][2] = py;
          po[nPositrons][3] = pz;
          nPositrons++;
        }
        else {
          el[nElectrons][0] = e;
          el[nElectrons][1] = px;
          el[nElectrons][2] = py;
          el[nElectrons][3] = pz;
          nElectrons++;
        }
        track->SetAttLineAttMarker(fTrackList);
        fTrackList->AddElement(track);
      }
      
     }
    fTrackList->MakeTracks(); 
    gEve->AddElement(fTrackList);
    gEve->Redraw3D();
  }

  void FillInvariantMassHistos() {
    if(fCurEv > fCheckEv) {
      if(fLoadTracks) {
        nEvents ++ ;
        for(int i = 0; i < nElectrons-1; i++)
        {
          for(int j = i+1; j < nElectrons; j++)
          {
            Float_t mass = TMath::Sqrt((el[i][0]+el[j][0])*(el[i][0]+el[j][0]) - (el[i][1]+el[j][1])*(el[i][1]+el[j][1]) - (el[i][2]+el[j][2])*(el[i][2]+el[j][2]) - (el[i][3]+el[j][3])*(el[i][3]+el[j][3]));
            geeHist->Fill(mass);
            geHist->Fill(mass);
          }
        }

        for(int i = 0; i < nPositrons-1; i++)
        {
          for(int j = i+1; j < nPositrons; j++)
          {
            Float_t mass = TMath::Sqrt((po[i][0]+po[j][0])*(po[i][0]+po[j][0]) - (po[i][1]+po[j][1])*(po[i][1]+po[j][1]) - (po[i][2]+po[j][2])*(po[i][2]+po[j][2]) - (po[i][3]+po[j][3])*(po[i][3]+po[j][3]));
            gppHist->Fill(mass);
            gpHist->Fill(mass);
          }
        }

        for(int i = 0; i < nElectrons; i++)
        {

          for(int j = 0; j < nPositrons; j++)
          {
            Float_t mass = TMath::Sqrt((el[i][0]+po[j][0])*(el[i][0]+po[j][0]) - (el[i][1]+po[j][1])*(el[i][1]+po[j][1]) - (el[i][2]+po[j][2])*(el[i][2]+po[j][2]) - (el[i][3]+po[j][3])*(el[i][3]+po[j][3]));
            gMinvHist->Fill(mass);
            gJpsiHist->Fill(mass);
          }
        }

        pad->cd(1);
        Float_t yMax = gMinvHist->GetBinContent(gMinvHist->GetMaximumBin());
        yMax += sqrt(yMax);
        yMax *= 1.05;
        gMinvHist->GetYaxis()->SetRangeUser(0,yMax );
        gMinvHist->Draw();

        pad->cd(2);
        gJpsiHist->Draw();

        pad->cd(3);
        geeHist->GetYaxis()->SetRangeUser(0, yMax );
        geeHist->Draw();

        pad->cd(4);
        gppHist->GetYaxis()->SetRangeUser(0, yMax );
        gppHist->Draw();

        pad->cd(1)->Update();
        pad->cd(2)->Update();
        pad->cd(3)->Update();
        pad->cd(4)->Update();


        gJpsiHist->Add(geHist,-1);
        gJpsiHist->Add(gpHist,-1);

        fCheckEv = fCurEv;

        ResetMatrices();

      } 
      else {
        TRootHelpDialog *startMessage = new TRootHelpDialog(gClient->GetRoot(),"warning",250,100);
        startMessage->SetText("Load tracks first!");
        startMessage->Popup();
      }
    }
    else{
      TRootHelpDialog *startMessage = new TRootHelpDialog(gClient->GetRoot(),"warning",250,100);
      startMessage->SetText("Event already added.\n");
      startMessage->Popup();
    }
  }

  void ResetMatrices() {
    for(int i = 0; i < 100; i++) {
      for(int j = 0; j < 4; j++) {
        el[i][j] = 0;
        po[i][j] = 0;
      }
    }
    nPositrons = 0;
    nElectrons = 0;
    geHist->Reset();
    gpHist->Reset();
  }
  
  
  void ResetDedxArray(){
    for(int i = 0; i < nSelectedTracks; i++) {
      gArrP[i] = 0;
      gArrdEdx[i] = 0;
    }
    nSelectedTracks = 0;
  }
  
  

  void ClearHisto() {

    pad->cd(1);
    gMinvHist->Reset();
    gMinvHist->Draw();
    pad->cd(1)->Update();

    pad2->cd ();  
    gEnergyLoss->Reset();
    gEnergyLoss->Draw();
    pad2->cd()->Update();
    
    pad->cd(3);
    geeHist->Reset();
    geeHist->Draw();
    pad->cd(3)->Update();

    pad->cd(4);
    gppHist->Reset();
    gppHist->Draw();
    pad->cd(4)->Update();

    pad->cd(2);
    gJpsiHist->Reset();
    gJpsiHist->Draw();
    pad->cd(2)->Update();
  }
  

  void FillEnergyLossHisto( ) {    
    if( fLoadTracks   ) {
      if(nSelectedTracks > 0){
        for(int i = 0; i < nSelectedTracks; i++) {

          gEnergyLoss->Fill( gArrP[i],gArrdEdx[i] );
        }
        pad2->cd();
        gEnergyLoss->Draw("colz");
        if(fApplyTrackCuts){
          l1->Draw();
          l2->Draw();
          l3->Draw();
          l4->Draw();
        }
        pad2->cd()->Update();
        ResetDedxArray();
      }
    } 
    else {
      TRootHelpDialog *startMessage = new TRootHelpDialog(gClient->GetRoot(),"warning",250,100);
      startMessage->SetText("Load tracks first!");
      startMessage->Popup();
    }
  }

  ClassDef(TVSDReader, 0);
};

TVSDReader *gVSDReader = 0;


//______________________________________________________________________________
void alice_vsd( Int_t dataset) {

  TString vsd_file_name;
  
  l1 = new TLine(p1,p1,0,de2);
  l2 = new TLine(p2,p2,0,de2);
  l3 = new TLine(0,p2,de1,de1);
  l4 = new TLine(0,p2,de2,de2);
  l5 = new TLine(0,p2,de1,de1);
  l6 = new TLine(0,p2,de2,de2);
  

  vsd_file_name = Form("eve_files/data/events_%i.root",dataset);
  cout << vsd_file_name.Data() << endl;

  globalDataset = dataset;

  // Main function, initializes the application.
  //
  // 1. Load the auto-generated library holding ESD classes and ESD dictionaries.
  // 2. Open ESD data-files.
  // 3. Load cartoon geometry.
  // 4. Spawn simple GUI.
  // 5. Load first event.

  TFile::SetCacheFileDir(".");

  TEveVSD::DisableTObjectStreamersForVSDStruct();

  TEveManager::Create(kTRUE,"FV");

  TEveGeoShape *gentle_geom = 0;

  TFile *geom = TFile::Open("alice_mini_geom.root");
  if (!geom) return;
  TEveGeoShapeExtract *gse = (TEveGeoShapeExtract *) geom->Get("Gentle");
  gentle_geom = TEveGeoShape::ImportShapeExtract(gse, 0);
  geom->Close();
  delete geom;

  gentle_geom->SetRnrSelf(kFALSE);

  //ITS

  TEveElement::List_i i = gentle_geom->BeginChildren();
  TEveGeoShape *lvl1 = (TEveGeoShape *) *i;

  //TPC
  i++;

  //TRD+TOF
  i++;

  lvl1 = (TEveGeoShape *) *i;
  lvl1->SetRnrSelf(kFALSE);
  TEveElement::List_i j = lvl1->BeginChildren();

  TEveGeoShape *trd1 = (TEveGeoShape *) *j;
  trd1->SetRnrSelf(kTRUE);
  trd1->SetMainTransparency(80);
  j++;

  trd1 = (TEveGeoShape *) *j;
  trd1->SetRnrSelf(kTRUE);
  trd1->SetMainTransparency(80);
  j++;

  j++;
  j++;
  j++;
  j++;

  trd1 = (TEveGeoShape *) *j;
  trd1->SetRnrSelf(kTRUE);
  trd1->SetMainTransparency(80);
  j++;

  trd1 = (TEveGeoShape *) *j;
  trd1->SetRnrSelf(kTRUE);
  trd1->SetMainTransparency(80);
  j++;

  trd1 = (TEveGeoShape *) *j;
  trd1->SetRnrSelf(kTRUE);
  trd1->SetMainTransparency(80);
  j++;

  j++;
  j++;
  j++;
  j++;
  j++;

  trd1 = (TEveGeoShape *) *j;
  trd1->SetRnrSelf(kTRUE);
  trd1->SetMainTransparency(80);
  j++;

  trd1 = (TEveGeoShape *) *j;
  trd1->SetRnrSelf(kTRUE);
  trd1->SetMainTransparency(80);
  j++;

  trd1 = (TEveGeoShape *) *j;
  trd1->SetRnrSelf(kTRUE);
  trd1->SetMainTransparency(80);
  j++;

  trd1 = (TEveGeoShape *) *j;
  trd1->SetRnrSelf(kTRUE);
  trd1->SetMainTransparency(80);
  j++;

  gEve->AddGlobalElement(gentle_geom);

  // Standard multi-view
  //=====================

  gMultiView = new MultiView();
  gMultiView->f3DView->GetGLViewer()->SetStyle(TGLRnrCtx::kOutline);
  gMultiView->f3DView->GetGLViewer()->SetStyle(0);
  gMultiView->SetDepth(-10);
  gMultiView->ImportGeomRPhi(gentle_geom);
  gMultiView->ImportGeomRhoZ(gentle_geom);
  gMultiView->SetDepth(0);

  // Final stuff
  //=============

  gEve->GetViewers()->SwitchColorSet();
  gEve->GetDefaultGLViewer()->SetStyle(TGLRnrCtx::kOutline);

  //   make_gui(mode);
  gVSDReader = new TVSDReader(vsd_file_name);



  TEveBrowser *browser = gEve->GetBrowser();

  browser->GetTabLeft()->RemoveTab(1);
  browser->GetTabLeft()->RemoveTab(0);

  //   browser->StartEmbedding(TRootBrowser::kRight);
  gStyle->SetOptStat(110);
  gStyle->SetPalette(1,0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTextSize(0.5);
  gStyle->SetLabelSize(0.03,"xyz");
  gStyle->SetLabelOffset(0.002,"xyz");
  gStyle->SetTitleFontSize(0.04);
  gStyle->SetTitleOffset(1,"y");
  gStyle->SetTitleOffset(0.7,"x");
  gStyle->SetCanvasColor(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  
  gJpsiHist = new TH1F("Statistics Jpsi","Difference: EP - (EE + PP) ",60,0.,6);
  gJpsiHist->SetLineColor(6);
  gJpsiHist->SetMarkerColor(6);
  gJpsiHist->GetXaxis()->SetTitle("m_{ee} (GeV/c^{2})");
  gJpsiHist->GetYaxis()->SetTitle("Counts");
  gJpsiHist->Sumw2();

  gMinvHist = new TH1F("Statistics e^{+} e^{-}","Invariant Mass Distribution: Electrons + Positrons",60,0.,6);
  gMinvHist->SetLineColor(2);
  gMinvHist->SetMarkerColor(2);
  gMinvHist->GetXaxis()->SetTitle("m_{ee} (GeV/c^{2})");
  gMinvHist->GetYaxis()->SetTitle("Counts");
  gMinvHist->Sumw2();

  geeHist = new TH1F("Statistics e^{-} e^{-}","Invariant Mass Distribution: Electrons + Electrons",60,0.,6);
  geeHist->SetLineColor(4);
  geeHist->SetMarkerColor(4);
  geeHist->GetXaxis()->SetTitle("m_{ee} (GeV/c^{2})");
  geeHist->GetYaxis()->SetTitle("Counts");
  geeHist->Sumw2();

  gppHist = new TH1F("Statistics e^{+} e^{+}","Invariant Mass Distribution: Positrons + Positrons",60,0.,6);
  gppHist->SetLineColor(416);
  gppHist->SetMarkerColor(416);
  gppHist->GetXaxis()->SetTitle("m_{ee} (GeV/c^{2})");
  gppHist->GetYaxis()->SetTitle("Counts");
  gppHist->Sumw2();

  geHist = new TH1F("copy e^{-} e^{-}","e^{-} e^{-} Distribution",60,0.,6);
  geHist->Sumw2();

  gpHist = new TH1F("copy e^{+} e^{+}","e^{+} e^{+} Distribution",60,0.,6);
  gpHist->Sumw2();

  
  Int_t nbinsX = 200;
  Int_t nbinsY = 120;
  Float_t  *binLimitsX = new Float_t[nbinsX+1];
  Float_t  *binLimitsY = new Float_t[nbinsX+1];
  Float_t firstX = 0.1;
  Float_t lastX = 20.;
  Float_t firstY = 20.;
  Float_t lastY = 140.;
  Float_t expMax=TMath::Log(lastX/firstX);
  for (Int_t i=0; i<nbinsX+1; ++i){
    binLimitsX[i]=firstX*TMath::Exp(expMax/nbinsX*(Float_t)i);
  }
  for (Int_t i=0; i<nbinsY+1; ++i){
    binLimitsY[i] = firstY + i * (lastY - firstY)/nbinsY; 
  }
  
  gEnergyLoss = new TH2F("Specific Energy Loss","Specific Energy Loss",nbinsX, binLimitsX,nbinsY, binLimitsY );
  gEnergyLoss->GetXaxis()->SetTitle("momentum (GeV/c)");
  gEnergyLoss->GetXaxis()->SetTitleOffset(1.5);
  gEnergyLoss->GetYaxis()->SetTitle("dE/dx in TPC (arb. units)");
  gEnergyLoss->SetStats(0);

  gEve->AddEvent(new TEveEventManager("Event", "ALICE VSD Event"));
  gVSDReader->StudentSet();
  gVSDReader->GotoEvent(0);

  gEve->GetWindowManager()->HideAllEveDecorations();
  gEve->Redraw3D(kTRUE); // Reset camera after the first event has been shown.
}