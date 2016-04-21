// Multi-view (3d, rphi, rhoz) service class using EVE Window Manager.
// Author: Matevz Tadel 2009

#include <TEveManager.h>

#include <TEveViewer.h>
#include <TGLViewer.h>

#include <TEveScene.h>

#include <TEveProjectionManager.h>
#include <TEveProjectionAxes.h>
#include <TEveProjectionAxesGL.h>

#include <TEveBrowser.h>
#include <TEveWindow.h>

#include <TAxis3D.h>

// MultiView
//
// Structure encapsulating standard views: 3D, r-phi and rho-z.
// Includes scenes and projection managers.
//
// Should be used in compiled mode.

struct MultiView
{
//   TEveProjectionManager *f3DMgr;
   TEveProjectionManager *fRPhiMgr;
   TEveProjectionManager *fRhoZMgr;

   TEveViewer            *f3DView;
   TEveViewer            *fRPhiView;
   TEveViewer            *fRhoZView;

//   TEveScene             *f3DGeomScene;

   TEveScene             *fRPhiGeomScene;
   TEveScene             *fRhoZGeomScene;
   TEveScene             *fRPhiEventScene;
   TEveScene             *fRhoZEventScene;

   TEveGeoShape          *fGeomGentle;     // Obvious meaning.
   TEveGeoShape          *fGeomGentleRPhi; // Obvious meaning.
   TEveGeoShape          *fGeomGentleRhoZ; // Obvious meaning.


   //---------------------------------------------------------------------------

   MultiView()
   {
      // Constructor --- creates required scenes, projection managers
      // and GL viewers.

      // Scenes
      //========


//      f3DGeomScene = (dynamic_cast<TEveScene*>(gEve->GetScenes()->FirstChild()));

      fRPhiGeomScene  = gEve->SpawnNewScene("RPhi Geometry",
                                            "Scene holding projected geometry for the RPhi view.");
      fRhoZGeomScene  = gEve->SpawnNewScene("RhoZ Geometry",
                                            "Scene holding projected geometry for the RhoZ view.");
      fRPhiEventScene = gEve->SpawnNewScene("RPhi Event Data",
                                            "Scene holding projected event-data for the RPhi view.");
      fRhoZEventScene = gEve->SpawnNewScene("RhoZ Event Data",
                                            "Scene holding projected event-data for the RhoZ view.");


      // Projection managers
      //=====================
/*
      f3DMgr = new TEveProjectionManager(TEve3DProjection::kPT_3D);
      gEve->AddToListTree(f3DMgr, kFALSE);
      {
         TEveProjectionAxesGL* a = new TEveProjectionAxesGL(f3DMgr);
//         a->SetMainColor(kWhite);
//         a->SetTitle("3D");
//         a->SetTitleSize(0.05);
//         a->SetTitleFont(102);
//         a->SetLabelSize(0.025);
//         a->SetLabelFont(102);
         f3DGeomScene->AddElement(a);
      }

//   TAxis3D* axis = new TAxis3D();
 //  axis->Paint();
*/
      fRPhiMgr = new TEveProjectionManager(TEveProjection::kPT_RPhi);
      gEve->AddToListTree(fRPhiMgr, kFALSE);
      {
         TEveProjectionAxes* a = new TEveProjectionAxes(fRPhiMgr);
         a->SetMainColor(kWhite);
         a->SetTitle("R-Phi");
         a->SetTitleSize(0.05);
         a->SetTitleFont(102);
         a->SetLabelSize(0.025);
         a->SetLabelFont(102);
         fRPhiGeomScene->AddElement(a);
      }

      fRhoZMgr = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
      gEve->AddToListTree(fRhoZMgr, kFALSE);
      {
         TEveProjectionAxes* a = new TEveProjectionAxes(fRhoZMgr);
         a->SetMainColor(kWhite);
         a->SetTitle("Rho-Z");
         a->SetTitleSize(0.05);
         a->SetTitleFont(102);
         a->SetLabelSize(0.025);
         a->SetLabelFont(102);
         fRhoZGeomScene->AddElement(a);
      }


      // Viewers
      //=========

      TEveWindowSlot *slot = 0;
      TEveWindowPack *pack = 0;

      slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
      pack = slot->MakePack();
      pack->SetElementName("Multi View");
      pack->SetHorizontal();
      pack->SetShowTitleBar(kFALSE);
      pack->NewSlot()->MakeCurrent();
      f3DView = gEve->SpawnNewViewer("3D View", "");

//      gEve->GetGlobalScene()->AddElement(axis);
      f3DView->AddScene(gEve->GetGlobalScene());
///      f3DView->AddScene(f3DGeomScene);
      f3DView->AddScene(gEve->GetEventScene());
//      f3DView->GetGLViewer()->SetPointScale(0);
//      f3DView->AddElement(axis);

      pack = pack->NewSlot()->MakePack();
      pack->SetShowTitleBar(kFALSE);
      pack->NewSlot()->MakeCurrent();
      fRPhiView = gEve->SpawnNewViewer("RPhi View", "");
      fRPhiView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
      fRPhiView->AddScene(fRPhiGeomScene);
      fRPhiView->AddScene(fRPhiEventScene);

      pack->NewSlot()->MakeCurrent();
      fRhoZView = gEve->SpawnNewViewer("RhoZ View", "");
      fRhoZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
      fRhoZView->AddScene(fRhoZGeomScene);
      fRhoZView->AddScene(fRhoZEventScene);
   }

