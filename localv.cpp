#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/statbmp.h>
#include <wx/mstream.h>
#include <wx/tglbtn.h>
#include <wx/dialog.h>
#include <wx/timer.h>
#include <wx/file.h>
#include <wx/menu.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/filefn.h>
#include <wx/filedlg.h>
#include <wx/stdpaths.h>
#include <iomanip>
#include <libgen.h>
#include <cstring>
#include <vector>
#include <string>
#include <mariadb/conncpp.hpp>
#include <curl/curl.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <future>
#include <regex>
#include <algorithm>
#include <cctype>
#include <cwctype>
#include <iostream>
#include <wx/base64.h>
#include <sstream>
#include <wx/log.h>
#include <memory>
#include <wx/font.h>
#include <cmath>
#include <unistd.h>
#include <wx/display.h>
#include <cstdlib>
#include <dlfcn.h>
#include <sys/stat.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtextstyles.h>
#include <wx/html/htmlwin.h>
#include <wx/choice.h>
#include <map>
#include "Command.h"
#include "pic/h/yybj.h"
#include "pic/h/yybj2.h"
#include "pic/h/play1.h"
#include "ImageButton.h"
#include "AutoCloseDialog.h"

std::string downloadDir = "";
std::string runDir = "";
std::string m_videoname = "";
sql::SQLString user("");
sql::SQLString pwd("");
std::string dbhost = "";
namespace fs = std::filesystem;
const int m_limit = 15;
int offsetps = 0;
Command zycmd;

enum {
    ID_MY_CUSTOM_ACTION = wxID_HIGHEST + 1,
    ID_ANOTHER_ACTION = wxID_HIGHEST + 2,
    ID_HELP = wxID_HIGHEST + 3,
    ID_ABOUT,
    ID_SHOW_HIDE,
    ID_EXIT
};

bool create_directory_recursive(const std::string& dir_path) {
    try {
        fs::path path(dir_path);
        if (fs::exists(path)) {
            return false;
        }
        if (fs::create_directories(path)) {
            return true;
        } else {
            return false;
        }
    } catch (const std::exception& e) {
        return false;
    }
}

std::string RemoveInvalidChars(const std::string& input) {
    std::string result = input;
    result.erase(std::remove_if(result.begin(), result.end(),
        [](unsigned char c) {
            return c < 0x20 || c == static_cast<unsigned char>(0xFFFD);
        }), result.end());
    return result;
}

std::string RemoveReplacementChar(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if ((unsigned char)str[i] != 0xEF || (unsigned char)str[i + 1] != 0xBF || (unsigned char)str[i + 2] != 0xBD) {
            result += str[i];
        } else {
            i += 2;
        }
    }
    return result;
}

std::string trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    while (start < end && std::isspace(str[start])) {
        ++start;
    }
    while (end > start && std::isspace(str[end - 1])) {
        --end;
    }
    return str.substr(start, end - start);
}

std::string Trimstr(const std::string& str) {
    auto start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    auto end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

void Trim(std::string& str) {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !(std::isspace(static_cast<int>(ch)) || static_cast<int>(ch) == 0x3000);
    }));
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !(std::isspace(static_cast<int>(ch)) || static_cast<int>(ch) == 0x3000);
    }).base(), str.end());
}

void RemoveAllSpaces(std::string& str) {
    str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char ch) {
        return std::isspace(static_cast<int>(ch)) || static_cast<int>(ch) == 0x3000;
    }), str.end());
}

void RemoveExtraSpaces(std::string& str) {
    auto endPos = std::unique(str.begin(), str.end(), [](unsigned char a, unsigned char b) {
        return (std::isspace(static_cast<int>(a)) || static_cast<int>(a) == 0x3000) &&
               (std::isspace(static_cast<int>(b)) || static_cast<int>(b) == 0x3000);
    });
    str.erase(endPos, str.end());
}

void TrimAndRemoveExtraSpaces(std::string& str) {
    Trim(str);
    RemoveExtraSpaces(str);
}

void processPart(const std::string& segmentData, std::ofstream& outputFile) {
    size_t startPos = 0;
    size_t endPos;
    while ((endPos = segmentData.find('#', startPos)) != std::string::npos) {
        std::string segment = segmentData.substr(startPos, endPos - startPos);
        size_t delimiterPos = segment.find('$');
        if (delimiterPos != std::string::npos) {
            std::string title = segment.substr(0, delimiterPos);
            std::string url = segment.substr(delimiterPos + 1);
            if (url.find("kuaikan") == std::string::npos && url.find("leshi") == std::string::npos) {
                outputFile << "#EXTINF:," << m_videoname + "[" + wxString::FromUTF8(title) + "]" << "\n";
                outputFile << url << "\n";
            }
        }
        startPos = endPos + 1;
    }
    if (startPos < segmentData.size()) {
        std::string segment = segmentData.substr(startPos);
        size_t delimiterPos = segment.find('$');
        if (delimiterPos != std::string::npos) {
            std::string title = segment.substr(0, delimiterPos);
            std::string url = segment.substr(delimiterPos + 1);
            if (url.find("kuaikan") == std::string::npos && url.find("leshi") == std::string::npos) {
                outputFile << "#EXTINF:," << m_videoname + "[" + wxString::FromUTF8(title) + "]" << "\n";
                outputFile << url << "\n";
            }
        }
    }
}

