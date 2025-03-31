#include "Command.h"

#include <wx/wx.h>
#include <wx/dir.h>
#include <algorithm>
#include <wx/stdpaths.h>
#include <wx/dcclient.h>
#include <wx/mstream.h>
#include <wx/filename.h>
#include <sstream>

#include <regex>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <array>
#include <cstdio>
#include <curl/curl.h>
#include <openssl/md5.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

#ifdef __APPLE__
#include <iostream>
#include <Cocoa/Cocoa.h>

#include <objc/objc.h>
#include <objc/objc-runtime.h>
#endif


#include "pic/h/yybj.h"
#include "pic/h/yybj2.h"
#include "pic/h/bubj.h"
#include "pic/h/bubj2.h"
#include "pic/h/imageon.h"
#include "pic/h/imageoff.h"
#include "pic/h/imageon1.h"
#include "pic/h/imageoff1.h"

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <Cocoa/Cocoa.h>

#include <objc/objc.h>
#include <objc/objc-runtime.h>

// 构造函数定义
Command::Command() {
    // 在构造函数中初始化 wxSingleInstanceChecker 指针
    m_checker = nullptr;  // 初始化为 nullptr
}

// 析构函数定义
Command::~Command() {
    // 如果 m_checker 被实例化，释放它
    if (m_checker) {
        delete m_checker;
    }
}

//以下调用方式Command::fun(...)
// 通过 NSAppearance 检测 macOS Dark Mode
extern "C" bool IsDarkModeEnabledcommand() {
    @autoreleasepool {
        // 必须通过 NSApplication 的实例访问
        NSAppearance* appearance = [[NSApplication sharedApplication] effectiveAppearance];
        NSAppearanceName basicAppearance = [appearance bestMatchFromAppearancesWithNames:@[
            NSAppearanceNameAqua,
            NSAppearanceNameDarkAqua
        ]];
        return [basicAppearance isEqualToString:NSAppearanceNameDarkAqua];
    }
}
#else
// 其他平台默认返回 false
extern "C" bool IsDarkModeEnabledcommand() {
    return false;
}
#endif

bool Command::IsDarkMode() {
    return IsDarkModeEnabledcommand();
}


// 颜色和样式设置
void Command::SetColorsAndStylesRecursiveN(wxWindow* window, wxPanel* Getpanel) {
    //bool isDarkMode = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW) == *wxBLACK;
    bool isDarkMode = Command::IsDarkMode();
    wxColour backgroundColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    wxColour textColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    wxColour customColor2 = isDarkMode ? wxColour(168, 168, 168) : wxColour(242, 242, 242);
    wxBitmap bitmapOn, bitmapOff, bitmapbubj, bitmapbubj2;

    wxMemoryInputStream memStreambubj(bubj_png, bubj_png_len);
    wxImage imagebubj(memStreambubj, wxBITMAP_TYPE_PNG);
    if (imagebubj.IsOk()) {
        wxImage scaledImagebubj = imagebubj.Rescale(120, 30, wxIMAGE_QUALITY_HIGH);
        bitmapbubj = wxBitmap(scaledImagebubj);
    }
    wxMemoryInputStream memStreambubj2(bubj2_png, bubj2_png_len);
    wxImage imagebubj2(memStreambubj2, wxBITMAP_TYPE_PNG);
    if (imagebubj2.IsOk()) {
        wxImage scaledImagebubj2 = imagebubj2.Rescale(120, 30, wxIMAGE_QUALITY_HIGH);
        bitmapbubj2 = wxBitmap(scaledImagebubj2);
    }


    if (isDarkMode) {
        wxMemoryInputStream memStreamOn(imageon1_png, imageon1_png_len);
        wxMemoryInputStream memStreamOff(imageoff1_png, imageoff1_png_len);
        wxImage imageOn(memStreamOn, wxBITMAP_TYPE_PNG);
        wxImage imageOff(memStreamOff, wxBITMAP_TYPE_PNG);
        if (imageOn.IsOk() && imageOff.IsOk()) {
            bitmapOn = wxBitmap(imageOn.Rescale(30, 30, wxIMAGE_QUALITY_HIGH));
            bitmapOff = wxBitmap(imageOff.Rescale(30, 30, wxIMAGE_QUALITY_HIGH));
        }
    } else {
        wxMemoryInputStream memStreamOn(imageon_png, imageon_png_len);
        wxMemoryInputStream memStreamOff(imageoff_png, imageoff_png_len);
        wxImage imageOn(memStreamOn, wxBITMAP_TYPE_PNG);
        wxImage imageOff(memStreamOff, wxBITMAP_TYPE_PNG);
        if (imageOn.IsOk() && imageOff.IsOk()) {
            bitmapOn = wxBitmap(imageOn.Rescale(30, 30, wxIMAGE_QUALITY_HIGH));
            bitmapOff = wxBitmap(imageOff.Rescale(30, 30, wxIMAGE_QUALITY_HIGH));
        }
    }

    // 处理 CustomButton
    if (auto* clickedButton = dynamic_cast<CustomButton*>(window)) {
        clickedButton->SetBackgroundColour(backgroundColor);
        clickedButton->SetForegroundColour(textColor);
        clickedButton->SetBitmap(isDarkMode ? bitmapbubj2 : bitmapbubj);
    }

    // 处理 wxBitmapToggleButton
    if (auto* clickedButton = dynamic_cast<wxBitmapToggleButton*>(window)) {
        clickedButton->SetBitmapLabel(clickedButton->GetValue() ? bitmapOn : bitmapOff);
    }

    window->Layout();
    window->Refresh();
    window->Update();
    Getpanel->Layout();
    Getpanel->Refresh();
    Getpanel->Update();

    for (wxWindow* child : window->GetChildren()) {
        SetColorsAndStylesRecursiveN(child, Getpanel);
    }
}

