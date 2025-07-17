#include "scan.h"

void Scan::Scanner::listeningPorts() {
    PMIB_TCPTABLE_OWNER_PID pTcpTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    // Allocate initial buffer
    pTcpTable = (MIB_TCPTABLE_OWNER_PID*)malloc(sizeof(MIB_TCPTABLE_OWNER_PID));
    dwSize = sizeof(MIB_TCPTABLE_OWNER_PID);

    // Get required buffer size
    if ((dwRetVal = GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)) == ERROR_INSUFFICIENT_BUFFER) {
        free(pTcpTable);
        pTcpTable = (MIB_TCPTABLE_OWNER_PID*)malloc(dwSize);
    }

    // Retrieve TCP table
    if ((dwRetVal = GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)) == NO_ERROR) {
        for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
            MIB_TCPROW_OWNER_PID row = pTcpTable->table[i];

            if (row.dwState == MIB_TCP_STATE_LISTEN) {
                // Convert IP address using InetNtop
                struct in_addr ipAddr;
                ipAddr.S_un.S_addr = row.dwLocalAddr;

                char ipStr[INET_ADDRSTRLEN] = { 0 };
                if (InetNtopA(AF_INET, &ipAddr, ipStr, INET_ADDRSTRLEN) == NULL) {
                    std::cerr << "Failed to convert IP address\n";
                    continue;
                }

                int port = ntohs((u_short)row.dwLocalPort);

                std::cout << "LISTENING on IP: " << ipStr
                    << " Port: " << port
                    << " PID: " << row.dwOwningPid << std::endl;
            }
        }
    }
    else {
        std::cerr << "GetExtendedTcpTable failed with code: " << dwRetVal << std::endl;
    }

    if (pTcpTable) {
        free(pTcpTable);
    }
}

int Scan::Scanner::run() {
    std::cout << "Scanning for listening ports...\n";
    listeningPorts();
    return 0;
}