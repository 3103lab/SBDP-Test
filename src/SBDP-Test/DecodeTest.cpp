// SPDX-License-Identifier: LicenseRef-SBPD-1.0
/******************************************************************************
 * @file    DecodeTest.cpp
 * @brief   SimpleBinaryDictionaryProtocol Decode Test
 * @author  Satoh
 * @note    
 * Copyright (c) 2026 Satoh(3103lab.com)
 *****************************************************************************/
#include "SBDP.h"
#include "LightTest.h"

LTEST_DEFINE_TEST(TestDecodeFormatInt64)
{
    const std::vector<uint8_t> encoded = {
        0x00, 0x00, 0x00, 0x0C,
        0x00, 0x01, 0x6B,
        0x01,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };

    const sbdp::Message decoded = sbdp::DecodeMessage(encoded);
    const sbdp::Message expected = {
        { "k", static_cast<int64_t>(0x0102030405060708LL) }
    };

    LTEST_EXPECT_EQ(decoded, expected);
}

LTEST_DEFINE_TEST(TestDecodeFormatUint64)
{
    const std::vector<uint8_t> encoded = {
        0x00, 0x00, 0x00, 0x0C,
        0x00, 0x01, 0x6B,
        0x02,
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };

    const sbdp::Message decoded = sbdp::DecodeMessage(encoded);
    const sbdp::Message expected = {
        { "k", static_cast<uint64_t>(0x8899AABBCCDDEEFFULL) }
    };

    LTEST_EXPECT_EQ(decoded, expected);
}

LTEST_DEFINE_TEST(TestDecodeFormatFloat64)
{
    const std::vector<uint8_t> encoded = {
        0x00, 0x00, 0x00, 0x0C,
        0x00, 0x01, 0x6B,
        0x03,
        0x3F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    const sbdp::Message decoded = sbdp::DecodeMessage(encoded);
    const sbdp::Message expected = {
        { "k", static_cast<sbdp::float64_t>(1.0) }
    };

    LTEST_EXPECT_EQ(decoded, expected);
}

LTEST_DEFINE_TEST(TestDecodeFormatString)
{
    const std::vector<uint8_t> encoded = {
        0x00, 0x00, 0x00, 0x0B,
        0x00, 0x01, 0x6B,
        0x04,
        0x00, 0x00, 0x00, 0x03,
        0x61, 0x62, 0x63
    };

    const sbdp::Message decoded = sbdp::DecodeMessage(encoded);
    const sbdp::Message expected = {
        { "k", std::string("abc") }
    };

    LTEST_EXPECT_EQ(decoded, expected);
}

LTEST_DEFINE_TEST(TestDecodeFormatBinary)
{
    const std::vector<uint8_t> encoded = {
        0x00, 0x00, 0x00, 0x0C,
        0x00, 0x01, 0x6B,
        0x05,
        0x00, 0x00, 0x00, 0x04,
        0xDE, 0xAD, 0xBE, 0xEF
    };

    const sbdp::Message decoded = sbdp::DecodeMessage(encoded);
    const sbdp::Message expected = {
        { "k", std::vector<uint8_t>{ 0xDE, 0xAD, 0xBE, 0xEF } }
    };

    LTEST_EXPECT_EQ(decoded, expected);
}

