#ifndef shopListDB_h
#define shopListDB_h

#include <editLabel.h>
#include <modalKbd.h>
#include <alertObj.h>

class frame : public drawObj {

	public:
				frame(rect* inRect);
				~frame(void);
				
	virtual	void	drawSelf(void);
};


// **************************************************************
// **********************   addItemDBox    **********************
// **************************************************************

// NOTE: Add another function needs to be removed for now. Until 
// the redraw issues are dealt with.
// 
// **Probably** By adding a name queue then adding them all as
// a set. Later, deal with it later.

class addItemDBox :	public alertObj,
							public kbdUser {

	public:
				addItemDBox(listener* inListenerp);
				~addItemDBox(void);
				
				const char*	getName(void);
	virtual	void  		handleCom(stdComs comID);
	virtual	void			idle(void);
			
				editLabel*	nameField;
				//stdComBtn*	addAnother;	
};



// **************************************************************
// **********************   editItemDBox    *********************
// **************************************************************


class editItemDBox :	public alertObj,
							public kbdUser {

	public:
				editItemDBox(listener* inListener,const char* inName=NULL);
				~editItemDBox(void);
				
				const char*	getName(void);
			
				editLabel*	nameField;
};
			
#endif