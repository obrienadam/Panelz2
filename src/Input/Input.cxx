#include "Input.h"
#include "VersionInfo.h"

Input::Input()
    :
      commandLineOptions_("Available Options:")
{
    using namespace std;
    using namespace boost::program_options;

    // Construct the available command line options


    commandLineOptions_.add_options()
            ("help", "Displays this help message")
            ("version", "Displays the current version")
            ("user-file", value<string>(),  "Specify the user input file")
            ("airfoil-file", value<string>(), "Specify the file containing the airfoil data");

}

Input::Input(int argc, const char *argv[])
    :
      Input()
{

    readCommandLineArgs(argc, argv);

}

void Input::readCommandLineArgs(int argc, const char *argv[])
{
    using namespace std;
    using namespace boost::program_options;

    store(parse_command_line(argc, argv, commandLineOptions_), commandLineVarMap_);
    notify(commandLineVarMap_);

    if(commandLineVarMap_.count("help"))
    {

        cout << commandLineOptions_ << endl;
        exit(0);

    }
    else if (commandLineVarMap_.count("version"))
    {

        versionInfo();
        exit(0);

    }

    if (commandLineVarMap_.count("user-file"))
    {

        readUserInputFile(commandLineVarMap_["user-file"].as<string>());

    }
    else
    {

        cout << commandLineOptions_ << endl;
        throw "No user input file specified.";

    }

    if (commandLineVarMap_.count("airfoil-file"))
    {

        readAirfoilCoordinatesFile(commandLineVarMap_["airfoil-file"].as<string>());

    }
    else
    {

        cout << commandLineOptions_ << endl;
        throw "No user airfoil file specified.";

    }
}

void Input::readUserInputFile(std::string filename)
{

}

void Input::readAirfoilCoordinatesFile(std::string filename)
{

}
