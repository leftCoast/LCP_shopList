#ifndef shopListItems_h
#define shopListItems_h



#include <blockFile.h>
#include <drawObj.h>
#include <iconButton.h>
#include <label.h>



// **********************************************************************
// Definitions.
// **********************************************************************

#define LIST_W			90
#define ITEM_H			20
#define NUM_ITEMS		12

#define ITEMS_X		10
#define ITEMS_Y		ITEM_LBL_Y + 25
#define ITEMS_W		LIST_W				// See shopListItems.h
#define ITEMS_H		ITEM_H * NUM_ITEMS

#define CART_X			CART_LBL_X - 20
#define CART_Y			ITEMS_Y
#define CART_W			ITEMS_W
#define CART_H			ITEMS_H

#define IV_NAME_X		10
#define IV_NAME_Y		6
#define IV_NAME_W		80
#define IV_NAME_H		20

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


extern colorObj	SLBackColor;
extern colorObj	SLDefTextColor;
extern colorObj	SLEditTextColor;


// **********************************************************************
// itemView
// **********************************************************************


class itemView :	public drawGroup {
						
	public:				
				itemView(unsigned long itemID,item* anItem);
	virtual	~itemView(void);
	
				void			setupView(void);
	virtual	void			doAction(event* inEvent,point* localPt);
	virtual	void			setThisFocus(bool setLoose);
				void			handleDrag(float angle);
				drawList*	calcualteOurList(void);
				void			changeState(itemStates newState);
				void			addToList(void);
	virtual	void			draw(void);
	virtual	void			drawSelf(void);
					
					unsigned long	ourItemID;
					item				ourItem;
					label*			name;
};


// **********************************************************************
// itemList
// **********************************************************************


class itemList : public drawList {

	public:
				itemList(rect* frame);
	virtual	~itemList(void);
	
	virtual	void	drawSelf(void);

};



// **********************************************************************
// cartList
// **********************************************************************


class cartList : public drawList {

	public:
				cartList(rect* frame);
	virtual	~cartList(void);
				
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
				
	protected:
				bool	readItem(unsigned long itemID,item* anItem);
				
				blockFile*	ourBlockFile;
				itemList* 	iList;
				cartList* 	cList;
};


#endif