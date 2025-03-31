#include "CustomButton.h"

wxBEGIN_EVENT_TABLE(CustomButton, wxPanel)
    EVT_PAINT(CustomButton::OnPaint)
    EVT_LEFT_DOWN(CustomButton::OnLeftDown)
    EVT_LEFT_UP(CustomButton::OnLeftUp)
wxEND_EVENT_TABLE()

CustomButton::CustomButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxBitmap& bitmap, const wxPoint& pos, const wxSize& size)
    : wxPanel(parent, id, pos, size), m_label(label), m_bitmap(bitmap), m_pressed(false)
{
    m_bgColour = *wxLIGHT_GREY;
    m_fgColour = *wxBLACK;
}

bool CustomButton::SetBackgroundColour(const wxColour& colour)
{
    m_bgColour = colour;
    Refresh();
    return true; // 返回 true 表示成功
}

bool CustomButton::SetForegroundColour(const wxColour& colour)
{
    m_fgColour = colour;
    Refresh();
    return true; // 返回 true 表示成功
}

void CustomButton::SetLabel(const wxString& label)
{
    m_label = label;
    Refresh();
}

void CustomButton::SetBitmap(const wxBitmap& bitmap)
{
    m_bitmap = bitmap;
    Refresh();
}

void CustomButton::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    //dc.SetBackground(*wxTRANSPARENT_BRUSH);
    //dc.Clear(); // 清除背景
    DrawButton(dc);
}

void CustomButton::OnLeftDown(wxMouseEvent& event)
{
    m_pressed = true;
    Refresh();
}

void CustomButton::OnLeftUp(wxMouseEvent& event)
{
    m_pressed = false;
    Refresh();
    wxCommandEvent clickEvent(wxEVT_BUTTON, GetId());
    GetParent()->ProcessWindowEvent(clickEvent);
}

void CustomButton::DrawButton(wxDC& dc)
{
    wxSize size = GetSize();

    // 绘制按钮背景
    //dc.SetBrush(m_pressed ? *wxWHITE_BRUSH : wxBrush(m_bgColour));
    //dc.SetBrush(m_pressed ? *wxLIGHT_GREY_BRUSH : *wxTRANSPARENT_BRUSH);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(*wxTRANSPARENT_PEN);
    //dc.SetPen(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

    // 绘制按钮图片
    if (m_bitmap.IsOk())
    {
        int bmpX = (size.GetWidth() - m_bitmap.GetWidth()) / 2;
        int bmpY = (size.GetHeight() - m_bitmap.GetHeight()) / 2;
        dc.DrawBitmap(m_bitmap, bmpX, bmpY, true);
    }

    // 绘制按钮文字（覆盖在图片上）
    dc.SetTextForeground(m_fgColour);
    wxCoord textWidth, textHeight;
    dc.GetTextExtent(m_label, &textWidth, &textHeight);

    // 文字的 x 和 y 坐标，确保文字居中在图片上方
    int textX = (size.GetWidth() - textWidth) / 2;
    int textY = (size.GetHeight() - textHeight) / 2;

    dc.DrawText(m_label, textX, textY);
}