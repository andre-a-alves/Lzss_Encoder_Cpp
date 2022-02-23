//
// Created by andre on 2/22/22.
//

#ifndef LZSS_ENCODER_CPP_GUIWINDOW_H
#define LZSS_ENCODER_CPP_GUIWINDOW_H

#include <gtkmm.h>

enum Action { encode, decode};

class GuiWindow : public Gtk::Window
{
public:
    GuiWindow();
    ~GuiWindow() override;

protected:
    //Signal handlers
    void onButtonChooseFileClicked();
    void onButtonEncodeClicked();
    void onButtonDecodeClicked();
    void openFileSaveWindow();
    void noFileDialog(Action action);
    void postActionDialog(Action action);
    static void clearLocations();

    //Member widgets
    Gtk::Label labelFile;
    Gtk::Grid grid;
    Gtk::Box box;
    Gtk::Button buttonChooseFile, buttonEncode, buttonDecode;
};


#endif //LZSS_ENCODER_CPP_GUIWINDOW_H