   //---------------------------------------------------------------------------

   void SetDepth(Float_t d)
   {
      // Set current depth on all projection managers.

      fRPhiMgr->SetCurrentDepth(d);
      fRhoZMgr->SetCurrentDepth(d);
   }

   //---------------------------------------------------------------------------

   void InitGeomGentle(TEveGeoShape* g3d, TEveGeoShape* grphi, TEveGeoShape* grhoz)
   {
     // Initialize gentle geometry.

      fGeomGentle     = g3d;
      fGeomGentleRPhi = grphi; fGeomGentleRPhi->IncDenyDestroy();
      fGeomGentleRhoZ = grhoz; fGeomGentleRhoZ->IncDenyDestroy();

      ImportGeomRPhi(fGeomGentleRPhi);
      ImportGeomRhoZ(fGeomGentleRhoZ);

   }

   void ImportGeomRPhi(TEveElement* el)
   { 
      fRPhiMgr->ImportElements(el, fRPhiGeomScene);
   }

   void ImportGeomRhoZ(TEveElement* el)
   { 
      fRhoZMgr->ImportElements(el, fRhoZGeomScene);
   }

   void ImportEventRPhi(TEveElement* el)
   { 
      fRPhiMgr->ImportElements(el, fRPhiEventScene);
   }

   void ImportEventRhoZ(TEveElement* el)
   { 
      fRhoZMgr->ImportElements(el, fRhoZEventScene);
   }

   //---------------------------------------------------------------------------

   void DestroyEventRPhi()
   {
      fRPhiEventScene->DestroyElements();
   }

   void DestroyEventRhoZ()
   {
      fRhoZEventScene->DestroyElements();
   }

   void DestroyGeomRPhi()
   {
      fRPhiGeomScene->DestroyElements();
   }

   void DestroyGeomRhoZ()
   {
      fRhoZGeomScene->DestroyElements();
   }

   void DestroyAllGeometries()
   {
      // Destroy 3d, r-phi and rho-z geometries.

      fGeomGentle->DestroyElements();
      fGeomGentleRPhi->DestroyElements();
      fGeomGentleRhoZ->DestroyElements();

   }

   void SetRPhiAxes()
   {
      gEve->AddToListTree(fRPhiMgr, kFALSE);
      {
         TEveProjectionAxes* a = new TEveProjectionAxes(fRPhiMgr);
         a->SetMainColor(kWhite);
         a->SetTitle("R-Phi");
         a->SetTitleSize(0.05);
         a->SetTitleFont(102);
         a->SetLabelSize(0.025);
         a->SetLabelFont(102);
         fRPhiGeomScene->AddElement(a);
      }
   }

   void SetRhoZAxes()
   {
      gEve->AddToListTree(fRhoZMgr, kFALSE);
      {
         TEveProjectionAxes* a = new TEveProjectionAxes(fRhoZMgr);
         a->SetMainColor(kWhite);
         a->SetTitle("Rho-Z");
         a->SetTitleSize(0.05);
         a->SetTitleFont(102);
         a->SetLabelSize(0.025);
         a->SetLabelFont(102);
         fRhoZGeomScene->AddElement(a);
      }
   }

   TEveViewer* Get3DView()
   {
      return f3DView;
   }

   TEveViewer* GetRPhiView()
   {
      return fRPhiView;
   }

   TEveScene* GetRPhiEventScene()
   {
      return fRPhiEventScene;
   }

   TEveScene* GetRhoZEventScene()
   {
      return fRhoZEventScene;
   }

};