void generateM3U8(const std::string& input, const std::string& outputFilename) {
    std::ofstream outputFile(outputFilename);
    if (!outputFile.is_open()) {
        #ifdef DEBUG
        std::cerr << "无法打开文件: " << outputFilename << std::endl;
        #endif
        return;
    }
    outputFile << "#EXTM3U\n";
    size_t startPos = 0;
    size_t splitPos;
    while ((splitPos = input.find("$$$", startPos)) != std::string::npos) {
        std::string segmentData = input.substr(startPos, splitPos - startPos);
        processPart(segmentData, outputFile);
        startPos = splitPos + 3;
    }
    if (startPos < input.size()) {
        std::string lastSegment = input.substr(startPos);
        processPart(lastSegment, outputFile);
    }
    outputFile.close();
    #ifdef DEBUG
    std::cout << "M3U8 文件已生成: " << outputFilename << std::endl;
    #endif
}

void changeFilePermissions(const std::string& filePath) {
    if (chmod(filePath.c_str(), 0777) == -1) {
        #ifdef DEBUG
        perror("Error changing file permissions");
        #endif
    } else {
        #ifdef DEBUG
        std::cout << "Permissions changed successfully!" << std::endl;
        #endif
    }
}

void commPlayVideo(wxWindow* parent, const std::string& filePath) {
    AutoCloseDialog* dlg = new AutoCloseDialog(parent, wxString::FromUTF8("正在打开视频，请稍候..."), 3000);
    dlg->Show();
    std::string input = filePath.c_str();
    std::string outputFilename = downloadDir + "/playlistl.m3u8";
    generateM3U8(input, outputFilename);
    changeFilePermissions(outputFilename);
    try {
        auto fileSize = std::filesystem::file_size(outputFilename);
        if (fileSize < 5) {
            #ifdef DEBUG
            std::cout << "File size is less than 32 bytes. Performing additional actions..." << std::endl;
            #endif
            std::filesystem::remove(outputFilename);
            #ifdef DEBUG
            std::cout << "File removed: " << outputFilename << std::endl;
            #endif
            wxMessageBox("资源已经失效。", "提示", wxOK | wxICON_INFORMATION);
        } else {
            #ifdef DEBUG
            std::cout << "File generated successfully. Size: " << fileSize << " bytes." << std::endl;
            #endif
        }
    } catch (const std::exception& e) {
        #ifdef DEBUG
        std::cerr << "Error checking file size: " << e.what() << std::endl;
        #endif
        wxMessageBox("资源已经失效。", "提示", wxOK | wxICON_INFORMATION);
    }
#ifdef _WIN32
    wxString command = wxString::Format("cmd /c start \"\" \"%s\"", filePath);
#elif defined(__APPLE__)
    wxStandardPaths& stdPaths = wxStandardPaths::Get();
    wxString resourceexecutableFilePath = stdPaths.GetResourcesDir() + "/IINA.app/Contents/MacOS/iina-cli";
    wxString cmd;
    cmd = "/bin/sh -c \"ps -ef | grep -i iina | grep -v grep | awk '{print \\\"kill -15 \\\" $2}' | sh\"";
    wxExecute(cmd, wxEXEC_SYNC | wxEXEC_HIDE_CONSOLE);
    wxString command = wxString::Format("%s --no-stdin --separate-windows --playlist \"%s\"", resourceexecutableFilePath, outputFilename.c_str());
#else
    wxString command = wxString::Format("xdg-open \"%s\"", filePath);
#endif
    wxExecute(command, wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE);
}

class Person {
public:
    std::string furl;
    std::string fdir;
    std::string fname;
    std::string img;
    std::string count;

    Person(std::string n, std::string g, std::string b, std::string img, std::string u)
        : furl(n), fdir(g), fname(b), img(img), count(u) {}

    bool operator==(const Person& other) const {
        return trim(furl) == trim(other.furl) && trim(fname) == trim(other.fname);
    }

    std::string to_json() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"furl\": \"" << trim(furl) << "\",\n";
        oss << "  \"fdir\": \"" << trim(fdir) << "\",\n";
        oss << "  \"fname\": \"" << trim(fname) << "\",\n";
        oss << "  \"img\": \"" << trim(img) << "\",\n";
        oss << "  \"count\": \"" << trim(count) << "\",\n";
        oss << "}";
        return oss.str();
    }

    static Person from_json(const std::string& json_str) {
        Person p("", "", "", "", "");
        size_t pos = 0;
        pos = json_str.find("\"furl\": \"") + 9;
        p.furl = json_str.substr(pos, json_str.find("\"", pos) - pos);
        pos = json_str.find("\"fdir\": \"") + 9;
        p.fdir = json_str.substr(pos, json_str.find("\"", pos) - pos);
        pos = json_str.find("\"fname\": \"") + 10;
        p.fname = json_str.substr(pos, json_str.find("\"", pos) - pos);
        pos = json_str.find("\"img\": \"") + 8;
        p.img = json_str.substr(pos, json_str.find("\"", pos) - pos);
        pos = json_str.find("\"count\": \"") + 10;
        p.count = json_str.substr(pos, json_str.find("\"", pos) - pos);
        return p;
    }

    void print() const {
        std::cout << "Furl: " << furl << "\n";
        std::cout << "Fdir: " << fdir << "\n";
        std::cout << "Fname: " << fname << "\n";
        std::cout << "Img: " << img << "\n";
        std::cout << "Count: " << count << "\n";
    }
};

