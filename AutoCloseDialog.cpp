#include "AutoCloseDialog.h"

#include "pic/h/yybj.h"
#include "pic/h/yybj2.h"

AutoCloseDialog::AutoCloseDialog(wxWindow* parent, const wxString& message, int timeout_ms)
    : wxDialog(parent, wxID_ANY, "", wxDefaultPosition, wxSize(400, 100), wxBORDER_NONE | wxBG_STYLE_TRANSPARENT)
{
    this->SetWindowStyle(this->GetWindowStyle() | wxSTAY_ON_TOP);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    MemoryImageButton* textButton1 = new MemoryImageButton(this, wxID_ANY, yybj_png, yybj_png_len, yybj2_png, yybj2_png_len, message, wxDefaultPosition, wxSize(400, 100));

    // 设置定时器
    m_timer3 = new wxTimer(this);
    Bind(wxEVT_TIMER, &AutoCloseDialog::OnTimer3, this, m_timer3->GetId());

    // 启动定时器
    m_timer3->Start(timeout_ms);
    CenterOnParent();

    // 绑定事件
    Bind(wxEVT_PAINT, &AutoCloseDialog::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &AutoCloseDialog::OnEraseBackground, this);
}

void AutoCloseDialog::OnTimer3(wxTimerEvent& event)
{
    Close();
}

void AutoCloseDialog::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    dc.SetBackground(*wxTRANSPARENT_BRUSH);
    dc.Clear();
}

void AutoCloseDialog::OnEraseBackground(wxEraseEvent& event)
{
    // 保持透明，什么都不做
}

wxBEGIN_EVENT_TABLE(AutoCloseDialog, wxDialog)
    EVT_PAINT(AutoCloseDialog::OnPaint)
wxEND_EVENT_TABLE()
