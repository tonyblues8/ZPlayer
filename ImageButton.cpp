#include "ImageButton.h"
#include <wx/dcbuffer.h>
// 基类 ImageButtonBase 实现
ImageButtonBase::ImageButtonBase(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size)
    : wxWindow(parent, id, pos, size, wxNO_BORDER | wxCLIP_CHILDREN | wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxBG_STYLE_TRANSPARENT), m_label(label)
{
    //setlocale(LC_ALL, "zh_CN.UTF-8"); // 设置本地语言环境，确保中文显示
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_ERASE_BACKGROUND, &ImageButtonBase::OnEraseBackground, this);
    Bind(wxEVT_PAINT, &ImageButtonBase::OnPaint, this);
    //Bind(wxEVT_SIZE, &ImageButtonBase::OnSize, this);
    Bind(wxEVT_LEFT_DOWN, &ImageButtonBase::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &ImageButtonBase::OnLeftUp, this);
}
void ImageButtonBase::OnEraseBackground(wxEraseEvent& event)
{
    // 什么都不做，避免默认的背景擦除行为
    // 这样可以防止闪烁，并确保我们自己清除背景
}
void ImageButtonBase::DrawButton(wxDC& dc)
{
	//Refresh();
    wxSize size = GetClientSize();

    // 清除 dc 内容，防止图片叠加
    //dc.SetBackground(*wxTRANSPARENT_BRUSH);
    //dc.Clear();

    // 确保当前图片有效
    if (m_currentBitmap.IsOk())
    {
        // 先将 wxBitmap 转换为 wxImage，进行缩放，然后再转换回 wxBitmap
        wxImage image = m_currentBitmap.ConvertToImage();
        wxBitmap scaledBitmap(image.Scale(size.GetWidth(), size.GetHeight(), wxIMAGE_QUALITY_HIGH));

        // 绘制图片，确保处理透明区域
        dc.DrawBitmap(scaledBitmap, 0, 0, true);  // 'true' 参数确保透明区域
    }
    else
    {
        wxLogError("当前图片无效");
    }

    // 如果有文本标签，则绘制
    if (!m_label.IsEmpty())
    {
        wxFont font = GetFont();
        font = font.MakeBold();  // 将字体加粗
        dc.SetFont(font);
        dc.SetTextForeground(GetForegroundColour());

        // 自动换行文本，并居中显示
        wxString wrappedLabel = WrapText(dc, m_label, size.GetWidth() - 50);
        wxSize textSize = dc.GetMultiLineTextExtent(wrappedLabel);
        wxPoint textPos((size.GetWidth() - textSize.GetWidth()) / 2, (size.GetHeight() - textSize.GetHeight()) / 2);
        dc.DrawLabel(wrappedLabel, wxRect(0, 0, size.GetWidth(), size.GetHeight()), wxALIGN_CENTER);
    }
}


void ImageButtonBase::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    //wxClientDC dc(this);
    //wxAutoBufferedPaintDC dc(this);  // 使用自动缓冲减少闪烁
    //dc.SetBackground(wxBrush(GetBackgroundColour(), wxBRUSHSTYLE_TRANSPARENT));
    dc.SetBackground(*wxTRANSPARENT_BRUSH);
    dc.Clear(); // 清除背景
    //dc.SetBackground(*wxTRANSPARENT_BRUSH);
    //wxLogError("no ok");
    Update();
    DrawButton(dc);  // 调用绘制函数绘制按钮和文本
}

void ImageButtonBase::OnSize(wxSizeEvent& event)
{
    Refresh();
    event.Skip();
}

// 文本换行函数
wxString ImageButtonBase::WrapText(wxDC& dc, const wxString& text, int maxWidth)
{
    wxString wrappedText;
    wxString currentLine;

    // 中文不能按单词拆分，所以按字符处理
    for (size_t i = 0; i < text.length(); ++i)
    {
        wxString tempLine = currentLine + text[i];
        wxSize textSize = dc.GetTextExtent(tempLine);

        if (textSize.GetWidth() > maxWidth)
        {
            wrappedText += currentLine + "\n";
            currentLine = text[i];  // 超过宽度时，将当前字符作为新行的开头
        }
        else
        {
            currentLine = tempLine;
        }
    }

    wrappedText += currentLine;  // 添加最后一行
    return wrappedText;
}


// 子类 FileImageButton 实现
FileImageButton::FileImageButton(wxWindow* parent, wxWindowID id, const wxString& defaultImagePath, const wxString& pressedImagePath, const wxString& label, const wxPoint& pos, const wxSize& size)
    : ImageButtonBase(parent, id, label, pos, size)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxBitmap defaultBitmap(defaultImagePath, wxBITMAP_TYPE_PNG);
    wxBitmap pressedBitmap(pressedImagePath, wxBITMAP_TYPE_PNG);

    if (!defaultBitmap.IsOk() || !pressedBitmap.IsOk())
    {
        wxLogError("Failed to load image from file");
        return;
    }

    SetDefaultBitmap(defaultBitmap);
    SetPressedBitmap(pressedBitmap);

}