void save_to_json(const std::vector<Person>& people, const std::string& filename) {
    std::ofstream outFile(filename);
    outFile << "[\n";
    for (size_t i = 0; i < people.size(); ++i) {
        outFile << "  " << people[i].to_json();
        if (i != people.size() - 1) {
            outFile << ",";
        }
        outFile << "\n";
    }
    outFile << "]\n";
}

std::vector<Person> read_from_json(const std::string& filename) {
    std::ifstream inFile(filename);
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();
    std::vector<Person> people;
    size_t pos = 0;
    while ((pos = content.find("{", pos)) != std::string::npos) {
        size_t end_pos = content.find("}", pos);
        std::string person_str = content.substr(pos, end_pos - pos + 1);
        people.push_back(Person::from_json(person_str));
        pos = end_pos + 1;
    }
    return people;
}

void saveHistory(const Person& p, const std::string& filename) {
    std::vector<Person> people = read_from_json(filename);
    auto it = std::find(people.begin(), people.end(), p);
    if (it != people.end()) {
        #ifdef DEBUG
        std::cout << "Record already exists, moving it to the front." << std::endl;
        #endif
        people.erase(it);
    }
    people.insert(people.begin(), p);
    if (people.size() > m_limit) {
        people.pop_back();
    }
    save_to_json(people, filename);
}

std::string ExtractValue(const std::string& jsonObject, const std::string& key) {
    size_t startPos = jsonObject.find("\"" + key + "\":");
    if (startPos == std::string::npos) {
        #ifdef DEBUG
        std::cerr << "Key \"" << key << "\" not found in JSON object!" << std::endl;
        #endif
        return "";
    }
    startPos += key.length() + 3;
    if (jsonObject[startPos] == '"') {
        size_t endPos = startPos + 1;
        bool inEscape = false;
        while (endPos < jsonObject.size()) {
            if (jsonObject[endPos] == '"' && !inEscape) {
                break;
            }
            if (jsonObject[endPos] == '\\') {
                inEscape = !inEscape;
            } else {
                inEscape = false;
            }
            ++endPos;
        }
        if (endPos == jsonObject.size()) {
            #ifdef DEBUG
            std::cerr << "No closing quote found for key \"" << key << "\"!" << std::endl;
            #endif
            return "";
        }
        return trim(jsonObject.substr(startPos + 1, endPos - startPos - 1));
    }
    size_t endPos = jsonObject.find_first_of(",}", startPos);
    if (endPos == std::string::npos) {
        endPos = jsonObject.size();
    }
    return trim(jsonObject.substr(startPos, endPos - startPos));
}

std::string ExtractPlayUrl(const std::string& jsonObject) {
    size_t startPos = jsonObject.find("\"playUrl\":");
    if (startPos == std::string::npos) {
        #ifdef DEBUG
        std::cerr << "Key \"playUrl\" not found in JSON object!" << std::endl;
        #endif
        return "";
    }
    startPos += 10;
    while (std::isspace(jsonObject[startPos])) {
        ++startPos;
    }
    if (jsonObject[startPos] == '"') {
        size_t endPos = startPos + 1;
        bool inEscape = false;
        while (endPos < jsonObject.size()) {
            if (jsonObject[endPos] == '"' && !inEscape) {
                break;
            }
            if (jsonObject[endPos] == '\\') {
                inEscape = !inEscape;
            } else {
                inEscape = false;
            }
            ++endPos;
        }
        if (endPos == jsonObject.size()) {
            #ifdef DEBUG
            std::cerr << "No closing quote found for playUrl!" << std::endl;
            #endif
            return "";
        }
        return trim(jsonObject.substr(startPos + 1, endPos - startPos - 1));
    }
    size_t endPos = jsonObject.find_first_of(",}", startPos);
    if (endPos == std::string::npos) {
        endPos = jsonObject.size();
    }
    return trim(jsonObject.substr(startPos, endPos - startPos));
}

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title);
    void LoadData(int offset, const std::string& keyword = "", bool loadFromJson = false, const std::string& typeId = "");
    std::string buildVideoUrl(const std::string& fdir);
    void OnNext(wxCommandEvent& event);
    void OnPrevious(wxCommandEvent& event);
    void OnSearch(wxCommandEvent& event);
    void OnHistory(wxCommandEvent& event);
    void OnChoiceSelected(wxCommandEvent& event);
    void OnTextCtrlKey(wxKeyEvent& event);
    void OnImageClick(wxMouseEvent& event);
    void DownloadImageAsync(const std::string& url, const std::string& savePath, int index);
    void OnImageDownloaded(wxCommandEvent& event);
    void OnClosewindowClicked(wxCloseEvent& event);
    void PrintPeople();
    void OnMainWindowInitComplete() {
        isInterfaceReady = true;
    }

private:
    wxBoxSizer* mainSizer;
    wxGridSizer* m_sizer;
    wxBoxSizer* buttonSizer;
    wxScrolledWindow* m_panel;
    wxTextCtrl* searchTextCtrl;
    wxButton* searchButton;
    wxButton* historyButton;
    wxButton* prevButton;
    wxButton* nextButton;
    wxStaticText* pageText;
    std::vector<Person> m_people;
    std::vector<wxStaticBitmap*> m_imageControls;
    int m_offset;
    std::string m_keyword = "";
    std::string m_typeId = "";
    bool m_loadFromJson;
    std::unique_ptr<sql::Connection> conn;
    bool isInterfaceReady = false;
    wxComboBox* typeChoice;
    std::map<std::string, wxString> typeMap;
    wxDECLARE_EVENT_TABLE();
};

