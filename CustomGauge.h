#ifndef CUSTOMGAUGE_H
#define CUSTOMGAUGE_H

#include <wx/wx.h>

class CustomGauge : public wxWindow
{
public:
    CustomGauge(wxWindow* parent, wxWindowID id, int range, const wxPoint& pos, const wxSize& size);

    void SetValue(int value);
    void SetRange(int range);
    void SetBackgroundAndForeground(const wxColour& bgColour, const wxColour& fgColour);

private:
    wxColour m_bgColour = *wxWHITE;
    wxColour m_fgColour = *wxBLUE;
    int m_value;
    int m_range;

    void OnPaint(wxPaintEvent& event);

    wxDECLARE_EVENT_TABLE();
};

#endif // CUSTOMGAUGE_H
