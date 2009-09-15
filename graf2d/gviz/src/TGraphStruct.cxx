// @(#)root/hist:$Id$
// Author: Olivier Couet 13/07/09

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "Riostream.h"
#include "TPad.h"
#include "TGraphStruct.h"

#include <stdio.h>

#include <gvc.h>

ClassImp(TGraphStruct)


//______________________________________________________________________________
/* Begin_Html
<center><h2>Graph Structure class</h2></center>
The Graph Structure is an interface to the graphviz package.
End_Html */


//______________________________________________________________________________
TGraphStruct::TGraphStruct()
{
   // Graph Structure default constructor.

   fNodes   = 0;
   fEdges   = 0;
   fGVGraph = 0;
   fGVC     = 0;

   SetMargin();
}


//______________________________________________________________________________
TGraphStruct::~TGraphStruct()
{
   // Graph Structure default destructor.

   gvFreeLayout(fGVC,fGVGraph);
   agclose(fGVGraph);
   gvFreeContext(fGVC);

   if (fNodes) delete fNodes;
   if (fEdges) delete fEdges;

}


//______________________________________________________________________________
void TGraphStruct::AddEdge(TGraphEdge *edge)
{
   // Add the edge "edge" in this TGraphStruct.

   if (!fEdges) fEdges = new TList;

   fEdges->Add(edge);
}


//______________________________________________________________________________
TGraphEdge *TGraphStruct::AddEdge(TGraphNode *n1, TGraphNode *n2)
{
   // Create an edge between n1 and n2 and put it in this graph.
   //
   // Two edges can connect the same nodes the same way, so there
   // is no need to check if an edge already exists.

   if (!fEdges) fEdges = new TList;

   TGraphEdge *edge = new TGraphEdge(n1, n2);
   fEdges->Add(edge);

   return edge;
}


//______________________________________________________________________________
void TGraphStruct::AddNode(TGraphNode *node)
{
   // Add the node "node" in this TGraphStruct.

   if (!fNodes) fNodes = new TList;

   fNodes->Add(node);
}


//______________________________________________________________________________
TGraphNode *TGraphStruct::AddNode(const char *name, const char *title)
{
   // Create the node "name" if it does not exist and add it to this TGraphStruct.

   if (!fNodes) fNodes = new TList;

   TGraphNode *node = (TGraphNode*)fNodes->FindObject(name);

   if (!node) {
      node = new TGraphNode(name, title);
      fNodes->Add(node);
   }

   return node;
}



//______________________________________________________________________________
void TGraphStruct::DumpAsDotFile(const char *filename)
{
   // Dump this graph structure as a "dot" file.

   if (!fGVGraph) Layout();
   FILE  *file;
   file=fopen(filename,"wt");
   agwrite(fGVGraph, file);
   fclose(file);
}


//______________________________________________________________________________
void TGraphStruct::Draw(Option_t *option)
{
   // Draw the graph

   if (!fGVGraph) Layout();

   // Get the bounding box
   if (gPad) {
      gPad->Range(GD_bb(fGVGraph).LL.x-fMargin, GD_bb(fGVGraph).LL.y-fMargin,
                  GD_bb(fGVGraph).UR.x+fMargin, GD_bb(fGVGraph).UR.y+fMargin);
   }

   AppendPad(option);

   // Draw the nodes
   if (fNodes) {
      TGraphNode *node;
      node = (TGraphNode*) fNodes->First();
      node->Draw();
      for(Int_t i = 1; i < fNodes->GetSize(); i++){
         node = (TGraphNode*)fNodes->After(node);
         node->Draw();
      }
   }

   // Draw the edges
   if (fEdges) {
      TGraphEdge *edge;
      edge = (TGraphEdge*) fEdges->First();
      edge->Draw();
      for(Int_t i = 1; i < fEdges->GetSize(); i++){
         edge = (TGraphEdge*)fEdges->After(edge);
         edge->Draw();
      }
   }
}


//______________________________________________________________________________
void TGraphStruct::Layout()
{
   // Layout the graph into a GraphViz data structure

   TGraphNode *node;
   TGraphEdge *edge;

   // Create the graph context.
   if (fGVC) gvFreeContext(fGVC);
   fGVC = gvContext();

   // Create the graph.
   if (fGVGraph) {
      gvFreeLayout(fGVC,fGVGraph);
      agclose(fGVGraph);
   }
   fGVGraph = agopen((char*)"GVGraph", AGDIGRAPH);

   // Put the GV nodes into the GV graph
   if (fNodes) {
      node = (TGraphNode*) fNodes->First();
      node->CreateGVNode(fGVGraph);
      for(Int_t i = 1; i < fNodes->GetSize(); i++){
         node = (TGraphNode*)fNodes->After(node);
         node->CreateGVNode(fGVGraph);
      }
   }

   // Put the edges into the graph
   if (fEdges) {
      edge = (TGraphEdge*) fEdges->First();
      edge->CreateGVEdge(fGVGraph);
      for(Int_t i = 1; i < fEdges->GetSize(); i++){
         edge = (TGraphEdge*)fEdges->After(edge);
         edge->CreateGVEdge(fGVGraph);
      }
   }

   // Layout the graph
   gvLayout(fGVC, fGVGraph, (char*)"dot");

   // Layout the nodes
   if (fNodes) {
      node = (TGraphNode*) fNodes->First();
      node->Layout();
      for(Int_t i = 1; i < fNodes->GetSize(); i++){
         node = (TGraphNode*)fNodes->After(node);
         node->Layout();
      }
   }

   // Layout the edges
   if (fEdges) {
      edge = (TGraphEdge*) fEdges->First();
      edge->Layout();
      for(Int_t i = 1; i < fEdges->GetSize(); i++){
         edge = (TGraphEdge*)fEdges->After(edge);
         edge->Layout();
      }
   }
}


//______________________________________________________________________________
void TGraphStruct::SavePrimitive(ostream &out, Option_t * /*= ""*/)
{
   // Save primitive as a C++ statement(s) on output stream out

   out<<"   TGraphStruct *graphstruct = new  TGraphStruct();"<<endl;

   // Save the nodes
   if (fNodes) {
      TGraphNode *node;
      node = (TGraphNode*) fNodes->First();
      out<<"   TGraphNode *"<<node->GetName()<<" = graphstruct->AddNode(\""<<
                            node->GetName()<<"\",\""<<
                            node->GetTitle()<<"\");"<<endl;
      for(Int_t i = 1; i < fNodes->GetSize(); i++){
         node = (TGraphNode*)fNodes->After(node);
         out<<"   TGraphNode *"<<node->GetName()<<" = graphstruct->AddNode(\""<<
                               node->GetName()<<"\",\""<<
                               node->GetTitle()<<"\");"<<endl;
      }
   }

   // Save the edges
   if (fEdges) {
      TGraphEdge *edge;
      edge = (TGraphEdge*) fEdges->First();
      out<<"   graphstruct->AddEdge("<<
                            edge->GetNode1()->GetName()<<","<<
                            edge->GetNode2()->GetName()<<");"<<endl;
      for(Int_t i = 1; i < fEdges->GetSize(); i++){
         edge = (TGraphEdge*)fEdges->After(edge);
         out<<"   graphstruct->AddEdge("<<
                               edge->GetNode1()->GetName()<<","<<
                               edge->GetNode2()->GetName()<<");"<<endl;
      }
   }

   out<<"   graphstruct->Draw();"<<endl;
}


//______________________________________________________________________________
void TGraphStruct::Streamer(TBuffer &/*b*/)
{
}
