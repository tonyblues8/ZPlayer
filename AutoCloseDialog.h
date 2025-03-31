#ifndef AUTO_CLOSE_DIALOG_H
#define AUTO_CLOSE_DIALOG_H

#include <wx/wx.h>
#include <wx/timer.h>


#include "ImageButton.h"
//#include "pic/yybj.h"
//#include "pic/yybj2.h"

class AutoCloseDialog : public wxDialog
{
public:
    AutoCloseDialog(wxWindow* parent, const wxString& message, int timeout_ms);

private:
    void OnTimer3(wxTimerEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

    wxTimer* m_timer3;
    wxDECLARE_EVENT_TABLE();
};

#endif // AUTO_CLOSE_DIALOG_H
