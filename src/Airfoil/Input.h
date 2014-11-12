#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include <boost/program_options.hpp>

#include "Freestream.h"
#include "Airfoil.h"

class Input
{

private:

    typedef boost::program_options::options_description OptionsDescription;
    typedef boost::program_options::variables_map VariablesMap;
    typedef std::map<std::string, std::string> Map;

private:

    //- Command line variables

    OptionsDescription commandLineOptions_;
    VariablesMap commandLineVarMap_;

    //- Values read from the user input file

    Map inputMap_;

    //- Helper methods

    static void processBuffer(std::string& buffer);

public:

    Input();
    Input(int argc, const char* argv[], Freestream& freestream, Airfoil& airfoil);

    void readCommandLineArgs(int argc, const char* argv[], Freestream& freestream, Airfoil& airfoil);

    void readUserInputFile(std::string filename, Freestream& freestream);
    void readAirfoilCoordinatesFile(std::string filename, Airfoil& airfoil);

};

#endif
