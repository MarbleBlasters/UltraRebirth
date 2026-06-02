//-----------------------------------------------------------------------------
// Torque Game Engine Advanced 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/containers/guiDynamicCtrlArrayCtrl.h"

IMPLEMENT_CONOBJECT(GuiDynamicCtrlArrayControl);

GuiDynamicCtrlArrayControl::GuiDynamicCtrlArrayControl()
{
   mCols = 0;
   mColSize = 64;
   mRowSize = 64;
   mRowSpacing = 0;
   mColSpacing = 0;
   //mIsContainer = true;

   mResizing = false;
}

GuiDynamicCtrlArrayControl::~GuiDynamicCtrlArrayControl()
{
}

void GuiDynamicCtrlArrayControl::initPersistFields()
{
  Parent::initPersistFields();

  addField("colCount",     TypeS32,       Offset(mCols,        GuiDynamicCtrlArrayControl));
  addField("colSize",      TypeS32,       Offset(mColSize,     GuiDynamicCtrlArrayControl));
  addField("rowSize",      TypeS32,       Offset(mRowSize,     GuiDynamicCtrlArrayControl));
  addField("rowSpacing",   TypeS32,       Offset(mRowSpacing,  GuiDynamicCtrlArrayControl));
  addField("colSpacing",   TypeS32,       Offset(mColSpacing,  GuiDynamicCtrlArrayControl));
}

void GuiDynamicCtrlArrayControl::inspectPostApply()
{
   resize(getPosition(), getExtent());
   Parent::inspectPostApply();
}

void GuiDynamicCtrlArrayControl::updateChildControls()
{
   // Prevent recursion
   if(mResizing) 
      return;

   mResizing = true;

   Point2I curPos = getPosition();

   // Calculate the number of columns
   mCols = ( getExtent().x / (mColSize));// + mColSpacing) ) - mColSpacing;
   if(mCols < 1)
      mCols = 1;

   // Place each child...
   S32 childcount = 0;
   S32 rows = 0;
   for(S32 i=0; i<size(); i++)
   {
      // Place control
      GuiControl * gc = dynamic_cast<GuiControl*>(operator [](i));

      if(gc && gc->isVisible()) //  Added check if child is visible.  Invisible children don't take part
      {
         // Get the current column and row...
         S32 curCol = childcount % mCols;
         S32 curRow = childcount / mCols;

         rows = getMax( rows, curRow );

         // Reposition and resize
         Point2I newPos(curCol * (mColSize + mColSpacing), curRow * (mRowSize + mRowSpacing));
         gc->resize(newPos, Point2I(mColSize, mRowSize));

		 childcount++;
	  }
   }

   rows++;

   // Conform our size to the sum of the child sizes.
   curPos.x = getExtent().x;
   curPos.y = rows * (mRowSize + mRowSpacing);
   setHeight( curPos.y );

   mResizing = false;
}

void GuiDynamicCtrlArrayControl::resize(const Point2I &newPosition, const Point2I &newExtent)
{

    Parent::resize(newPosition, newExtent);
   /*if( !Parent::resize(newPosition, newExtent) )
      return false;*/

   // If we don't have any children, return.
    if (mCols < 1 || size() < 1)
        return;

   GuiControl *parent = getParent();
   if (parent)
      parent->childResized(this);
   setUpdate();

   updateChildControls();

   return;
}

void GuiDynamicCtrlArrayControl::addObject(SimObject *obj)
{
   Parent::addObject(obj);

   updateChildControls();
}

void GuiDynamicCtrlArrayControl::childResized(GuiControl *child)
{
   Parent::childResized(child);

   updateChildControls();
}

ConsoleMethod(GuiDynamicCtrlArrayControl, refresh, void, 2, 2, "Forces the child controls to recalculate")
{
   object->updateChildControls();
}
