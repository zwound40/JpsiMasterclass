#include <TApplication.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGListBox.h>
#include <TGComboBox.h>
#include <TList.h>
#include <TGLabel.h>
#include <TRootHelpDialog.h>

class MasterClassFrame : public TGMainFrame {

private:

   TGTextButton		*fExample;
   TGTextButton		*fStudent;
   TGTextButton		*fTeacher;
   TGTextButton		*fExit;
   TGComboBox		*fDataset;

public:

   MasterClassFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~MasterClassFrame();
   void Start();

   ClassDef(MasterClassFrame, 0)
};

void MasterClassFrame::Start()
{
  #if defined(__CINT__)
  {
    Info("alice_vsd.C", "Has to be run in compiled mode ... doing this for you.");
    gSystem->CompileMacro("alice_vsd.C");
  }
  #else
    # include "alice_vsd.C"
  # endif
  alice_vsd(fDataset->GetSelected());

  UnmapWindow();
}





MasterClassFrame::MasterClassFrame(const TGWindow *p, UInt_t w, UInt_t h) :
   TGMainFrame(p, w, h)
{

   TGVerticalFrame* hf = new TGVerticalFrame(this);
   {

      TGLabel* label = new TGLabel(hf,"Data Selection");
      hf->AddFrame(label, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

      TGPictureButton* b1 = new TGPictureButton(hf, gClient->GetPicture("eve_files/alice_logo.png"));
      hf->AddFrame(b1, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

      fStudent = new TGTextButton(hf,"Start Analysis");
      hf->AddFrame(fStudent, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
      fStudent->Connect("Clicked()", "MasterClassFrame", this, "Start()");
      
      
      fDataset = new TGComboBox(hf);
      fDataset->AddEntry("p-p collision", 1);
      fDataset->AddEntry("p-Pb collision", 2);

      fDataset->Resize(200,20);
      fDataset->Select(1,kFALSE);
      hf->AddFrame(fDataset, new TGLayoutHints(kLHintsExpandX));

      fExit = new TGTextButton(hf,"Exit");
      hf->AddFrame(fExit, new TGLayoutHints(kLHintsExpandX, 5, 5, 10, 10));
      fExit->Connect("Clicked()", "TApplication", gApplication, "Terminate()");

   }

   AddFrame(hf);

   SetWindowName("J/psi Masterclass");
   MapSubwindows();

   Resize(GetDefaultSize());

   MapWindow();

}

MasterClassFrame::~MasterClassFrame()
{
   Cleanup();
}

void masterclass()
{
   new MasterClassFrame(gClient->GetRoot(), 800, 600);
   TRootHelpDialog* instructions = new TRootHelpDialog(gClient->GetRoot(), "MASTERCLASS MENU INSTRUCTIONS", 700, 250);
   instructions->SetText("\
Welcome to ALICE J/psi Masterclass!\n\n\
\
   You see the window \"Data Selection\". In this window you can select the dataset which you want\n\
   to analyse, either proton-proton collision data or proton-lead collision data. \n\
");
   instructions->Popup();
}
