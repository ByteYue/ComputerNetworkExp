#include "config.h"

using namespace std;

Config::Config()
{
}

Config::~Config()
{
}

const string Config::SERVERADDRESS="127.0.0.1";
const int Config::MAXCONNECTION=5;
const int Config::BUFFERLENGTH=256;
const int Config::PORT=5050;
const u_long Config::BLOCKMODE=1;
