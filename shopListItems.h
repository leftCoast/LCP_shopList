#ifndef shopListItems_h
#define shopListItems_h


#include <blockFile.h>
#include <drawObj.h>
#include <iconButton.h>
#include <label.h>
#include <scrollingList.h>

// **********************************************************************
// Definitions.
// **********************************************************************

#define LIST_W			110	//90
#define ITEM_H			20
#define NUM_ITEMS		12

#define ITEMS_X		0	//10
#define ITEMS_Y		ITEM_LBL_Y + 25
#define ITEMS_W		LIST_W				// See shopListItems.h
#define ITEMS_H		ITEM_H * NUM_ITEMS

#define CART_X			CART_LBL_X - 30  //20
#define CART_Y			ITEMS_Y
#define CART_W			ITEMS_W
#define CART_H			ITEMS_H

#define IV_NAME_X		5
#define IV_NAME_Y		6
#define IV_NAME_W		100
#define IV_NAME_H		12

#define ITEM_VERSION		1;
#define MAX_NAME_BYTES	40						

enum	itemStates {
	suggested,
	listed,
	grabbed
};


struct item {
	int				version;
	char				itemName[MAX_NAME_BYTES];
	int				timesListed;
	itemStates		state;	
};


// **********************************************************************
// pallete - Colors for this and that.
// **********************************************************************

class pallete {

	public:
				pallete(void);
	virtual	~pallete(void);
	
	virtual	void		setupColors(void);
	
				colorObj	dispBackColor;
				colorObj	focusBackColor;
				colorObj	outlineColor;
				colorObj	listTextColor;
				colorObj	strikeTextColor;
				colorObj	labelTextColor;
				colorObj	editTextColor;
					
};


extern pallete* colors;

// **********************************************************************
// STLabel - Adafruit text than can have a strike through line added.
// **********************************************************************


class STLabel :	public label {
						
	public:				
				STLabel(rect* inRect,const char* inText,int textSize=1);
				STLabel(int inLocX, int inLocY, int inWidth,int inHeight,const char* inText,int textSize=1);
	virtual	~STLabel(void);
	
				void	setStrikeColor(colorObj* aColor);
				void	setStrike(bool onOff);
				void  calcStrike(void);
				void	doStrike(void);
	virtual	void	drawSelf(void);
	
				bool		strike;
				colorObj	strikeColor;
				int		txtX;
				int		txtY;
				int		textWidth;
};



// **********************************************************************
// itemView
// **********************************************************************


class itemView :	public drawGroup {
						
	public:				
				itemView(unsigned long itemID,item* anItem);
	virtual	~itemView(void);
	
				void				setupView(void);
	virtual	void				doAction(event* inEvent,point* localPt);
	virtual	void				setThisFocus(bool setLoose);
				void				setItemName(const char* aName);
				char*				getItemName(void);
				void				handleDrag(float angle);
				scrollingList*	calcualteOurList(void);
				itemStates		ourState(void);
				void				changeState(itemStates newState);
				void				addToList(void);
				int				numListings(void);
	virtual	bool				isGreaterThan(dblLinkListObj* compObj);
	virtual	bool				isLessThan(dblLinkListObj* compObj);
	virtual	void				draw(void);
	virtual	void				drawSelf(void);
					
				unsigned long	ourItemID;
				item				ourItem;
				STLabel*			name;
				bool				scrolling;
};


// **********************************************************************
// itemList
// **********************************************************************


class itemList : public scrollingList {

	public:
				itemList(rect* frame);
	virtual	~itemList(void);
	
				void	sortList(void);
	virtual	void	drawSelf(void);

};



// **********************************************************************
// cartList
// **********************************************************************


class cartList : public scrollingList {

	public:
				cartList(rect* frame);
	virtual	~cartList(void);
	
				void	sortList(void);
				void	clearCart(void);
	virtual	void	drawSelf(void);

};


// **********************************************************************
// IDList
// **********************************************************************

class IDList {

	public:
				IDList(blockFile* aBlockFile);
	virtual	~IDList(void);
	
				unsigned long	getNumRootBytes(void);
				int				getNumItems(void);
				bool				getList(bool plusOne=false);
				unsigned long	addItem(void);
				bool				removeItem(unsigned long oldID);

				unsigned long*	IDBuff;
				blockFile*		ourBlockFile;
};



// **********************************************************************
// itemMgr
// **********************************************************************


class itemMgr :	public IDList {

	public:
				itemMgr(blockFile* aBlockFile);
	virtual	~itemMgr(void);
	
				bool	begin(itemList* inIList,cartList* inCList);
				void	populateLists(void);
				void	addNewItem(const char* name);
				void	deleteItem(itemView* oldView);
				void	clearCart(void);
				void	saveItem(unsigned long itemID,item* anItem);
				void	saveSelected(itemView* selected);
				
	protected:
				bool	readItem(unsigned long itemID,item* anItem);
				
				blockFile*	ourBlockFile;
				itemList* 	iList;
				cartList* 	cList;
};


#endif