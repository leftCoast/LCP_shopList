#include <shopListDB.h>
#include <shopList.h>
//#include <debug.h>


frame::frame(rect* inRect)
	:drawObj(inRect) {  }


frame::~frame(void) {  }

				
void frame::drawSelf(void) { screen->drawRect(this,&black); }
					
// **************************************************************
// **********************   addItemDBox    **********************
// **************************************************************

#define ADD_ITEM_MSG	"Add items"
#define EDIT_MSG	""	
						
addItemDBox::addItemDBox(listener* inListener)
	: alertObj(ADD_ITEM_MSG,inListener,noteAlert,false,false),
	kbdUser(this) {
	
	theMsg->x += 4;															// Kick the default message over a bit.
	y += 50;																		// Move our alertBox down a bit.
	
	editRect.setRect(52,40,75,10);										// Setup location for the edit box.
	nameField = new editLabel(&editRect,EDIT_MSG);					// Create it.
	nameField->setColors(&black,&white);								// Set colors
	setEditField(nameField);												// We also manage the keyboard. So, edit this.
	addObj(nameField);														// Attach the edit field to our list.
	
	frame*	editFrame = new frame(nameField);						// Using the edit field rect create a frame.
	editFrame->insetRect(-3);												// Expand it by 3 pixels per side.
	addObj(editFrame);														// And add that to our list.
	
	addAnother = newStdBtn(140,34,icon22,newItemCmd,this);		// Create the "add another" button.
	addAnother->setActive(false);											// Not active as yet..
	addObj(addAnother);														// Add the button to our list.
	hookup();																	// Hook into the idler queue.
}
	
	
addItemDBox::~addItemDBox(void) {  }


const char*	addItemDBox::getName(void) { return nameField->getTextBuff(); }


void addItemDBox::handleCom(stdComs comID) {

	alertObj::handleCom(comID);
	if (comID==newItemCmd) {
		nameField->clearStr();
	}
}


void addItemDBox::idle(void) {
	
	alertObj::idle();
	if (nameField->getNumChars()>0 && !addAnother->getActive()) {
		addAnother->setActive(true);
	} else if (nameField->getNumChars()==0 && addAnother->getActive()) {
		addAnother->setActive(false);
	}
}
	