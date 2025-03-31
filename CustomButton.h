#pragma once
#include <wx/wx.h>

class CustomButton : public wxPanel
{
public:
    CustomButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxBitmap& bitmap = wxNullBitmap, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

    bool SetBackgroundColour(const wxColour& colour) wxOVERRIDE; // 修改为 bool 类型
    bool SetForegroundColour(const wxColour& colour) wxOVERRIDE; // 修改为 bool 类型
    //void SetLabel(const wxString& label);
    void SetBitmap(const wxBitmap& bitmap);
    void SetLabel(const wxString& label) override; // 加上override

protected:
    void OnPaint(wxPaintEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);

private:
    wxColour m_bgColour;
    wxColour m_fgColour;
    wxString m_label;
    wxBitmap m_bitmap;
    bool m_pressed;

    void DrawButton(wxDC& dc);

    wxDECLARE_EVENT_TABLE();
};
