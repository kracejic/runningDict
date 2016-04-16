#include "./MainWindow.h"

#include <iostream>

MainWindow::MainWindow(Logic& logic)
    : mLogic(logic)
    , mAddWordButton("+")
    , mSettingsButton("O")
{
    // Sets the border width of the window.
    set_border_width(10);
    set_default_size(500, 250);

    // When the button receives the "clicked" signal, it will call the
    // on_button_clicked() method defined below.
    // mbutton.signal_clicked().connect(sigc::memfun(*this,
    //             &MainWindow::on_button_clicked));


    add(mGrid);

    mGrid.add(mWordInput);
    mGrid.add(mAddWordButton);
    mGrid.add(mSettingsButton);


    mWordInput.set_text("Put here text to translation...");
    mWordInput.set_hexpand();

    this->show_all_children();
}
//-----------------------------------------------------------------------------------

void MainWindow::on_button_clicked()
{
  std::cout << "Hello World" << std::endl;
}
