#include <shopListItems.h>
#include <shopList.h>


// Colors & fonts.. CHANGE TO PALLETTE CLASS

colorObj SLBackColor;
colorObj	SLDefTextColor;
colorObj	SLEditTextColor;
colorObj	focusBack(LC_CHARCOAL);

// **********************************************************************
// IDList
// **********************************************************************



IDList::IDList(blockFile* aBLockfile) {

	IDBuff = NULL;
	ourBlockFile = aBLockfile;
}


IDList::~IDList(void) { resizeBuff(0,(void**)&IDBuff); }


// Finds and returns the size of the Root block, in bytes.
unsigned long IDList::getNumRootBytes(void) {

	unsigned long	rootID;
	
	rootID = ourBlockFile->readRootBlockID();			// Grab root ID.
	if (rootID) {												// We got one?
		return ourBlockFile->getBlockSize(rootID);	// return the num bytes.
	}																//
	return 0;													// Guess we didn't have one.
}


// Calculates and returns the number of items we're storing.	
int IDList::getNumItems(void) {
	
	unsigned long	numBytes;
	
	numBytes = getNumRootBytes();						// get the number of bytes..
	if (numBytes) {										// If we have some..
		return numBytes/sizeof(unsigned long);		// Calculate and return the number of items.
	}															//
	return 0;												// Or none..
}


// Allocates and populates our IDBuff of item IDs.
bool IDList::getList(bool plusOne) {
	
	unsigned long	rootID;
	unsigned long	numBytes;
	unsigned long	allocate;
	bool				success;
	
	success = false;																		// We ain't a success yet.
	numBytes = getNumRootBytes();														// Get the number of items we have.
	allocate = numBytes;																	// Allocate set to numBytes. We need both.
	if (plusOne) {																			// If plus one..
		allocate = allocate + sizeof(unsigned long);								// Bump up allocate. Bigger buffer than needed.
	}																							//
	if (resizeBuff(allocate,(void**)&IDBuff)) {									// Resize the IDBuff to the allocation byes value.
		rootID = ourBlockFile->readRootBlockID();									// Grab root ID.
		success = ourBlockFile->getBlock(rootID,(char*)IDBuff,numBytes);	// Have a go at grabbing the buffer.
		success = success || plusOne;													// If first one? Then it's ok.
		if (!success) {																	//	If we failed..
			resizeBuff(0,(void**)&IDBuff);											// Recycle the buffer we allocated.
		}																						//
	}																							//
	return success;																		// Return our result.
}


// Allocates and ID number, adds this new value to the rootID block
// nd saves the block to file.
unsigned long IDList::addItem(void) {
	
	unsigned long 	newID;
	unsigned long 	rootID;
	unsigned long	numBytes;
	int				numItems;
	unsigned long 	result;
	
	result = 0;																				// A new ID can not be zero.
	if (ourBlockFile->isEmpty()) {													// If it's an empty file..
		rootID = ourBlockFile->getNewBlockID();									// First value is rootID.
	} else {																					// Oh, we got a file with stuff?
		rootID = ourBlockFile->readRootBlockID();									// Just in case..
	}																							//
	numItems = 	getNumItems();															// Get an item count.
	if (getList(true)) {																	// If we can get our list + 1..
		newID = ourBlockFile->getNewBlockID();										// Create the newID.
		IDBuff[numItems] = newID;														// Poke the new ID into the end.
		numItems++;																			// Bump up the number of items.
		numBytes = numItems * sizeof(unsigned long);								// Update the numbBytes value.
		if (ourBlockFile->writeBlock(rootID,(char*)IDBuff,numBytes)) {		// If we can updating the file.
			result = newID;																// Everything worked? Set result.
		}																						//
		resizeBuff(0,(void**)&IDBuff);												// Recycle the buffer we allocated.
	}																							//
	return result;																			// Return our result.
}


// If this can locate the delivered ID, it will delete it from our
// rootID block and save the change. Make sure to delette the actual
// data block in question first. The does NOT do that.
bool IDList::removeItem(unsigned long oldID) {
	
	unsigned long	rootID;
	int				numItems;
	unsigned long	numBytes;
	bool				foundIt;
	
	foundIt = false;																	// Ain't found anything yet.
	numItems = getNumItems();														// Grab number of items.
	if (numItems) {																	// If we got items.
		if (getList()) {																// And we got the list.
			for(int i=0;i<numItems;i++) {											// For every item..
				if (IDBuff[i]==oldID) {												// If we found it..
					foundIt = true;													// Note it.
				}																			// pop
				if (foundIt) {															// Again, if we found it..
					IDBuff[i] = IDBuff[i+1];										// copy the next item into this one.
				}																			// The last will get gibberish, but don't worry.
			}																				// It's going to be chopped off.
			if (foundIt) {																// If we found it..
				rootID = ourBlockFile->readRootBlockID();						// Grab root ID.
				numBytes = (numItems-1) * sizeof(unsigned long);			// Update the numbBytes value.
				ourBlockFile->writeBlock(rootID,(char*)IDBuff,numBytes);	// Stuff back the buffer.
			}																				// pop
		}																					// pop
	}																						// pop
	return foundIt;																	// Let 'em know if we found one.
}



