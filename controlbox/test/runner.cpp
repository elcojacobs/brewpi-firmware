#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_CONSOLE_WIDTH 300 // workaround for compatiblity with vscode Test Explorer

#include "Connections.h"
#include "testinfo.h"
#include <catch.hpp>

TestInfo testInfo;

void
handleReset(bool)
{
    ++testInfo.rebootCount;
};

namespace cbox {
void
connectionStarted(DataOut& out)
{
}
}