class MyApp : public wxApp {
public:
    virtual bool OnInit();
    virtual int OnExit();
private:
    MyFrame* frame = nullptr;
};

class MyLog : public wxLog {
public:
    void DoLogString(const wxString& msg) {
    }
};
#define MY_BASE_ID (wxID_HIGHEST + 1)
enum { ID_Next = MY_BASE_ID, ID_Previous, ID_ImageDownloaded, ID_searchButton, ID_historyButton };

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_BUTTON(ID_Next, MyFrame::OnNext)
    EVT_BUTTON(ID_Previous, MyFrame::OnPrevious)
    EVT_BUTTON(ID_searchButton, MyFrame::OnSearch)
    EVT_BUTTON(ID_historyButton, MyFrame::OnHistory)
    EVT_COMMAND(ID_ImageDownloaded, wxEVT_COMMAND_BUTTON_CLICKED, MyFrame::OnImageDownloaded)
wxEND_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
    Command::SetupEnvironment();

    if (frame) {
    	Command cmd;
      if (cmd.CheckRun(frame)){
      	return true;
      }
    };
    std::string homeDir = wxGetHomeDir().ToStdString();
    runDir = homeDir + "/zsprundir";
    create_directory_recursive(runDir);
    downloadDir = runDir + "/download";
    create_directory_recursive(downloadDir);
    wxInitAllImageHandlers();
    frame = new MyFrame(wxString::FromUTF8("周视频"));
    frame->Show(true);
    //frame->SetWindowStyle(frame->GetWindowStyle() | wxSTAY_ON_TOP);

    std::string startplayFilename = downloadDir + "/playlistl.m3u8";
    if (zycmd.fileExists(startplayFilename)) {
        auto fileSize = std::filesystem::file_size(startplayFilename);
        if (fileSize > 20) {
#ifdef _WIN32
            wxString command = wxString::Format("cmd /c start \"\" \"%s\"", filePath);
#elif defined(__APPLE__)
            wxStandardPaths& stdPaths = wxStandardPaths::Get();
            wxString resourceexecutableFilePath = stdPaths.GetResourcesDir() + "/IINA.app/Contents/MacOS/iina-cli";
            wxString cmd;
            cmd = "/bin/sh -c \"ps -ef | grep -i iina | grep -v grep | awk '{print \\\"kill -15 \\\" $2}' | sh\"";
            wxExecute(cmd, wxEXEC_SYNC | wxEXEC_HIDE_CONSOLE);
            wxString command = wxString::Format("%s --no-stdin --separate-windows \"%s\"", resourceexecutableFilePath, startplayFilename.c_str());
#else
            wxString command = wxString::Format("xdg-open \"%s\"", filePath);
#endif
            wxExecute(command, wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE);
        }
    }
    return true;
}

int MyApp::OnExit() {
    return wxApp::OnExit();
}

MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)), m_offset(0) {
    wxDisplay display;
    wxRect screenSize = display.GetGeometry();
    SetSize(screenSize.GetWidth(), screenSize.GetHeight() - 30);
    Centre(wxBOTH);
    m_panel = new wxScrolledWindow(this);
    m_panel->SetScrollRate(5, 5);
    mainSizer = new wxBoxSizer(wxVERTICAL);
    m_sizer = new wxGridSizer(3, 5, 5, 5);
    buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* searchSizer = new wxBoxSizer(wxHORIZONTAL);
    searchTextCtrl = new wxTextCtrl(m_panel, wxID_ANY, "", wxDefaultPosition, wxSize(200, 30));
    searchButton = new wxButton(m_panel, ID_searchButton, wxString::FromUTF8("搜索"));
    historyButton = new wxButton(m_panel, ID_historyButton, wxString::FromUTF8("历史记录"));
    typeChoice = new wxComboBox(m_panel, wxID_ANY,"", wxDefaultPosition, wxSize(200, 30), 0, nullptr, wxCB_READONLY);
    typeChoice->Append(wxString::FromUTF8("推荐栏目"), (void*)nullptr);
    searchSizer->Add(typeChoice, 0, wxALL, 5);
    typeChoice->Bind(wxEVT_COMBOBOX, &MyFrame::OnChoiceSelected, this);
    pageText = new wxStaticText(m_panel, wxID_ANY, wxString::FromUTF8("第1页"));
    pageText->SetMinSize(wxSize(100,30));
    pageText->SetMaxSize(wxSize(100,30));
    searchSizer->Add(pageText, 0, wxEXPAND | wxALL, 5);
    searchSizer->Add(searchTextCtrl, 0, wxEXPAND | wxALL, 5);
    searchSizer->Add(searchButton, 0, wxALL, 5);
    searchSizer->Add(historyButton, 0, wxALL, 5);
    m_panel->SetSizer(mainSizer);
    mainSizer->Add(searchSizer, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);
    mainSizer->Add(m_sizer, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);
    mainSizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);
    prevButton = new wxButton(m_panel, ID_Previous, wxString::FromUTF8("上页"));
    nextButton = new wxButton(m_panel, ID_Next, wxString::FromUTF8("下页"));
    buttonSizer->Add(prevButton, 0, wxEXPAND | wxALL, 5);
    buttonSizer->Add(nextButton, 0, wxEXPAND | wxALL, 5);
    searchTextCtrl->Bind(wxEVT_KEY_DOWN, &MyFrame::OnTextCtrlKey, this);

    try {
        sql::Driver* driver = sql::mariadb::get_driver_instance();
        conn = std::unique_ptr<sql::Connection>(driver->connect(dbhost, user, pwd));
        LoadData(m_offset, "", false, "");
    }
    catch (sql::SQLException& e) {
        wxString message = wxString::Format(wxString::FromUTF8("数据库连接失败: %s"), e.what());
        wxMessageBox(message, "提示", wxOK | wxICON_INFORMATION);
    }

    try {
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM f_search_b WHERE fdir NOT LIKE '%孜孜不倦%' AND fdir NOT LIKE '%评书%' AND fdir NOT LIKE '%相声%' GROUP BY fdirp ORDER BY convert(fdirp USING gbk) DESC LIMIT 560 OFFSET 0"));
        while (res->next()) {
            std::string* typeId = new std::string(res->getString("fdirp"));
            wxString typeName = wxString::FromUTF8(res->getString("fdirp").c_str());
            typeMap[*typeId] = typeName;
            typeChoice->Append(typeName, reinterpret_cast<void*>(typeId));
        }
        if (!typeMap.empty()) {
            typeChoice->SetSelection(0);
        }
    }
    catch (sql::SQLException& e) {
        wxString message = wxString::Format(wxString::FromUTF8("加载类型数据失败: %s"), e.what());
        wxMessageBox(message, "提示", wxOK | wxICON_ERROR);
    }

    Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnClosewindowClicked, this);
