// $Id$

#include <iostream>
#include <sstream>
#include <map>

#include "WMain.h"
#include "WTreeDrawing.h"

WMain::WMain()
    : WMain_wxg(NULL, -1, wxT(""))
{
    window_TreeDrawing->SetWMain(this);
}

void WMain::OnButtonInsert(wxCommandEvent &)
{
    wxString inputkey = textctrl_Key->GetValue();
    wxString inputdata = textctrl_Data->GetValue();

    long key, data;
    if (!inputkey.ToLong(&key)) {
	textctrl_OpResult->SetValue(wxT("Could not interpret key string as integer."));
	return;
    }
    if (!inputdata.ToLong(&data)) {
	textctrl_OpResult->SetValue(wxT("Could not interpret data string as integer."));
	return;
    }

    if (!btree_int_4slots.insert2(key, data).second) {
	textctrl_OpResult->SetValue(wxT("Insert returned false: key already exists."));
    }

    UpdateTextDump();
}

void WMain::OnButtonErase(wxCommandEvent &)
{
    wxString inputkey = textctrl_Key->GetValue();

    long key;
    if (!inputkey.ToLong(&key)) {
	textctrl_OpResult->SetValue(wxT("Could not interpret key string as integer."));
	return;
    }

    if (!btree_int_4slots.erase(key)) {
	textctrl_OpResult->SetValue(wxT("Erase returned false: key does not exist."));
    }

    UpdateTextDump();
}

void WMain::OnButtonInsertRandom(wxCommandEvent &)
{
    wxMenu* menu = new wxMenu;
    
    menu->Append(500,	wxT("Insert 10 Random Integers"));
    menu->Append(501,	wxT("Insert 20 Random Integers"));
    menu->Append(502,	wxT("Insert 50 Random Integers"));
    menu->Append(503,	wxT("Insert 100 Random Integers"));

    PopupMenu(menu);
}

void WMain::OnMenuInsertRandom(wxCommandEvent &ce)
{
    srand(time(NULL));
    if (ce.GetId() == 500)
    {
	for(unsigned int i = 0; i < 10; i++)
	{
	    btree_int_4slots.insert2(rand() % 1000, rand() % 1000);
	}
    }
    else if (ce.GetId() == 501)
    {
	for(unsigned int i = 0; i < 20; i++)
	{
	    btree_int_4slots.insert2(rand() % 1000, rand() % 1000);
	}
    }
    else if (ce.GetId() == 502)
    {
	for(unsigned int i = 0; i < 50; i++)
	{
	    btree_int_4slots.insert2(rand() % 1000, rand() % 1000);
	}
    }
    else if (ce.GetId() == 503)
    {
	for(unsigned int i = 0; i < 100; i++)
	{
	    btree_int_4slots.insert2(rand() % 1000, rand() % 1000);
	}
    }

    UpdateTextDump();
}

void WMain::OnButtonClear(wxCommandEvent &)
{
    btree_int_4slots.clear();
    UpdateTextDump();
}

void WMain::UpdateTextDump()
{
    std::ostringstream oss;

    btree_int_4slots.print(oss);
    std::string os = oss.str();

    textctrl_TextDump->SetValue( wxString(os.data(), wxConvISO8859_1, os.size()) );

    window_TreeDrawing->Refresh();
}

BEGIN_EVENT_TABLE(WMain, wxFrame)

    EVT_MENU_RANGE (500, 510,			WMain::OnMenuInsertRandom)

    EVT_BUTTON	(ID_BUTTON_INSERT,		WMain::OnButtonInsert)
    EVT_BUTTON	(ID_BUTTON_ERASE,		WMain::OnButtonErase)
    EVT_BUTTON	(ID_BUTTON_INSERTRANDOM,	WMain::OnButtonInsertRandom)
    EVT_BUTTON	(ID_BUTTON_CLEAR,	WMain::OnButtonClear)

END_EVENT_TABLE();

// *** Main Application

class AppBTreeDemo : public wxApp
{
public:
    bool 		OnInit();
};

IMPLEMENT_APP(AppBTreeDemo)

bool AppBTreeDemo::OnInit()
{
    wxInitAllImageHandlers();

    WMain* wm = new WMain();
    SetTopWindow(wm);
    wm->Show();

    return true;
}