// **********************************************************************
// itemMgr - The interface for the App.
// **********************************************************************



itemMgr::itemMgr(blockFile* aBlockFile)
	: IDList(aBlockFile) { ourBlockFile = aBlockFile; }
	
	
itemMgr::~itemMgr(void) { }

	
bool itemMgr::begin(itemList* inIList,cartList* inCList) {

	iList = inIList;
	cList = inCList;
	return (iList && cList);
}


void itemMgr::populateLists(void) {
	
	int			numItems;
	item			anItem;
	itemView*	newView;
	
	numItems = getNumItems();										// How many items we got here?
	if (numItems && getList()) {									// We got items and a list?
		for(int i=0;i<numItems;i++) {								// For each one
			if (readItem(IDBuff[i],&anItem)) {					// If we get the data from the file..
				newView = new itemView(IDBuff[i],&anItem);	// Create the view Item..
				if (newView) {											// Got one?
					newView->addToList();							// Tell it to go add itself to a list.
				}															//
			}																//
		}																	//
	}																		//
}


void itemMgr::addNewItem(const char* name) {
	
	item				newItem;
	unsigned long	newID;
	itemView*		newItemView;
	
	if (name) {																							// We got a name?
		if (strlen(name)<MAX_NAME_BYTES) {														// Name will fit?
			newItem.version = ITEM_VERSION;														// Set version num.														
			strcpy(newItem.itemName,name);														// Copy in new name.
			newItem.timesListed = 1;																// Probably go to cart list.
			newItem.state = listed;																	// We actually control that.
			newID = addItem();																		// Add item to root list.
			if (newID) {																				// Non zero ID means success.
				if (ourBlockFile->writeBlock(newID,(char*)&newItem,sizeof(item))) {	// Save the result.
					newItemView = new itemView(newID,&newItem);								// Create the new item.
					if (newItemView) {																// Got one?
						newItemView->addToList();													// Tell it to go add itself to a list.
					}																						// *pop*
				}																							// *pop*
			}																								// *pop*
		}																									// *pop*
	}																										// *pop*
}


void itemMgr::deleteItem(itemView* oldView) {

	shopList*	ourApp;
	
	if (oldView) {													// Sanity, make sure we have one.
		ourApp = (shopList*)ourPanel;							// Need pointer to our application.
		setFocusPtr(NULL);										// Remove it from focus.
		oldView->unhook();										// Pull the poor thing from it's list.
		if (oldView->ourItem.state==suggested) {			// If it was in the item list..
			ourApp->ourItemList->setPositions();			// Close up the gap.
			ourApp->ourItemList->setNeedRefresh();			// Redraw everything without it.
		} else {														// Else, cart list?
			ourApp->ourCartList->setPositions();			// Close up the gap there.
			ourApp->ourCartList->setNeedRefresh();			// And redraw everything without it.
		}																//
		removeItem(oldView->ourItemID);						// Pull it's ID from the root list.
		ourBlockFile->deleteBlock(oldView->ourItemID);	// Pull it's data block.
		delete(oldView);											// Recycle it.
	}																	// Man! Talk about "Erased from existence!"
}


void itemMgr::clearCart(void) { cList->clearCart(); }


bool itemMgr::readItem(unsigned long itemID,item* anItem) {

	if (anItem) {					// Behave, no writing to NULL.
		return ourBlockFile->getBlock(itemID,(char*)anItem,sizeof(item));
	}
	return false;
}
				
				
void itemMgr::saveItem(unsigned long itemID,item* anItem) {

	ourBlockFile->writeBlock(itemID,(char*)(anItem),sizeof(item));
}


// **********************************************************************
// itemView - The actual item that the user sees with controls on it.
// **********************************************************************
	
	
// Give us an item and an ID to store it to? We're good.
itemView::itemView(unsigned long itemID,item* anItem)
	: drawGroup(0,0,LIST_W,ITEM_H) {
	
	if (itemID && anItem) {			// Sanity. Valid inputs?
		ourItemID = itemID;			// Copy the ID.
		ourItem = *anItem;			// Copy the item info.
		setupView();					//	Call setup to build our draw group.
		setEventSet(touchNDrag);	// We want the fancy event set.
	}										//
}
	