#ifdef _WIN32
    this->SetTransparent(250);
#else
    this->SetTransparent(255);
#endif
    OnMainWindowInitComplete();
}

bool isConnectionValid(std::unique_ptr<sql::Connection>& conn) {
    try {
        if (conn && conn->isValid()) {
            return true;
        }
    } catch (sql::SQLException& e) {
        wxLogError(wxString::Format(wxString::FromUTF8("连接验证失败: %s"), e.what()));
    }
    return false;
}

std::string MyFrame::buildVideoUrl(const std::string& fdir) {
    if (!isConnectionValid(conn)) {
        try {
            sql::Driver* driver = sql::mariadb::get_driver_instance();
            conn = std::unique_ptr<sql::Connection>(driver->connect(dbhost, user, pwd));
            AutoCloseDialog* dlg = new AutoCloseDialog(this, wxString::FromUTF8("数据库连接重新建立"), 3000);
            dlg->Show();
        } catch (sql::SQLException& e) {
            wxString message = wxString::Format(wxString::FromUTF8("数据库连接失败: %s"), e.what());
            wxMessageBox(message, "错误", wxOK | wxICON_ERROR);
            return "";
        }
    }
    if (!conn) {
        try {
            sql::Driver* driver = sql::mariadb::get_driver_instance();
            conn = std::unique_ptr<sql::Connection>(driver->connect(dbhost, user, pwd));
            AutoCloseDialog* dlg = new AutoCloseDialog(this, wxString::FromUTF8("数据库连接重新建立"), 3000);
            dlg->Show();
        } catch (sql::SQLException& e) {
            wxString message = wxString::Format(wxString::FromUTF8("数据库连接失败: %s"), e.what());
            wxMessageBox(message, "错误", wxOK | wxICON_ERROR);
        }
    }
    std::string sqlQuery;
    std::unique_ptr<sql::PreparedStatement> pstmt;
    sqlQuery = "SELECT * FROM f_search WHERE fdir = ? ORDER BY pid ASC";
    pstmt.reset(conn->prepareStatement(sqlQuery));
    pstmt->setString(1, fdir);
    std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
    wxString resultinfo = "";
    while (res->next()) {
        std::string stdString = res->getString("furl").c_str();
        zycmd.ReplaceAll(stdString, " ", "%20");
        zycmd.ReplaceAll(stdString, "#", "%23");
        zycmd.ReplaceAll(stdString, "$", "%24");
        zycmd.ReplaceAll(stdString, "/wwwvideos", "https://mv.6deng.cn:9443/wwwvideos");
        std::string stdString2 = zycmd.getFileNameWithoutExtension(res->getString("fname").c_str());
        zycmd.ReplaceAll(stdString2, " ", "");
        zycmd.ReplaceAll(stdString2, "#", "");
        zycmd.ReplaceAll(stdString2, "$", "");
        resultinfo += wxString(stdString2) + "$" + wxString(stdString) + "#";
    }
    return std::string(resultinfo.ToStdString());
}

