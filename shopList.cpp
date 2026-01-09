#include <shopList.h>
#include <strTools.h>
#include <editLabel.h>
#include <resizeBuff.h>

#define ITEM_LBL_X	40
#define ITEM_LBL_Y	40
#define ITEM_LBL_W	40
#define ITEM_LBL_H	20

#define CART_LBL_X	160
#define CART_LBL_Y	ITEM_LBL_Y
#define CART_LBL_W	40
#define CART_LBL_H	20

#define NEW_X			49
#define NEW_Y			0

#define EDIT_X			109
#define EDIT_Y			0

#define DELETE_X		169
#define DELETE_Y		NEW_Y

#define NEW_LBL_X		NEW_X + 24
#define NEW_LBL_Y		10
#define NEW_LBL_W		80
#define NEW_LBL_H		12

#define DELETE_LBL_X	DELETE_X + 24
#define DELETE_LBL_Y	NEW_LBL_Y
#define DELETE_LBL_W	NEW_LBL_W
#define DELETE_LBL_H	NEW_LBL_H

#define CLEAR_X		EDIT_X
#define CLEAR_Y		ITEM_LBL_Y
 

// **********************************************************************
// clearCartBtn
// **********************************************************************



clearCartBtn::clearCartBtn(shopList* inApp,const char* path)
	:iconButton(CLEAR_X,CLEAR_Y,path,22) {
	
	ourApp = inApp;
	setEventSet(fullClick);
}

clearCartBtn::~clearCartBtn(void) {  }
	
	
void clearCartBtn::doAction(void) {

	OSPtr->beep();
	if (ourApp) ourApp->clearCart();
}




// **********************************************************************
// shopList
// **********************************************************************


// We create our shopList object and, at least, pass the newAppID into our
// base panel constructor. Then you are free to do whatever you need in here.
shopList::shopList(int newAppID)
  : panel(newAppID) {
  	
  	bool	success;
  	
  	success				= false;									// Let's see if this all works.
	ourAddItemDBox		= NULL;									// Auto delete.
	ourEditItemDBox	= NULL;									// Auto delete.
	checkClear			= NULL;									// Auto delete.
	checkDelete			= NULL;									// Auto delete.
	ourBlockFile		= NULL;									// We delete.
	ourItemMgr			= NULL;									// We delete.
	selectedView		= NULL;									// No allocation.
	colors 				= NULL;									// We delete.
	numObj				= -1;										// Number of cart items. Flagged to force an init.
	colors 				= new pallete();						// All set, start with colors.
	if (colors) {													// Got 'em?
		if (setFilePath("itemFile")) {						// We want this name in our folder.
			ourBlockFile = new blockFile(mFilePath);		// Have a go at creating the block file.
			if (ourBlockFile) {									// Got the block file?
				ourItemMgr = new itemMgr(ourBlockFile);	// Create the item manager.
				if (ourItemMgr) {									// Got that last item?
					success = true;								// We'll call this good.
				}														//
			}															//
		}																//
	}																	//
	if (!success) {												// If not a success..
		Serial.println("Allocation failure!");				// If any are listening, tell 'em.
		close();														// Close up shop. We can't do this.
	}																	//
}


// The world as you know it, is ending. Deallocate what you need to. Don't
// deallocate stuff you've handed to addObj(). Those items will be dealt
// with internally.
shopList::~shopList(void) {

	if (ourBlockFile) delete(ourBlockFile);
	if (ourItemMgr) delete(ourItemMgr);
	if (colors) delete(colors);
}


