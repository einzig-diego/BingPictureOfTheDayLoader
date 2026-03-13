//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#pragma hdrstop

#include "main.h"
#include "internet.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------

__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

// Source: https://stackoverflow.com/a/8032108
// Posted by Andrew Shepherd
// Retrieved 2026-03-13, License: CC BY-SA 3.0
const wchar_t* GetWC(const char* c)
{
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs(wc, c, cSize);
    return wc;
}
//---------------------------------------------------------------------------

void SetWallpaper(const std::string& filePath)
{
    const wchar_t* path = GetWC(filePath.c_str());
    int result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, 
                                       (void*)path, SPIF_UPDATEINIFILE);
    std::cout << "SystemParametersInfo result: " << result << std::endl;
    delete[] path; // Fix memory leak - delete the allocated wchar_t array
}
//---------------------------------------------------------------------------

bool FetchURLToFile(const std::string& url, const std::string& outputPath)
{
    const wchar_t* URL = GetWC(url.c_str());
    const HINTERNET session = netstart();
    bool success = false;
    
    if (session != 0)
    {
        const HINTERNET istream = netopen(session, URL);
        if (istream != 0)
        {
            std::ofstream ostream(outputPath.c_str(), std::ios::binary);
            if (ostream.is_open())
            {
                netfetch(istream, ostream);
                success = true;
            }
            else
            {
                std::cerr << "Could not open output file: " << outputPath << std::endl;
            }
            netclose(istream);
        }
        netclose(session);
    }
    
    delete[] URL; // Fix memory leak
    return success;
}
//---------------------------------------------------------------------------

std::string ExtractImageUrlFromXML(const std::string& xmlFilePath)
{
    // Open and read XML file
    FILE* f = fopen(xmlFilePath.c_str(), "r");
    if (!f) return "";
    
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    
    char* buf = new char[sz + 1];
    size_t bytesRead = fread(buf, 1, sz, f);
    buf[bytesRead] = '\0';
    fclose(f);
    
    std::string xmlContent(buf);
    delete[] buf;
    
    // Simple XML parsing - find image URL
    // Format: <url>https://example.com/image.jpg</url>
    std::string urlStartTag = "<url>";
    std::string urlEndTag = "</url>";
    
    size_t startPos = xmlContent.find(urlStartTag);
    if (startPos == std::string::npos) return "";
    
    startPos += urlStartTag.length();
    size_t endPos = xmlContent.find(urlEndTag, startPos);
    if (endPos == std::string::npos) return "";
    
    std::string imageUrl = xmlContent.substr(startPos, endPos - startPos);
    
    // Bing URLs might need base URL prepended
    if (imageUrl.substr(0, 4) != "http")
    {
        imageUrl = "https://www.bing.com" + imageUrl;
    }
    
    return imageUrl;
}
//---------------------------------------------------------------------------

void EnsureDirectoryExists(const std::string& path)
{
    // Create directory if it doesn't exist
    // Note: This is Windows-specific
    CreateDirectoryA(path.c_str(), NULL);
}
//---------------------------------------------------------------------------

std::string GetTempPathString()
{
    char tp[128];
    GetTempPathA(128, tp);
    return std::string(tp);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormActivate(TObject* Sender)
{
    // Setup paths
    std::string tempPath = GetTempPathString();
    std::string appDir = tempPath + "BPOTD";
    std::string xmlPath = appDir + "\\xml.xml";
    std::string imagePath = appDir + "\\bpotd.jpg";
    
    // Ensure directory exists
    EnsureDirectoryExists(appDir);
    
    // Step 1: Fetch XML
    Status->Caption = L"Fetching XML from Bing...";
    this->Update();
    
    std::string xmlUrl = "https://www.bing.com/HPImageArchive.aspx?"
                         "format=xml&idx=0&n=1&mkt=en-US";
    
    if (!FetchURLToFile(xmlUrl, xmlPath))
    {
        Status->Caption = L"Failed to fetch XML";
        return;
    }
    
    // Step 2: Parse XML to get image URL
    Status->Caption = L"Parsing XML...";
    this->Update();
    
    std::string imageUrl = ExtractImageUrlFromXML(xmlPath);
    if (imageUrl.empty())
    {
        Status->Caption = L"Failed to extract image URL from XML";
        return;
    }
    
    // Step 3: Fetch image
    Status->Caption = L"Fetching image...";
    this->Update();
    
    if (!FetchURLToFile(imageUrl, imagePath))
    {
        Status->Caption = L"Failed to fetch image";
        return;
    }
    
    // Step 4: Set wallpaper
    Status->Caption = L"Setting wallpaper...";
    this->Update();
    
    SetWallpaper(imagePath);
    
    // Step 5: Done
    Status->Caption = L"Wallpaper updated successfully!";
    Sleep(500);
    this->Close();
}
//---------------------------------------------------------------------------
