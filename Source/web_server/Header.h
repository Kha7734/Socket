// web_server.cpp : Defines the entry point for the console application.
//
// Ref: https://learn.microsoft.com/en-us/windows/win32/winsock/complete-server-code?source=recommendations
//#pragma once
#undef UNICODE

#define WIN32_LEAN_AND_MEAN
// #include "stdafx.h"
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <thread>


using namespace std;

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8080"

vector<string> split(string, string);
string parse(string);
void readFile(string, int&, string& );
string setContentType(string);
string makeSendData(string, string, string );
string handleRequest(string );
void sendResponse(SOCKET );
DWORD WINAPI receive_cmds(LPVOID lpParam);