void Command::SetColorsAndStylesRecursive(wxWindow* window, const wxColour& textColour, const wxColour& bgColour, const wxColour& customColor, const wxColour& customColor2, bool noBorder)
{
    // 设置背景色和前景色
    bool isDarkMode = Command::IsDarkMode();
    window->SetBackgroundColour(bgColour);
    window->SetForegroundColour(textColour);
    wxBitmap bitmapOn, bitmapOff, bitmapbubj, bitmapbubj2;

    wxMemoryInputStream memStreambubj(bubj_png, bubj_png_len);
    wxImage imagebubj(memStreambubj, wxBITMAP_TYPE_PNG);
    if (imagebubj.IsOk()) {
        wxImage scaledImagebubj = imagebubj.Rescale(120, 30, wxIMAGE_QUALITY_HIGH);
        bitmapbubj = wxBitmap(scaledImagebubj);
    }
    wxMemoryInputStream memStreambubj2(bubj2_png, bubj2_png_len);
    wxImage imagebubj2(memStreambubj2, wxBITMAP_TYPE_PNG);
    if (imagebubj2.IsOk()) {
        wxImage scaledImagebubj2 = imagebubj2.Rescale(120, 30, wxIMAGE_QUALITY_HIGH);
        bitmapbubj2 = wxBitmap(scaledImagebubj2);
    }


    if (isDarkMode) {
        wxMemoryInputStream memStreamOn(imageon1_png, imageon1_png_len);
        wxMemoryInputStream memStreamOff(imageoff1_png, imageoff1_png_len);
        wxImage imageOn(memStreamOn, wxBITMAP_TYPE_PNG);
        wxImage imageOff(memStreamOff, wxBITMAP_TYPE_PNG);
        if (imageOn.IsOk() && imageOff.IsOk()) {
            bitmapOn = wxBitmap(imageOn.Rescale(30, 30, wxIMAGE_QUALITY_HIGH));
            bitmapOff = wxBitmap(imageOff.Rescale(30, 30, wxIMAGE_QUALITY_HIGH));
        }
    } else {
        wxMemoryInputStream memStreamOn(imageon_png, imageon_png_len);
        wxMemoryInputStream memStreamOff(imageoff_png, imageoff_png_len);
        wxImage imageOn(memStreamOn, wxBITMAP_TYPE_PNG);
        wxImage imageOff(memStreamOff, wxBITMAP_TYPE_PNG);
        if (imageOn.IsOk() && imageOff.IsOk()) {
            bitmapOn = wxBitmap(imageOn.Rescale(30, 30, wxIMAGE_QUALITY_HIGH));
            bitmapOff = wxBitmap(imageOff.Rescale(30, 30, wxIMAGE_QUALITY_HIGH));
        }
    }


    // 特殊处理按钮的背景色
    if (wxButton* button = dynamic_cast<wxButton*>(window))
    {
        button->SetBackgroundColour(customColor);
        button->SetForegroundColour(textColour);
        if (noBorder) {
           button->SetWindowStyle(button->GetWindowStyle() | wxBORDER_NONE); // 去掉边框
           button->Refresh(); // 刷新按钮以应用新样式
        }
    }

if (CustomButton* button = dynamic_cast<CustomButton*>(window)) {
    // 调用自定义方法设置按钮背景色和文字颜色
    //button->SetButtonColors(customColor, textColour);
    isDarkMode = Command::IsDarkMode();
    button->SetBackgroundColour(customColor);
    button->SetForegroundColour(textColour);
    if (isDarkMode) {
        button->SetBitmap(bitmapbubj2);
    }else{
        button->SetBitmap(bitmapbubj);
    }
    //button->SetBackgroundColour(customColor);
    //button->SetBackgroundColour(textColour);

    // 根据 noBorder 值决定是否移除按钮边框
    //if (noBorder) {
    //    button->RemoveButtonBorder(true); // 去掉边框
    //}
}

    // 特殊处理输入框的背景色
    if (wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(window))
    {
        textCtrl->SetBackgroundColour(customColor2);
        textCtrl->SetForegroundColour(textColour);
        if (noBorder) {
            textCtrl->SetWindowStyle(textCtrl->GetWindowStyle() | wxBORDER_NONE); // 去掉边框
            textCtrl->Refresh(); // 刷新输入框以应用新样式
        }
    }

    // 特殊处理下拉列表的背景色
    if (wxComboBox* comboBox = dynamic_cast<wxComboBox*>(window))
    {
        comboBox->SetBackgroundColour(customColor2);
        comboBox->SetForegroundColour(textColour);
        if (noBorder) {
            comboBox->SetWindowStyle(comboBox->GetWindowStyle() | wxBORDER_NONE); // 去掉边框
            comboBox->Refresh(); // 刷新下拉列表以应用新样式
        }
    }

    // 特殊处理进度条的背景色和前景色
    if (wxGauge* gauge = dynamic_cast<wxGauge*>(window))
    {
        gauge->SetBackgroundColour(bgColour);
        gauge->SetForegroundColour(textColour); // 设置进度条的颜色
        if (noBorder) {
            gauge->SetWindowStyle(gauge->GetWindowStyle() | wxBORDER_NONE); // 去掉边框
            gauge->Refresh(); // 刷新进度条以应用新样式
        }
    }

    // 特殊处理进度条的背景色和前景色
    if (wxChoice* choice = dynamic_cast<wxChoice*>(window))
    {
        choice->SetBackgroundColour(bgColour);
        choice->SetForegroundColour(textColour); // 设置进度条的颜色
        if (noBorder) {
            choice->SetWindowStyle(choice->GetWindowStyle() | wxBORDER_NONE); // 去掉边框
            choice->Refresh(); // 刷新进度条以应用新样式
        }
    }


    // 递归设置每个子控件的颜色和样式
    for (wxWindow* child : window->GetChildren())
    {
        //if (dynamic_cast<wxBitmap*>(child) == nullptr)  // 如果 child 不是 wxDialog
        //if (child->GetId() != 9999 && child->GetId() != 9998 && child->GetId() != 9997)
        //{
            SetColorsAndStylesRecursive(child, textColour, bgColour, customColor, customColor2, noBorder);
        //}
    }
}
// 设置环境变量
void Command::SetupEnvironment() {
    wxString homeDir = wxGetHomeDir();
    wxString filePath = homeDir + "/zsprundir/bin";
    wxString newPath = wxString::Format("/usr/local/opt/gettext/bin:/usr/local/opt/zip/bin:/opt/homebrew/opt/curl/bin:/Library/Frameworks/Python.framework/Versions/3.13/bin:/Applications/CMake.app/Contents/bin:/opt/homebrew/opt/libtool/libexec/gnubin:/opt/homebrew/bin:/opt/homebrew/sbin:/usr/local/bin:/System/Cryptexes/App/usr/bin:/usr/bin:/bin:/usr/sbin:/sbin:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/local/bin:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/bin:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/appleinternal/bin:/Library/Apple/usr/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:%s", filePath);
    setenv("PATH", newPath.mb_str().data(), 1);
    setenv("DYLD_LIBRARY_PATH", ".", 1);
    setenv("WX_CXXFLAGS", "-I/opt/wxWidgets-3.2.6/build-cocoa-debug/lib/wx/include/osx_cocoa-unicode-static-3.2 -I/opt/wxWidgets-3.2.6/include -D_FILE_OFFSET_BITS=64 -D__WXMAC__ -D__WXOSX__ -D__WXOSX_COCOA__", 1);
    setenv("WX_LIBS", "-L/opt/wxWidgets-3.2.6/build-cocoa-debug/lib -framework IOKit -framework Carbon -framework Cocoa -framework QuartzCore -framework AudioToolbox -framework System -framework OpenGL /opt/wxWidgets-3.2.6/build-cocoa-debug/lib/libwx_osx_cocoau_xrc-3.2.a /opt/wxWidgets-3.2.6/build-cocoa-debug/lib/libwx_osx_cocoau_qa-3.2.a /opt/wxWidgets-3.2.6/build-cocoa-debug/lib/libwx_baseu_net-3.2.a /opt/wxWidgets-3.2.6/build-cocoa-debug/lib/libwx_osx_cocoau_html-3.2.a /opt/wxWidgets-3.2.6/build-cocoa-debug/lib/libwx_osx_cocoau_core-3.2.a /opt/wxWidgets-3.2.6/build-cocoa-debug/lib/libwx_baseu_xml-3.2.a /opt/wxWidgets-3.2.6/build-cocoa-debug/lib/libwx_baseu-3.2.a -framework WebKit -lwxtiff-3.2 -lwxjpeg-3.2 -lwxpng-3.2 -lwxregexu-3.2 -lwxscintilla-3.2 -lexpat -lz -framework Security -lpthread -liconv", 1);
}