void MyFrame::LoadData(int offset, const std::string& keyword, bool loadFromJson, const std::string& typeId) {
    prevButton->Enable(false);
    nextButton->Enable(false);
    try {
    if (loadFromJson) {
        std::string jsonfilename = downloadDir + "/videol.json";
        std::ifstream jsonFile(jsonfilename);
        if (jsonFile.is_open()) {
            std::string jsonContent((std::istreambuf_iterator<char>(jsonFile)),
                                     std::istreambuf_iterator<char>());
            jsonFile.close();
            #ifdef DEBUG
            std::cout << "JSON Content Loaded: " << std::endl;
            std::cout << jsonContent << std::endl;
            #endif
            m_sizer->Clear(true);
            m_people.clear();
            m_imageControls.clear();
            size_t pos = 0;
            wxString pageInfo = wxString::Format(wxString::FromUTF8("当前页: 1"));
            jsonContent.erase(0, jsonContent.find("[") + 1);
            jsonContent.erase(jsonContent.find("]"), jsonContent.size());
            while ((pos = jsonContent.find("{")) != std::string::npos) {
                size_t endPos = jsonContent.find("}", pos);
                if (endPos == std::string::npos) break;
                std::string jsonObject = jsonContent.substr(pos + 1, endPos - pos - 1);
                jsonContent.erase(pos, endPos - pos + 1);
                #ifdef DEBUG
                std::cout << "Parsing Object: " << std::endl;
                std::cout << jsonObject << std::endl;
                #endif
                std::string furl, fdir, fname, img, count;
                furl = ExtractValue(jsonObject, "furl");
                zycmd.ReplaceAll(furl, "\"", "");
                trim(furl);
                fdir = ExtractValue(jsonObject, "fdir");
                zycmd.ReplaceAll(fdir, "\"", "");
                trim(fdir);
                fname = ExtractValue(jsonObject, "fname");
                zycmd.ReplaceAll(fname, "\"", "");
                trim(fname);
                img = ExtractValue(jsonObject, "img");
                zycmd.ReplaceAll(img, "\"", "");
                trim(img);
                count = ExtractValue(jsonObject, "count");
                zycmd.ReplaceAll(count, "\"", "");
                trim(count);
                #ifdef DEBUG
                std::cout << "Furl: " << furl << std::endl;
                std::cout << "Fdir: " << fdir << std::endl;
                std::cout << "Fname: " << fname << std::endl;
                std::cout << "Img: " << img << std::endl;
                std::cout << "Count: " << count << std::endl;
                #endif
                m_people.emplace_back(furl, fdir, fname, img, count);
            }
        } else {
            #ifdef DEBUG
            std::cerr << wxString::FromUTF8("无法打开 JSON 文件: ") << jsonfilename << std::endl;
            #endif
            AutoCloseDialog* dlg = new AutoCloseDialog(this, wxString::FromUTF8("没有观看记录"), 3000);
            dlg->Show();
            return;
        }
    } else {
        if (!isConnectionValid(conn)) {
            try {
                sql::Driver* driver = sql::mariadb::get_driver_instance();
                conn = std::unique_ptr<sql::Connection>(driver->connect(dbhost, user, pwd));
                AutoCloseDialog* dlg = new AutoCloseDialog(this, wxString::FromUTF8("数据库连接重新建立"), 3000);
                dlg->Show();
            } catch (sql::SQLException& e) {
                wxString message = wxString::Format(wxString::FromUTF8("数据库连接失败: %s"), e.what());
                wxMessageBox(message, "错误", wxOK | wxICON_ERROR);
                return;
            }
        }
        if (!conn) {
            try {
                sql::Driver* driver = sql::mariadb::get_driver_instance();
                conn = std::unique_ptr<sql::Connection>(driver->connect(dbhost, user, pwd));
                AutoCloseDialog* dlg = new AutoCloseDialog(this, wxString::FromUTF8("数据库连接重新建立"), 3000);
                dlg->Show();
            } catch (sql::SQLException& e) {
                wxString message = wxString::Format(wxString::FromUTF8("数据库连接失败: %s"), e.what());
                wxMessageBox(message, "错误", wxOK | wxICON_ERROR);
            }
        }
        std::string sqlQuery;
        std::unique_ptr<sql::PreparedStatement> pstmt;
        if (keyword != "") {
            if (typeId != "") {
                sqlQuery = "SELECT pid,furl,fdir,fdir as fdirp,fname,img,1 as count,1 as ee FROM f_search WHERE fdir LIKE ? AND fname LIKE ? group by fdir ORDER BY convert(fdir USING gbk) ASC LIMIT ? OFFSET ?";
                pstmt.reset(conn->prepareStatement(sqlQuery));
                pstmt->setString(1, "%" + typeId + "%");
                pstmt->setString(2, "%" + keyword + "%");
                pstmt->setInt(3, m_limit);
                pstmt->setInt(4, offset);
            } else {
                sqlQuery = "SELECT pid,furl,fdir,fdir as fdirp,fname,img,1 as count,1 as ee FROM f_search WHERE fdir LIKE ? OR fname LIKE ? group by fdir ORDER BY convert(fdir USING gbk) ASC LIMIT ? OFFSET ?";
                pstmt.reset(conn->prepareStatement(sqlQuery));
                pstmt->setString(1, "%" + keyword + "%");
                pstmt->setString(2, "%" + keyword + "%");
                pstmt->setInt(3, m_limit);
                pstmt->setInt(4, offset);
            }
        } else {
            if (typeId != "") {
                sqlQuery = "SELECT * FROM f_search_b WHERE fdir LIKE ? group by fdir ORDER BY convert(fdir USING gbk) ASC LIMIT ? OFFSET ?";
                pstmt.reset(conn->prepareStatement(sqlQuery));
                pstmt->setString(1, "%" + typeId + "%");
                pstmt->setInt(2, m_limit);
                pstmt->setInt(3, offset);
            } else {
                sqlQuery = "SELECT * FROM f_search_b WHERE fdir NOT LIKE '%孜孜不倦%' AND fdir NOT LIKE '%有声书%' AND fdir NOT LIKE '%评书%' AND fdir NOT LIKE '%相声%' AND fdir NOT LIKE '%音乐%' AND fdir NOT LIKE '%公开课%' AND fdir NOT LIKE '%直播软件教学%' AND fdir NOT LIKE '%TED演讲集%' AND fdir NOT LIKE '%AdobeAudition零基础教程%' AND fdir NOT LIKE '%朱韬教材%' AND fdir NOT LIKE '%达芬奇学习%' AND fdir NOT LIKE '%有声读物%' group by fdir ORDER BY ee DESC LIMIT ? OFFSET ?";
                pstmt.reset(conn->prepareStatement(sqlQuery));
                pstmt->setInt(1, m_limit);
                pstmt->setInt(2, offset);
            }
        }
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (!res || !res->next()) {
            if (typeId == "" and keyword == "") {
                sqlQuery = "SELECT * FROM f_search_b WHERE fdir NOT LIKE '%孜孜不倦%' AND (fdir LIKE '%有声书%' OR fdir LIKE '%评书%' OR fdir LIKE '%相声%') group by fdir ORDER BY ee DESC LIMIT ? OFFSET ?";
                pstmt.reset(conn->prepareStatement(sqlQuery));
                pstmt->setInt(1, m_limit);
                pstmt->setInt(2, offsetps);
                res.reset(pstmt->executeQuery());
                offsetps += m_limit;
            }

            if (!res || !res->next()) {
                m_offset = (m_offset >= m_limit) ? m_offset - m_limit : 0;
                #ifdef DEBUG
                std::cerr << "查询未返回任何结果！" << std::endl;
                #endif
                AutoCloseDialog* dlg = new AutoCloseDialog(this, wxString::FromUTF8("没有记录了"), 3000);
                dlg->Show();
                prevButton->Enable(true);
                return;
            }
        }
        m_sizer->Clear(true);
        m_people.clear();
        m_imageControls.clear();
        int currentPage = static_cast<int>(std::ceil(static_cast<double>(m_offset) / m_limit));
        wxString pageInfo = wxString::Format(wxString::FromUTF8("当前页: %d"), currentPage + 1);
        if (pageText != nullptr) {
            pageText->SetLabel(pageInfo);
        } else {
            #ifdef DEBUG
            std::cerr << "pageText is nullptr" << std::endl;
            #endif
        }
        do {
            std::string stdString = res->getString("furl").c_str();
            zycmd.ReplaceAll(stdString, "/wwwvideos", "https://mv.6deng.cn:9443/wwwvideos");
            zycmd.ReplaceAll(stdString, " ", "%20");
            std::string imgPath = res->getString("img").c_str();
            std::string dirName = zycmd.GetDirName(imgPath);
            std::string stdString2 = dirName + "/cover.jpg";
            zycmd.ReplaceAll(stdString2, "/wwwvideos", "https://mv.6deng.cn:9443/wwwvideos");
            zycmd.ReplaceAll(stdString2, " ", "%20");
            m_people.emplace_back(stdString,
                                  res->getString("fdir").c_str(),
                                  res->getString("fname").c_str(),
                                  stdString2,
                                  res->getString("count").c_str());
        } while (res->next());
    }
    for (int i = 0; i < m_people.size(); ++i) {
        Person& p = m_people[i];
        wxPanel* personPanel = new wxPanel(m_panel);
        wxBoxSizer* personSizer = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* firstRowSizer = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* textSizer = new wxBoxSizer(wxVERTICAL);
        wxSize textSize(120, 18);
        wxSize textSize1(120, 35);
        wxSize textSize2(120, 80);
        wxSize textSize3(180, 40);
        wxSize textSize4(90, 40);
        wxBitmap bitmaploading;
        wxMemoryInputStream memStreamloading(play1_png, play1_png_len);
        wxImage imageloading(memStreamloading, wxBITMAP_TYPE_ANY);
        if (imageloading.IsOk()) {
            wxImage scaledImageloading = imageloading.Rescale(240, 150, wxIMAGE_QUALITY_HIGH);
            bitmaploading = wxBitmap(scaledImageloading);
        } else {
            bitmaploading = wxBitmap(wxSize(240, 150));
        }
        wxStaticBitmap* imgCtrl = new wxStaticBitmap(personPanel, wxID_ANY, bitmaploading);
        firstRowSizer->Add(imgCtrl, 0, wxEXPAND | wxALL, 5);
        m_imageControls.push_back(imgCtrl);
        personSizer->Add(firstRowSizer, 0, wxEXPAND | wxALL, 5);
        std::string playNmae = zycmd.getFileNameWithoutExtension(p.fdir.c_str());
        std::string playUrlthis = buildVideoUrl(p.fdir);
        std::string jsonfilename = downloadDir + "/videol.json";
        imgCtrl->Bind(wxEVT_LEFT_UP, [this, playUrlthis, playNmae, p, jsonfilename](wxMouseEvent& event) {
            m_videoname = wxString::FromUTF8(playNmae);
            commPlayVideo(this, playUrlthis);
            saveHistory(p, jsonfilename);
        });
        std::string directoryPath = downloadDir + "/pic";
        create_directory_recursive(directoryPath);
        std::string imageUrl = p.img;
        std::string imageExtension = ".jpg";
        size_t pos = imageUrl.find_last_of('.');
        if (pos != std::string::npos) {
            imageExtension = imageUrl.substr(pos);
        }
        std::string localImagePath = directoryPath + "/imagel_" + std::to_string(i) + imageExtension;
        DownloadImageAsync(imageUrl, localImagePath, i);
        std::string vname = p.fdir;
        zycmd.ReplaceAll(vname, " ", "");
        wxBoxSizer* secondRowSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* playurlText = new wxStaticText(personPanel, wxID_ANY, vname);
        playurlText->SetMinSize(textSize3);
        playurlText->SetMaxSize(textSize3);
        secondRowSizer->Add(playurlText, 0, wxEXPAND);
        wxStaticText* playurlText2 = new wxStaticText(personPanel, wxID_ANY, wxString::FromUTF8("共计:") + wxString(p.count));
        playurlText2->SetMinSize(textSize4);
        playurlText2->SetMaxSize(textSize4);
        playurlText2->SetForegroundColour(wxColour(255, 255, 0));
        secondRowSizer->Add(playurlText2, 0, wxEXPAND);
        personSizer->Add(secondRowSizer, 0, wxEXPAND | wxALL, 5);
        personPanel->SetSizer(personSizer);
        m_sizer->Add(personPanel, 0, wxEXPAND, 5);
    }
} catch (const std::exception& e) {
        wxString message = wxString::Format(wxString::FromUTF8("数据库连接失败: %s"), e.what());
        wxMessageBox(message, "提示", wxOK | wxICON_INFORMATION);
}
    m_panel->Layout();
    if (!loadFromJson) {
        prevButton->Enable(true);
        nextButton->Enable(true);
    }
}

