#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <iostream>

#include <boost/program_options.hpp>

class Input
{

private:

    typedef boost::program_options::options_description OptionsDescription;
    typedef boost::program_options::variables_map VariablesMap;

private:

    OptionsDescription commandLineOptions_;
    VariablesMap commandLineVarMap_;

public:

    Input();
    Input(int argc, const char* argv[]);

    void readCommandLineArgs(int argc, const char* argv[]);

    void readUserInputFile(std::string filename);
    void readAirfoilCoordinatesFile(std::string filename);

};

#endif