// 规范化路径
std::string Command::NormalizePath(const std::string& path) {
    std::string normalized = path;
#ifdef _WIN32
    std::replace(normalized.begin(), normalized.end(), '/', '\\');
#else
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
#endif
    return normalized;
}

// 清除空格
wxString Command::ClearAllSpace(const wxString& str) {
    wxString result = str;
    int index = 0;
    while ((index = result.Find(' ', index)) != wxNOT_FOUND) {
        result.Remove(index, 1);
    }
    return result;
}

// 窗口居中
void Command::CenterWindow(wxWindow* window) {
    wxScreenDC screenDC;
    int screenWidth = screenDC.GetSize().x;
    int screenHeight = screenDC.GetSize().y;

    wxSize windowSize = window->GetSize();
    int windowWidth = windowSize.x;
    int windowHeight = windowSize.y;

    int xPos = (screenWidth - windowWidth) / 2;
    int yPos = (screenHeight - windowHeight) / 2;
    window->Move(xPos, yPos);
}

// 播放视频
void Command::PlayVideo(const wxString& filePath) {
    try {
#ifdef _WIN32
        wxString command = wxString::Format("cmd /c start \"\" \"%s\"", filePath);
#elif defined(__APPLE__)
        wxStandardPaths& stdPaths = wxStandardPaths::Get();
        wxString resourceExecutable = stdPaths.GetResourcesDir() + "/IINA.app/Contents/MacOS/iina-cli";
        wxString cmd = "sh -c \"ps -ef | grep -i iina | grep -v grep | awk '{print \\\"kill -15 \\\" $2}' | sh\"";
        wxExecute(cmd, wxEXEC_SYNC | wxEXEC_HIDE_CONSOLE);
        wxString command = wxString::Format("open \"%s\"", filePath);
#else
        wxString command = wxString::Format("xdg-open \"%s\"", filePath);
#endif
        wxExecute(command, wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE);
    } catch (const std::exception& e) {
        wxLogMessage("error: %s", e.what());
    }
}

