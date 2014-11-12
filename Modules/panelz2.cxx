#include <iostream>
#include <iomanip>
#include <string>

#include "Input.h"
#include "Freestream.h"
#include "Airfoil.h"

int main(int argc, const char* argv[])
{
    using namespace std;

    //- Output a program header

    cout << setfill('-') << setw(64) << "\n"
         << "\n"
         << "Panelz2" << "\n"
         << "Author: Adam O'Brien" << "\n"
         << "E-mail: obrienadam89@gmail.com" << "\n"
         << "\n"
         << setfill('-') << setw(64) << "\n";

    try
    {

        Freestream freestream;
        Airfoil airfoil;
        Input input(argc, argv, freestream, airfoil);

    }
    catch(const char* errorMessage)
    {

        cerr << "Error: " << errorMessage << endl;

    }

    return 0;

}
