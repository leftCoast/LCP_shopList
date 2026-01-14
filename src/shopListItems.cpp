#include <shopListItems.h>
#include <shopList.h>
//#include <debug.h>



// **********************************************************************
// pallete - Colors for this and that.
// **********************************************************************


pallete::pallete(void) { setupColors(); }


pallete::~pallete(void) {  }
	
void pallete::setupColors(void) {
	
	dispBackColor.setColor(&black);
	focusBackColor.setColor(LC_CHARCOAL);
	outlineColor.setColor(&white);
	listTextColor.setColor(&yellow);
	strikeTextColor.setColor(&yellow);
	strikeTextColor.blend(&white,25);
	strikeTextColor.blend(&black,50);
	editTextColor.setColor(&black);
	labelTextColor.setColor(&white);
}


pallete* colors = NULL;



// **********************************************************************
// STLabel - Adafruit text than can have a strike through line added.
// **********************************************************************


				
STLabel::STLabel(rect* inRect,const char* inText,int textSize)
	: label(inRect->x,inRect->y,inRect->width,inRect->height,inText,textSize) {

	strike = false;			// Off to start with.
	setStrikeColor(&red);	// Good default.
	txtX = 0;
	txtY = 0;
	textWidth = 0;
}


STLabel::STLabel(int inLocX, int inLocY, int inWidth,int inHeight,const char* inText,int textSize)
	: label(inLocX,inLocY,inWidth,inHeight,inText,textSize) {
	
	strike = false;			// Off to start with.
	setStrikeColor(&red);	// Good default.
	txtX = 0;
	txtY = 0;
	textWidth = 0;
}


STLabel::~STLabel(void) {  }

	
void STLabel::setStrikeColor(colorObj* aColor) {

	strikeColor.setColor(aColor);
	setNeedRefresh();
}


void STLabel::setStrike(bool onOff) {

	strike = onOff;
	setNeedRefresh();
}


// Call right as your going to print the text out.
void  STLabel::calcStrike(void) {

	if (strike) {
		txtX = screen->getCursorX()+x;	
		txtY = screen->getCursorY()+ y + (CHAR_HEIGHT * textSize)/2;
		textWidth = getTextWidth();			// Calculate how wide the line may be.
		if (width<textWidth) {
			textWidth = width;
		}
	}
}


// Call right after.	
void STLabel::doStrike(void) {

	if (strike) {
		screen->drawHLine(txtX,txtY,textWidth,&strikeColor);
	}
}


