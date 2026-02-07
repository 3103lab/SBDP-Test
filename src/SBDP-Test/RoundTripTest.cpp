// SPDX-License-Identifier: LicenseRef-SBPD-1.0
/******************************************************************************
 * @file    EncodeTest.cpp
 * @brief   SimpleBinaryDictionaryProtocol Round Trip Test
 * @author  Satoh
 * @note    
 * Copyright (c) 2026 Satoh(3103lab.com)
 *****************************************************************************/
#include "SBDP.h"
#include "LightTest.h"

LTEST_DEFINE_TEST(TestRoundTrip)
{
    sbdp::Message msg{};
    msg["int64"] = static_cast<int64_t>(-1234567890123LL);
    msg["uint64"] = static_cast<uint64_t>(18446744073709551610ULL);
    msg["float64"] = static_cast<sbdp::float64_t>(3.141592653589793);
    msg["string"] = std::string("hello sbdp");
    msg["binary"] = std::vector<uint8_t>{ 0x00, 0x7F, 0x80, 0xFF };

    const std::vector<uint8_t> encoded = sbdp::EncodeMessage(msg);
    const sbdp::Message decoded = sbdp::DecodeMessage(encoded);

    LTEST_EXPECT_EQ(decoded.size(), msg.size());
    LTEST_EXPECT_TRUE(decoded == msg);
}

LTEST_DEFINE_TEST(TestDecodeMalformedMessage)
{
    sbdp::Message msg{};
    msg["key"] = std::string("value");

    std::vector<uint8_t> encoded_truncated = sbdp::EncodeMessage(msg);
    encoded_truncated.pop_back();

    bool thrown_truncated = false;
    try {
        (void)sbdp::DecodeMessage(encoded_truncated);
    }
    catch (const std::runtime_error&) {
        thrown_truncated = true;
    }

    std::vector<uint8_t> encoded_appended = sbdp::EncodeMessage(msg);
    encoded_appended.push_back(0x00);

    bool thrown_appended = false;
    try {
        (void)sbdp::DecodeMessage(encoded_appended);
    }
    catch (const std::runtime_error&) {
        thrown_appended = true;
    }

    LTEST_EXPECT_TRUE(thrown_truncated);
    LTEST_EXPECT_TRUE(thrown_appended);
}