void MyFrame::DownloadImageAsync(const std::string& url, const std::string& savePath, int index) {
    std::thread([this, url, savePath, index]() {
        CURL* curl;
        FILE* file;
        CURLcode res;
        curl = curl_easy_init();
        if (curl) {
            file = fopen(savePath.c_str(), "wb");
            if (file) {
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
                res = curl_easy_perform(curl);
                fclose(file);
                curl_easy_cleanup(curl);
                if (res == CURLE_OK) {
                    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, ID_ImageDownloaded);
                    event.SetInt(index);
                    wxQueueEvent(this, event.Clone());
                }
            }
        }
    }).detach();
}

void MyFrame::OnImageDownloaded(wxCommandEvent& event) {
    wxLogNull logNull;
    std::string currentPath = std::filesystem::current_path().string();
    wxLog* oldLog = wxLog::SetActiveTarget(new MyLog());
    int index = event.GetInt();
    std::string downloadPicDir = downloadDir + "/pic";
    create_directory_recursive(downloadPicDir);
    wxFileName localFile(downloadPicDir, "imagel_" + std::to_string(index));
    size_t pos = m_people[index].img.find_last_of('.');
    std::string imageExtension = (pos != std::string::npos) ? m_people[index].img.substr(pos) : ".jpg";
    localFile.SetFullName("imagel_" + std::to_string(index) + imageExtension);
    std::string localImagePath = localFile.GetFullPath().ToStdString();
    wxImage image;
    if (wxFileExists(localImagePath)) {
        if (image.LoadFile(localImagePath, wxBITMAP_TYPE_ANY)) {
            wxImage resizedImage = image.Rescale(240, 150, wxIMAGE_QUALITY_HIGH);
            wxBitmap bitmap(resizedImage);
            m_imageControls[index]->SetBitmap(bitmap);
        }
    }
    m_imageControls[index]->Refresh();
    wxLog::SetActiveTarget(oldLog);
}

