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


class addItemDBox :	public alertObj,
							public kbdUser {

	public:
				addItemDBox(listener* inListenerp);
				~addItemDBox(void);
				
				const char*	getName(void);
	virtual	void  		handleCom(stdComs comID);
	virtual	void			idle(void);
	
				editLabel*	nameField;
				rect			editRect;
				stdComBtn*	addAnother;	
};
			
#endif