void STLabel::drawSelf(void) {

	int	charDif; 
   int	numCharsDisp;
	char*	temp;
	
	if (!strike) {																		// If we don't want the strike through..
		label::drawSelf();															// Just let them do it.
		return;																			// And bolt!
	}																						// really bad design. Tn hurry.
	if (buff) {																			// Sanity. If we have anything to display..
		temp = NULL;
		numCharsDisp = getViewChars();											// Save off how many chars we can display.
		if (transp) {																	// If we don't draw background bits..
			screen->setTextColor(&textColor);									// We just set the one color as a flag for this.
		} else {																			// Else, we DO want to draw the background bits.
			screen->setTextColor(&textColor,&backColor);						// We set BOTH colors as a flag for both.
		}
		screen->setTextSize(textSize);											// Set the text size.
		screen->setTextWrap(false);												// Turn off wrap. It just makes a mess of everything.
		screen->setCursor(x,y);														// Move the cursor to our XY location.
		charDif =  numCharsDisp - strlen(buff);								// Calculate the total amount of blanks we'll need.
		if (charDif==0) {																// If its a perfect fit..
			calcStrike();
			screen->drawText(buff);													// Just draw it out. EASY PEASY! (And we're done)
			doStrike();
		} else if (charDif>0) {														// Else, we'll need some padding..
			textWidth = getTextWidth();
			switch (justify) {														// Left, Right, Center will be handled differently.
				case TEXT_LEFT :														// ** LEFT **
					calcStrike();
					screen->drawText(buff);											// Draw the text.
					doStrike();
					for(int i=1;i<=charDif;i++) screen->drawText(" ");		// Add the padding.
				  break;																	// And that's it.
				case TEXT_RIGHT :														// ** RIGHT **
				  for(int i=1;i<=charDif;i++) screen->drawText(" ");		// First we add the padding.
				  calcStrike();
				  screen->drawText(buff);											// Then draw the text.
				  doStrike();
				  break;																	// And we're done.
				case TEXT_CENTER :													// ** CENTER **
					int leadSp = charDif/2;											// Do integer divide by 2 for the lead blank count.
					int trailSp = charDif-leadSp;									// Subtract the lead count from the blank count for the trailing count.
				  for(int i=1;i<=leadSp;i++) screen->drawText(" ");		// Add the lead padding.
				  calcStrike();
				  screen->drawText(buff);											// Draw the string.
				  doStrike();
				  for(int i=1;i<=trailSp;i++) screen->drawText(" ");		// Add the trailing padding.
				  break;																	// And again, we're done.
				}
		} else {                      											// Else the string needs truncation..
			if (numCharsDisp<(TEMP_BUFF_SIZE-1)) {								// Make sure we have enough room to maneuver.
				textWidth = width;
				if (resizeBuff(TEMP_BUFF_SIZE,&temp)) {
					calcStrike();
					switch (justify) {													// Left, Right, Center will be handled differently.
						case TEXT_LEFT :													// ** LEFT **
							temp[0] = '\0';												// Clear the temp buffer.
							strncat(temp,buff,numCharsDisp);							// Stamp in the characters to display.
							screen->drawText(temp);										// Draw the string.
						break;																// And we're done.
						case TEXT_RIGHT :													// ** RIGHT**						
							screen->drawText((char*)&(buff[-charDif])); 			// Just draw the text starting after the clipped portion.
						break;
						case TEXT_CENTER :												// ** CENTER **
							int firstChar = -charDif/2;								// Calculate first char to display.
							temp[0] = '\0';												// "Clear" the temp buffer.
							strncat(temp,&buff[firstChar],numCharsDisp);			// Stamp in the characters to display.
							screen->drawText(temp);										// Draw the string.
						break;																// And we're done.
					}
					doStrike();
					resizeBuff(0,&temp);
				}
			} else {
				screen->drawText("Overflow");										// Our temp buffer is just too small!
			}
		}
	}
}



// **********************************************************************
// itemView - The actual item that the user sees with controls on it.
// **********************************************************************
	
	
// Give us an item and an ID to store it to? We're good.
itemView::itemView(unsigned long itemID,item* anItem)
	: drawGroup(0,0,LIST_W,ITEM_H) {
	
	if (itemID && anItem) {				// Sanity. Valid inputs?
		ourItemID = itemID;				// Copy the ID.
		ourItem = *anItem;				// Copy the item info.
		setupView();						//	Call setup to build our draw group.
		setEventSet(touchNDrag);		// We want the fancy event set.
		scrolling = false;				// Ain't scrolling yet.
	}											//
}
	

// We should always be current in the file, so, nothing to do here.
itemView::~itemView(void) {  }


void itemView::setupView(void) {
	
	if (ourItem.version) {																			// Sanity. Non zero
		name = new STLabel(IV_NAME_X,IV_NAME_Y,IV_NAME_W,IV_NAME_H,ourItem.itemName);
		name->setColors(&colors->listTextColor);
		name->setTextSize(1);
		name->setJustify(TEXT_LEFT);
		addObj(name);
	}
}
	