// We should always be current in the file, so, nothing to do here.
itemView::~itemView(void) {  }


void itemView::setupView(void) {
	
	if (ourItem.version) {																			// Sanity. Non zero
		name = new label(IV_NAME_X,IV_NAME_Y,IV_NAME_W,IV_NAME_H,ourItem.itemName);
		name->setColors(&yellow);
		name->setTextSize(1);
		addObj(name);
	}
}


void itemView::doAction(event* inEvent,point* localPt) {

	float	angle;
	
	if (inEvent->mType==touchEvent) {
		if (haveFocus()) {
			setFocusPtr(NULL);
		} else {
			setFocusPtr(this);
		}
	} else if (inEvent->mType==liftEvent) {
		if (inEvent->mDist>20) {
			angle = inEvent->mAngle;
			if (angle > -1 && angle < 1) {
				changeState(listed);
			} else if (angle > M_PI-1 && angle < M_PI+1) {
				changeState(suggested);
			}
		}
	}
}


void  itemView::setThisFocus(bool setLoose) {
	
	shopList*	ourApp;
	
	ourApp = (shopList*)ourPanel;
	drawObj::setThisFocus(setLoose);	// Do the normal stuff..
	if (setLoose) {
		ourApp->selected(this);
	} else {
		ourApp->selected(NULL);
	}
}


// This looks at the state value and from that calculates where we "should" be.
drawList* itemView::calcualteOurList(void) {

	shopList*	ourApp;
	
	ourApp = (shopList*)ourPanel;
	if (ourItem.state==suggested) {
		return ourApp->ourItemList;
	}
	return ourApp->ourCartList;
}

	
// We change state value here and this'll move us to the correct list once changed.
void itemView::changeState(itemStates newState) {
	
	shopList*	ourApp;
	
	ourApp = (shopList*)ourPanel;
	ourItem.state = newState;
	ourApp->ourItemMgr->saveItem(ourItemID,&ourItem);
	addToList();
	setFocusPtr(NULL);
}
	

// This will add us to the correct list according to our state value.
void itemView::addToList(void) {

	shopList*	ourApp;
	
	ourApp = (shopList*)ourPanel;
	unhook();
	calcualteOurList()->addObj(this);
	ourApp->ourItemList->setPositions();
	ourApp->ourItemList->setNeedRefresh();
	ourApp->ourCartList->setPositions();
	ourApp->ourCartList->setNeedRefresh();
}


// We don't want to draw stuff where it ain't wanted.
void itemView::itemView::draw(void) {

	if (calcualteOurList()->isVisible(this)) {
		drawGroup::draw();
	}
}


void itemView::drawSelf(void) {

	rect			aFrame(this);
	
	aFrame.insetRect(1);
	if (haveFocus()) {
		screen->fillRect(&aFrame,&focusBack);
	} else {
		screen->fillRect(&aFrame,&black);
	}
}


// **********************************************************************
// itemList
// **********************************************************************


itemList::itemList(rect* frame)
	: drawList(frame) {  }

	
itemList::~itemList(void) {  }


void itemList::drawSelf(void) {

	screen->fillRect(this,&black);
	screen->drawRect(this,&white);
}



// **********************************************************************
// cartList
// **********************************************************************


cartList::cartList(rect* frame)
	: drawList(frame) {  }


cartList::~cartList(void) {  }


void  cartList::clearCart(void) {

	itemView*	aView;
	
	do {
		aView = (itemView*)getObj(0);
		if (aView) {
			aView->changeState(suggested);
		}
	} while(aView);
	setNeedRefresh();
}


void cartList::drawSelf(void) {

	screen->fillRect(this,&black);
	screen->drawRect(this,&white);
}


/*
void showItem(item* anItem) {

	Serial.println("********* item *********");
	Serial.print("Item name :\t");
	Serial.println(anItem->itemName);
	Serial.print("Version num :\t");
	Serial.println(anItem->version);
	Serial.print("Times listed :\t");
	Serial.println(anItem->timesListed);
	Serial.print("Item state :\t");
	switch(anItem->state) {
		case suggested	: Serial.println("Sugested");	break;
		case listed		: Serial.println("listed");	break;
		case grabbed	: Serial.println("In cart");	break;
	}
	Serial.println("************************");
	Serial.println();
};
*/	
