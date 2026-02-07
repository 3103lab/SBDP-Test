// SPDX-License-Identifier: LicenseRef-SBPD-1.0
/******************************************************************************
 * @file    main.cpp
 * @brief   SimpleBinaryDictionaryProtocol Test Entrypoint
 * @author  Satoh
 * @note    
 * Copyright (c) 2026 Satoh(3103lab.com)
 *****************************************************************************/
 
#include "SBDP.h"
#include "LightTest.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    bool bRet = ltest::LightTest::Instance().RunAllTests();
    if(!bRet){
        return -1;
    }
    return 0;
}