int Command::ExecCommand(const std::string& command, std::string* output = nullptr) {
    std::string cmd = command + " 2>&1"; // 捕获标准输出和错误输出
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return -1;

    char buffer[128];
    std::ostringstream result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result << buffer;
    }
    int retCode = pclose(pipe);
    if (output) {
        *output = result.str();
    }
    return WEXITSTATUS(retCode);
}
bool Command::CheckPip(const std::string& pythonPath) {
    std::string command = pythonPath + " -m pip --version > /dev/null 2>&1";
    return Command::ExecCommand(command) == 0;
}
bool Command::CheckPythonVersion(const std::string& pythonPath){
    std::string output;
    std::string command = pythonPath + " -c 'import sys; print(0) if sys.version_info < (3, 10) else print(1)'";
    int ret = Command::ExecCommand(command, &output);
    return ret == 0 && output.find('1') != std::string::npos;
}
void Command::InstallPythonPackage(const std::string& pythonPath, const std::string& package, const std::string& pipName = ""){
    std::string pkgName = pipName.empty() ? package : pipName;
    // 检查包是否已安装
    std::string checkCommand = pythonPath + " -c 'import " + package + "' > /dev/null 2>&1";
    if (Command::ExecCommand(checkCommand) == 0) {
        #ifdef DEBUG
        std::cout << "ok:" << package << std::endl;
        #endif
        return; // 包已安装，直接返回
    }
    // 尝试安装包
    std::string installCommand = pythonPath + " -m pip install --user " + pkgName;
    std::cout << "Installing " << pkgName << "..." << std::endl;
    if (Command::ExecCommand(installCommand) != 0) {
        //#ifdef DEBUG
        throw std::runtime_error("Failed to install " + pkgName);
        //#endif
    }
    #ifdef DEBUG
    std::cout << "Successfully installed " << pkgName << "." << std::endl;
    #endif
}



