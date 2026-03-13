//---------------------------------------------------------------------------

#include <vcl.h>
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
 // Source - https://stackoverflow.com/a/8032108
// Posted by Andrew Shepherd
// Retrieved 2026-03-13, License - CC BY-SA 3.0

const wchar_t *GetWC(const char *c)
{
    const size_t cSize = strlen(c)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, c, cSize);

    return wc;
}

void SetWP(AnsiString fp)
{
    const wchar_t *path = GetWC(fp.c_str());
    int result;
    result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *)path, SPIF_UPDATEINIFILE);
    std::cout << result;
    return;
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::FormActivate(TObject *Sender)
{
    char tp[128];
        GetTempPath(128,tp);
    AnsiString tpbpotd = AnsiString(tp)+"BPOTD";
    MkDir(tpbpotd);

    Status->Caption = "Fetching image from \"https://img-s-msn-com.akamaized.net/tenant/amp/entityid/BB1msOOR\"";
    this->Update();
    const ::WCHAR URL[] = L"https://img-s-msn-com.akamaized.net/tenant/amp/entityid/BB1msOOR";
    const ::HINTERNET session = ::netstart();
    if ( session != 0 )
    {
        const ::HINTERNET istream = ::netopen(session, URL);
        if ( istream != 0 )
        {
            std::ofstream ostream((tpbpotd+"\\bpotd.jpg").c_str(), std::ios::binary);
            if ( ostream.is_open() ) {
                ::netfetch(istream, ostream);
            }
            else {
                std::cerr << "Could not open 'bpotd.jpg'." << std::endl;
                Status->Caption = "Could not open \""+(tpbpotd+"\\bpotd.jpg")+"\".";
                this->Update();
            }
            ::netclose(istream);
        }
        ::netclose(session);
        Status->Caption = "Bild wurde als \""+(tpbpotd+"\\bpotd.jpg")+"\" gespeichert.";
        this->Update();
        SetWP((tpbpotd+"\\bpotd.jpg"));
        Status->Caption = "Hintergrund aktualisiert.";
        Sleep(500);
        this->Close();
    }
}
//---------------------------------------------------------------------------
