// SPDX-License-Identifier: LicenseRef-SBPD-1.0
/******************************************************************************
 * @file    EncodeTest.cpp
 * @brief   SimpleBinaryDictionaryProtocol Socket Test
 * @author  Satoh
 * @note    
 * Copyright (c) 2026 Satoh(3103lab.com)
 *****************************************************************************/

#include <atomic>
#include <chrono>
#include <cstdint>
#include <string>
#include <thread>

#include "SBDP.h"
#include "SBDPSocket.h"
#include "LightTest.h"

namespace {

unsigned short NextTestPort()
{
    static std::atomic<unsigned short> s_unPort{42000};
    return s_unPort.fetch_add(1);
}

} // namespace

LTEST_DEFINE_TEST(TestSocketSendRecvNormal)
{
    LTEST_EXPECT_TRUE(sbdp::InitSockets());

    const unsigned short unPort = NextTestPort();
    std::atomic<bool> bServerReady{false};
    std::atomic<bool> bServerDone{false};
    sbdp::Message msgServerReceived{};
    sbdp::Message msgClientReceived{};

    std::thread thServer([&]() {
        sbdp::Socket cListen{};
        LTEST_EXPECT_TRUE(cListen.Create());
        LTEST_EXPECT_TRUE(cListen.Bind(unPort));
        LTEST_EXPECT_TRUE(cListen.Listen());
        bServerReady = true;

        sbdp::Socket cClient = cListen.Accept();
        msgServerReceived = cClient.RecvMessage(1000);

        sbdp::Message msgReply{};
        msgReply["type"] = std::string("ack");
        msgReply["ok"] = static_cast<uint64_t>(1);
        LTEST_EXPECT_TRUE(cClient.SendMessage(msgReply));
        bServerDone = true;
    });

    while (!bServerReady.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    sbdp::Socket cClient{};
    LTEST_EXPECT_TRUE(cClient.Create());
    LTEST_EXPECT_TRUE(cClient.Connect("127.0.0.1", unPort));

    sbdp::Message msgSend{};
    msgSend["type"] = std::string("hello");
    msgSend["value"] = static_cast<int64_t>(123);
    LTEST_EXPECT_TRUE(cClient.SendMessage(msgSend));

    msgClientReceived = cClient.RecvMessage(1000);

    thServer.join();

    LTEST_EXPECT_TRUE(bServerDone.load());
    LTEST_EXPECT_EQ(msgServerReceived, msgSend);

    sbdp::Message msgExpectedReply{};
    msgExpectedReply["type"] = std::string("ack");
    msgExpectedReply["ok"] = static_cast<uint64_t>(1);
    LTEST_EXPECT_EQ(msgClientReceived, msgExpectedReply);

    sbdp::CleanupSockets();
}

LTEST_DEFINE_TEST(TestSocketCreateFailureByDoubleBind)
{
    LTEST_EXPECT_TRUE(sbdp::InitSockets());

    const unsigned short unPort = NextTestPort();
    sbdp::Socket cSock1{};
    sbdp::Socket cSock2{};

    LTEST_EXPECT_TRUE(cSock1.Create());
    LTEST_EXPECT_TRUE(cSock2.Create());
    LTEST_EXPECT_TRUE(cSock1.Bind(unPort));
    LTEST_EXPECT_FALSE(cSock2.Bind(unPort));

    sbdp::CleanupSockets();
}

LTEST_DEFINE_TEST(TestSocketConnectFailure)
{
    LTEST_EXPECT_TRUE(sbdp::InitSockets());

    const unsigned short unPort = NextTestPort();
    sbdp::Socket cClient{};
    LTEST_EXPECT_TRUE(cClient.Create());
    LTEST_EXPECT_FALSE(cClient.Connect("127.0.0.1", unPort));

    sbdp::CleanupSockets();
}

LTEST_DEFINE_TEST(TestSocketSendFailure)
{
    LTEST_EXPECT_TRUE(sbdp::InitSockets());

    sbdp::Socket cClient{};
    LTEST_EXPECT_TRUE(cClient.Create());

    sbdp::Message msg{};
    msg["k"] = std::string("v");

    bool bThrown = false;
    try {
        (void)cClient.SendMessage(msg);
    }
    catch (const std::system_error&) {
        bThrown = true;
    }
    catch (...) {
        bThrown = true;
    }
    LTEST_EXPECT_TRUE(bThrown);

    sbdp::CleanupSockets();
}

LTEST_DEFINE_TEST(TestSocketRecvFailure)
{
    LTEST_EXPECT_TRUE(sbdp::InitSockets());

    const unsigned short unPort = NextTestPort();
    std::atomic<bool> bServerReady{false};

    std::thread thServer([&]() {
        sbdp::Socket cListen{};
        LTEST_EXPECT_TRUE(cListen.Create());
        LTEST_EXPECT_TRUE(cListen.Bind(unPort));
        LTEST_EXPECT_TRUE(cListen.Listen());
        bServerReady = true;

        sbdp::Socket cClient = cListen.Accept();
        cClient.Close();
    });

    while (!bServerReady.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    sbdp::Socket cClient{};
    LTEST_EXPECT_TRUE(cClient.Create());
    LTEST_EXPECT_TRUE(cClient.Connect("127.0.0.1", unPort));

    bool bThrown = false;
    try {
        (void)cClient.RecvMessage(1000);
    }
    catch (const std::exception&) {
        bThrown = true;
    }

    thServer.join();
    LTEST_EXPECT_TRUE(bThrown);

    sbdp::CleanupSockets();
}

LTEST_DEFINE_TEST(TestSocketRecvTimeout)
{
    LTEST_EXPECT_TRUE(sbdp::InitSockets());

    const unsigned short unPort = NextTestPort();
    std::atomic<bool> bServerReady{false};

    std::thread thServer([&]() {
        sbdp::Socket cListen{};
        LTEST_EXPECT_TRUE(cListen.Create());
        LTEST_EXPECT_TRUE(cListen.Bind(unPort));
        LTEST_EXPECT_TRUE(cListen.Listen());
        bServerReady = true;

        sbdp::Socket cClient = cListen.Accept();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        cClient.Close();
    });

    while (!bServerReady.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    sbdp::Socket cClient{};
    LTEST_EXPECT_TRUE(cClient.Create());
    LTEST_EXPECT_TRUE(cClient.Connect("127.0.0.1", unPort));

    bool bTimedOut = false;
    try {
        (void)cClient.RecvMessage(50);
    }
    catch (const std::system_error& e) {
        bTimedOut = (e.code() == std::errc::timed_out);
    }
    catch (...) {
        bTimedOut = false;
    }

    thServer.join();
    LTEST_EXPECT_TRUE(bTimedOut);

    sbdp::CleanupSockets();
}
