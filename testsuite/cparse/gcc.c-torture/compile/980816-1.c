typedef __SIZE_TYPE__ size_t;
typedef void *XtPointer;

typedef struct _WidgetRec *Widget;
typedef struct _WidgetClassRec *WidgetClass;

extern WidgetClass commandWidgetClass;

typedef void (*XtCallbackProc)(
    Widget 		 ,
    XtPointer 		 ,	 
    XtPointer 		 	 
);

extern const  char XtStrings[];


typedef struct						 
{
	char			*Name,			 
				*Label;			 
	XtCallbackProc		Callback;		 
	XtPointer		ClientData;		 
	Widget			W;				 
} DialogButtonType, *DialogButtonTypePtr;

 
Widget AddButtons(Widget Parent, Widget Top,
	DialogButtonTypePtr Buttons, size_t Count)
{
	int		i;

	for (i = 0; i < Count; i++)
	{
		if (!Buttons[i].Label)
			continue;
		Buttons[i].W  = XtVaCreateManagedWidget(Buttons[i].Name,
			commandWidgetClass,
			Parent,
			((char*)&XtStrings[429]) , Buttons[i].Label,
			"fromHoriz" , i ? Buttons[i-1].W : ((void *)0) ,
			"fromVert" , Top,
			"resizable" , 1 ,
			((void *)0) );

		XtAddCallback(((char*)&XtStrings[136]),
				 Buttons[i].Callback, Buttons[i].ClientData);
	}
	return(Buttons[Count-1].W);
}

/* cp-out: warning: [^:]*: line 37, columns 18-41: identifier "XtVaCreateManagedWidget" not declared */
/* cp-out: warning: [^:]*: line 46, columns 2-15: identifier "XtAddCallback" not declared */
/* cp-out: warning: [^:]*: line 37, columns 18-41: assignment to pointer from integer which is not NULL */