void itemView::doAction(event* inEvent,point* localPt) {

	dragType	dragDir;
	point		listPt;
	
	if (inEvent->mType==touchEvent) {
		if (haveFocus()) {
			setFocusPtr(NULL);
		} else {
			setFocusPtr(this);
		}
	} else if (inEvent->mType==dragBegin) {
		dragDir = dDirection(inEvent->mAngle,DRAG_TOL);
		if (dragDir==dragUp||dragDir==dragDn) {
			setFocusPtr(NULL);
			scrolling = true;
			listPt.x = localPt->x - x;
			listPt.y = localPt->y - y;
			calcualteOurList()->doAction(inEvent,&listPt);
		}
	}  else if (inEvent->mType==dragOn) {							// Or else, we're actually dragging..
		if (scrolling) {
			listPt.x = localPt->x - x;
			listPt.y = localPt->y - y;
			calcualteOurList()->doAction(inEvent,&listPt);
		}
	} else if (inEvent->mType==liftEvent) {							// Or else, we're finished dragging..
	 	if (scrolling) {
	 		listPt.x = localPt->x - x;
			listPt.y = localPt->y - y;
			calcualteOurList()->doAction(inEvent,&listPt);
			scrolling = false;
		} else {
			dragDir = dDirection(inEvent->mAngle,DRAG_TOL);
			if (inEvent->mDist>20) {
				if (dragDir==dragRight) {
					changeState(listed);
				} else if (dragDir==dragLeft) {
					changeState(suggested);
				}
			} else if (ourState()==listed) {
				changeState(grabbed);
			}  else if (ourState()==grabbed) {
				changeState(listed);
			}
		}
	}
} 	


// This is called when OUR focus changes. Usually used to cause a redraw to show the
// change.
void  itemView::setThisFocus(bool setLoose) {
	
	shopList*	ourApp;
	
	ourApp = (shopList*)ourPanel;
	drawObj::setThisFocus(setLoose);	// Do the normal stuff.. ('Causes subsequent redraw)
	if (setLoose) {
		ourApp->selected(this);
	} else {
		ourApp->selected(NULL);
	}
}


// Call this to change our item's name.
void itemView::setItemName(const char* aName) {
	
	strncpy(ourItem.itemName,aName,MAX_NAME_BYTES);
	ourItem.itemName[MAX_NAME_BYTES-1] = '\0';
	name->setValue(ourItem.itemName);
	setNeedRefresh();
}


// Call this to read out our item's name.
char* itemView::getItemName(void) { return ourItem.itemName; }


// This looks at the state value and from that, calculates where we "should" be.
scrollingList* itemView::calcualteOurList(void) {
	
	shopList*	ourApp;
	
	ourApp = (shopList*)ourPanel;
	if (ourState()==suggested) {
		return ourApp->ourItemList;
	}
	return ourApp->ourCartList;
}


// Mostly a typing aid. Also, now you don't need remember where to look it up anymore.
itemStates itemView::ourState(void) { return ourItem.state; }

	
// We change state value here and this'll move us to the correct list once changed.
void itemView::changeState(itemStates newState) {
		
	shopList*	ourApp;
	
	if (ourItem.state==suggested && newState!=suggested) {
		ourItem.timesListed++;
	}
	ourItem.state = newState;
	setFocusPtr(NULL);
	addToList();
	ourApp = (shopList*)ourPanel;
	ourApp->ourItemMgr->saveItem(ourItemID,&ourItem);
	ourApp->ourItemList->sortList();
	ourApp->ourCartList->sortList();
}
	
	
int itemView::numListings(void) { return ourItem.timesListed; }


// This will add us to the correct list according to our state value.
void itemView::addToList(void) {
	
	shopList*	ourApp;
	
	ourApp = (shopList*)ourPanel;
	unhook();
	switch(ourState()) {
		case suggested :
			name->setColors(&colors->listTextColor);
			name->setStrike(false);
			ourApp->ourItemList->addObj(this);
		break;
		case listed :
			name->setColors(&colors->listTextColor);
			name->setStrike(false);
			ourApp->ourCartList->addObj(this);
		break;
		case grabbed :
			name->setColors(&colors->strikeTextColor);
			name->setStrike(true);
			ourApp->ourCartList->addObj(this);
		break;
	}
}


bool itemView::isGreaterThan(dblLinkListObj* compObj) {

	itemView* otherObj;
	
	otherObj = (itemView*)compObj;
	if (compObj) {
		if (ourState() == otherObj->ourState()) {				// If the states match..
			return numListings() > otherObj->numListings();	// We go on the number times listed.
		} else if (ourState()==suggested) {						// So, we have suggested and they can not..
			return true;												// Then we are greater.
		} else if (ourState()==listed) {							// Else we have listed and they can not..
			return otherObj->ourState()==grabbed;				// Return true if their state is grabbed.
		}																	//
		return false;													// The last possible case is us listed and them suggested.
	}
	return false;
}


