#ifndef COMMAND_H
#define COMMAND_H

#include <wx/wx.h>
#include <string>
#include "CustomButton.h"
#include <wx/tglbtn.h>
#include <wx/snglinst.h>
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif

bool IsDarkModeEnabledcommand();  // 函数声明

#ifdef __cplusplus
}
#endif
namespace fs = std::filesystem;
//extern bool isDarkMode;
//extern wxBitmap bitmapbubj, bitmapbubj2, bitmapOn, bitmapOff;
//extern wxColour backgroundColor, textColor, customColor, customColor2;

class Command {
public:
    Command();  // 构造函数声明
    ~Command(); // 析构函数声明
    //bool isDarkMode;
    ////以下调用方式
    ////Command cmd
    ////cmd.fun()
    bool CheckRun(wxFrame* m_frame);//检查程序是否在运行
    void ForceDeleteFile(const std::string& filePath);//强制删除文件或目录
    void saveToFile(const std::string& filename, const std::string& data);//数据保存到文件：saveToFile(filenamept.ToStdString(), ptfilePath.ToStdString());
    std::string loadFromFile(const std::string& filename);//std::string loadedDatapt = loadFromFile(filenamept.ToStdString());
    void ReplaceAll(std::string& str, const std::string& search, const std::string& replace);
                //std::string loadedDatapt = loadFromFile(filePathmt.ToStdString());
                //wxLogMessage("loadedDatapt:'%s'.", loadedDatapt);
                //ReplaceAll(loadedDatapt, "\r\n", "\n");
    void AddToLibraryPath(const wxString& path);//AddToLibraryPath(myPath)
    std::string getCommandOutput(const std::string& command);//getCommandOutput("/Library/Frameworks/Python.framework/Versions/Current/bin/python --version 2>&1");

    bool downloadFile(const std::string& url, const std::string& filename);//if (downloadFile(url, filenametemp)) {
    std::string getStringMD5(const std::string& input);//std::string urlmd5 = getStringMD5("thisis test!");
    std::string getFileMD5(const std::string& filename);//std::string filepath = "/etc/the/ffsf.mp4";std::string md5 = getFileMD5(filepath);
    void CopyDirectoryRecursive(const wxString& sourceDir, const wxString& targetDir);
    bool stringToBool(const std::string& str);
    std::string GetDirName(const std::string& path);
    std::string getFileNameWithoutExtension(const std::string& filePath);
    bool fileExists(const std::string& filename);
    std::string ConvertToUTF8(const std::string& input);
    std::string RemoveHTMLTags(const std::string& input);

    ////以下调用方式:Command::fun()
    static void SetColorsAndStylesRecursiveN(wxWindow* window, wxPanel* Getpanel);
    static void SetColorsAndStylesRecursive(wxWindow* window, const wxColour& textColour, const wxColour& bgColour, const wxColour& customColor, const wxColour& customColor2, bool noBorder);
    static void SetupEnvironment();
    static std::string NormalizePath(const std::string& path);
    static wxString ClearAllSpace(const wxString& str);
    static void CenterWindow(wxWindow* window);
    static void PlayVideo(const wxString& filePath);
    static bool IsDarkMode();
    static int ExecCommand(const std::string& command, std::string* output);//if (Command::ExecCommand("command -v /usr/local/bin/python3 > /dev/null 2>&1") == 0)
    static bool CheckPip(const std::string& pythonPath);
    static bool CheckPythonVersion(const std::string& pythonPath);

    static void InstallPythonPackage(const std::string& pythonPath, const std::string& package, const std::string& pipName);
#ifdef _WIN32
    static HWND FindWindowByPartialTitle(const wxString& partialTitle);
#endif


private:
    wxSingleInstanceChecker* m_checker;
};
#endif // COMMAND_H