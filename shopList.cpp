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

#define CLEAR_X		120-11
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
  	
	ourAddItemDBox	= NULL;
	ourBlockFile	= NULL;
	ourItemMgr		= NULL;
	selectedView	= NULL;
	if (setFilePath("itemFile")) {						// This name in our folder.
		ourBlockFile = new blockFile(mFilePath);		// Have a go at creating the block file.
		if (ourBlockFile) {									// Got the block file?
			ourItemMgr = new itemMgr(ourBlockFile);	// Create the item manager.
		}
	}
}


// The world as you know it, is ending. Deallocate what you need to. Don't
// deallocate stuff you've handed to addObj(). Those items will be dealt
// with internally.
shopList::~shopList(void) {

	if (ourBlockFile) delete(ourBlockFile);
	if (ourItemMgr) delete(ourItemMgr);
}


void shopList::setupColors(void) {

	SLBackColor.setColor(&black);
	SLDefTextColor.setColor(&cyan);
	SLEditTextColor.setColor(&yellow);
}


// setup() & loop() panel style.
void shopList::setup(void) {

	//filePath		aPath;
	rect			aFrame;
	tempStr		pathStr;
	bmpObj*		aBmp;
	label*		aLabel;
	
	
	setupColors();
	aFrame.setRect(ITEM_LBL_X,ITEM_LBL_Y,ITEM_LBL_W,ITEM_LBL_H);
	setFilePath("items.bmp");
	aBmp = new bmpObj(&aFrame,mFilePath);
	addObj(aBmp);
	
	aFrame.setRect(CART_LBL_X,CART_LBL_Y,CART_LBL_W,CART_LBL_H);
	setFilePath("list.bmp");
	aBmp = new bmpObj(&aFrame,mFilePath);
	addObj(aBmp);
	
	setFilePath("back22.bmp");
	clearCartBtn* clearBtn = new clearCartBtn(this,mFilePath);
	addObj(clearBtn);
	
	addItem = newStdBtn(NEW_X,NEW_Y,icon22,newItemCmd,this);
	addObj(addItem);
	
	delItem = newStdBtn(DELETE_X,DELETE_Y,icon22,deleteItemCmd,this);
	delItem->setActive(false);
	addObj(delItem);
	
	aLabel = new label(NEW_LBL_X,NEW_LBL_Y,NEW_LBL_W,NEW_LBL_H,"Add");
	aLabel->setColors(&white);
	addObj(aLabel);
	
	aLabel = new label(DELETE_LBL_X,DELETE_LBL_Y,DELETE_LBL_W,DELETE_LBL_H,"Del");
	aLabel->setColors(&white);
	addObj(aLabel);
	
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


void shopList::setItemIcons(bool addActive,bool delActive) {

	addItem->setActive(addActive);
	delItem->setActive(delActive);
}


void shopList::handleCom(stdComs comID) {
	
	switch(comID) {
		case newItemCmd	:
			Serial.println("Got new item cmd!");
			if (ourAddItemDBox) {
				Serial.print("Add item : ");
				Serial.println(ourAddItemDBox->getName());
				ourItemMgr->addNewItem(ourAddItemDBox->getName());
			} else {
				ourAddItemDBox = new addItemDBox(this);
				setItemIcons(false,false);
			}
		break;
		case deleteItemCmd	:
			Serial.println("Got delete item cmd!");
			ourItemMgr->deleteItem(selectedView);
			setItemIcons(true,false);
		break;	
		case okCmd 		:
			Serial.println("Got Ok cmd");
			if (ourAddItemDBox) {
				Serial.print("Add item : ");
				Serial.println(ourAddItemDBox->getName());
				ourItemMgr->addNewItem(ourAddItemDBox->getName());
				ourAddItemDBox = NULL;
			} else {
				Serial.println("But no DBox to get item from!");
			}
			setItemIcons(true,false);
		break;
		case cancelCmd	:
			Serial.println("Got cancel cmd");
			ourAddItemDBox = NULL;
			setItemIcons(true,false);
		break;
		default			:
			Serial.print("Seeing comID ");
			Serial.println((int)comID);
			panel::handleCom(comID);
		break;
	}
}


// Being told that this view has focus.
void shopList::selected(itemView* aView) {
	
	selectedView = aView;
	if (selectedView) {
		setItemIcons(true,true);
	} else {
		setItemIcons(true,false);
	}
}


void shopList::clearCart(void) { ourItemMgr->clearCart(); }


void shopList::loop(void) {  }


// The default here is to not draw ourselves. You can change that.
void shopList::drawSelf(void) {

	screen->fillScreen(&SLBackColor);
	//screen->drawVLine(120,50,300,&cyan); 
	//screen->drawVLine(60,50,300,&cyan);
	//screen->drawVLine(180,50,300,&cyan);
}


// Whereas you call close() when you are complete and want to close.. Sometimes
// you don't have control of that. So? No matter if you call close, or something
// else calls close on you, this gets called so you can clean up before being
// deleted.
void shopList::closing(void) {  }
