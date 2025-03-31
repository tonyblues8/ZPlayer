#include "CustomGauge.h"

wxBEGIN_EVENT_TABLE(CustomGauge, wxWindow)
    EVT_PAINT(CustomGauge::OnPaint)
wxEND_EVENT_TABLE()

CustomGauge::CustomGauge(wxWindow* parent, wxWindowID id, int range, const wxPoint& pos, const wxSize& size)
    : wxWindow(parent, id, pos, size), m_range(range), m_value(0)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void CustomGauge::SetValue(int value)
{
    m_value = value;
    Refresh();
}

void CustomGauge::SetRange(int range)
{
    m_range = range;
    Refresh();
}

void CustomGauge::SetBackgroundAndForeground(const wxColour& bgColour, const wxColour& fgColour)
{
    m_bgColour = bgColour;
    m_fgColour = fgColour;
    Refresh();
}

void CustomGauge::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    wxSize size = GetSize();

    dc.SetBackground(*wxTRANSPARENT_BRUSH);
    dc.Clear(); // 

    // 
    dc.SetBrush(wxBrush(m_bgColour));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

    // 
    dc.SetBrush(wxBrush(m_fgColour));
    int gaugeWidth = static_cast<int>(size.GetWidth() * (static_cast<double>(m_value) / m_range));
    dc.DrawRectangle(0, 0, gaugeWidth, size.GetHeight());
}
