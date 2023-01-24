//
//  socket.hpp
//
//  Created by Johnathan Chiu on 2/21/19.
//  Copyright © 2019 Johnathan Chiu. All rights reserved.
//

#ifndef socket_hpp
#define socket_hpp

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <io.h>

#endif /* socket_hpp */

namespace serversock {

    struct objectData {
        unsigned int value;
    };

    void createConnection();
    int readValues(objectData* a);

}