void FileImageButton::SetDefaultBitmap(const wxBitmap& bitmap)
{
    m_defaultBitmap = bitmap;
    m_currentBitmap = m_defaultBitmap;
}

void FileImageButton::SetPressedBitmap(const wxBitmap& bitmap)
{
    m_pressedBitmap = bitmap;
}

void FileImageButton::OnLeftDown(wxMouseEvent& event)
{
    m_currentBitmap = m_pressedBitmap;
    //Refresh();
    //event.Skip();
}

void FileImageButton::OnLeftUp(wxMouseEvent& event)
{
    m_currentBitmap = m_defaultBitmap;
    if (m_clickCallback)
    {
        m_clickCallback();  // 触发点击回调
    }
    //Refresh();
    //Update();   // 确保立即重绘
    wxCommandEvent clickEvent(wxEVT_BUTTON, GetId());
    GetEventHandler()->ProcessEvent(clickEvent);
    event.Skip();
}

void FileImageButton::SetClickCallback(std::function<void()> callback)
{
    m_clickCallback = callback;
}

// 子类 MemoryImageButton 实现
MemoryImageButton::MemoryImageButton(wxWindow* parent, wxWindowID id, const unsigned char* defaultImageData, size_t defaultImageSize, const unsigned char* pressedImageData, size_t pressedImageSize, const wxString& label, const wxPoint& pos, const wxSize& size)
    : ImageButtonBase(parent, id, label, pos, size),
      m_defaultImageData(defaultImageData),
      m_defaultImageSize(defaultImageSize),
      m_pressedImageData(pressedImageData),
      m_pressedImageSize(pressedImageSize),
      m_pressedBitmapLoaded(false) // 新增标志位，表示按下图片是否已加载
{
    // 只加载默认图片
    wxMemoryInputStream memStreamDefault(m_defaultImageData, m_defaultImageSize);
    wxImage imageDefault(memStreamDefault, wxBITMAP_TYPE_PNG);

    if (!imageDefault.IsOk())
    {
        wxLogError("Failed to load default image from memory stream");
        return;
    }

    wxBitmap defaultBitmap(imageDefault);
    SetDefaultBitmap(defaultBitmap);
    m_currentBitmap = defaultBitmap;
    //if (!m_currentBitmap.IsOk()) {
    //	 m_defaultBitmap = defaultBitmap;
    //	 m_currentBitmap = m_defaultBitmap;
    //   //m_currentBitmap = m_defaultBitmap;
    //   wxLogError("no ok");
    //}else{
    //	 wxLogError("ok");
    //}

}



void MemoryImageButton::SetDefaultBitmap(const wxBitmap& bitmap)
{
    m_defaultBitmap = bitmap;
    m_currentBitmap = m_defaultBitmap;
}

void MemoryImageButton::SetPressedBitmap(const wxBitmap& bitmap)
{
    m_pressedBitmap = bitmap;
}

void MemoryImageButton::OnLeftDown(wxMouseEvent& event)
{
	  //wxClientDC dc(this);
	  //dc.SetBackground(*wxTRANSPARENT_BRUSH);
    //dc.Clear();
    //SetBackgroundStyle(wxBG_STYLE_PAINT);
    // 如果按下图片尚未加载，则首次点击时加载它
    if (!m_pressedBitmapLoaded)
    {
        wxMemoryInputStream memStreamPressed(m_pressedImageData, m_pressedImageSize);
        wxImage imagePressed(memStreamPressed, wxBITMAP_TYPE_PNG);

        if (!imagePressed.IsOk())
        {
            wxLogError("Failed to load pressed image from memory stream");
            return;
        }

        wxBitmap pressedBitmap(imagePressed);
        SetPressedBitmap(pressedBitmap);
        m_pressedBitmapLoaded = true;  // 标记按下图片已加载
    }

    // 切换到按下图片
    //m_currentBitmap = wxBitmap(1, 1);
    m_currentBitmap = m_pressedBitmap;
    //
    //Update();
    //Refresh();
    event.Skip();
}

void MemoryImageButton::OnLeftUp(wxMouseEvent& event)
{
	  //wxClientDC dc(this);
	  //dc.SetBackground(*wxTRANSPARENT_BRUSH);
    //dc.Clear();
    //SetBackgroundStyle(wxBG_STYLE_PAINT);
    //m_currentBitmap = wxBitmap(1, 1);
    m_currentBitmap = m_defaultBitmap;
    if (m_clickCallback)
    {
        m_clickCallback();  // 触发点击回调
    }

    //Update();   // 确保立即重绘
    //Refresh();
    wxCommandEvent clickEvent(wxEVT_BUTTON, GetId());
    GetEventHandler()->ProcessEvent(clickEvent);
    event.Skip();
}
void MemoryImageButton::SetClickCallback(std::function<void()> callback)
{
    m_clickCallback = callback;
}