// setup() & loop() panel style.
void shopList::setup(void) {

	rect			aFrame;
	bmpObj*		aBmp;
	
	colors = new pallete();
	
	aFrame.setRect(ITEM_LBL_X,ITEM_LBL_Y,ITEM_LBL_W,ITEM_LBL_H);
	setFilePath("items.bmp");
	aBmp = new bmpObj(&aFrame,mFilePath);
	addObj(aBmp);
	
	aFrame.setRect(CART_LBL_X,CART_LBL_Y,CART_LBL_W,CART_LBL_H);
	setFilePath("list.bmp");
	aBmp = new bmpObj(&aFrame,mFilePath);
	addObj(aBmp);
	
	setFilePath("back22.bmp");
	clearBtn = new clearCartBtn(this,mFilePath);
	addObj(clearBtn);
	
	addItem = newStdBtn(NEW_X,NEW_Y,icon22,newItemCmd,this);
	addObj(addItem);
	
	editItem = newStdBtn(EDIT_X,EDIT_Y,icon22,editCmd,this);
	editItem->setActive(false);
	addObj(editItem);
	
	delItem = newStdBtn(DELETE_X,DELETE_Y,icon22,deleteItemCmd,this);
	delItem->setActive(false);
	addObj(delItem);
	
	aFrame.setRect(ITEMS_X,ITEMS_Y,ITEMS_W,ITEMS_H);
	ourItemList = new itemList(&aFrame);
	addObj(ourItemList);
	
	aFrame.setRect(CART_X,CART_Y,CART_W,CART_H);
	ourCartList = new cartList(&aFrame);
	addObj(ourCartList);
	
	if (ourItemMgr->begin(ourItemList,ourCartList)) {
		ourItemMgr->populateLists();
	}
}


void shopList::setItemIcons(bool addActive,bool editActive,bool delActive) {

		addItem->setActive(addActive);
		editItem->setActive(editActive);
		delItem->setActive(delActive);
}


void shopList::handleCom(stdComs comID) {
	
	switch(comID) {
		case newItemCmd		:
			if (ourAddItemDBox) {
				ourItemMgr->addNewItem(ourAddItemDBox->getName());
			} else {
				ourAddItemDBox = new addItemDBox(this);
				setItemIcons(false,false,false);
			}
		break;
		case deleteItemCmd	:
			checkDelete = new deleteOkAlert(this);
		break;
		case editCmd			:
			if (selectedView) {
				ourEditItemDBox = new editItemDBox(this,selectedView->getItemName());
				if (ourEditItemDBox) {
					setItemIcons(false,false,false);
				}
			}
		break;	
		case okCmd 				:
			if (ourAddItemDBox) {
				ourItemMgr->addNewItem(ourAddItemDBox->getName());
				setItemIcons(true,false,false);
				ourAddItemDBox = NULL;
			} else if (ourEditItemDBox){
				selectedView->setItemName(ourEditItemDBox->getName());
				ourItemMgr->saveSelected(selectedView);
				setItemIcons(true,true,true);
				ourAddItemDBox = NULL;
			} else if (checkClear) {
				ourItemMgr->clearCart();
				checkClear = NULL;
				setFocusPtr(NULL);
				setItemIcons(true,false,false);
			} else if (checkDelete) {
				checkDelete = NULL;
				ourItemMgr->deleteItem(selectedView);
				setItemIcons(true,false,false);
			}	
		break;
		case cancelCmd			:
			if (ourAddItemDBox) {
				setItemIcons(true,false,false);
				ourAddItemDBox = NULL;
			} else if (ourEditItemDBox){
				setItemIcons(true,true,true);
				ourEditItemDBox = NULL;
			} else if (checkClear) {
				checkClear = NULL;
				setFocusPtr(NULL);
				setItemIcons(true,false,false);							
			} else if (checkDelete) {
				checkDelete = NULL;
			}
		break;
		default					:
			//Serial.print("Seeing comID ");
			//Serial.println((int)comID);
			panel::handleCom(comID);
		break;
	}
}


// Being told that this view has focus.
void shopList::selected(itemView* aView) {
	
	selectedView = aView;
	if (selectedView) {
		setItemIcons(true,true,true);
	} else {
		setItemIcons(true,false,false);
	}
}


void shopList::clearCart(void) { checkClear = new clearOkAlert(this); }


void shopList::loop(void) {

	if (ourCartList->numObjects()!=numObj) {
		numObj = ourCartList->numObjects();
		clearBtn->setActive(numObj>0);
	}
}


// The default here is to not draw ourselves. You can change that.
void shopList::drawSelf(void) {

	colorObj	lineColor(LC_GREY);
	
	screen->fillScreen(&colors->dispBackColor);
	screen->drawVLine(120,ITEM_LBL_Y+40,205,&lineColor);
}


// Whereas you call close() when you are complete and want to close.. Sometimes
// you don't have control of that. So? No matter if you call close, or something
// else calls close on you, this gets called so you can clean up before being
// deleted.
void shopList::closing(void) {  }
