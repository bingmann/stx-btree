// $Id$

#ifndef _WTreeDrawing_H_
#define _WTreeDrawing_H_

#include <wx/wx.h>

/** The Custom wxScrolledWindow Canvas on which the B+ tree is drawng. It
 * supports zooming via mouse wheel and scrolling from wxScrolledWindow. */
class WTreeDrawing : public wxScrolledWindow
{
public:
    WTreeDrawing(wxWindow *parent, int id);

    /// Used to determine when to update the scroll bars.
    wxSize		oldTreeSize;
    double		oldscalefactor;

    /// Zoom factor changed by the mouse wheel.
    double		scalefactor;

    virtual void	OnDraw(wxDC& dc);
    void		OnSize(wxSizeEvent &se);
    void		OnMouseWheel(wxMouseEvent &me);

    void		DrawBTree(wxDC &dc);

    /// Tree operation to draw the nodes on this canvas.
    struct BTreeOp_Draw
    {
	BTreeOp_Draw(WTreeDrawing &_w, wxDC &_dc, const class BTreeBundle &_tb)
	    : w(_w), dc(_dc), tb(_tb)
	{
	}

	WTreeDrawing &w;
	wxDC &dc;
	const BTreeBundle &tb;

	typedef	wxSize	result_type;

	template <class BTreeType>
	wxSize draw_node(int offsetx, int offsety, const class BTreeType::btree_impl::node* node);

	template <class BTreeType>
	wxSize draw_tree(BTreeType &bt);

	template <class BTreeType>
	wxSize opInteger(BTreeType &bt);

	template <class BTreeType>
	wxSize opIntegerMulti(BTreeType &bt);

	template <class BTreeType>
	wxSize opString(BTreeType &bt);

	template <class BTreeType>
	wxSize opStringMulti(BTreeType &bt);
    };

    class WMain* 	wmain;
    void		SetWMain(class WMain *wm);

    DECLARE_EVENT_TABLE();
};

#endif // _WTreeDrawing_H_
