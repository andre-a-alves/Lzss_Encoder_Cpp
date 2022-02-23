/**
 * @file GuiWindow.cpp
 *
 * @brief Contains the member functions of the GuiWindow class.
 *
 * @version 23 Feb 2022
 *
 * @author Andre Alves
 * Contact: andre@alves.me
 *
 */
#include <iostream>
#include "GuiWindow.h"
#include "Lzss.h"

std::string chosenFile;
std::string saveLocation;

GuiWindow::GuiWindow()
:
    labelFile("Test label"),
    buttonEncode("Encode"),
    buttonDecode("Decode"),
    buttonChooseFile("Choose File")
{
    set_title("LZSS Encoder/Decoder");
    set_border_width(36);

    buttonChooseFile.signal_clicked().connect(sigc::mem_fun(*this, &GuiWindow::onButtonChooseFileClicked));
    buttonEncode.signal_clicked().connect(sigc::mem_fun(*this, &GuiWindow::onButtonEncodeClicked));
    buttonDecode.signal_clicked().connect(sigc::mem_fun(*this, &GuiWindow::onButtonDecodeClicked));

    buttonEncode.set_margin_right(12);
    buttonEncode.set_margin_top(12);
    buttonDecode.set_margin_left(12);
    buttonDecode.set_margin_top(12);

    add(grid);
    grid.add(buttonEncode);
    grid.add(buttonDecode);
    grid.attach_next_to(buttonChooseFile, buttonEncode, Gtk::POS_TOP, 2, 1);

    show_all_children();
}

GuiWindow::~GuiWindow()
{

}

void GuiWindow::onButtonChooseFileClicked()
{
    std::cout << "Choose File!" << std::endl;
    Gtk::FileChooserDialog fileDialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
    fileDialog.set_transient_for(*this);
    fileDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    fileDialog.add_button("Select", Gtk::RESPONSE_OK);

    fileDialog.run();

    chosenFile = fileDialog.get_filename();
}

void GuiWindow::onButtonEncodeClicked()
{
    if (chosenFile.empty())
    {
        noFileDialog(encode);
    }
    else
    {
        openFileSaveWindow();
        Lzss::encode(chosenFile, saveLocation);
        postActionDialog(encode);
        clearLocations();
    }
}

void GuiWindow::onButtonDecodeClicked()
{
    if (chosenFile.empty())
    {
        noFileDialog(decode);
    }
    else
    {
        openFileSaveWindow();
        Lzss::decode(chosenFile, saveLocation);
        postActionDialog(decode);
        clearLocations();
    }
}

void GuiWindow::openFileSaveWindow()
{
    std::cout << "Choose File!" << std::endl;
    Gtk::FileChooserDialog fileDialog("Choose where to save the output", Gtk::FILE_CHOOSER_ACTION_SAVE);
    fileDialog.set_transient_for(*this);
    fileDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    fileDialog.add_button("Save", Gtk::RESPONSE_OK);

    fileDialog.run();

    saveLocation = fileDialog.get_filename();
}

void GuiWindow::noFileDialog(Action action)
{
    std::string messageString = "Please select a file to " +
            std::string(action == encode ? "encode" : "decode") +
            " before clicking this button.";
    Gtk::MessageDialog dialog(*this, "No file chosen");
    dialog.set_secondary_text(messageString);
    dialog.run();
}

void GuiWindow::postActionDialog(Action action)
{
    std::string messageString = "Your file has been " + std::string(action == encode ? "encoded" : "decoded") + ".";
    Gtk::MessageDialog dialog(*this, "Success!");
    dialog.set_secondary_text(messageString);
    dialog.run();
}

void GuiWindow::clearLocations()
{
    chosenFile.clear();
    saveLocation.clear();
}