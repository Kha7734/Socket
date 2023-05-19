#include "stdafx.h"
#include "Header.h"

vector<string> split(string data, string Seperator)
{
    vector<string> result;

    int startPos = 0;
    size_t foundPos = data.find_first_of(Seperator, startPos);

    while (foundPos != string::npos)
    {
        int count = foundPos - startPos;
        string token = data.substr(startPos, count);
        result.push_back(token);

        startPos = foundPos + Seperator.length();
        foundPos = data.find(Seperator, startPos);
    }
    string token = data.substr(startPos, data.length() - startPos);
    result.push_back(token);

    return result;
}

string parse(string request)
{
    // If request's not GET & POST, fileName returned "404"
    string fileName = "404";
    const string Space = " ";

    // Get the first line of request to parse
    size_t foundPos = request.find_first_of('\n');
    string firstLine = request.substr(0, foundPos);

    // Get /path HTTP/1.1
    vector<string> tokens = split(firstLine, Space);
    string requestType = tokens[0];
    string path = tokens[1];

    if (requestType == "GET")
    {
        if (path == "/")
            fileName = "index.htm";
        else
            fileName = tokens[1].substr(1, tokens[1].length() - 1);
    }
    else if (requestType == "POST")
    {
        foundPos = request.find("uname", 0);

        string lastLine = request.substr(foundPos, request.length() - foundPos);
        vector<string> token2 = split(lastLine, "&");

        vector<string> pairs1 = split(token2[0], "=");
        string uname = pairs1[1];

        vector<string> pairs2 = split(token2[1], "=");
        string psw = pairs2[1];

        if (uname == "admin" && psw == "123456")
            fileName = "images.htm";
        else
            fileName = "401.htm";

        token2.clear();
        pairs1.clear();
        pairs2.clear();
    }

    tokens.clear();
    return fileName;
}

void readFile(string fileName, int& length, string& data)
{
    ifstream ifs(fileName, ios::in | ios::binary);
    if (!ifs)
    {
        fileName = "404.htm";
        ifs.open(fileName, ios::in | ios::binary);
    }

    char Buffer[10'000];
    memset(Buffer, '0', 10'000);
    int count;

    do
    {
        ifs.read((char*)Buffer, sizeof(Buffer));

        count = ifs.gcount();

        data.insert(length, Buffer, count);
        length += count;

    } while (count == 10'000);

    ifs.close();
}

string setContentType(string fileName)
{
    size_t startPos = fileName.find_first_of(".");
    string fileType = fileName.substr(startPos + 1, fileName.length() - startPos);
    string contentType;
    if (fileType == "html" || fileType == "htm")
        contentType = "text/html";
    else if (fileType == "txt")
        contentType = "text/plain";
    else if (fileType == "jpg" || fileType == "jpeg")
        contentType = "text/jpeg";
    else if (fileType == "gif")
        contentType = "text/gif";
    else if (fileType == "png")
        contentType = "text/png";
    else if (fileType == "css")
        contentType = "text/css";
    else
        contentType = "application/octet-stream";

    return contentType;
}

string makeSendData(string fileName, string body, string contentType)
{
    string status = "";

    if (fileName == "401.htm")
        status = "401 Unauthorized";
    else if (fileName == "404")
        status = "404 Not Found";
    else
        status = "200 OK";

    stringstream builder;
    builder << "HTTP/1.1 " << status << "\r\n"
        << "Content-Type: " << contentType << "\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "\r\n"
        << body;

    string result = builder.str();
    return result;
}

string handleRequest(string data)
{
    // If data is empty -> return ""
    if (data == "")
        return data;

    // Else: parse data & add header before sending
    // SendFile = Header + Data
    string fileName = parse(data);
    string bodyData = "", contentType = "";
    int length = 0;

    readFile(fileName, length, bodyData);
    contentType = setContentType(fileName);

    string Result = makeSendData(fileName, bodyData, contentType);
    return Result;
}

void sendResponse(SOCKET ClientSocket)
{
    bool isConnection = 1;

    do
    {
        char Buffer[10'000] = "";
        isConnection = recv(ClientSocket, Buffer, 10'000, 0);
        if (isConnection == SOCKET_ERROR)
        {
            printf("Send failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            break;
        }
        if (isConnection == 0)
        {
            break;
        }

        string temp = handleRequest(Buffer);
        if (temp.length())
        {
            printf("Request by Client\n");
            int iSendResult = send(ClientSocket, (char*)temp.c_str(), temp.length(), 0);
            printf("==>Bytes sent: %d\n\n", iSendResult);
            memset(Buffer, 0, strlen(Buffer));
        }
    } while (isConnection);
}

DWORD WINAPI receive_cmds(LPVOID lpParam)
{
    // set out socket to the socket passed in as a parameter
    SOCKET currentClient = (SOCKET)lpParam;
    thread response(&sendResponse, currentClient);
    response.detach();

    return 0;
}