void MyFrame::OnHistory(wxCommandEvent& event) {
    if (!isInterfaceReady) {
        wxMessageBox("主界面还未加载完成", "提示", wxOK | wxICON_INFORMATION);
        return;
    }
    m_offset = 0;
    offsetps = 0;
    LoadData(m_offset, "", true, "");
}

void MyFrame::OnSearch(wxCommandEvent& event) {
    std::string keyword = searchTextCtrl->GetValue().ToStdString();
    zycmd.ReplaceAll(keyword, " ", "");
    m_keyword = keyword;
    m_offset = 0;
    offsetps = 0;
    if (m_keyword == "") {
        m_typeId = "";
        typeChoice->SetSelection(0);
    }
    LoadData(m_offset, m_keyword, false, m_typeId);
    searchTextCtrl->SetValue(m_keyword);
}

void MyFrame::OnTextCtrlKey(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_RETURN) {
        wxCommandEvent buttonEvent(wxEVT_BUTTON, ID_searchButton);
        ProcessEvent(buttonEvent);
    } else {
        event.Skip();
    }
}

void MyFrame::OnChoiceSelected(wxCommandEvent& event) {
    int selection = typeChoice->GetSelection();
    if (selection != wxNOT_FOUND) {
        void* clientData = typeChoice->GetClientData(selection);
        if (clientData != nullptr) {
            std::string typeId = *reinterpret_cast<std::string*>(clientData);
            if (m_typeId == typeId) {
                return;
            }
            m_typeId = typeId;
        } else {
            if (m_typeId == "") {
                return;
            }
            m_typeId = "";
        }
        m_offset = 0;
        offsetps = 0;
        LoadData(m_offset, "", false, m_typeId);
        searchTextCtrl->SetValue("");
    }
}

void MyFrame::OnNext(wxCommandEvent& event) {
    m_offset += m_limit;
    LoadData(m_offset, m_keyword, false, m_typeId);
}

void MyFrame::OnPrevious(wxCommandEvent& event) {
    if (m_offset > 0) {
        m_offset -= m_limit;
        LoadData(m_offset, m_keyword, false, m_typeId);
    } else {
        AutoCloseDialog* dlg = new AutoCloseDialog(this, wxString::FromUTF8("已经是第一页了"), 2000);
        dlg->Show();
        prevButton->Enable(false);
    }
}

void MyFrame::OnClosewindowClicked(wxCloseEvent& event) {
    wxMessageDialog dialog(this, "确认结束程序?", "确认信息", wxYES_NO | wxICON_QUESTION);
    if (dialog.ShowModal() == wxID_YES) {
        event.Skip();
    } else {
        event.Veto();
    }
}