//以下调用方式
//Command cmd
//cmd.fun()
bool Command::CheckRun(wxFrame* m_frame){
#ifdef _WIN32
    m_checker = new wxSingleInstanceChecker(wxT("zdelv_unique_instance_checker"));

    if (m_checker->IsAnotherRunning())
    {
        HWND hwnd = Command::FindWindowByPartialTitle(wxT("熊猫"));
        if (m_frame)
        {
            m_frame->Show(true);
            m_frame->Raise();
        }

        if (hwnd)
        {
            SetForegroundWindow(hwnd);
            ShowWindow(hwnd, SW_RESTORE);
        }
        return false;
    }
#else
    m_checker = new wxSingleInstanceChecker(wxT("zdelv_unique_instance_checker"));
    if (m_checker->IsAnotherRunning())
    {
        bool isShown = m_frame->IsShown();
        bool isIconized = m_frame->IsIconized();
        if (isShown) {
        	if(isIconized){
        		m_frame->Iconize(false);
            m_frame->Raise();
        	}else{
        		m_frame->Iconize(true);
        		//m_frame->Hide();
        	}
        } else {
          m_frame->Show();
          m_frame->Raise();
        }
        m_frame->SetWindowStyle(m_frame->GetWindowStyle() | wxSTAY_ON_TOP);
        return true;
    }
#endif
    return false;
}


#ifdef _WIN32
#include <windows.h>
#include <string>

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    // 获取窗口标题
    int length = GetWindowTextLength(hwnd);
    if (length == 0) return TRUE;

    // 分配缓冲区并获取窗口标题
    char* windowText = new char[length + 1];
    GetWindowText(hwnd, windowText, length + 1);

    // 检查标题是否包含"熊猫下载"
    std::string title(windowText);
    delete[] windowText;

    if (title.find("熊猫下载") != std::string::npos) {
        // 找到匹配的窗口
        *(HWND*)lParam = hwnd;
        return FALSE;  // 停止枚举
    }

    return TRUE;
}

HWND Command::FindWindowByPartialTitle(const wxString& partialTitle) {
    HWND hwndFound = NULL;
    EnumWindows(EnumWindowsProc, (LPARAM)&hwndFound);
    return hwndFound;
}
#endif

void Command::ForceDeleteFile(const std::string& filePath) {
#ifdef _WIN32
    std::string command = "cmd /c del /f /q " + filePath;
    executeCommandInBackground(command);
#else
    std::string command = "rm -rf " + filePath + " &";
    std::system(command.c_str());
#endif
}

void Command::saveToFile(const std::string& filename, const std::string& data) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << data;
        file.close();
    }
}

std::string Command::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string data;
    if (file.is_open()) {
        std::getline(file, data, (char)file.eof());
        file.close();
    }
    return data;
}

