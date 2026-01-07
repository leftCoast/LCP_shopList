#include <shopListDB.h>
#include <shopList.h>
//#include <debug.h>


#define ADD_ITEM_MSG		"Add huge items"
#define ADD_NAME_MSG		""
#define EDIT_ITEM_MSG	"Have a go at changing an item."



frame::frame(rect* inRect)
	:drawObj(inRect) {  }


frame::~frame(void) {  }

				
void frame::drawSelf(void) { screen->drawRect(this,&black); }
		
		
					
// **************************************************************
// **********************   addItemDBox    **********************
// **************************************************************
	
						
addItemDBox::addItemDBox(listener* inListener)
	: alertObj(ADD_ITEM_MSG,inListener,noteAlert,false,false),
	kbdUser(this) {
	
	rect	editRect;
	
	theMsg->x += 4;															// Kick the default message over a bit.
	y += 50;																		// Move our alertBox down a bit.
	editRect.setRect(52,40,75,10);										// Setup location for the edit box.
	nameField = new editLabel(&editRect,ADD_NAME_MSG);				// Create it.
	nameField->setColors(&black,&white);								// Set colors
	setEditField(nameField);												// We also manage the keyboard. So, edit this.
	addObj(nameField);														// Attach the edit field to our list.
	
	frame*	editFrame = new frame(nameField);						// Using the edit field rect create a frame.
	editFrame->insetRect(-3);												// Expand it by 3 pixels per side.
	addObj(editFrame);														// And add that to our list.
	
	hookup();																	// Hook into the idler queue.
}
	
	
addItemDBox::~addItemDBox(void) {  }


const char*	addItemDBox::getName(void) { return nameField->editBuff; }


void addItemDBox::handleCom(stdComs comID) {

	alertObj::handleCom(comID);
	if (comID==newItemCmd) {
		nameField->clearStr();
	}
}


void addItemDBox::idle(void) {
	
	alertObj::idle();
	/*
	if (nameField->getNumChars()>0 && !addAnother->getActive()) {
		addAnother->setActive(true);
	} else if (nameField->getNumChars()==0 && addAnother->getActive()) {
		addAnother->setActive(false);
	}
	*/
}


// **************************************************************
// **********************   editItemDBox    **********************
// **************************************************************

						
editItemDBox::editItemDBox(listener* inListener,const char* inName)
	: alertObj(EDIT_ITEM_MSG,inListener,noteAlert,false,false),
	kbdUser(this) {
	
	rect	editRect;
	
	theMsg->x += 4;															// Kick the default message over a bit.
	y += 50;																		// Move our alertBox down a bit.
	editRect.setRect(52,40,75,10);										// Setup location for the edit box.
	nameField = new editLabel(&editRect,inName);						// Create it.
	nameField->setColors(&black,&white);								// Set colors
	setEditField(nameField);												// We also manage the keyboard. So, edit this.
	addObj(nameField);														// Attach the edit field to our list.
	
	frame*	editFrame = new frame(nameField);						// Using the edit field rect create a frame.
	editFrame->insetRect(-3);												// Expand it by 3 pixels per side.
	addObj(editFrame);														// And add that to our list.
	
	hookup();																	// Hook into the idler queue.
}
	
	
editItemDBox::~editItemDBox(void) {  }


//void	editItemDBox::setName(const char* itemName) { nameField->setValue(itemName); }


const char*	editItemDBox::getName(void) { return nameField->editBuff; }


	