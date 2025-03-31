#ifndef IMAGE_BUTTON_H
#define IMAGE_BUTTON_H

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/mstream.h>

// 基类：通用按钮逻辑
class ImageButtonBase : public wxWindow
{
public:
    ImageButtonBase(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size);

    // 设置默认图像和按下时的图像
    virtual void SetDefaultBitmap(const wxBitmap& bitmap) = 0;
    virtual void SetPressedBitmap(const wxBitmap& bitmap) = 0;
    void OnEraseBackground(wxEraseEvent& event);
    void OnPaint(wxPaintEvent& event);


protected:
    // 子类重写点击事件
    virtual void OnLeftDown(wxMouseEvent& event) = 0;
    virtual void OnLeftUp(wxMouseEvent& event) = 0;

    void DrawButton(wxDC& dc);  // 绘制按钮
    void OnSize(wxSizeEvent& event);

    wxString WrapText(wxDC& dc, const wxString& text, int maxWidth);  // 添加文本换行函数声明

    wxBitmap m_currentBitmap;
    wxString m_label;
};

// 子类：基于文件路径的按钮
class FileImageButton : public ImageButtonBase
{
public:
    FileImageButton(wxWindow* parent, wxWindowID id, const wxString& defaultImagePath, const wxString& pressedImagePath, const wxString& label, const wxPoint& pos, const wxSize& size);

    void SetDefaultBitmap(const wxBitmap& bitmap) override;
    void SetPressedBitmap(const wxBitmap& bitmap) override;
    void SetClickCallback(std::function<void()> callback);

protected:
    void OnLeftDown(wxMouseEvent& event) override;
    void OnLeftUp(wxMouseEvent& event) override;
    std::function<void()> m_clickCallback;  // 点击后的回调函数

private:
    wxBitmap m_defaultBitmap;
    wxBitmap m_pressedBitmap;
};

// 子类：基于 wxMemoryInputStream 的按钮
class MemoryImageButton : public ImageButtonBase
{
public:
    MemoryImageButton(wxWindow* parent, wxWindowID id, const unsigned char* defaultImageData, size_t defaultImageSize, const unsigned char* pressedImageData, size_t pressedImageSize, const wxString& label, const wxPoint& pos, const wxSize& size);

    void SetDefaultBitmap(const wxBitmap& bitmap) override;
    void SetPressedBitmap(const wxBitmap& bitmap) override;
    void SetClickCallback(std::function<void()> callback);

protected:
    void OnLeftDown(wxMouseEvent& event) override;
    void OnLeftUp(wxMouseEvent& event) override;
    std::function<void()> m_clickCallback;  // 点击后的回调函数

private:
    wxBitmap m_defaultBitmap;
    wxBitmap m_pressedBitmap;
        const unsigned char* m_defaultImageData;  // 保存默认图片数据
    size_t m_defaultImageSize;  // 保存默认图片数据大小
    const unsigned char* m_pressedImageData;  // 保存按下图片数据
    size_t m_pressedImageSize;  // 保存按下图片数据大小

    bool m_pressedBitmapLoaded;  // 表示是否已加载按下的图片
};

#endif // IMAGE_BUTTON_H