void Command::ReplaceAll(std::string& str, const std::string& search, const std::string& replace) {
    std::string::size_type pos = 0;
    while ((pos = str.find(search, pos)) != std::string::npos) {
        str.erase(pos, search.length());
        str.insert(pos, replace);
        pos += replace.length();
    }
}

void Command::AddToLibraryPath(const wxString& path) {
#ifdef __WINDOWS__
    wxString pathVar;
    wxGetEnv(wxT("PATH"), &pathVar);
    wxSetEnv(wxT("PATH"), path + ";" + pathVar);
#else
    wxString ldLibraryPath;
    wxGetEnv(wxT("LD_LIBRARY_PATH"), &ldLibraryPath);
    wxSetEnv(wxT("LD_LIBRARY_PATH"), path + ":" + ldLibraryPath);
#endif
}

std::string Command::getCommandOutput(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;

#ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif

    if (!pipe) return "ERROR";

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif

    return result;
}

size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}
bool Command::downloadFile(const std::string& url, const std::string& filename) {
    CURL *curl;
    CURLcode res;
    FILE *fp;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        fp = fopen(filename.c_str(), "wb");
        if (!fp) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fclose(fp);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }

        double download_size;
        curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &download_size);

        if (download_size < 1048576) {
            fclose(fp);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }

        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (response_code != 200) {
            fclose(fp);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }

        fclose(fp);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return true;
}

std::string Command::getStringMD5(const std::string& input) {
    unsigned char md5Hash[MD5_DIGEST_LENGTH];
    MD5_CTX md5Context;
    MD5_Init(&md5Context);

    MD5_Update(&md5Context, input.c_str(), input.size());
    MD5_Final(md5Hash, &md5Context);

    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)md5Hash[i];
    }

    return ss.str();
}

std::string Command::getFileMD5(const std::string& filename) {
    unsigned char md5Hash[MD5_DIGEST_LENGTH];
    MD5_CTX md5Context;
    MD5_Init(&md5Context);

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return "";
    }

    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
        MD5_Update(&md5Context, buffer, file.gcount());
    }
    MD5_Update(&md5Context, buffer, file.gcount());

    MD5_Final(md5Hash, &md5Context);

    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)md5Hash[i];
    }

    return ss.str();
}

void Command::CopyDirectoryRecursive(const wxString& sourceDir, const wxString& targetDir) {
    wxDir dir(sourceDir);

    if (!dir.IsOpened()) {
        return;
    }

    if (!wxDirExists(targetDir)) {
        wxFileName::Mkdir(targetDir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }

    wxString filename;
    bool cont = dir.GetFirst(&filename);
    while (cont) {
        wxString sourcePath = sourceDir + "/" + filename;
        wxString targetPath = targetDir + "/" + filename;

        if (wxFileExists(sourcePath)) {
            if (!wxCopyFile(sourcePath, targetPath)) {
                // 错误处理
            }
        }

        if (wxDirExists(sourcePath)) {
            CopyDirectoryRecursive(sourcePath, targetPath);
        }

        cont = dir.GetNext(&filename);
    }
}

bool Command::stringToBool(const std::string& str) {
    // Convert the string to lower case for case-insensitive comparison
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    // Check for common true values
    if (lowerStr == "1" || lowerStr == "true" || lowerStr == "yes") {
        return true;
    }

    // Check for common false values
    if (lowerStr == "0" || lowerStr == "false" || lowerStr == "no" || lowerStr.empty()) {
        return false;
    }

    // Default case (if the string is not recognized)
    //#ifdef DEBUG
    throw std::invalid_argument("Invalid string for conversion to bool");
    //#endif
}
std::string Command::GetDirName(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) {
        return ".";
    }
    return path.substr(0, pos);
}

std::string Command::getFileNameWithoutExtension(const std::string& filePath) {
    fs::path path(filePath);
    return path.stem().string();
}
bool Command::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

std::string Command::ConvertToUTF8(const std::string& input) {
    wxString wxStr(input);  // 直接将 std::string 转换为 wxString
    return std::string(wxStr.ToUTF8().data());  // 转换为 UTF-8 编码的 std::string
}

std::string Command::RemoveHTMLTags(const std::string& input) {
    std::regex htmlTagRegex("<[^>]*>");
    return std::regex_replace(input, htmlTagRegex, "");
}
