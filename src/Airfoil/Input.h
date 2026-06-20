#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <map>
#include <vector>

#include "Freestream.h"
#include "Airfoil.h"

/// Pure file-reader class.
/// CLI argument parsing is handled by absl::flags in panelz2.cxx (main).
class Input
{
public:
    Input();

    void readUserInputFile(const std::string& filename, Freestream& freestream);
    void readAirfoilCoordinatesFile(const std::string& filename, Airfoil& airfoil);

private:
    typedef std::map<std::string, std::string> Map;

    Map inputMap_;

    /// Strip whitespace and inline comments from a buffer line.
    static void processBuffer(std::string& buffer);
};

#endif // INPUT_H
