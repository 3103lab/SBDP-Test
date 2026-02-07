// SPDX-License-Identifier: LicenseRef-SBPD-1.0
/******************************************************************************
 * @file    EncodeTest.cpp
 * @brief   SimpleBinaryDictionaryProtocol Encode Test
 * @author  Satoh
 * @note    
 * Copyright (c) 2026 Satoh(3103lab.com)
 *****************************************************************************/
#include "SBDP.h"
#include "LightTest.h"

LTEST_DEFINE_TEST(TestEncodeFormatInt64)
{
    sbdp::Message msg{};
    msg["k"] = static_cast<int64_t>(0x0102030405060708LL);

    const std::vector<uint8_t> encoded = sbdp::EncodeMessage(msg);
    const std::vector<uint8_t> expected = {
        0x00, 0x00, 0x00, 0x0C,
        0x00, 0x01, 0x6B,
        0x01,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };

    LTEST_EXPECT_EQ(encoded, expected);
}

LTEST_DEFINE_TEST(TestEncodeFormatUint64)
{
    sbdp::Message msg{};
    msg["k"] = static_cast<uint64_t>(0x8899AABBCCDDEEFFULL);

    const std::vector<uint8_t> encoded = sbdp::EncodeMessage(msg);
    const std::vector<uint8_t> expected = {
        0x00, 0x00, 0x00, 0x0C,
        0x00, 0x01, 0x6B,
        0x02,
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };

    LTEST_EXPECT_EQ(encoded, expected);
}

LTEST_DEFINE_TEST(TestEncodeFormatFloat64)
{
    sbdp::Message msg{};
    msg["k"] = static_cast<sbdp::float64_t>(1.0);

    const std::vector<uint8_t> encoded = sbdp::EncodeMessage(msg);
    const std::vector<uint8_t> expected = {
        0x00, 0x00, 0x00, 0x0C,
        0x00, 0x01, 0x6B,
        0x03,
        0x3F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    LTEST_EXPECT_EQ(encoded, expected);
}

LTEST_DEFINE_TEST(TestEncodeFormatString)
{
    sbdp::Message msg{};
    msg["k"] = std::string("abc");

    const std::vector<uint8_t> encoded = sbdp::EncodeMessage(msg);
    const std::vector<uint8_t> expected = {
        0x00, 0x00, 0x00, 0x0B,
        0x00, 0x01, 0x6B,
        0x04,
        0x00, 0x00, 0x00, 0x03,
        0x61, 0x62, 0x63
    };

    LTEST_EXPECT_EQ(encoded, expected);
}

LTEST_DEFINE_TEST(TestEncodeFormatBinary)
{
    sbdp::Message msg{};
    msg["k"] = std::vector<uint8_t>{0xDE, 0xAD, 0xBE, 0xEF};

    const std::vector<uint8_t> encoded = sbdp::EncodeMessage(msg);
    const std::vector<uint8_t> expected = {
        0x00, 0x00, 0x00, 0x0C,
        0x00, 0x01, 0x6B,
        0x05,
        0x00, 0x00, 0x00, 0x04,
        0xDE, 0xAD, 0xBE, 0xEF
    };

    LTEST_EXPECT_EQ(encoded, expected);
}