bool itemView::isLessThan(dblLinkListObj* compObj) {
	
	itemView* otherObj;
	
	otherObj = (itemView*)compObj;
	if (compObj) {
		if (ourState() == otherObj->ourState()) {				// If the states match..
			return numListings() < otherObj->numListings();	// We go on the number times listed.
		} else if (ourState()==suggested) {						// So, we have suggested and they can not..
			return false;												// Then we are greater.
		} else if (ourState()==listed) {							// Else we have listed and they can not..
			return otherObj->ourState()==suggested;			// Return true if their state is listed.
		}																	//
		return true;													// The last possible case is us listed and them suggested.
	}
	return false;
}


// We don't want to draw stuff where it ain't wanted.
void itemView::itemView::draw(void) {

	if (calcualteOurList()->isVisible(this)) {
		drawGroup::draw();
	}
	needRefresh = false;					  // But in all cases. We no longer need to be drawn.		
}


void itemView::drawSelf(void) {

	rect			aFrame(this);
	
	aFrame.insetRect(1);
	if (haveFocus()) {
		screen->fillRect(&aFrame,&colors->focusBackColor);
	} else {
		screen->fillRect(&aFrame,&colors->dispBackColor);
	}
}

	
	
// **********************************************************************
// itemList
// **********************************************************************


itemList::itemList(rect* frame)
	: scrollingList(frame->x,frame->y,frame->width,frame->height,touchScroll,dragEvents) {  }

	
itemList::~itemList(void) {  }


void itemList::sortList(void) {

	listHeader.sortTail(false);
	setPositions();
	setNeedRefresh();
}


void itemList::drawSelf(void) { screen->fillRect(this,&colors->dispBackColor); }



// **********************************************************************
// cartList
// **********************************************************************


cartList::cartList(rect* frame)
	: scrollingList(frame->x,frame->y,frame->width,frame->height,touchScroll,dragEvents) {  }


cartList::~cartList(void) {  }


void cartList::sortList(void) {
	
	listHeader.sortTail(false);
	setPositions();
	setNeedRefresh();
}

	
void cartList::clearCart(void) {

	itemView*	aView;
	
	do {
		aView = (itemView*)getObj(0);
		if (aView) {
			aView->changeState(suggested);
		}
	} while(aView);
	setNeedRefresh();
}


void cartList::drawSelf(void) { screen->fillRect(this,&colors->dispBackColor); }

	
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
		success = ourBlockFile->getBlock(rootID,(uint8_t*)IDBuff,numBytes);	// Have a go at grabbing the buffer.
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
		if (ourBlockFile->writeBlock(rootID,(uint8_t*)IDBuff,numBytes)) {		// If we can updating the file.
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
				ourBlockFile->writeBlock(rootID,(uint8_t*)IDBuff,numBytes);	// Stuff back the buffer.
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
		cList->sortList();											// All filled. Let's get them sorted.
		iList->sortList();											// This one too.
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
				if (ourBlockFile->writeBlock(newID,(uint8_t*)&newItem,sizeof(item))) {	// Save the result.
					newItemView = new itemView(newID,&newItem);								// Create the new item.
					if (newItemView) {																// Got one?
						newItemView->addToList();													// Tell it to go add itself to a list.
						cList->sortList();																//
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


void itemMgr::clearCart(void) {

	cList->clearCart();
	iList->sortList();
}


bool itemMgr::readItem(unsigned long itemID,item* anItem) {

	if (anItem) {					// Behave, no writing to NULL.
		return ourBlockFile->getBlock(itemID,(uint8_t*)anItem,sizeof(item));
	}
	return false;
}
				
				
void itemMgr::saveItem(unsigned long itemID,item* anItem) {

	ourBlockFile->writeBlock(itemID,(uint8_t*)(anItem),sizeof(item));
}


void itemMgr::saveSelected(itemView* selected) {

	if (selected) {
		saveItem(selected->ourItemID,&(selected->ourItem));
	}
}
		





