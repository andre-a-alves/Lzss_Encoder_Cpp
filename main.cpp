/**
 * @file main.cpp
 *
 * @brief Contains the main() function that runs the application.
 *
 * @version 23 Feb 2022
 *
 * @author Andre Alves
 * Contact: andre@alves.me
 *
 */

#include <gtkmm/application.h>
#include "GuiWindow.h"

int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create(argc, argv, "edu.haw.andre-a-alves.LZSS-encoder");
    GuiWindow GuiWindow;
    return app->run(GuiWindow);
}
